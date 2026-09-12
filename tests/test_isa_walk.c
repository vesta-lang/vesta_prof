/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_isa_walk.c
 * @brief
 * \~english The walk, against decoders that are made up on purpose.
 * \~spanish El recorrido, contra decodificadores inventados a proposito.
 * \~
 *
 * \~english
 * WHY MADE-UP DECODERS ARE BETTER HERE THAN A REAL PROCESSOR.  What is being
 * checked is not whether the machine decodes `0F 38 F6` -- that is the oracle's
 * test -- but whether the traversal visits what it should and skips only what it
 * may.  Against real silicon neither is knowable in advance: you cannot tell "the
 * walk lost a region" from "the region did not exist".  Against a decoder whose
 * every answer was written here, both are arithmetic.
 *
 * THE THREE THINGS THAT WOULD RUIN THE SEARCH WITHOUT SAYING SO:
 *
 *      a repeat        the same candidate handed out twice is wasted work, and
 *                      if it can repeat once it can loop forever
 *      a hole          a candidate neither tried nor skipped is an instruction
 *                      the sweep reports as non-existent.  A false negative, and
 *                      silent
 *      a trespass      a candidate outside the owned subtree is two workers
 *                      answering the same question, and with core-class tagging
 *                      it is two DIFFERENT answers to it
 *
 * HOW THEY ARE CAUGHT, and it took two tries to get right.  Marking every
 * candidate the walk hands out does NOT work, and neither does marking the
 * interval between one candidate and the next: those intervals are contiguous by
 * construction -- each step starts where the previous one ended -- so they always
 * tile perfectly and a hole can never show up.  A check that cannot fail was
 * checking nothing, and a deliberately broken walk sailed through it.
 *
 * What actually decides it is the SUBTREE A STEP IS ENTITLED TO.  A measurement of
 * `c` consumed bytes justifies exactly one claim: "every candidate sharing my
 * first `c` bytes is this same instruction".  That claim covers one aligned block
 * and no more.  So two things get checked per step, and between them they pin the
 * walk down:
 *
 *      it starts where it left off     the candidate handed out is the first one
 *                                      not yet answered.  A hole would skip past
 *                                      it
 *      it does not overreach           what the step covers stays inside the
 *                                      block its measurement justifies.  Landing
 *                                      mid-block means the rest of that block was
 *                                      declared answered by a claim about a
 *                                      DIFFERENT block
 *
 * And the candidate counts, derived from each decoder rather than copied from a
 * first run, are what catches the complementary mistake: a walk that increments at
 * the wrong depth never leaves a hole, it just does far more or far less work than
 * the decoder shape calls for.
 *
 * \~spanish
 * POR QUE AQUI DECODIFICADORES INVENTADOS SON MEJORES QUE UN PROCESADOR DE
 * VERDAD.  Lo que se comprueba no es si la maquina decodifica `0F 38 F6` -- eso es
 * la prueba del oraculo -- sino si el recorrido visita lo que debe y se salta solo
 * lo que puede.  Contra silicio de verdad ninguna de las dos cosas se sabe por
 * adelantado: no se distingue "el recorrido perdio una region" de "la region no
 * existia".  Contra un decodificador cuyas respuestas se escribieron aqui, las dos
 * son aritmetica.
 *
 * LAS TRES COSAS QUE ARRUINARIAN LA BUSQUEDA SIN DECIRLO:
 *
 *      una repeticion  la misma candidata entregada dos veces es trabajo
 *                      perdido, y si puede repetirse una vez puede no acabar
 *      un agujero      una candidata ni probada ni saltada es una instruccion
 *                      que el barrido informa como inexistente.  Un falso
 *                      negativo, y callado
 *      una invasion    una candidata fuera del subarbol propio son dos
 *                      trabajadores respondiendo la misma pregunta, y con el
 *                      etiquetado por clase de nucleo son dos respuestas
 *                      DISTINTAS a ella
 *
 * COMO SE CAZAN, y costo dos intentos acertar.  Marcar cada candidata que el
 * recorrido entrega NO sirve, y tampoco marcar el intervalo entre una candidata y
 * la siguiente: esos intervalos son contiguos por construccion -- cada paso empieza
 * donde acabo el anterior --, asi que teselan siempre perfecto y un agujero no
 * puede aparecer jamas.  Una comprobacion que no puede fallar no comprobaba nada, y
 * un recorrido roto a proposito paso por ella sin enterarse.
 *
 * Lo que lo decide de verdad es EL SUBARBOL AL QUE UN PASO TIENE DERECHO.  Una
 * medida de `c` bytes consumidos justifica exactamente una afirmacion: "toda
 * candidata que comparta mis primeros `c` bytes es esta misma instruccion".  Esa
 * afirmacion cubre un bloque alineado y ni uno mas.  Asi que se comprueban dos
 * cosas por paso, y entre las dos dejan al recorrido sin sitio:
 *
 *      empieza donde lo dejo      la candidata que entrega es la primera que
 *                                 todavia no estaba respondida.  Un agujero se la
 *                                 saltaria
 *      no se pasa de su bloque    lo que el paso cubre se queda dentro del bloque
 *                                 que su medida justifica.  Caer a mitad de bloque
 *                                 quiere decir que el resto de ese bloque quedo
 *                                 declarado respondido por una afirmacion sobre
 *                                 OTRO bloque
 *
 * Y las cuentas de candidatas, derivadas de cada decodificador en vez de copiadas
 * de una primera corrida, son las que cazan el error complementario: un recorrido
 * que incrementa a la profundidad equivocada no deja ningun agujero, simplemente
 * hace mucho mas o mucho menos trabajo del que la forma del decodificador pide.
 */

