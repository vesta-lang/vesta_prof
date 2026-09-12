/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file fuzz/main.c
 * @brief
 * \~english `vxp_fuzz`: sweeps the instruction space and says what it found.
 * \~spanish `vxp_fuzz`: barre el espacio de instrucciones y dice que encontro.
 * \~
 *
 * \~english
 * ONE PROGRAM, TWO ROLES, and it spawns itself for the second one:
 *
 *      vxp_fuzz [depth] [workers] [ms]     the PARENT.  Hands out work, notices
 *                                          deaths and hangs, refines what gets
 *                                          lost until it can name the candidate
 *      vxp_fuzz worker <item>              a WORKER.  Sweeps one item and writes
 *                                          the tally out raw.  Expendable
 *      vxp_fuzz solo <depth> <lo> <hi>     one sweep in THIS process, printed for
 *                                          a person.  Dies on the first lethal
 *                                          candidate, which is what it is for
 *
 * There is no second binary because the worker has to agree with the parent about
 * the oracle, the traversal and the shape of a tally, and a separate program is a
 * second place for those to drift.
 *
 * WHAT THE PARENT IS FOR, and it is not speed.  Executing arbitrary bytes kills the
 * process doing it -- measured, `01 C4` is `add esp, eax` and writes the stack
 * pointer, and `73 FE` is `jae -2` and never comes back -- so a sweep that is one
 * process cannot finish.  The parent is what makes the run exist at all; running
 * several workers at once is a side benefit.
 *
 * \~spanish
 * UN PROGRAMA, DOS PAPELES, y se arranca a si mismo para el segundo:
 *
 *      vxp_fuzz [prof] [trabajadores] [ms]  el PADRE.  Reparte trabajo, nota las
 *                                           muertes y los cuelgues, refina lo que
 *                                           se pierde hasta poder nombrar la
 *                                           candidata
 *      vxp_fuzz worker <trozo>              un TRABAJADOR.  Barre un trozo y
 *                                           escribe la cuenta en crudo.  Prescindible
 *      vxp_fuzz solo <prof> <lo> <hi>       un barrido en ESTE proceso, impreso para
 *                                           una persona.  Muere con la primera
 *                                           candidata letal, que es para lo que esta
 *
 * No hay un segundo binario porque el trabajador tiene que coincidir con el padre en
 * el oraculo, el recorrido y la forma de una cuenta, y un programa aparte es un
 * segundo sitio donde eso se separa.
 *
 * PARA QUE ESTA EL PADRE, y no es para ir rapido.  Ejecutar bytes arbitrarios mata
 * al proceso que lo hace -- medido, `01 C4` es `add esp, eax` y escribe el puntero de
 * pila, y `73 FE` es `jae -2` y no vuelve nunca --, asi que un barrido de un solo
 * proceso no puede terminar.  El padre es lo que hace que la corrida exista; correr
 * varios trabajadores a la vez es una ventaja de propina.
 */

#include "isa_pool.h"
#include "isa_win.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief
 * \~english How deep to enumerate when nothing is asked for.
 * \~spanish Cuanto enumerar de profundo cuando no se pide nada.
 * \~
 *
 * \~english
 * Three is the smallest depth that reaches past the two-byte escapes, so it says
 * something about `0F 38` and `0F 3A` instead of stopping at `0F`.  Each further
 * byte multiplies the work by up to 256, so this is a knob and the default is the
 * one that finishes while somebody is watching.
 *
 * \~spanish
 * Tres es la profundidad menor que llega mas alla de los escapes de dos bytes, asi
 * que dice algo de `0F 38` y `0F 3A` en vez de pararse en `0F`.  Cada byte mas
 * multiplica el trabajo por hasta 256, asi que esto es un mando y el valor por
 * defecto es el que acaba mientras alguien mira.
 */
#define FUZZ_DEPTH 3u

