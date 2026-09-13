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
 *      vxp_fuzz [depth] [workers] [ms]     the PARENT, CHARACTERISING.  Hands out
 *                                          work, notices deaths and hangs, and says
 *                                          what each lethal subtree is LIKE
 *      vxp_fuzz todo <prefix> [depth] ...  the parent, EXHAUSTIVE, under one prefix.
 *                                          Names every lethal candidate.  Costs what
 *                                          that is worth -- hours for the whole space
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
 *      vxp_fuzz [prof] [trabajadores] [ms]  el PADRE, CARACTERIZANDO.  Reparte
 *                                           trabajo, nota las muertes y los cuelgues,
 *                                           y dice COMO ES cada subarbol letal
 *      vxp_fuzz todo <prefijo> [prof] ...   el padre, EXHAUSTIVO, bajo un prefijo.
 *                                           Nombra cada candidata letal.  Cuesta lo
 *                                           que eso vale -- horas el espacio entero
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
    case ISA_NOTHING: return "SIN RESPUESTA";
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

/*
 * \~english Every counter in a tally is a 64-bit word, which is what lets the sum below
 * be written once instead of field by field.  If that ever stops being true the build
 * stops here, which is the point: a field of another width added silently would be
 * summed as garbage.
 *
 * \~spanish Todo contador de una cuenta es una palabra de 64 bits, y es lo que permite
 * escribir la suma de abajo una vez en vez de campo a campo.  Si eso deja de ser cierto
 * la construccion se para aqui, que es la gracia: un campo de otro ancho anadido sin
 * decir nada se sumaria como basura.
 */
STATIC_ASSERT(sizeof(isa_tally) % sizeof(u64) == 0,
              "isa_tally must be whole 64-bit counters for tally_add to work");

/**
 * @brief
 * \~english Adds one tally into another, to total the whole sweep.
 * \~spanish Suma una cuenta en otra, para totalizar el barrido entero.
 * \~
 *
 * \~english
 * WORD BY WORD AND NOT FIELD BY FIELD, because field by field was wrong the moment a
 * field was added.  It was: the four quadrant counters went into the tally and this
 * function kept summing the eight it knew about, so a whole run reported zero findings
 * with 256 candidates tried -- and nothing failed, nothing warned, the numbers simply
 * were not there.
 *
 * Summing the struct as the array of counters it is cannot forget a field.  It costs the
 * assertion above, which is a much cheaper thing to maintain than a list of names.
 *
 * \~spanish
 * PALABRA A PALABRA Y NO CAMPO A CAMPO, porque campo a campo estuvo mal en el momento en
 * que se anadio un campo.  Paso: los cuatro contadores de cuadrante entraron en la cuenta
 * y esta funcion siguio sumando los ocho que conocia, asi que una corrida entera informo
 * de cero hallazgos con 256 candidatas probadas -- y nada fallo, nada aviso, los numeros
 * simplemente no estaban.
 *
 * Sumar la estructura como el array de contadores que es no puede olvidarse un campo.
 * Cuesta la asercion de arriba, que es mucho mas barato de mantener que una lista de
 * nombres.
 */