#include "isa/walk.h"

#include <stdio.h>

static int failures = 0;

static void check(int cond, const char *what) {
    if (!cond) {
        printf("FAIL: %s\n", what);
        failures += 1;
    }
}

/* -------------------------------------------------------------------------- */
/*  Los decodificadores inventados                                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief
 * \~english Escape-shaped: how many bytes get read depends on the earlier bytes.
 * \~spanish Con forma de escape: cuantos bytes se leen depende de los bytes de
 *           antes.
 * \~
 *
 * \~english
 * It imitates the real thing in the one respect the walk cares about.  A
 * traversal that works against a fixed length proves nothing, because the whole
 * point is the length changing under it.
 *
 * \~spanish
 * Imita a lo de verdad en lo unico que le importa al recorrido.  Un recorrido que
 * funcione contra una longitud fija no demuestra nada, porque todo el asunto es
 * que la longitud cambie por debajo.
 */
static u32 escapes(const u8 *b) {
    if (b[0] == 0x0Fu) {
        if (b[1] == 0x38u || b[1] == 0x3Au) {
            return 3u;
        }
        return 2u;
    }
    if (b[0] >= 0x80u && b[0] <= 0x8Fu) {
        return 2u;
    }
    return 1u;
}

/** @brief
 *  \~english Always one byte: the most tunneling there is.
 *  \~spanish Siempre un byte: todo el tunneling que hay. \~ */
static u32 always_one(const u8 *b) {
    (void)b;
    return 1u;
}

/** @brief
 *  \~english Always reads everything: no tunneling at all.
 *  \~spanish Siempre lo lee todo: ningun tunneling. \~ */
static u32 always_three(const u8 *b) {
    (void)b;
    return 3u;
}

/**
 * @brief
 * \~english NOT MONOTONE: the length drops in the middle of a deep subtree.
 * \~spanish NO MONOTONO: la longitud baja en medio de un subarbol profundo.
 * \~
 *
 * \~english
 * A REAL DECODER CANNOT DO THIS, and that is exactly why it is here.  Once the
 * processor has read a byte, changing that byte cannot make it read fewer: so
 * the walk's increment point only ever moves deeper, and the carry -- which
 * zeroes as it climbs -- happens to leave the tail clean on its own.  Under a
 * monotone oracle the code that clears the tail is unreachable, and a test that
 * only uses monotone oracles cannot tell whether it is there.
 *
 * But the length does not come from a decoder, it comes from a MEASUREMENT: an
 * instruction that is not deterministic, or a page oracle that got one reading
 * wrong, reports exactly this.  And then the tail matters: without clearing it,
 * the walk jumps from `xx yy vv` to `xx yy+1 vv` and the 'vv' candidates below
 * are never tried and never counted.  A hole, silent, in the middle of the sweep.
 *
 * \~spanish
 * UN DECODIFICADOR DE VERDAD NO PUEDE HACER ESTO, y justo por eso esta aqui.  Una
 * vez que el procesador ha leido un byte, cambiar ese byte no puede hacer que lea
 * menos: asi que el punto de incremento del recorrido solo se mueve hacia
 * adentro, y el acarreo -- que pone a cero al subir -- deja la cola limpia por su
 * cuenta.  Con un oraculo monotono el codigo que limpia la cola es inalcanzable, y
 * una prueba que solo use oraculos monotonos no puede saber si esta.
 *
 * Pero la longitud no viene de un decodificador, viene de una MEDIDA: una
 * instruccion no determinista, o un oraculo de pagina que se equivoco en una
 * lectura, informa exactamente de esto.  Y entonces la cola importa: sin
 * limpiarla, el recorrido salta de `xx yy vv` a `xx yy+1 vv` y las candidatas por
 * debajo de 'vv' no se prueban nunca ni se cuentan nunca.  Un agujero, callado, en
 * medio del barrido.
 */