/**
 * @brief
 * \~english How long a worker gets before it counts as hung.
 * \~spanish Cuanto se le da a un trabajador antes de contar como colgado.
 * \~
 *
 * \~english
 * It has to clear the slowest honest item by a wide margin, or a sweep would report
 * candidates as lethal for being slow.  A whole first byte at depth three is a few
 * hundred thousand attempts at worst and each is an exception, so seconds; two of
 * them is room to spare, and a hang is infinite so the margin costs nothing.
 *
 * \~spanish
 * Tiene que pasar de largo el trozo honesto mas lento, o un barrido informaria de
 * candidatas letales por lentas.  Un primer byte entero a profundidad tres son unos
 * cientos de miles de intentos en el peor caso y cada uno es una excepcion, o sea
 * segundos; dos de ellos es sitio de sobra, y un cuelgue es infinito, asi que el
 * margen no cuesta nada.
 */
#define FUZZ_TIMEOUT_MS 20000u

static const char *outcome_name(u32 outcome) {
    switch (outcome) {
    case ISA_RAN: return "corrio";
    case ISA_TRUNCATED: return "corta";
    case ISA_INVALID: return "no existe";
    case ISA_PRIVILEGED: return "privilegiada";
    case ISA_MEMORY_FAULT: return "memoria";
    case ISA_ARITH: return "aritmetica";
    case ISA_UNKNOWN: return "sin clasificar";
    case ISA_RETURNED: return "volvio";
    default: return "?";
    }
}

/** @brief
 *  \~english Adds one tally into another, to total the whole sweep.
 *  \~spanish Suma una cuenta en otra, para totalizar el barrido entero. \~ */
static void tally_add(isa_tally *acc, const isa_tally *one) {
    u32 i;
    acc->candidates += one->candidates;
    acc->skipped += one->skipped;
    acc->truncated += one->truncated;
    for (i = 0; i < ISA_OUTCOME_COUNT; ++i) {
        acc->by_outcome[i] += one->by_outcome[i];
    }
    for (i = 0; i <= ISA_MAX_LEN; ++i) {
        acc->by_length[i] += one->by_length[i];
    }
}

/* -------------------------------------------------------------------------- */
/*  El padre                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief
 * \~english What the parent keeps while the run happens.
 * \~spanish Lo que el padre guarda mientras pasa la corrida.
 */
typedef struct run_state {
    isa_tally total;
    u64 lethal;  /**< \~english named candidates that killed a worker \~spanish candidatas nombradas que mataron a un trabajador \~ */
    u64 hangers; /**< \~english named candidates that never came back \~spanish candidatas nombradas que no volvieron nunca \~ */
} run_state;

/** @brief
 *  \~english Whether this item is one single candidate, so a loss names it.
 *  \~spanish Si este trozo es una sola candidata, asi que perderlo la nombra. \~ */
static int is_single(const isa_work *w) {
    return ((u32)w->fixed + 1u == (u32)w->depth && w->lo == w->hi) ? 1 : 0;
}

/** @brief
 *  \~english Prints the bytes of an item, which is how a finding gets read.
 *  \~spanish Imprime los bytes de un trozo, que es como se lee un hallazgo. \~ */
static void put_item(const isa_work *w) {
    u32 i;
    for (i = 0; i < (u32)w->fixed; ++i) {
        printf("%02X ", (unsigned)w->prefix[i]);
    }
    if (w->lo == w->hi) {
        printf("%02X", (unsigned)w->lo);
    } else {
        printf("%02X..%02X", (unsigned)w->lo, (unsigned)w->hi);
    }
}

/**
 * @brief
 * \~english Called as each item resolves.  Prints only what is worth a line.
 * \~spanish Se llama al resolverse cada trozo.  Imprime solo lo que merece linea.
 * \~
 *
 * \~english
 * A DEATH ON A RANGE IS NOT PRINTED, and that is not hiding anything: it is about to
 * be split and asked again, so the line would be noise that says "narrowing".  What
 * gets a line is a death that NAMES a candidate, because that is the finding, and a
 * subtree that came back with a sweep in it, because that is the data.
 *
 * \~spanish
 * UNA MUERTE SOBRE UN RANGO NO SE IMPRIME, y no es esconder nada: esta a punto de
 * partirse y volver a preguntarse, asi que la linea seria ruido diciendo "estrechando".
 * Lo que se lleva una linea es una muerte que NOMBRA una candidata, porque eso es el
 * hallazgo, y un subarbol que volvio con un barrido dentro, porque eso es el dato.
 */