static void tally_add(isa_tally *acc, const isa_tally *one) {
    u64 *a = (u64 *)acc;
    const u64 *b = (const u64 *)one;
    u32 words = (u32)(sizeof(isa_tally) / sizeof(u64));
    u32 i;
    for (i = 0; i < words; ++i) {
        a[i] += b[i];
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
    /**
     * \~english The lengths of the ones that killed, kept apart from the sweep's own
     * histogram.  They are measured differently -- from what the worker said before it
     * died, not from a tally it wrote -- and mixing two provenances in one column is
     * how a number stops meaning anything.
     *
     * \~spanish Las longitudes de las que mataron, aparte del histograma del propio
     * barrido.  Se miden de otra forma -- de lo que dijo el trabajador antes de morir, no
     * de una cuenta que escribiera -- y mezclar dos procedencias en una columna es como
     * un numero deja de significar nada.
     */
    u64 by_length[ISA_MAX_LEN + 1u];
    u64 zones; /**< \~english subtrees characterised instead of listed \~spanish subarboles caracterizados en vez de listados \~ */
    /**
     * \~english Zones where the sampling found nothing lethal at all, even though a
     * worker died in them.  They are the only output of a characterising run that says
     * "look here with the other mode": the lethal thing is there and sixteen values did
     * not meet it.
     *
     * \~spanish Zonas donde el muestreo no encontro nada letal, aunque un trabajador
     * muriera dentro.  Son la unica salida de una corrida caracterizadora que dice
     * "mira aqui con el otro modo": lo letal esta ahi y dieciseis valores no se lo
     * encontraron.
     */
    u64 blind;
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
static void on_item(void *ctx, const isa_work *w, const isa_worker_outcome *o,
                    const isa_tally *t) {
    run_state *st = (run_state *)ctx;
    u32 how = o->how;
    u32 reached = o->reached;

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
    /*
     * \~english A CHARACTERISED SUBTREE IS NOT A NAMED CANDIDATE, and calling it one
     * produced a line that contradicted itself: `MATA FE` next to `0 of 16 killed`.
     * What is known about it is weaker and has to read that way -- something in here
     * took a worker down, and this is how many of the sampled values did.  A profile of
     * zero means the sampling did not find it, which is a pointer to where the
     * exhaustive mode earns its cost, not a finding about `FE`.
     *
     * \~spanish UN SUBARBOL CARACTERIZADO NO ES UNA CANDIDATA NOMBRADA, y llamarlo asi
     * producia una linea que se contradecia: `MATA FE` al lado de `matan 0 de 16`.  Lo
     * que se sabe de el es mas flojo y tiene que leerse asi -- algo de aqui dentro se
     * llevo a un trabajador, y esto es cuantos de los valores muestreados lo hicieron.
     * Un perfil de cero quiere decir que el muestreo no lo encontro, que es una senal de
     * donde se gana el modo exhaustivo lo que cuesta, y no un hallazgo sobre `FE`.
     */
    if (o->tried != 0) {
        st->zones += 1u;
        if (o->killed == 0) {
            st->blind += 1u;
        }
        printf("  ZONA      ");
        put_item(w);
        printf("   hasta %u   matan %u de %u probados, y hay %llu candidatas\n",
               (unsigned)reached, (unsigned)o->killed, (unsigned)o->tried,
               (unsigned long long)(o->stands_for + 1ull));
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
    /*
     * \~english THE LENGTH COMES FROM WHAT THE WORKER SAID, not from a tally it never
     * got to write.  A candidate that takes the process down cannot report anything
     * afterwards, so the count it had reached is the last thing it announced -- and
     * since the sliding loop stops at the first count the decoder accepts, that count
     * IS the length.
     *
     * \~spanish LA LONGITUD SALE DE LO QUE DIJO EL TRABAJADOR, no de una cuenta que no
     * llego a escribir.  Una candidata que se lleva el proceso no puede informar de
     * nada despues, asi que el numero al que habia llegado es lo ultimo que anuncio --
     * y como el bucle deslizante para en el primer numero que el decodificador acepta,
     * ese numero ES la longitud.
     */
    if (reached != 0) {
        printf("   mide %u", (unsigned)reached);
        if (reached <= ISA_MAX_LEN) {
            st->by_length[reached] += 1u;
        }
    }
    /* \~english And whether it speaks for more than itself, saying which of the two
     * kinds of claim that is: `y por N mas` when those bytes are provably never read,
     * `muestreado` when four values of them behaved alike.  \~spanish Y si habla por mas
     * que por si misma, diciendo cual de las dos afirmaciones es: `y por N mas` cuando
     * esos bytes esta demostrado que no se leen, `muestreado` cuando cuatro valores se
     * portaron igual. \~ */
    if (o->stands_for != 0) {
        printf("   y por %llu mas", (unsigned long long)o->stands_for);
        if (o->tried != 0) {
            /* La caracterizacion sale con sus numeros: quien lea la linea no tiene
             * que creerse nada, ve sobre cuantas se dijo. */
            printf(" (matan %u de %u probados)", (unsigned)o->killed,
                   (unsigned)o->tried);
        } else if (o->by_sampling) {
            printf(" (muestreado)");
        }
    }
    printf("\n");
}

static int parent_main(const char *exe, u32 mode, const u8 *prefix, u32 fixed,
                       u32 depth, u32 workers, u32 timeout_ms,
                       const char *ref_path) {
    isa_queue q;
    isa_pool_stats ps;
    run_state st;
    status rc;
    u32 i;

    /*
     * \~english THE TABLE IS CHECKED HERE AND AGAIN IN EVERY WORKER, which is not
     * duplication: the parent checks so a wrong file is refused in one line instead of as
     * hundreds of processes that each exit quietly -- and a worker exiting quietly is
     * indistinguishable from a lethal candidate, so the run would report findings that are
     * a bad argument.  The worker checks because it is the one that compares.
     *
     * \~spanish LA TABLA SE COMPRUEBA AQUI Y OTRA VEZ EN CADA TRABAJADOR, y no es
     * duplicar: el padre comprueba para que un fichero equivocado se rechace en una linea
     * en vez de como cientos de procesos que salen callados -- y un trabajador que sale
     * callado no se distingue de una candidata letal, asi que la corrida informaria de
     * hallazgos que son un argumento mal puesto.  El trabajador comprueba porque es quien
     * compara.
     */
    if (ref_path != 0) {
        isa_ref probe;
        u8 *buf = isa_ref_load(ref_path, &probe);
        if (buf == 0) {
            return 2;
        }
        if (probe.depth != depth) {
            printf("la referencia es de profundidad %u y se pide %u\n",
                   probe.depth, depth);
            free(buf);
            return 2;
        }
        free(buf);
    }

    memset(&st, 0, sizeof(st));
    isa_queue_open(&q);
    /* \~english Seeded one byte value per item.  Small items because around half of
     * the subtrees kill the worker, and a death costs the whole item plus the
     * bisection after it.  \~spanish Sembrada con un valor de byte por trozo.  Trozos
     * pequenos porque cerca de la mitad de los subarboles matan al trabajador, y una
     * muerte cuesta el trozo entero mas la biseccion de detras. \~ */
    if (isa_queue_seed(&q, prefix, fixed, depth, 1u) != OK) {
        printf("la cola no admite eso: %u bytes congelados a profundidad %u\n",
               fixed, depth);
        return 2;
    }

    printf("barrido de la ISA en anillo tres, modo %s%s\n",
           (mode == (u32)ISA_MODE_EXHAUSTIVE) ? "EXHAUSTIVO" : "caracterizador",
           (ref_path != 0) ? ", contra la referencia" : "");
    if (fixed != 0) {
        printf("  bajo el prefijo ");
        for (i = 0; i < fixed; ++i) {
            printf("%02X ", (unsigned)prefix[i]);
        }
        printf("\n");
    }
    printf("  profundidad %u, %u trabajadores, plazo %u ms\n\n", depth, workers,
           timeout_ms);

    rc = isa_pool_run(exe, mode, ref_path, workers, timeout_ms, &q, on_item, &st,
                      &ps);
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
    /* El canje de las sondas, con los dos numeros: lo que costaron y lo que
     * evitaron.  Uno solo de los dos no dice si se pagan. */
    printf("  sondas de longitud    %llu, que respondieron por %llu candidatas\n",
           (unsigned long long)ps.probes, (unsigned long long)ps.covered);
    printf("  muestreos             %llu, que respondieron por %llu\n",
           (unsigned long long)ps.samples, (unsigned long long)ps.sampled);
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

    /*
     * \~english THE FOUR QUADRANTS, and only two of them are findings.  Where both say
     * the same there is nothing to look at; where they disagree, one of the two is wrong
     * about THIS processor, and which one is not decided here.
     *
     * They only appear when a reference was given, because their absence is not agreement:
     * a run with nothing to compare against has no findings, and printing zeros would read
     * as if it had looked.
     *
     * \~spanish LOS CUATRO CUADRANTES, y solo dos son hallazgos.  Donde los dos dicen lo
     * mismo no hay nada que mirar; donde discrepan, uno de los dos se equivoca sobre ESTE
     * procesador, y cual de ellos no se decide aqui.
     *
     * Solo salen cuando se dio referencia, porque su ausencia no es acuerdo: una corrida
     * sin nada con lo que compararse no tiene hallazgos, e imprimir ceros se leeria como
     * si hubiera mirado.
     */
    if (ref_path != 0) {
        printf("\n--- contra la referencia ---\n");
        printf("  de acuerdo            %12llu\n",
               (unsigned long long)st.total.agree);
        printf("  CONFLICTO de longitud %12llu\n",
               (unsigned long long)st.total.conflict);
        printf("  SOLO EL SILICIO       %12llu\n",
               (unsigned long long)st.total.only_cpu);
        printf("  solo la referencia    %12llu\n",
               (unsigned long long)st.total.only_ref);
    }

    printf("\n--- el total ---\n");
    printf("  %llu probadas, %llu saltadas por tunneling\n",
           (unsigned long long)st.total.candidates,
           (unsigned long long)st.total.skipped);
    printf("  %llu candidatas matan al trabajador, %llu lo cuelgan\n",
           (unsigned long long)st.lethal, (unsigned long long)st.hangers);
    if (st.zones != 0) {
        printf("  %llu zonas caracterizadas en vez de listadas\n",
               (unsigned long long)st.zones);
        if (st.blind != 0) {
            /* Lo unico que esta corrida NO respondio, dicho como lo que es: una lista
             * de sitios donde apuntar el otro modo. */
            printf("  y en %llu de ellas el muestreo no encontro lo letal: piden "
                   "`todo <prefijo>`\n",
                   (unsigned long long)st.blind);
        }
    }
    {
        int any = 0;
        for (i = 1; i <= ISA_MAX_LEN; ++i) {
            if (st.by_length[i] != 0) {
                if (!any) {
                    printf("  y de las que matan, la longitud que alcanzaron a "
                           "decir:\n");
                    any = 1;
                }
                printf("    %2u bytes %12llu\n", i,
                       (unsigned long long)st.by_length[i]);
            }
        }
        if (!any && st.lethal != 0) {
            printf("  ninguna de las que matan dijo su longitud\n");
        }
    }

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
        u64 seen = st.total.candidates + st.total.skipped + ps.named +
                   ps.covered + ps.sampled;
        int fits = 1;

        /*
         * \~english THE SPACE IS THE SUBTREE THAT WAS SWEPT, not the whole instruction
         * space, and getting that wrong turned a correct run into an alarm: pointed at one
         * prefix, the accounting demanded 65.536 candidates from a region that holds 256
         * and announced 65.280 holes that were not there.  A false alarm this loud is
         * worse than no check, because the next real one gets ignored.
         *
         * With `fixed` bytes frozen, what was owned is `256^(depth - fixed)`.
         *
         * \~spanish EL ESPACIO ES EL SUBARBOL QUE SE BARRIO, no el espacio de
         * instrucciones entero, y equivocarse en eso convirtio una corrida correcta en una
         * alarma: apuntada a un prefijo, la contabilidad exigia 65.536 candidatas de una
         * region que tiene 256 y anuncio 65.280 agujeros que no existian.  Una falsa
         * alarma tan ruidosa es peor que no comprobar, porque a la siguiente de verdad no
         * le hace caso nadie.
         *
         * Con `fixed` bytes congelados, lo poseido es `256^(depth - fixed)`.
         */
        for (i = fixed; i < depth; ++i) {
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
            printf("  probadas + saltadas + nombradas + representadas = %llu, "
                   "que es 256^%u exacto\n",
                   (unsigned long long)seen, depth - fixed);
        } else if (seen < space) {
            printf("  FALTAN %llu de %llu: el barrido tiene AGUJEROS, y un "
                   "agujero informa de instrucciones como inexistentes\n",
                   (unsigned long long)(space - seen), (unsigned long long)space);
        } else {
            /*
             * \~english OVER, NOT UNDER, and the two are different failures.  A short
             * count is a region nobody swept.  A long one is a claim that overlaps
             * something already answered, and the known way that happens is a length
             * measurement that was not monotone: the traversal declares a whole subtree
             * skipped when part of it had been tried one by one.  It cannot hide a hole
             * -- coming out long means at least everything was reached -- but it does
             * mean the saving is not exactly what it says.
             *
             * \~spanish DE MAS, NO DE MENOS, y son fallos distintos.  Quedarse corto es
             * una region que nadie barrio.  Pasarse es una afirmacion que solapa algo ya
             * respondido, y la forma conocida de que eso ocurra es una medida de longitud
             * que no fue monotona: el recorrido declara saltado un subarbol entero cuando
             * parte de el se habia probado una a una.  No puede esconder un agujero --
             * pasarse quiere decir que al menos se alcanzo todo --, pero si quiere decir
             * que el ahorro no es exactamente el que dice.
             */
            u64 over = seen - space;
            u64 parts = (over * 100000ull) / space;
            printf("  SOBRAN %llu de %llu (%llu,%03llu%%): una medida de longitud "
                   "no monotona hace que un salto solape lo ya probado\n",
                   (unsigned long long)over, (unsigned long long)space,
                   (unsigned long long)(parts / 1000ull),
                   (unsigned long long)(parts % 1000ull));
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
static int solo_main(u32 depth, u32 lo, u32 hi, const char *ref_path) {
    isa_arena arena;
    isa_probe_ops ops;
    isa_tally total;
    isa_ref ref;
    u8 *ref_buf = 0;
    u32 b;

    memset(&ref, 0, sizeof(ref));
    if (ref_path != 0) {
        ref_buf = isa_ref_load(ref_path, &ref);
        if (ref_buf == 0) {
            return 2;
        }
        /* \~english A table built for another depth would be indexed with numbers it
         * does not have, and the failure would be thousands of loud disagreements that
         * are nothing but a mismatch.  \~spanish Una tabla hecha para otra profundidad se
         * indexaria con numeros que no tiene, y el fallo serian miles de desacuerdos
         * ruidosos que no son mas que un desajuste. \~ */
        if (ref.depth != depth) {
            printf("la referencia es de profundidad %u y se pide %u\n", ref.depth,
                   depth);
            free(ref_buf);
            return 2;
        }
    }

    /* \~english This mode executes arbitrary bytes too, so it dies like the others -- and
     * with the system's crash reporting on, each death costs about five seconds of being
     * asked about it.  Measured: it is what made sweeping one first byte at a time take
     * ten minutes.  \~spanish Este modo tambien ejecuta bytes arbitrarios, asi que muere
     * como los demas -- y con el informe de errores del sistema encendido cada muerte
     * cuesta unos cinco segundos de que te pregunten.  Medido: es lo que hacia que barrer
     * de un primer byte en uno tardara diez minutos. \~ */
    isa_quiet_death();

    if (isa_arena_open(&arena) != OK) {
        printf("la arena no abre\n");
        free(ref_buf);
        return 1;
    }
    isa_arena_ops(&arena, &ops);
    memset(&total, 0, sizeof(total));

    printf("barrido en este proceso: profundidad %u, bytes %02X a %02X%s\n\n",
           depth, lo, hi, (ref_buf != 0) ? ", contra la referencia" : "");
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
        if (isa_sweep(&ops, &work, (ref_buf != 0) ? &ref : 0, &t) != OK) {
            printf("\nel oraculo fallo en %02X\n", b);
            isa_arena_close(&arena);
            free(ref_buf);
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

    /*
     * \~english THE FOUR QUADRANTS, and only two of them are findings.  Where both say
     * the same thing there is nothing to look at; where they disagree, one of the two is
     * wrong about this processor, and which one is not decided here.
     *
     * \~spanish LOS CUATRO CUADRANTES, y solo dos son hallazgos.  Donde los dos dicen lo
     * mismo no hay nada que mirar; donde discrepan, uno de los dos se equivoca sobre este
     * procesador, y cual de ellos no se decide aqui.
     */
    if (ref_buf != 0) {
        printf("\n--- contra la referencia ---\n");
        printf("  de acuerdo            %10llu\n",
               (unsigned long long)total.agree);
        printf("  CONFLICTO de longitud %10llu\n",
               (unsigned long long)total.conflict);
        printf("  SOLO EL SILICIO       %10llu\n",
               (unsigned long long)total.only_cpu);
        printf("  solo la referencia    %10llu\n",
               (unsigned long long)total.only_ref);
    }
    isa_arena_close(&arena);
    free(ref_buf);
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
    /* \~english `-` is how a zero-length prefix travels, so the tokens after it keep
     * their positions.  Accepting the empty string too costs nothing and lets a person
     * type the command without it.  \~spanish `-` es como viaja un prefijo de longitud
     * cero, para que los tokens de detras conserven su sitio.  Aceptar tambien la cadena
     * vacia no cuesta nada y deja teclear la orden sin el. \~ */
    if (strcmp(hex, "-") == 0) {
        hex = "";
    }
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
        return isa_worker_main(&w, (argc > 7) ? argv[7] : 0);
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
        return solo_main(depth, lo, hi, (argc > 5) ? argv[5] : 0);
    }

    {
        u32 mode = (u32)ISA_MODE_CHARACTERISE;
        u8 prefix[ISA_MAX_LEN];
        u32 fixed = 0;
        u32 workers = 8u;
        u32 timeout_ms = FUZZ_TIMEOUT_MS;
        const char *ref_path = 0;
        int arg = 1;

        memset(prefix, 0, sizeof(prefix));

        /*
         * \~english THE EXHAUSTIVE MODE TAKES A PREFIX, and that is the whole shape of
         * it: it is not a faster or slower way of doing the same run, it is the
         * expensive treatment of ONE region -- normally the one a characterising sweep
         * flagged as mixed.  An empty prefix means the whole space, which is what the
         * ground truth costs and is measured in hours.
         *
         * \~spanish EL MODO EXHAUSTIVO LLEVA UN PREFIJO, y esa es toda su forma: no es
         * una manera mas rapida o mas lenta de hacer la misma corrida, es el trato caro
         * de UNA region -- normalmente la que un barrido caracterizador senalo como
         * mezclada.  Un prefijo vacio quiere decir el espacio entero, que es lo que
         * cuesta la verdad completa y se mide en horas.
         */
        if (argc > 1 && strcmp(argv[1], "todo") == 0) {
            mode = (u32)ISA_MODE_EXHAUSTIVE;
            arg = 2;
            if (argc > 2) {
                const char *hex = argv[2];
                size_t len = strlen(hex);
                u32 i;
                if ((len % 2u) != 0 || len / 2u >= ISA_MAX_LEN) {
                    printf("el prefijo son parejas de digitos hexadecimales\n");
                    return 2;
                }
                fixed = (u32)(len / 2u);
                for (i = 0; i < fixed; ++i) {
                    int v = hex_pair(hex + 2u * i);
                    if (v < 0) {
                        printf("el prefijo son parejas de digitos hexadecimales\n");
                        return 2;
                    }
                    prefix[i] = (u8)v;
                }
                arg = 3;
            }
        }

        if (argc > arg) {
            depth = (u32)strtoul(argv[arg], 0, 10);
        }
        if (argc > arg + 1) {
            workers = (u32)strtoul(argv[arg + 1], 0, 10);
        }
        if (argc > arg + 2) {
            timeout_ms = (u32)strtoul(argv[arg + 2], 0, 10);
        }
        /* La referencia va al final en las dos formas, que es el unico sitio donde
         * cabe sin que el resto cambie de posicion segun el modo. */
        if (argc > arg + 3) {
            ref_path = argv[arg + 3];
        }
        if (depth < 1u || depth > ISA_WALK_DEPTH_MAX || workers < 1u ||
            workers > ISA_POOL_MAX || timeout_ms == 0 || fixed >= depth) {
            printf("argumentos invalidos\n");
            return 2;
        }
        return parent_main(argv[0], mode, prefix, fixed, depth, workers,
                           timeout_ms, ref_path);
    }
}
