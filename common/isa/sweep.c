/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/sweep.c
 * @brief
 * \~english The loop: ask, write down, move on.
 * \~spanish El bucle: preguntar, apuntar, avanzar.
 * \~
 *
 * \~english
 * It is short because the two hard parts are elsewhere -- the traversal in
 * `walk.c` and the apparatus that executes bytes in the system layer.  What is
 * here is the join and the bookkeeping, and the reasoning for the shape is in
 * `sweep.h`.
 *
 * \~spanish
 * Es corto porque las dos partes dificiles estan en otro sitio -- el recorrido en
 * `walk.c` y el aparato que ejecuta bytes en la capa del sistema.  Lo que hay aqui
 * es la union y la contabilidad, y el razonamiento de la forma esta en `sweep.h`.
 */

#include "isa/sweep.h"

/*
 * \~english The histogram's size against the enum it indexes.  `ISA_RETURNED` is
 * the last outcome, so the count is its value plus one; adding an outcome without
 * touching `ISA_OUTCOME_COUNT` stops the build here instead of silently counting
 * it as somebody else.
 *
 * \~spanish El tamano del histograma contra el enum que indexa.  `ISA_RETURNED` es
 * el ultimo resultado, asi que la cuenta es su valor mas uno; anadir un resultado
 * sin tocar `ISA_OUTCOME_COUNT` para la construccion aqui en vez de contarlo
 * calladamente como si fuera otro.
 */
STATIC_ASSERT(ISA_OUTCOME_COUNT == (u32)ISA_RETURNED + 1u,
              "ISA_OUTCOME_COUNT must match the outcome enum in trial.h");

/** @brief
 *  \~english Zeroes a tally.  Every field, so a reused one cannot carry over.
 *  \~spanish Pone a cero una cuenta.  Todos los campos, para que una reutilizada
 *            no arrastre nada. \~ */
static void tally_clear(isa_tally *t) {
    u32 i;
    t->candidates = 0;
    t->skipped = 0;
    t->truncated = 0;
    t->agree = 0;
    t->conflict = 0;
    t->only_cpu = 0;
    t->only_ref = 0;
    for (i = 0; i < ISA_OUTCOME_COUNT; ++i) {
        t->by_outcome[i] = 0;
    }
    for (i = 0; i <= ISA_MAX_LEN; ++i) {
        t->by_length[i] = 0;
    }
}

/**
 * @brief
 * \~english Puts one candidate in one of the four quadrants.
 * \~spanish Coloca una candidata en uno de los cuatro cuadrantes.
 * \~
 *
 * \~english
 * WHAT COUNTS AS "THE SILICON HAS IT", and getting this wrong would invent findings by
 * the thousand.  `ISA_INVALID` is #UD: the processor says it does not know these bytes,
 * so the answer is NO -- even though the sliding loop did measure how many bytes it
 * consumed before saying so.  Everything else that decoded -- ran, privileged, faulted
 * on memory, returned -- means the instruction EXISTS, whatever it then did.
 *
 * `ISA_TRUNCATED` is not compared at all: there is no answer at this length, so there
 * is nothing to disagree with.
 *
 * \~spanish
 * QUE CUENTA COMO "EL SILICIO LA TIENE", y equivocarse aqui inventaria hallazgos por
 * miles.  `ISA_INVALID` es #UD: el procesador dice que no conoce estos bytes, asi que la
 * respuesta es NO -- aunque el bucle deslizante si midiera cuantos bytes consumio antes
 * de decirlo.  Todo lo demas que decodifico -- corrio, privilegiada, fallo en memoria,
 * volvio -- quiere decir que la instruccion EXISTE, hiciera luego lo que hiciera.
 *
 * `ISA_TRUNCATED` no se compara: no hay respuesta a esta longitud, asi que no hay con
 * que discrepar.
 */