static u32 jumpy(const u8 *b) {
    if (b[0] == 0x0Fu && b[1] == 0x38u) {
        /* Lee tres hasta la mitad del tercer byte, y despues solo dos. */
        return (b[2] < 0x80u) ? 3u : 2u;
    }
    return escapes(b);
}

/* -------------------------------------------------------------------------- */
/*  El teselado                                                               */
/* -------------------------------------------------------------------------- */

/** @brief
 *  \~english The first `depth` bytes as one number, which is what has to grow.
 *  \~spanish Los primeros `depth` bytes como un numero, que es lo que tiene que
 *            crecer. \~ */
static u64 as_number(const u8 *b, u32 depth) {
    u64 n = 0;
    u32 i;
    for (i = 0; i < depth; ++i) {
        n = (n << 8) | (u64)b[i];
    }
    return n;
}

/** \~english The whole space at depth three.
 *  \~spanish El espacio entero a profundidad tres. \~ */
#define SPACE_3 (256ull * 256ull * 256ull)

/**
 * @brief
 * \~english How big the block is that a measurement of `consumed` justifies.
 * \~spanish Lo grande que es el bloque que justifica una medida de `consumed`.
 * \~
 *
 * \~english
 * It repeats the walk's own choice of increment point, and that is deliberate: the
 * point of this is not to second-guess where the walk increments -- the candidate
 * counts do that -- but to hold it to the claim that choice IMPLIES.  Given that
 * the walk answers at depth `i`, what it may declare answered is the aligned block
 * of `256^(depth-1-i)` candidates around it, and nothing else.
 *
 * \~spanish
 * Repite la eleccion de punto de incremento del propio recorrido, y es a
 * proposito: lo que esto hace no es dudar de donde incrementa -- de eso se encargan
 * las cuentas de candidatas -- sino sujetarlo a la afirmacion que esa eleccion
 * IMPLICA.  Dado que el recorrido responde a profundidad `i`, lo que puede declarar
 * respondido es el bloque alineado de `256^(depth-1-i)` candidatas a su alrededor, y
 * nada mas.
 */
static u64 block_of(u32 consumed, u32 depth, u32 fixed) {
    u64 size = 1;
    u32 i = (consumed < depth) ? consumed : depth;
    u32 k;
    if (i == 0) {
        i = 1;
    }
    i -= 1;
    if (i < fixed) {
        i = fixed;
    }
    for (k = depth - 1u - i; k > 0; --k) {
        size *= 256u;
    }
    return size;
}

/**
 * @brief
 * \~english Runs a whole walk, holding every step to what it is entitled to.
 * \~spanish Corre un recorrido entero, sujetando cada paso a lo que le
 *           corresponde.
 * \~
 *
 * @param owned_lo \~english the first candidate of the owned subtree \~spanish la primera candidata del subarbol propio \~
 * @param owned_end \~english one past the last \~spanish una mas alla de la ultima \~
 * @return \~english how many candidates were handed out \~spanish cuantas candidatas se entregaron \~
 *
 * \~english
 * THE FRONTIER IS THE WHOLE IDEA.  It is the first candidate not yet answered, it
 * starts at the bottom of the owned subtree and it has to arrive exactly at the
 * top.  A step that hands out anything other than the frontier has either gone
 * backwards -- so it could loop -- or jumped over candidates nobody answered.
 *
 * \~spanish
 * LA FRONTERA ES TODA LA IDEA.  Es la primera candidata que todavia no esta
 * respondida, arranca en el fondo del subarbol propio y tiene que llegar
 * exactamente al techo.  Un paso que entregue cualquier cosa que no sea la
 * frontera o ha ido hacia atras -- asi que podria dar vueltas -- o se ha saltado
 * candidatas que nadie respondio.
 */
