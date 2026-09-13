/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_isa_queue.c
 * @brief
 * \~english The descent: does it end, and does it end on the right candidate.
 * \~spanish El descenso: termina, y termina en la candidata correcta.
 * \~
 *
 * \~english
 * WHAT COULD GO WRONG HERE IS NOT A CRASH.  The refinement is a loop that feeds
 * itself -- a lost item produces items that may also be lost -- and the two ways it
 * can be wrong are both quiet:
 *
 *      it never ends       a split that does not actually shrink anything keeps
 *                          producing work forever, and the sweep looks busy
 *      it names the wrong  the bisection converges on a candidate that is not the
 *      candidate           one that killed the worker, and the report is a
 *                          plausible lie about which bytes are dangerous
 *
 * Both are checked here by SIMULATING a lethal candidate: a chosen byte string is
 * declared deadly, and then the descent is driven exactly as the parent would drive
 * it -- refine whatever contains the deadly one, discard the rest -- until it names
 * something.  What it names has to be the string that was chosen, and it has to get
 * there in a bounded number of steps.
 *
 * That is the whole apparatus tested without a single process, which is the point of
 * the refinement living in `common/` rather than inside the pool.
 *
 * \~spanish
 * LO QUE PUEDE SALIR MAL AQUI NO ES UNA CAIDA.  El refinamiento es un bucle que se
 * alimenta a si mismo -- un trozo perdido produce trozos que tambien pueden
 * perderse -- y las dos formas de estar equivocado son calladas las dos:
 *
 *      no termina nunca      una particion que en realidad no encoge nada sigue
 *                            produciendo trabajo para siempre, y el barrido parece
 *                            ocupado
 *      nombra la candidata   la biseccion converge en una candidata que no es la
 *      equivocada            que mato al trabajador, y el informe es una mentira
 *                            plausible sobre que bytes son peligrosos
 *
 * Las dos se comprueban aqui SIMULANDO una candidata letal: se declara mortal una
 * cadena de bytes elegida, y despues se conduce el descenso exactamente como lo
 * conduciria el padre -- refinar lo que contenga la mortal, descartar el resto --
 * hasta que nombre algo.  Lo que nombre tiene que ser la cadena que se eligio, y
 * tiene que llegar ahi en un numero acotado de pasos.
 *
 * Y eso es el aparato entero probado sin un solo proceso, que es la razon de que el
 * refinamiento viva en `common/` y no dentro del grupo de trabajadores.
 */

#include "isa/queue.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

static void check(int cond, const char *what) {
    if (!cond) {
        printf("FAIL: %s\n", what);
        failures += 1;
    }
}

/**
 * @brief
 * \~english Whether a chosen deadly candidate falls inside this item.
 * \~spanish Si una candidata mortal elegida cae dentro de este trozo.
 * \~
 *
 * \~english
 * It is what a worker's death means, expressed as arithmetic: the item covers every
 * candidate whose first `fixed` bytes are the frozen prefix and whose next byte is
 * within `lo .. hi`.  The bytes past that are not constrained, because the item
 * covers all of them.
 *
 * \~spanish
 * Es lo que significa la muerte de un trabajador, puesto como aritmetica: el trozo
 * cubre toda candidata cuyos primeros `fixed` bytes sean el prefijo congelado y cuyo
 * byte siguiente este entre `lo` y `hi`.  Los bytes de mas alla no estan sujetos,
 * porque el trozo los cubre todos.
 */
static int item_covers(const isa_work *w, const u8 *deadly) {
    u32 i;
    for (i = 0; i < (u32)w->fixed; ++i) {
        if (w->prefix[i] != deadly[i]) {
            return 0;
        }
    }
    return (deadly[w->fixed] >= w->lo && deadly[w->fixed] <= w->hi) ? 1 : 0;
}

/**
 * @brief
 * \~english Drives the descent the way the parent does, and says what it named.
 * \~spanish Conduce el descenso como lo hace el padre, y dice que nombro.
 * \~
 *
 * @return \~english how many refinements it took; 0 means it never named anything \~spanish cuantos refinamientos costo; 0 quiere decir que no nombro nada \~
 *
 * \~english
 * THE STEP LIMIT IS THE TEST, not a safety net.  A refinement that fails to shrink
 * its item would spin here for as long as anybody let it, and the bound is what
 * turns "it hangs" into a failure with a number attached.  `8 * depth` is what the
 * header claims; the limit here is generous against that so a real regression shows
 * up as exceeding it rather than as a flaky pass.
 *
 * \~spanish
 * EL LIMITE DE PASOS ES LA PRUEBA, no una red de seguridad.  Un refinamiento que no
 * consiga encoger su trozo daria vueltas aqui mientras alguien lo dejara, y la cota
 * es lo que convierte "se cuelga" en un fallo con un numero al lado.  `8 * depth` es
 * lo que dice la cabecera; el limite de aqui es generoso frente a eso para que una
 * regresion de verdad salga como pasarse y no como un aprobado con suerte.
 */
