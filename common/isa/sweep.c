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
    for (i = 0; i < ISA_OUTCOME_COUNT; ++i) {
        t->by_outcome[i] = 0;
    }
    for (i = 0; i <= ISA_MAX_LEN; ++i) {
        t->by_length[i] = 0;
    }
}

status isa_sweep(const isa_probe_ops *ops, const isa_work *work,
                 isa_tally *out) {
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