static u64 run(isa_walk *w, u64 owned_lo, u64 owned_end,
               u32 (*decoder)(const u8 *), const char *what) {
    u64 count = 0;
    u64 frontier = owned_lo;
    u32 depth = (u32)w->depth;

    for (;;) {
        u64 here = as_number(w->bytes, depth);
        u64 size;
        u64 block_end;
        u64 next;
        int more;
        u32 consumed;

        if (here != frontier) {
            check(0, what);
            printf("  entrego %llu con la frontera en %llu\n",
                   (unsigned long long)here, (unsigned long long)frontier);
            return count;
        }
        count += 1;

        consumed = decoder(w->bytes);
        size = block_of(consumed, depth, (u32)w->fixed);
        block_end = (here - (here % size)) + size;

        more = isa_walk_step(w, consumed);
        next = more ? as_number(w->bytes, depth) : owned_end;

        if (next <= here) {
            check(0, what);
            printf("  el numero no crecio: %llu tras %llu\n",
                   (unsigned long long)next, (unsigned long long)here);
            return count;
        }
        /* \~english Beyond the block, the step is declaring answered what its own
         * measurement says nothing about.  \~spanish Mas alla del bloque, el paso
         * declara respondido algo de lo que su propia medida no dice nada. \~ */
        if (next > block_end) {
            check(0, what);
            printf("  desde %llu salto a %llu, y su bloque acaba en %llu\n",
                   (unsigned long long)here, (unsigned long long)next,
                   (unsigned long long)block_end);
            return count;
        }
        frontier = next;
        if (!more) {
            break;
        }
    }
    if (frontier != owned_end) {
        check(0, what);
        printf("  la frontera acabo en %llu y el subarbol en %llu\n",
               (unsigned long long)frontier, (unsigned long long)owned_end);
    }
    return count;
}