static u32 descend(u32 depth, u32 chunk, const u8 *deadly, u8 *named,
                   const char *what) {
    isa_queue q;
    u32 steps = 0;
    const u32 limit = 64u * (u32)ISA_MAX_LEN;

    isa_queue_open(&q);
    if (isa_queue_seed(&q, 0, 0, depth, chunk) != OK) {
        check(0, what);
        return 0;
    }

    for (;;) {
        isa_work item;
        isa_work single;

        if (!isa_queue_pop(&q, &item)) {
            /* La cola se vacio sin nombrar nada: el descenso perdio a la mortal. */
            return 0;
        }
        /* Un trozo que no la contiene lo barre un trabajador y vuelve vivo, asi que
         * sale de la cola y no vuelve.  Eso es lo que hace el padre. */
        if (!item_covers(&item, deadly)) {
            continue;
        }
        steps += 1u;
        if (steps > limit) {
            check(0, what);
            printf("  el descenso paso de %u pasos sin nombrar nada\n", limit);
            return 0;
        }
        if (isa_queue_refine(&q, &item, &single)) {
            memcpy(named, single.prefix, ISA_MAX_LEN);
            return steps;
        }
    }
}

/** @brief
 *  \~english Prints a candidate as bytes, which is how a finding gets read.
 *  \~spanish Imprime una candidata como bytes, que es como se lee un hallazgo. \~ */
static void put_bytes(const u8 *b, u32 n) {
    u32 i;
    for (i = 0; i < n; ++i) {
        printf("%02X ", b[i]);
    }
}