void isa_quadrant(isa_tally *t, const isa_result *r, u32 ref_len) {
    int cpu_has;
    u32 cpu_len;

    /* \~english Neither of these is a verdict: one says the instruction is longer than
     * what was offered, the other that nobody answered at all.  \~spanish Ninguno de los
     * dos es un veredicto: uno dice que la instruccion mide mas que lo que se ofrecio, el
     * otro que nadie respondio en absoluto. \~ */
    if (r->outcome == (u32)ISA_TRUNCATED || r->outcome == (u32)ISA_NOTHING) {
        return;
    }
    cpu_has = (r->outcome != (u32)ISA_INVALID) ? 1 : 0;
    cpu_len = r->length;

    /*
     * \~english NO LENGTH MEANS NO ANSWER, and that is the whole test -- not the outcome.
     * A missing length is not "length zero": comparing it against a reference that says
     * three produces a disagreement about nothing.
     *
     * It is not hypothetical.  `C2 60` is `ret 0x0060`, which returns to the landing label
     * AND leaves the stack pointer 96 bytes high; the flow resumes elsewhere in our own
     * code without faulting, so nobody fills the result in and it comes back as it was
     * zeroed.  That single candidate was the only "length conflict" in 39.744 comparisons.
     *
     * WHAT THIS DOES NOT REJECT, and rejecting it cost 66 good comparisons before the test
     * was narrowed to the length: an outcome of `ISA_UNKNOWN` that came from the
     * CLASSIFIER.  That one means a real exception nobody could place -- and the length is
     * perfectly good, because the sliding loop still stopped where it stopped.  The two
     * look alike in the outcome and differ in exactly this field.
     *
     * \~spanish SIN LONGITUD NO HAY RESPUESTA, y esa es toda la prueba -- no el resultado.
     * Una longitud que falta no es "longitud cero": compararla contra una referencia que
     * dice tres produce un desacuerdo sobre nada.
     *
     * No es hipotetico.  `C2 60` es `ret 0x0060`, que vuelve a la etiqueta de aterrizaje Y
     * deja el puntero de pila 96 bytes mas arriba; el flujo sigue en otro punto de nuestro
     * propio codigo sin fallar, asi que nadie rellena el resultado y vuelve tal como se
     * puso a cero.  Esa sola candidata fue el unico "conflicto de longitud" en 39.744
     * comparaciones.
     *
     * LO QUE ESTO NO RECHAZA, y rechazarlo costo 66 comparaciones buenas antes de
     * estrechar la prueba a la longitud: un resultado `ISA_UNKNOWN` que venga del
     * CLASIFICADOR.  Ese quiere decir una excepcion de verdad que nadie supo colocar -- y
     * la longitud es perfectamente buena, porque el bucle deslizante paro donde paro.  Los
     * dos se parecen en el resultado y se diferencian exactamente en este campo.
     */
    if (cpu_has && cpu_len == 0) {
        return;
    }

    if (cpu_has && ref_len != 0) {
        if (cpu_len == ref_len) {
            t->agree += 1u;
        } else {
            t->conflict += 1u;
        }
    } else if (cpu_has) {
        t->only_cpu += 1u;
    } else if (ref_len != 0) {
        t->only_ref += 1u;
    }
    /* Ni el uno ni la otra: los dos dicen que no existe, y eso es un acuerdo que no
     * hace falta contar -- lo que se busca son las discrepancias. */
}

status isa_sweep(const isa_probe_ops *ops, const isa_work *work,
                 const isa_ref *ref, isa_tally *out) {
    isa_walk w;
    status rc;

    if (ops == 0 || ops->measure == 0 || work == 0 || out == 0) {
        return ERR_INVALID;
    }
    tally_clear(out);

    rc = isa_walk_open(&w, work->prefix, (u32)work->fixed, work->lo, work->hi,
                       (u32)work->depth);
    if (rc != OK) {
        return rc;
    }

    for (;;) {
        isa_result r;
        u32 consumed;

        rc = ops->measure(ops->ctx, w.bytes, &r);
        if (rc != OK) {
            /* \~english The tally keeps what it had: how far it got before the
             * apparatus failed is worth more than a zeroed struct.
             * \~spanish La cuenta se queda con lo que tenia: hasta donde llego
             * antes de fallar el aparato vale mas que una estructura a cero. \~ */
            out->skipped = w.skipped;
            return rc;
        }

        out->candidates += 1u;
        if (r.outcome < ISA_OUTCOME_COUNT) {
            out->by_outcome[r.outcome] += 1u;
        }
        if (ref != 0) {
            isa_quadrant(out, &r, isa_ref_length(ref, w.bytes));
        }

        /*
         * \~english WHAT GETS FED BACK TO THE WALK IS A LENGTH, and there is only
         * one when something actually decoded.  A truncated attempt never told us
         * how long the instruction was -- only that it was longer than what was
         * there -- so `ISA_MAX_LEN` goes back, which makes the walk skip nothing
         * and try the next byte down.  Passing a made-up length here would move
         * the traversal by an amount nobody measured.
         *
         * \~spanish LO QUE SE LE DEVUELVE AL RECORRIDO ES UNA LONGITUD, y solo hay
         * una cuando algo llego a decodificar.  Un intento truncado nunca dijo
         * cuanto media la instruccion -- solo que media mas que lo que habia --,
         * asi que se devuelve `ISA_MAX_LEN`, que hace que el recorrido no salte
         * nada y pruebe el byte siguiente hacia dentro.  Pasar aqui una longitud
         * inventada moveria el recorrido una cantidad que nadie midio.
         */
        if (r.outcome == ISA_TRUNCATED) {
            out->truncated += 1u;
            consumed = ISA_MAX_LEN;
        } else if (r.length >= 1u && r.length <= ISA_MAX_LEN) {
            out->by_length[r.length] += 1u;
            consumed = r.length;
        } else {
            /* \~english No length and not truncated: a verdict that needed no
             * decoding, like returning on its own.  Same treatment -- assume
             * nothing.  \~spanish Sin longitud y sin truncar: un veredicto que no
             * necesito decodificar, como volver por su cuenta.  Mismo trato -- no
             * suponer nada. \~ */
            consumed = ISA_MAX_LEN;
        }

        if (!isa_walk_step(&w, consumed)) {
            break;
        }
    }

    out->skipped = w.skipped;
    return OK;
}
