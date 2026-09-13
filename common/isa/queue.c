/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/queue.c
 * @brief
 * \~english Pushing, popping, and the three shapes a refinement can take.
 * \~spanish Meter, sacar, y las tres formas que puede tomar un refinamiento.
 * \~
 *
 * \~english
 * The reasoning -- why a queue at all, why last in first out, why a lost item is a
 * finding -- is in `queue.h`.  Here is the arithmetic.
 *
 * \~spanish
 * El razonamiento -- por que una cola, por que el ultimo primero, por que un trozo
 * perdido es un hallazgo -- esta en `queue.h`.  Aqui esta la aritmetica.
 */

#include "isa/queue.h"

void isa_queue_open(isa_queue *q) {
    if (q == 0) {
        return;
    }
    q->count = 0;
    q->_pad = 0;
    q->pushed = 0;
    q->dropped = 0;
}

status isa_queue_push(isa_queue *q, const isa_work *w) {
    u32 i;

    if (q == 0 || w == 0) {
        return ERR_INVALID;
    }
    if (q->count >= ISA_QUEUE_CAP) {
        /* \~english Counted, not ignored: this is a region nobody will sweep.
         * \~spanish Contado, no ignorado: esta es una region que nadie va a
         * barrer. \~ */
        q->dropped += 1u;
        return ERR_NOSPACE;
    }
    q->items[q->count] = *w;
    /* \~english The prefix is copied whole rather than up to `fixed`, so two items
     * that describe the same subtree compare equal byte for byte.  It costs fifteen
     * bytes and it means a stale tail can never make an item look different from
     * itself.  \~spanish El prefijo se copia entero y no hasta `fixed`, asi que dos
     * trozos que describan el mismo subarbol se comparan iguales byte a byte.
     * Cuesta quince bytes y significa que una cola rancia no puede hacer que un
     * trozo parezca distinto de si mismo. \~ */
    for (i = (u32)w->fixed; i < ISA_MAX_LEN; ++i) {
        q->items[q->count].prefix[i] = 0;
    }
    q->count += 1u;
    q->pushed += 1u;
    return OK;
}

int isa_queue_pop(isa_queue *q, isa_work *out) {
    if (q == 0 || out == 0 || q->count == 0) {
        return 0;
    }
    q->count -= 1u;
    *out = q->items[q->count];
    return 1;
}

status isa_queue_seed(isa_queue *q, const u8 *prefix, u32 fixed, u32 depth,
                      u32 chunk) {
    u32 lo;

    if (q == 0 || depth < 1u || depth > ISA_WALK_DEPTH_MAX) {
        return ERR_INVALID;
    }
    if (chunk < 1u || chunk > 256u) {
        return ERR_INVALID;
    }
    /* \~english The same rule as opening a walk: freezing as many bytes as are
     * enumerated leaves nothing to sweep.  \~spanish La misma regla que abrir un
     * recorrido: congelar tantos bytes como se enumeran no deja nada que barrer. \~ */
    if (fixed >= depth) {
        return ERR_INVALID;
    }
    if (fixed != 0 && prefix == 0) {
        return ERR_INVALID;
    }

    for (lo = 0; lo < 256u; lo += chunk) {
        isa_work w;
        u32 hi = lo + chunk - 1u;
        u32 i;

        if (hi > 255u) {
            hi = 255u;
        }
        for (i = 0; i < ISA_MAX_LEN; ++i) {
            w.prefix[i] = 0;
        }
        for (i = 0; i < fixed; ++i) {
            w.prefix[i] = prefix[i];
        }
        w.fixed = (u8)fixed;
        w.lo = (u8)lo;
        w.hi = (u8)hi;
        w.depth = (u8)depth;
        if (isa_queue_push(q, &w) != OK) {
            return ERR_NOSPACE;
        }
    }
    return OK;
}

int isa_queue_refine(isa_queue *q, const isa_work *lost, isa_work *single) {
    isa_work a;
    isa_work b;
    u32 i;

    if (q == 0 || lost == 0 || single == 0) {
        return 0;
    }

    /*
     * \~english FIRST SHAPE: a range of more than one value halves.  Ordinary
     * bisection, and the cheap half of the descent -- it needs no extra depth and
     * each step throws away half the suspects.
     *
     * \~spanish PRIMERA FORMA: un rango de mas de un valor se parte por la mitad.
     * Biseccion normal, y la mitad barata del descenso -- no necesita profundidad de
     * mas y cada paso tira la mitad de los sospechosos.
     */
    if (lost->lo < lost->hi) {
        u32 mid = ((u32)lost->lo + (u32)lost->hi) / 2u;
        a = *lost;
        b = *lost;
        a.hi = (u8)mid;
        b.lo = (u8)(mid + 1u);
        /* \~english The upper half goes in first so the LOWER one comes out first:
         * the queue is last in, first out, and a sweep that reports its findings in
         * ascending order is a sweep somebody can read.  \~spanish La mitad alta
         * entra primero para que salga primero la BAJA: la cola saca el ultimo que
         * entro, y un barrido que informa en orden creciente es un barrido que
         * alguien puede leer. \~ */
        (void)isa_queue_push(q, &b);
        (void)isa_queue_push(q, &a);
        return 0;
    }

    /*
     * \~english SECOND SHAPE: one value left, and room to go deeper.  The value gets
     * frozen into the prefix and the byte after it opens up whole.  This is where
     * the descent stops being about which first byte and starts being about which
     * candidate.
     *
     * \~spanish SEGUNDA FORMA: queda un valor, y hay sitio para bajar.  El valor se
     * congela en el prefijo y el byte siguiente se abre entero.  Aqui es donde el
     * descenso deja de ir de que primer byte y empieza a ir de que candidata.
     */
    if ((u32)lost->fixed + 1u < (u32)lost->depth) {
        a = *lost;
        a.prefix[lost->fixed] = lost->lo;
        a.fixed = (u8)(lost->fixed + 1u);
        a.lo = 0x00u;
        a.hi = 0xFFu;
        for (i = (u32)a.fixed + 1u; i < ISA_MAX_LEN; ++i) {
            a.prefix[i] = 0;
        }
        (void)isa_queue_push(q, &a);
        return 0;
    }

    /*
     * \~english THIRD SHAPE: there is nothing left to split.  One frozen prefix plus
     * one value, with the depth used up, is a single candidate -- and since a worker
     * did not come back from it, it is THE one.  Which is the whole point: the sweep
     * does not report "something in here killed it", it reports the bytes.
     *
     * \~spanish TERCERA FORMA: no queda nada que partir.  Un prefijo congelado mas un
     * valor, con la profundidad agotada, es una sola candidata -- y como un trabajador
     * no volvio de ella, es ESA.  Que es toda la gracia: el barrido no informa de
     * "algo de aqui dentro lo mato", informa de los bytes.
     */
    *single = *lost;
    single->prefix[lost->fixed] = lost->lo;
    return 1;
}