int main(void) {
    setvbuf(stdout, 0, _IONBF, 0);

    /*
     * El reparto inicial, que es lo unico que la cola hace antes de que algo muera.
     */
    printf("--- el reparto inicial ---\n");
    {
        isa_queue q;
        isa_work w;
        u32 seen = 0;

        isa_queue_open(&q);
        check(isa_queue_seed(&q, 0, 0, 3u, 1u) == OK, "siembra de uno en uno");
        check(q.count == 256u, "256 trozos, uno por primer byte");
        check(q.dropped == 0, "y ninguno perdido");
        while (isa_queue_pop(&q, &w)) {
            seen += 1u;
            if (w.lo != w.hi || w.fixed != 0 || w.depth != 3u) {
                check(0, "cada trozo es un primer byte a profundidad tres");
                break;
            }
        }
        check(seen == 256u, "y salen los 256");

        isa_queue_open(&q);
        check(isa_queue_seed(&q, 0, 0, 3u, 16u) == OK, "siembra de 16 en 16");
        check(q.count == 16u, "16 trozos de 16 bytes cada uno");

        /* Y bajo un prefijo, que es como se apunta el modo exhaustivo a una region:
         * los bytes congelados tienen que llegar a cada trozo. */
        {
            u8 pre[2];
            isa_work under;
            int all = 1;
            pre[0] = 0x0Fu;
            pre[1] = 0x38u;
            isa_queue_open(&q);
            check(isa_queue_seed(&q, pre, 2u, 3u, 1u) == OK,
                  "siembra bajo 0F 38");
            check(q.count == 256u, "256 trozos bajo el prefijo");
            while (isa_queue_pop(&q, &under)) {
                if (under.fixed != 2u || under.prefix[0] != 0x0Fu ||
                    under.prefix[1] != 0x38u) {
                    all = 0;
                }
            }
            check(all, "y todos llevan el prefijo congelado");
            /* Congelar tanto como la profundidad no deja nada que barrer, igual que
             * al abrir un recorrido: tiene que salir error y no una cola a medias. */
            isa_queue_open(&q);
            check(isa_queue_seed(&q, pre, 2u, 2u, 1u) == ERR_INVALID,
                  "congelar la profundidad entera se rechaza");
            check(isa_queue_seed(&q, 0, 2u, 3u, 1u) == ERR_INVALID,
                  "un prefijo nulo con fixed no nulo se rechaza");
            check(q.count == 0, "y la cola sigue vacia");
        }

        /* Errores de uso: un reparto sin sentido no puede salir OK y dejar la cola a
         * medias, porque eso es un barrido con un agujero del tamano de lo que
         * falte. */
        isa_queue_open(&q);
        check(isa_queue_seed(&q, 0, 0, 0u, 1u) == ERR_INVALID, "profundidad cero");
        check(isa_queue_seed(&q, 0, 0, 3u, 0u) == ERR_INVALID, "trozo cero");
        check(isa_queue_seed(&q, 0, 0, 3u, 257u) == ERR_INVALID,
              "trozo mayor que 256");
        check(q.count == 0, "y la cola sigue vacia");
    }

    /*
     * EL DESCENSO, que es para lo que existe todo esto.  Se declara mortal una
     * candidata y se comprueba que la biseccion acaba diciendo exactamente esa.
     *
     * Las dos primeras son las que mataron de verdad al barredor en la maquina, asi
     * que no son un ejemplo inventado: `01 C4` es `add esp, eax` y `73 FE` es
     * `jae -2`.
     */
    printf("\n--- el descenso nombra a la culpable ---\n");
    {
        static const struct {
            u8 deadly[ISA_MAX_LEN];
            u32 depth;
            u32 chunk;
            const char *what;
        } cases[] = {
            {{0x01u, 0xC4u}, 2u, 1u, "01 C4 con trozos de uno"},
            {{0x01u, 0xC4u}, 2u, 16u, "01 C4 con trozos de 16"},
            {{0x01u, 0xC4u}, 2u, 256u, "01 C4 con la cola entera de un trozo"},
            {{0x73u, 0xFEu}, 2u, 1u, "73 FE"},
            {{0x0Fu, 0x38u, 0xF6u}, 3u, 1u, "0F 38 F6 a profundidad tres"},
            {{0x00u, 0x00u, 0x00u}, 3u, 256u, "el primero de todos"},
            {{0xFFu, 0xFFu, 0xFFu}, 3u, 256u, "y el ultimo"},
        };
        u32 c;

        for (c = 0; c < sizeof(cases) / sizeof(cases[0]); ++c) {
            u8 named[ISA_MAX_LEN];
            u32 steps;

            memset(named, 0xAAu, sizeof(named));
            steps = descend(cases[c].depth, cases[c].chunk, cases[c].deadly,
                            named, cases[c].what);
            printf("  %-40s ", cases[c].what);
            if (steps == 0) {
                printf("NO LA NOMBRO\n");
                check(0, cases[c].what);
                continue;
            }
            put_bytes(named, cases[c].depth);
            printf("en %u pasos\n", steps);
            check(memcmp(named, cases[c].deadly, cases[c].depth) == 0,
                  cases[c].what);
        }
    }

    /*
     * Y LA COTA, que es la otra mitad de lo que hay que comprobar.  Que nombre bien
     * no sirve si para nombrar hace falta medio barrido: el descenso tiene que ser
     * logaritmico en el rango y lineal en la profundidad, `9 * depth` -- ocho
     * particiones por nivel mas la transicion, derivado en la cabecera.
     */
    printf("\n--- y cuesta lo que dice costar ---\n");
    {
        u8 named[ISA_MAX_LEN];
        u8 deadly[ISA_MAX_LEN];
        u32 steps;
        u32 worst = 0;
        u32 v;

        memset(deadly, 0, sizeof(deadly));
        /* Se prueban los 256 valores del segundo byte con el primero fijo, que es
         * el rango entero de una biseccion, y se mira el PEOR caso: un promedio
         * esconderia justo el que duele. */
        for (v = 0; v < 256u; ++v) {
            deadly[0] = 0x0Fu;
            deadly[1] = (u8)v;
            steps = descend(2u, 256u, deadly, named, "cota");
            if (steps == 0) {
                check(0, "la cota: no nombro");
                break;
            }
            if (memcmp(named, deadly, 2u) != 0) {
                check(0, "la cota: nombro otra");
                break;
            }
            if (steps > worst) {
                worst = steps;
            }
        }
        printf("  el peor de los 256 segundos bytes: %u pasos\n", worst);
        check(worst <= 9u * 2u, "y no pasa de 9 por la profundidad");
    }

    if (failures == 0) {
        printf("\nOK\n");
        return 0;
    }
    printf("\n%d fallos\n", failures);
    return 1;
}
