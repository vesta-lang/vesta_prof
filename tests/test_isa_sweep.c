/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_isa_sweep.c
 * @brief
 * \~english The sweep, driven by an oracle that answers whatever is convenient.
 * \~spanish El barrido, conducido por un oraculo que responde lo que convenga.
 * \~
 *
 * \~english
 * WHAT IS BEING CHECKED, and none of it needs a processor.  The sweep is a loop
 * around two things that are already tested on their own, so what is left is what
 * only IT does: turning verdicts into a tally, and deciding what to feed back to
 * the walk.
 *
 * The second is where it could go wrong quietly.  A truncated attempt has NO
 * length -- the decoder wanted more bytes than there were and never said how many
 * -- so the only honest thing to feed back is "assume nothing", and then the walk
 * skips nothing.  If instead something made up went back, the traversal would move
 * by an amount nobody measured, and the sweep would come out with a plausible
 * total and a hole in it.
 *
 * And the tally has to keep "the candidate was bad" apart from "the apparatus
 * could not ask".  A sweep that counts its own failures as verdicts reports a
 * complete answer after measuring nothing, which is the one failure this whole
 * tree is built to avoid.
 *
 * \~spanish
 * QUE SE COMPRUEBA, y nada de ello necesita un procesador.  El barrido es un bucle
 * alrededor de dos cosas ya probadas por su cuenta, asi que lo que queda es lo que
 * hace solo EL: convertir veredictos en una cuenta, y decidir que devolverle al
 * recorrido.
 *
 * Lo segundo es donde podria equivocarse en silencio.  Un intento truncado NO
 * tiene longitud -- al decodificador le faltaban bytes y nunca dijo cuantos --, asi
 * que lo unico honesto que devolver es "no supongas nada", y entonces el recorrido
 * no salta nada.  Si en su lugar volviera algo inventado, el recorrido se moveria
 * una cantidad que nadie midio, y el barrido saldria con un total plausible y un
 * agujero dentro.
 *
 * Y la cuenta tiene que separar "la candidata era mala" de "el aparato no pudo
 * preguntar".  Un barrido que cuenta sus propios fallos como veredictos informa de
 * una respuesta completa despues de no haber medido nada, que es el unico modo de
 * fallar que este arbol entero esta construido para evitar.
 */

#include "isa/sweep.h"

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
 * \~english What a made-up oracle is told to answer, and how often it was asked.
 * \~spanish Lo que se le dice a un oraculo inventado que responda, y cuantas veces
 *           se le pregunto.
 */
typedef struct fake_oracle {
    u32 asked;      /**< \~english how many times it was called \~spanish cuantas veces se le llamo \~ */
    u32 fail_after; /**< \~english 0 never fails; otherwise fails on that call \~spanish 0 no falla nunca; si no, falla en esa llamada \~ */
} fake_oracle;

/**
 * @brief
 * \~english An oracle with the shape of a decoder: `0F` opens two bytes.
 * \~spanish Un oraculo con forma de decodificador: `0F` abre dos bytes.
 * \~
 *
 * \~english
 * It also answers `ISA_TRUNCATED` for one chosen prefix, which is the case with no
 * length and the one worth pinning down.
 *
 * \~spanish
 * Tambien responde `ISA_TRUNCATED` para un prefijo elegido, que es el caso sin
 * longitud y el que merece quedar sujeto.
 */
static status fake_measure(void *ctx, const u8 *bytes, isa_result *out) {
    fake_oracle *f = (fake_oracle *)ctx;

    f->asked += 1u;
    if (f->fail_after != 0 && f->asked >= f->fail_after) {
        /* \~english The apparatus failed.  Not a verdict about the candidate.
         * \~spanish Fallo el aparato.  No es un veredicto sobre la candidata. \~ */
        return ERR_STATE;
    }

    memset(out, 0, sizeof(*out));
    if (bytes[0] == 0x66u) {
        /* El prefijo que siempre se queda corto: sin longitud. */
        out->outcome = (u32)ISA_TRUNCATED;
        out->length = 0;
        return OK;
    }
    if (bytes[0] == 0x0Fu) {
        out->outcome = (u32)ISA_INVALID;
        out->length = 2u;
        return OK;
    }
    if (bytes[0] == 0xF4u) {
        out->outcome = (u32)ISA_PRIVILEGED;
        out->length = 1u;
        return OK;
    }
    out->outcome = (u32)ISA_RAN;
    out->length = 1u;
    return OK;
}