static void on_item(void *ctx, const isa_work *w, u32 how, const isa_tally *t) {
    run_state *st = (run_state *)ctx;

    if (how == (u32)ISA_WORKER_DONE) {
        tally_add(&st->total, t);
        if (t->candidates > 1u) {
            printf("  ");
            put_item(w);
            printf("   %10llu probadas  %8llu corrio  %8llu no existe\n",
                   (unsigned long long)t->candidates,
                   (unsigned long long)t->by_outcome[ISA_RAN],
                   (unsigned long long)t->by_outcome[ISA_INVALID]);
        }
        return;
    }
    if (!is_single(w)) {
        return; /* se va a partir: todavia no hay nada que decir */
    }
    if (how == (u32)ISA_WORKER_DIED) {
        st->lethal += 1u;
        printf("  MATA      ");
    } else {
        st->hangers += 1u;
        printf("  CUELGA    ");
    }
    put_item(w);
    printf("\n");
}

static int parent_main(const char *exe, u32 depth, u32 workers, u32 timeout_ms) {
    isa_queue q;
    isa_pool_stats ps;
    run_state st;
    status rc;
    u32 i;

    memset(&st, 0, sizeof(st));
    isa_queue_open(&q);
    /* \~english Seeded one first byte per item.  Small items because around half of
     * the subtrees kill the worker, and a death costs the whole item plus the
     * bisection after it.  \~spanish Sembrada con un primer byte por trozo.  Trozos
     * pequenos porque cerca de la mitad de los subarboles matan al trabajador, y una
     * muerte cuesta el trozo entero mas la biseccion de detras. \~ */
    if (isa_queue_seed(&q, depth, 1u) != OK) {
        printf("la cola no admite profundidad %u\n", depth);
        return 2;
    }

    printf("barrido de la ISA en anillo tres\n");
    printf("  profundidad %u, %u trabajadores, plazo %u ms\n\n", depth, workers,
           timeout_ms);

    rc = isa_pool_run(exe, workers, timeout_ms, &q, on_item, &st, &ps);
    if (rc != OK) {
        printf("el grupo de trabajadores no arranca: %d\n", (int)rc);
        return 1;
    }

    printf("\n--- los trabajadores ---\n");
    printf("  arrancados      %llu\n", (unsigned long long)ps.spawned);
    printf("  terminaron      %llu\n", (unsigned long long)ps.done);
    printf("  murieron        %llu\n", (unsigned long long)ps.died);
    printf("  se colgaron     %llu\n", (unsigned long long)ps.hung);
    printf("  candidatas nombradas  %llu\n", (unsigned long long)ps.named);
    if (ps.unspawnable != 0) {
        printf("  SIN PROCESO     %llu  (trozos que nadie barrio)\n",
               (unsigned long long)ps.unspawnable);
    }
    if (q.dropped != 0) {
        /* \~english A dropped item is a region of the space nobody swept.  It is said
         * out loud rather than left to be inferred from a total that looks fine.
         * \~spanish Un trozo perdido es una region del espacio que nadie barrio.  Se
         * dice en voz alta en vez de dejarlo deducir de un total con buena cara. \~ */
        printf("  COLA LLENA      %llu trozos no cupieron: el barrido tiene "
               "agujeros\n",
               (unsigned long long)q.dropped);
    }

    printf("\n--- los veredictos, en total ---\n");
    for (i = 0; i < ISA_OUTCOME_COUNT; ++i) {
        printf("  %-16s %12llu\n", outcome_name(i),
               (unsigned long long)st.total.by_outcome[i]);
    }

    printf("\n--- las longitudes que midio el procesador ---\n");
    for (i = 1; i <= ISA_MAX_LEN; ++i) {
        if (st.total.by_length[i] != 0) {
            printf("  %2u bytes %12llu\n", i,
                   (unsigned long long)st.total.by_length[i]);
        }
    }

    printf("\n--- el total ---\n");
    printf("  %llu probadas, %llu saltadas por tunneling\n",
           (unsigned long long)st.total.candidates,
           (unsigned long long)st.total.skipped);
    printf("  %llu candidatas matan al trabajador, %llu lo cuelgan\n",
           (unsigned long long)st.lethal, (unsigned long long)st.hangers);

    /*
     * \~english THE ACCOUNTING, and it is the only statement here that can catch a
     * hole.  Every candidate in the space ends up in exactly one of three places:
     * tried, skipped by the tunneling, or named as the one that took a worker down.
     * If the three do not add up to the space, something was handed out and never
     * came back -- a queue that overflowed, a process that could not start, an item
     * refined into nothing -- and a sweep with a hole in it reports instructions as
     * non-existent.
     *
     * It is the same law the traversal is tested against on its own, now across
     * processes, where the pieces can actually get lost.
     *
     * \~spanish LA CONTABILIDAD, y es la unica afirmacion de aqui capaz de cazar un
     * agujero.  Cada candidata del espacio acaba en exactamente uno de tres sitios:
     * probada, saltada por el tunneling, o nombrada como la que se llevo a un
     * trabajador.  Si las tres no suman el espacio, algo se entrego y no volvio -- una
     * cola que se desbordo, un proceso que no arranco, un trozo refinado hasta la
     * nada -- y un barrido con un agujero informa de instrucciones como inexistentes.
     *
     * Es la misma ley contra la que se prueba el recorrido por su cuenta, ahora entre
     * procesos, que es donde las piezas se pueden perder de verdad.
     */
    {
        u64 space = 1;
        u64 seen = st.total.candidates + st.total.skipped + ps.named;
        int fits = 1;

        for (i = 0; i < depth; ++i) {
            if (space > (u64)0xFFFFFFFFFFFFFFFFull / 256ull) {
                fits = 0;
                break;
            }
            space *= 256ull;
        }
        if (!fits) {
            printf("  el espacio a profundidad %u no cabe en 64 bits; sin "
                   "contabilidad\n",
                   depth);
        } else if (seen == space) {
            printf("  probadas + saltadas + nombradas = %llu, que es 256^%u "
                   "exacto\n",
                   (unsigned long long)seen, depth);
        } else {
            printf("  CUADRA MAL: %llu contabilizadas de %llu.  El barrido tiene "
                   "agujeros\n",
                   (unsigned long long)seen, (unsigned long long)space);
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/*  El barrido en este mismo proceso, para mirarlo                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief
 * \~english One sweep here, printed.  It dies on the first lethal candidate.
 * \~spanish Un barrido aqui, impreso.  Muere con la primera candidata letal.
 * \~
 *
 * \~english
 * IT IS KEPT PRECISELY BECAUSE IT DIES.  When the parent names a candidate, this is
 * how somebody checks it by hand: point it at that one subtree and watch what
 * happens.  A tool whose findings can only be reproduced by the tool is not much of
 * a tool.
 *
 * \~spanish
 * SE MANTIENE JUSTAMENTE PORQUE MUERE.  Cuando el padre nombra una candidata, esto es
 * como alguien la comprueba a mano: apuntarlo a ese subarbol y ver que pasa.  Una
 * herramienta cuyos hallazgos solo puede reproducir la herramienta no es gran
 * herramienta.
 */
static int solo_main(u32 depth, u32 lo, u32 hi) {
    isa_arena arena;
    isa_probe_ops ops;
    isa_tally total;
    u32 b;

    if (isa_arena_open(&arena) != OK) {
        printf("la arena no abre\n");
        return 1;
    }
    isa_arena_ops(&arena, &ops);
    memset(&total, 0, sizeof(total));

    printf("barrido en este proceso: profundidad %u, bytes %02X a %02X\n\n",
           depth, lo, hi);
    printf("  byte  candidatas   corrio  no existe  privileg   corta\n");
    for (b = lo; b <= hi; ++b) {
        isa_work work;
        isa_tally t;

        memset(&work, 0, sizeof(work));
        work.lo = (u8)b;
        work.hi = (u8)b;
        work.depth = (u8)depth;

        /* Se imprime ANTES de probar: si la candidata mata al proceso, esto es lo
         * unico que queda de la corrida. */
        printf("  %02X ", b);
        fflush(stdout);
        if (isa_sweep(&ops, &work, &t) != OK) {
            printf("\nel oraculo fallo en %02X\n", b);
            isa_arena_close(&arena);
            return 1;
        }
        printf("  %10llu  %7llu  %9llu  %8llu  %6llu\n",
               (unsigned long long)t.candidates,
               (unsigned long long)t.by_outcome[ISA_RAN],
               (unsigned long long)t.by_outcome[ISA_INVALID],
               (unsigned long long)t.by_outcome[ISA_PRIVILEGED],
               (unsigned long long)t.truncated);
        tally_add(&total, &t);
    }
    printf("\n  %llu probadas, %llu saltadas\n",
           (unsigned long long)total.candidates,
           (unsigned long long)total.skipped);
    isa_arena_close(&arena);
    return 0;
}

/* -------------------------------------------------------------------------- */

/** @brief
 *  \~english Reads a pair of hex digits.  Returns -1 on anything else.
 *  \~spanish Lee una pareja de digitos hexadecimales.  Devuelve -1 con otra cosa. \~ */
static int hex_pair(const char *s) {
    u32 i;
    int v = 0;
    for (i = 0; i < 2u; ++i) {
        char c = s[i];
        int d;
        if (c >= '0' && c <= '9') {
            d = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            d = 10 + (c - 'A');
        } else if (c >= 'a' && c <= 'f') {
            d = 10 + (c - 'a');
        } else {
            return -1;
        }
        v = (v << 4) | d;
    }
    return v;
}

static int worker_from_args(int argc, char **argv, isa_work *w) {
    u32 i;
    const char *hex;

    /* worker <depth> <fixed> <lo> <hi> <prefix-hex> */
    if (argc < 6) {
        return 0;
    }
    memset(w, 0, sizeof(*w));
    w->depth = (u8)strtoul(argv[2], 0, 10);
    w->fixed = (u8)strtoul(argv[3], 0, 10);
    {
        int lo = hex_pair(argv[4]);
        int hi = hex_pair(argv[5]);
        if (lo < 0 || hi < 0) {
            return 0;
        }
        w->lo = (u8)lo;
        w->hi = (u8)hi;
    }
    hex = (argc > 6) ? argv[6] : "";
    if (strlen(hex) != (size_t)(2u * (u32)w->fixed)) {
        return 0;
    }
    for (i = 0; i < (u32)w->fixed; ++i) {
        int v = hex_pair(hex + 2u * i);
        if (v < 0) {
            return 0;
        }
        w->prefix[i] = (u8)v;
    }
    return 1;
}

int main(int argc, char **argv) {
    u32 depth = FUZZ_DEPTH;

    setvbuf(stdout, 0, _IONBF, 0);

    if (argc > 1 && strcmp(argv[1], "worker") == 0) {
        isa_work w;
        if (!worker_from_args(argc, argv, &w)) {
            return 3;
        }
        return isa_worker_main(&w);
    }

    if (argc > 1 && strcmp(argv[1], "solo") == 0) {
        u32 lo = 0x00u;
        u32 hi = 0xFFu;
        if (argc > 2) {
            depth = (u32)strtoul(argv[2], 0, 10);
        }
        if (argc > 3) {
            lo = (u32)strtoul(argv[3], 0, 16);
        }
        if (argc > 4) {
            hi = (u32)strtoul(argv[4], 0, 16);
        }
        if (depth < 1u || depth > ISA_WALK_DEPTH_MAX || lo > 0xFFu ||
            hi > 0xFFu || lo > hi) {
            printf("argumentos invalidos\n");
            return 2;
        }
        return solo_main(depth, lo, hi);
    }

    {
        u32 workers = 8u;
        u32 timeout_ms = FUZZ_TIMEOUT_MS;

        if (argc > 1) {
            depth = (u32)strtoul(argv[1], 0, 10);
        }
        if (argc > 2) {
            workers = (u32)strtoul(argv[2], 0, 10);
        }
        if (argc > 3) {
            timeout_ms = (u32)strtoul(argv[3], 0, 10);
        }
        if (depth < 1u || depth > ISA_WALK_DEPTH_MAX || workers < 1u ||
            workers > ISA_POOL_MAX || timeout_ms == 0) {
            printf("argumentos invalidos\n");
            return 2;
        }
        return parent_main(argv[0], depth, workers, timeout_ms);
    }
}