int main(void) {
    setvbuf(stdout, 0, _IONBF, 0);

    /*
     * Las dos cotas, que es donde el tunneling se ve o no se ve.  Un
     * decodificador que lo lee todo tiene que dar el barrido COMPLETO, y uno que
     * lee un byte tiene que dar 256 candidatas y nada mas: si el primero salta
     * algo hay un agujero, y si el segundo no salta, el tunneling no sirve de
     * nada.
     */
    printf("--- las dos cotas: todo leido, y un solo byte ---\n");
    {
        isa_walk w;
        u64 n;

        check(isa_walk_open(&w, 0, 0, 0x00u, 0xFFu, 3u) == OK, "abre el completo");
        n = run(&w, 0, SPACE_3, always_three, "el barrido completo");
        printf("  todo leido    %llu probadas, %llu saltadas\n",
               (unsigned long long)n, (unsigned long long)w.skipped);
        check(n == SPACE_3, "el completo son 256^3");
        check(w.skipped == 0, "y no se salta ni una");

        check(isa_walk_open(&w, 0, 0, 0x00u, 0xFFu, 3u) == OK, "abre el de uno");
        n = run(&w, 0, SPACE_3, always_one, "el barrido con tunneling");
        printf("  un solo byte  %llu probadas, %llu saltadas\n",
               (unsigned long long)n, (unsigned long long)w.skipped);
        check(n == 256ull, "con un byte leido son 256 candidatas");
        check(n + w.skipped == SPACE_3, "y probadas mas saltadas es el espacio");
    }

    /*
     * El decodificador con forma de verdad.  La cuenta se DERIVA de lo que el
     * decodificador dice, no se observa de lo que el recorrido hizo: apuntar el
     * numero que salio la primera vez lo unico que comprueba es que el codigo no
     * ha cambiado.
     *
     *   0F 38 xx / 0F 3A xx   lee 3 -> se enumera el tercero    2 x 256
     *   0F yy, el resto       lee 2 -> el tercero se salta      254
     *   80..8F yy             lee 2 -> se enumera el segundo   16 x 256
     *   los otros 239         lee 1 -> se saltan los dos       239
     */
    printf("\n--- con escapes, que es donde la longitud cambia ---\n");
    {
        isa_walk w;
        u64 n;

        check(isa_walk_open(&w, 0, 0, 0x00u, 0xFFu, 3u) == OK, "abre con escapes");
        n = run(&w, 0, SPACE_3, escapes, "el barrido con escapes");
        printf("  %llu probadas, %llu saltadas\n", (unsigned long long)n,
               (unsigned long long)w.skipped);
        check(n == (2ull * 256ull) + 254ull + (16ull * 256ull) + 239ull,
              "la cuenta sale de lo que el decodificador dice");
        check(n + w.skipped == SPACE_3, "y probadas mas saltadas es el espacio");
    }

    /*
     * El reparto, que es la propiedad de correctitud del paralelismo.  El mapa de
     * bits COMPARTIDO entre los dos es lo que lo comprueba: si se pisaran, el
     * segundo encuentra puesto un bit del primero, y si dejaran algo en medio, el
     * hueco aparece al final.
     */
    printf("\n--- dos trabajadores, y ni un solapamiento ---\n");
    {
        isa_walk a;
        isa_walk b;
        u64 na;
        u64 nb;

        check(isa_walk_open(&a, 0, 0, 0x00u, 0x7Fu, 3u) == OK, "abre la mitad baja");
        check(isa_walk_open(&b, 0, 0, 0x80u, 0xFFu, 3u) == OK, "abre la mitad alta");
        /* Cada una con SU tramo, y el techo de la baja es el suelo de la alta: si
         * se solaparan o dejaran algo en medio, una de las dos fronteras no
         * llegaria a su sitio. */
        na = run(&a, 0, SPACE_3 / 2ull, escapes, "la mitad baja");
        nb = run(&b, SPACE_3 / 2ull, SPACE_3, escapes, "la mitad alta");
        printf("  baja %llu, alta %llu, juntas %llu\n", (unsigned long long)na,
               (unsigned long long)nb, (unsigned long long)(na + nb));
        check(na + nb == (2ull * 256ull) + 254ull + (16ull * 256ull) + 239ull,
              "las dos mitades cubren lo mismo que el barrido entero");
        check(na + a.skipped == SPACE_3 / 2ull, "la baja tesela su mitad");
        check(nb + b.skipped == SPACE_3 / 2ull, "la alta tesela la suya");
    }

    /*
     * El prefijo congelado, que es lo que reparte la cola de trabajo cuando el
     * reparto por primer byte sale desequilibrado.  Lo que hay que comprobar no es
     * cuantas salen, sino que los bytes congelados NO SE MUEVEN: si se movieran, el
     * trabajador estaria contestando por un subarbol que no es suyo y nadie se
     * enteraria.
     */
    printf("\n--- el prefijo congelado ---\n");
    {
        isa_walk w;
        u8 prefix[2];
        u64 n = 0;
        int moved = 0;

        prefix[0] = 0x0Fu;
        prefix[1] = 0x38u;
        check(isa_walk_open(&w, prefix, 2u, 0x00u, 0xFFu, 3u) == OK,
              "abre el prefijo 0F 38");
        for (;;) {
            if (w.bytes[0] != 0x0Fu || w.bytes[1] != 0x38u) {
                moved = 1;
            }
            n += 1;
            if (!isa_walk_step(&w, escapes(w.bytes))) {
                break;
            }
        }
        printf("  0F 38 xx -> %llu candidatas\n", (unsigned long long)n);
        check(!moved, "los dos bytes congelados no se mueven nunca");
        check(n == 256ull, "y se recorre el tercero entero");
        check(n + w.skipped == 256ull, "que es el subarbol propio, entero");
    }

    /*
     * Llegar a algo que vive PROFUNDO, que es la validacion que pide el diseno.
     * Un recorrido que funcione en la superficie y no baje no se distingue de uno
     * que si, salvo por lo que NO encuentra.
     */
    printf("\n--- llegar a algo profundo ---\n");
    {
        isa_walk w;
        int found = 0;
        u64 at = 0;
        u64 n = 0;

        check(isa_walk_open(&w, 0, 0, 0x00u, 0xFFu, 3u) == OK, "abre el barrido");
        for (;;) {
            n += 1;
            if (w.bytes[0] == 0x0Fu && w.bytes[1] == 0x38u &&
                w.bytes[2] == 0xF6u) {
                found = 1;
                at = n;
            }
            if (!isa_walk_step(&w, escapes(w.bytes))) {
                break;
            }
        }
        printf("  0F 38 F6 aparece en la candidata %llu de %llu\n",
               (unsigned long long)at, (unsigned long long)n);
        check(found, "el recorrido alcanza 0F 38 F6");
    }

    /*
     * EL ORACULO QUE NO ES MONOTONO, que es lo que ningun decodificador de verdad
     * hace y cualquier MEDIDA puede hacer.  Aqui la longitud baja de tres a dos en
     * medio del tercer byte, asi que el recorrido tiene que incrementar el segundo
     * teniendo el tercero a medias.
     *
     * Lo que se comprueba es que NO HAY AGUJERO: sin poner a cero la cola, el
     * recorrido saltaria de `0F 38 80` a `0F 39 80` y las candidatas `0F 39 00` a
     * `0F 39 7F` no se probarian ni se contarian, calladas.
     *
     * Y una propiedad que se PIERDE, que merece quedar dicha: el ahorro deja de
     * cuadrar.  Un salto desde una cola a medias declara respondido el subarbol
     * entero de debajo cuando parte ya se habia probado una a una, asi que
     * `skipped` cuenta de MAS.  El teselado sigue siendo exacto -- el intervalo
     * entre dos candidatas es el que es --, y por eso es el teselado y no la suma
     * lo que vale como comprobacion.
     */
    printf("\n--- un oraculo que no es monotono ---\n");
    {
        isa_walk w;
        u64 n;

        check(isa_walk_open(&w, 0, 0, 0x00u, 0xFFu, 3u) == OK, "abre el no monotono");
        n = run(&w, 0, SPACE_3, jumpy, "el barrido no monotono");
        printf("  %llu probadas, %llu saltadas, y el espacio son %llu\n",
               (unsigned long long)n, (unsigned long long)w.skipped,
               (unsigned long long)SPACE_3);
        check(n + w.skipped >= SPACE_3,
              "el ahorro cuenta de mas, nunca de menos");
    }

    /*
     * Y los errores de uso.  Congelar tantos bytes como se enumeran no deja nada
     * que recorrer: si eso devolviera una candidata sola en vez de un error, un
     * trabajador mal configurado informaria de un subarbol vacio y el barrido
     * saldria con un agujero del tamano de ese subarbol.
     */
    printf("\n--- errores de uso ---\n");
    {
        isa_walk w;
        u8 prefix[3];

        prefix[0] = 0x0Fu;
        prefix[1] = 0x38u;
        prefix[2] = 0xF6u;
        check(isa_walk_open(&w, prefix, 3u, 0x00u, 0xFFu, 3u) == ERR_INVALID,
              "congelar la profundidad entera se rechaza");
        check(isa_walk_open(&w, 0, 0, 0x40u, 0x30u, 3u) == ERR_INVALID,
              "un rango al reves se rechaza");
        check(isa_walk_open(&w, 0, 0, 0x00u, 0xFFu, 0u) == ERR_INVALID,
              "profundidad cero se rechaza");
        check(isa_walk_open(&w, 0, 0, 0x00u, 0xFFu, ISA_WALK_DEPTH_MAX + 1u) ==
                      ERR_INVALID,
              "pasarse del techo se rechaza");
        check(isa_walk_open(&w, 0, 2u, 0x00u, 0xFFu, 3u) == ERR_INVALID,
              "un prefijo nulo con fixed no nulo se rechaza");
        /* Un recorrido agotado se queda agotado: un trabajador que siga llamando
         * no debe volver a recibir candidatas. */
        check(isa_walk_open(&w, 0, 0, 0x10u, 0x10u, 1u) == OK,
              "abre uno de un byte");
        check(isa_walk_step(&w, 1u) == 0, "un rango de uno se agota en un paso");
        check(isa_walk_step(&w, 1u) == 0, "y sigue agotado");
    }

    if (failures == 0) {
        printf("\nOK\n");
        return 0;
    }
    printf("\n%d fallos\n", failures);
    return 1;
}