/** @brief
 *  \~english Fills a work item for one first byte at a given depth.
 *  \~spanish Rellena un trozo de trabajo para un primer byte a una profundidad. \~ */
static void work_for(isa_work *w, u8 lo, u8 hi, u8 depth) {
    memset(w, 0, sizeof(*w));
    w->fixed = 0;
    w->lo = lo;
    w->hi = hi;
    w->depth = depth;
}

int main(void) {
    setvbuf(stdout, 0, _IONBF, 0);

    /*
     * Un primer byte de una sola instruccion de un byte: una candidata y el
     * subarbol entero saltado.  Es la cota de abajo del barrido, y si esto no sale
     * no hay nada que mirar mas arriba.
     */
    printf("--- un byte, una candidata ---\n");
    {
        fake_oracle f;
        isa_probe_ops ops;
        isa_work work;
        isa_tally t;

        memset(&f, 0, sizeof(f));
        ops.measure = fake_measure;
        ops.ctx = &f;
        work_for(&work, 0x90u, 0x90u, 3u);

        check(isa_sweep(&ops, &work, &t) == OK, "el barrido de 90 sale");
        printf("  90 -> %llu probadas, %llu saltadas, %llu corrio\n",
               (unsigned long long)t.candidates, (unsigned long long)t.skipped,
               (unsigned long long)t.by_outcome[ISA_RAN]);
        check(t.candidates == 1u, "una sola candidata");
        check(t.by_outcome[ISA_RAN] == 1u, "y salio que corrio");
        check(t.by_length[1] == 1u, "con longitud uno");
        check(t.skipped == 256ull * 256ull - 1ull,
              "y se salto el resto del subarbol");
        check(t.candidates + t.skipped == 256ull * 256ull,
              "que junto es el subarbol entero");
        check(f.asked == 1u, "al oraculo se le pregunto una vez");
    }

    /*
     * El escape, donde el recorrido baja un nivel porque el oraculo dijo dos.
     */
    printf("\n--- el escape de dos bytes ---\n");
    {
        fake_oracle f;
        isa_probe_ops ops;
        isa_work work;
        isa_tally t;

        memset(&f, 0, sizeof(f));
        ops.measure = fake_measure;
        ops.ctx = &f;
        work_for(&work, 0x0Fu, 0x0Fu, 3u);

        check(isa_sweep(&ops, &work, &t) == OK, "el barrido de 0F sale");
        printf("  0F -> %llu probadas, %llu no existen\n",
               (unsigned long long)t.candidates,
               (unsigned long long)t.by_outcome[ISA_INVALID]);
        check(t.candidates == 256u, "256 candidatas, una por segundo byte");
        check(t.by_outcome[ISA_INVALID] == 256u, "todas no existen");
        check(t.by_length[2] == 256u, "todas de longitud dos");
        check(t.candidates + t.skipped == 256ull * 256ull,
              "y el subarbol sigue cuadrando");
    }

    /*
     * EL CASO QUE IMPORTA.  Un intento truncado no trae longitud, asi que el
     * barrido no puede darle al recorrido un numero inventado: le da "no supongas
     * nada", y eso hace que se pruebe el byte siguiente hacia dentro en vez de
     * saltarse un subarbol que nadie midio.
     *
     * A profundidad tres eso son las 65.536 candidatas del subarbol enteras: es
     * caro a proposito, porque lo barato seria mentir.
     */
    printf("\n--- el truncado, que no trae longitud ---\n");
    {
        fake_oracle f;
        isa_probe_ops ops;
        isa_work work;
        isa_tally t;

        memset(&f, 0, sizeof(f));
        ops.measure = fake_measure;
        ops.ctx = &f;
        work_for(&work, 0x66u, 0x66u, 3u);

        check(isa_sweep(&ops, &work, &t) == OK, "el barrido de 66 sale");
        printf("  66 -> %llu probadas, %llu cortas, %llu saltadas\n",
               (unsigned long long)t.candidates,
               (unsigned long long)t.truncated, (unsigned long long)t.skipped);
        check(t.candidates == 256ull * 256ull,
              "sin longitud no se salta nada: el subarbol entero");
        check(t.truncated == 256ull * 256ull, "y todas contadas como cortas");
        check(t.skipped == 0, "no hubo nada que saltar");
        /* Y lo que NO debe pasar: una candidata sin longitud no puede acabar en el
         * histograma de longitudes, que seria inventar una medida. */
        {
            u32 i;
            u64 with_length = 0;
            for (i = 0; i <= ISA_MAX_LEN; ++i) {
                with_length += t.by_length[i];
            }
            check(with_length == 0,
                  "y ninguna entro en el histograma de longitudes");
        }
    }

    /*
     * Un fallo del APARATO no es un veredicto.  El barrido tiene que parar y
     * decirlo: seguir produciria un barrido cuyos huecos nadie puede ver.
     */
    printf("\n--- cuando falla el aparato, no la candidata ---\n");
    {
        fake_oracle f;
        isa_probe_ops ops;
        isa_work work;
        isa_tally t;

        memset(&f, 0, sizeof(f));
        f.fail_after = 5u; /* falla en la quinta pregunta */
        ops.measure = fake_measure;
        ops.ctx = &f;
        work_for(&work, 0x0Fu, 0x0Fu, 3u);

        check(isa_sweep(&ops, &work, &t) == ERR_STATE,
              "el fallo del oraculo sale hacia arriba");
        printf("  paro con %llu probadas de las 256\n",
               (unsigned long long)t.candidates);
        check(t.candidates == 4u, "y con lo que llevaba contado, no a cero");
        {
            u32 i;
            u64 verdicts = 0;
            for (i = 0; i < ISA_OUTCOME_COUNT; ++i) {
                verdicts += t.by_outcome[i];
            }
            check(verdicts == 4u, "el fallo no se conto como veredicto");
        }
    }

    /*
     * Y los errores de uso.  Una tabla de oraculo sin funcion es lo que queda si
     * alguien la declara y se olvida de rellenarla, y eso tiene que ser un error y
     * no una llamada a traves de un puntero nulo.
     */
    printf("\n--- errores de uso ---\n");
    {
        isa_probe_ops ops;
        isa_work work;
        isa_tally t;

        work_for(&work, 0x00u, 0xFFu, 3u);
        ops.measure = 0;
        ops.ctx = 0;
        check(isa_sweep(&ops, &work, &t) == ERR_INVALID,
              "una tabla sin funcion se rechaza");

        ops.measure = fake_measure;
        check(isa_sweep(0, &work, &t) == ERR_INVALID, "sin tabla se rechaza");
        check(isa_sweep(&ops, 0, &t) == ERR_INVALID, "sin trabajo se rechaza");
        check(isa_sweep(&ops, &work, 0) == ERR_INVALID, "sin cuenta se rechaza");

        /* Un trabajo que no describe un subarbol tiene que salir por el mismo
         * sitio que en `isa_walk_open`, no colarse hasta el bucle. */
        work_for(&work, 0x40u, 0x30u, 3u);
        {
            fake_oracle f;
            memset(&f, 0, sizeof(f));
            ops.ctx = &f;
            check(isa_sweep(&ops, &work, &t) == ERR_INVALID,
                  "un rango al reves se rechaza");
            check(f.asked == 0u, "y sin preguntarle nada al oraculo");
        }
    }

    if (failures == 0) {
        printf("\nOK\n");
        return 0;
    }
    printf("\n%d fallos\n", failures);
    return 1;
}
