/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file count/counter.c
 * @brief
 * \~english The arm / read / disarm sequence, and why it is in that order.
 * \~spanish La secuencia de armar / leer / desarmar, y por que va en ese orden.
 * \~
 *
 * \~english
 * THE ORDER IS NOT A STYLE CHOICE.  The manual states one requirement outright:
 *
 *   "The event logic unit for a UMASK must be disabled by setting
 *    IA32_PERFEVTSELx[bit 22] = 0, before writing to IA32_PMCx."
 *
 * So zeroing the counter has to happen with the selector's switch OFF.  Doing it
 * the other way does not raise a fault and does not return an error: the counter
 * takes a value nobody asked for, and the measurement is wrong by an amount that
 * looks plausible.  That is exactly the failure this whole tree is written to
 * not have, so the sequence is written out step by step below.
 *
 * @code
 *   ARM                                     DISARM
 *   ---------------------------------       ---------------------------------
 *   read  GLOBAL_CTRL      save             write GLOBAL_CTRL = 0   stop
 *   read  PERFEVTSELi      save             write PERFEVTSELi = 0   switch off
 *   read  PMCi             save             write PMCi        = saved
 *                                           write PERFEVTSELi = saved
 *   write GLOBAL_CTRL = 0    stop           write GLOBAL_CTRL = saved
 *   write PERFEVTSELi = sel  (EN off)
 *   write PMCi        = 0    <- needs EN off
 *   write PERFEVTSELi = sel | EN
 *   write GLOBAL_CTRL = mask  start
 * @endcode
 *
 * WHY EVERYTHING STOPS BEFORE ANYTHING IS PROGRAMMED.  `GLOBAL_CTRL` is written
 * to zero first so that no counter is running while its selector is being
 * changed halfway.  And it is written LAST when starting, which is what makes
 * all the counters begin on the same instruction instead of each on the write
 * that armed it -- six counters that start at six different moments do not
 * measure the same window, and the difference is invisible in the result.
 *
 * \~spanish
 * EL ORDEN NO ES UNA CUESTION DE ESTILO.  El manual pone una exigencia por
 * escrito:
 *
 *   "The event logic unit for a UMASK must be disabled by setting
 *    IA32_PERFEVTSELx[bit 22] = 0, before writing to IA32_PMCx."
 *
 * O sea que poner el contador a cero tiene que hacerse con el interruptor del
 * selector APAGADO.  Hacerlo al reves no provoca una excepcion ni devuelve un
 * error: el contador coge un valor que nadie pidio, y la medida sale mal por una
 * cantidad que parece plausible.  Es justo el fallo que este arbol entero esta
 * escrito para no tener, asi que la secuencia va detallada paso a paso arriba.
 *
 * POR QUE SE PARA TODO ANTES DE PROGRAMAR NADA.  `GLOBAL_CTRL` se escribe a cero
 * lo primero para que ningun contador este corriendo mientras se le cambia el
 * selector a medias.  Y se escribe el ULTIMO al arrancar, que es lo que hace que
 * todos los contadores empiecen en la misma instruccion en vez de cada uno en la
 * escritura que lo armo -- seis contadores que arrancan en seis momentos
 * distintos no miden la misma ventana, y la diferencia no se ve en el resultado.
 */

#include "count/counter.h"

#include "msr/index.h"

/**
 * @brief
 * \~english Which register holds the counts: the plain one or the wide alias.
 * \~spanish Que registro lleva las cuentas: el normal o el alias ancho.
 * \~
 *
 * \~english
 * `IA32_PMCx` is 48 bits wide to READ, but a write to it only takes the low 32
 * and sign-extends them: writing back a saved count of, say, 0x1_0000_0000
 * leaves something else in the counter.  `IA32_A_PMCx` is the same counter
 * through an address that accepts the full width, and it exists when
 * `IA32_PERF_CAPABILITIES` bit 13 says so.
 *
 * It matters here only for putting back what was there.  Zeroing works through
 * either -- zero sign-extends to zero -- but restoring somebody else's count
 * through the narrow one would quietly corrupt their measurement, which is the
 * opposite of the point of saving it.
 *
 * \~spanish
 * `IA32_PMCx` tiene 48 bits para LEER, pero una escritura solo se queda con los
 * 32 bajos y los extiende con signo: devolver una cuenta guardada de, pongamos,
 * 0x1_0000_0000 deja otra cosa en el contador.  `IA32_A_PMCx` es el mismo
 * contador por una direccion que acepta el ancho entero, y existe cuando el bit
 * 13 de `IA32_PERF_CAPABILITIES` lo dice.
 *
 * Aqui solo importa para devolver lo que habia.  Poner a cero funciona por
 * cualquiera de los dos -- el cero se extiende a cero --, pero restaurar la
 * cuenta de otro por el estrecho le corromperia la medida en silencio, que es lo
 * contrario de para lo que se guardaba.
 */
static u32 counter_base(const pmu_caps *caps) {
    return caps->full_width_write ? IA32_A_PMC0 : IA32_PMC0;
}

/** @brief
 *  \~english The selector word for one event, with the switch still off.
 *  \~spanish La palabra del selector de un evento, con el interruptor aun
 *            apagado. \~ */
static u64 selector_of(const arch_event *e, u32 qualifiers) {
    u64 sel = (u64)e->event << PERFEVTSEL_EVENT_SHIFT;
    sel |= (u64)e->umask << PERFEVTSEL_UMASK_SHIFT;
    sel |= (u64)qualifiers;
    /* \~english The switch is NEVER set here: whoever arms adds it after zeroing
     * the counter, which is the order the manual demands.
     * \~spanish El interruptor NUNCA se pone aqui: quien arma lo anade despues
     * de poner el contador a cero, que es el orden que exige el manual. \~ */
    return sel & ~(u64)PERFEVTSEL_EN;
}

status counter_arm(const pmu_caps *caps, u32 qualifiers, const msr_ops *ops,
                   counter_state *st) {
    u32 available;
    u32 i;
    u64 mask;
    status rc;

    if (caps == 0 || ops == 0 || ops->read == 0 || ops->write == 0 || st == 0) {
        return ERR_INVALID;
    }

    /* \~english Everything to zero FIRST, so that a failure halfway leaves a
     * state `counter_disarm` can read: `saved` grows as each register is read,
     * so it always says exactly how much there is to put back.
     * \~spanish Todo a cero LO PRIMERO, para que un fallo a mitad deje un estado
     * que `counter_disarm` pueda leer: `saved` crece segun se lee cada registro,
     * asi que siempre dice exactamente cuanto hay que devolver. \~ */
    for (i = 0; i < COUNTER_MAX; ++i) {
        st->evtsel[i] = 0;
        st->pmc[i] = 0;
        st->event[i] = 0;
    }
    st->global_ctrl = 0;
    st->saved = 0;
    st->armed = 0;
    st->qualifiers = qualifiers;
    st->counter_msr = counter_base(caps);

    /* \~english Three refusals, and each one says a different thing.  None of
     * them is a fallback: a profiler that measures anyway on a part it does not
     * understand publishes a number, and a number is believed.
     * \~spanish Tres negativas, y cada una dice una cosa distinta.  Ninguna es
     * un respaldo: un perfilador que mide igualmente en una pieza que no
     * entiende publica un numero, y un numero se cree. \~ */
    if (caps->vendor != CPU_VENDOR_INTEL) {
        /* \~english AMD counts through another family of MSRs entirely
         * (`PERF_CTL`, `PERF_CTR`), not through these.  Writing here would go to
         * registers that mean something else.
         * \~spanish AMD cuenta por otra familia de MSR completamente distinta
         * (`PERF_CTL`, `PERF_CTR`), no por estos.  Escribir aqui iria a
         * registros que significan otra cosa. \~ */
        return ERR_UNSUPPORTED;
    }
    if (caps->pmu_version == 0 || caps->gp_counters == 0) {
        return ERR_UNSUPPORTED;
    }
    if (caps->counters_in_use) {
        /* \~english Somebody is already measuring on this core.  See
         * `counter_arm`'s contract: we refuse rather than share.
         * \~spanish Ya hay alguien midiendo en este nucleo.  Ver el contrato de
         * `counter_arm`: nos negamos en vez de compartir. \~ */
        return ERR_STATE;
    }

    available = caps->gp_counters;
    if (available > COUNTER_MAX) {
        available = COUNTER_MAX;
    }

    /* --- \~english save what is there \~spanish guardar lo que hay \~ ------ */
    rc = ops->read(IA32_PERF_GLOBAL_CTRL, &st->global_ctrl, ops->ctx);
    if (rc != OK) {
        return rc;
    }
    for (i = 0; i < available; ++i) {
        rc = ops->read(IA32_PERFEVTSEL0 + i, &st->evtsel[i], ops->ctx);
        if (rc != OK) {
            return rc;
        }
        rc = ops->read(st->counter_msr + i, &st->pmc[i], ops->ctx);
        if (rc != OK) {
            return rc;
        }
        /* \~english Only now: the pair for counter `i` is complete.
         * \~spanish Solo ahora: la pareja del contador `i` esta completa. \~ */
        st->saved = i + 1;
    }

    /* --- \~english choose the events \~spanish elegir los eventos \~ ------- */
    /* \~english BY THE TABLE, which is what makes this portable.  The events are
     * walked in the manual's order and each one is asked of THIS processor; the
     * ones it does not have are skipped instead of being programmed and read as
     * zero, which would look like "it happened zero times".
     * \~spanish POR LA TABLA, que es lo que hace esto portable.  Se recorren los
     * eventos en el orden del manual y se le pregunta cada uno a ESTE
     * procesador; los que no tenga se saltan en vez de programarse y leerse como
     * cero, que pareceria "paso cero veces". \~ */
    for (i = 0; i < ARCH_EVENT_MAX && st->armed < available; ++i) {
        if ((arch_events[i].flags & ARCH_EVENT_ENCODED) == 0) {
            continue;
        }
        if (!arch_event_present(caps->arch_events_listed,
                                caps->arch_events_absent, i)) {
            continue;
        }
        st->event[st->armed] = (u8)i;
        st->armed += 1;
    }
    if (st->armed == 0) {
        /* \~english A PMU with no architectural event to arm.  Nothing has been
         * written yet, so there is nothing to put back either.
         * \~spanish Un PMU sin ningun evento arquitectonico que armar.  Todavia
         * no se ha escrito nada, asi que tampoco hay nada que devolver. \~ */
        return ERR_UNSUPPORTED;
    }

    /* --- \~english program \~spanish programar \~ -------------------------- */
    rc = ops->write(IA32_PERF_GLOBAL_CTRL, 0, ops->ctx);
    if (rc != OK) {
        return rc;
    }
    for (i = 0; i < st->armed; ++i) {
        u64 sel = selector_of(&arch_events[st->event[i]], qualifiers);

        rc = ops->write(IA32_PERFEVTSEL0 + i, sel, ops->ctx);
        if (rc != OK) {
            return rc;
        }
        /* \~english The switch is off in `sel`, so this write is legal.
         * \~spanish El interruptor esta apagado en `sel`, asi que esta escritura
         * es legal. \~ */
        rc = ops->write(st->counter_msr + i, 0, ops->ctx);
        if (rc != OK) {
            return rc;
        }
        rc = ops->write(IA32_PERFEVTSEL0 + i, sel | PERFEVTSEL_EN, ops->ctx);
        if (rc != OK) {
            return rc;
        }
    }

    /* \~english And the single write that starts them all at once.
     * \~spanish Y la unica escritura que los arranca todos a la vez. \~ */
    mask = (st->armed >= 64u) ? ~(u64)0 : ((((u64)1) << st->armed) - 1u);
    return ops->write(IA32_PERF_GLOBAL_CTRL, mask, ops->ctx);
}

status counter_read(const counter_state *st, const msr_ops *ops, u64 *out) {
    u32 i;

    if (st == 0 || ops == 0 || ops->read == 0 || out == 0) {
        return ERR_INVALID;
    }
    for (i = 0; i < st->armed; ++i) {
        status rc = ops->read(st->counter_msr + i, &out[i], ops->ctx);
        if (rc != OK) {
            return rc;
        }
    }
    return OK;
}

status counter_disarm(const counter_state *st, const msr_ops *ops) {
    u32 i;
    status rc;
    status first = OK;

    if (st == 0 || ops == 0 || ops->write == 0) {
        return ERR_INVALID;
    }
    if (st->saved == 0) {
        /* \~english Nothing was ever read, so nothing was ever written either.
         * This is the `DriverUnload` that never got to arm.
         * \~spanish Nunca se leyo nada, asi que tampoco se escribio nada.  Este
         * es el `DriverUnload` que no llego a armar. \~ */
        return OK;
    }

    /* \~english EVERY step is attempted even if one fails, and the FIRST failure
     * is what comes back.  Stopping at the first one would leave the machine
     * with some of our selectors still armed, which is worse than the error we
     * would be reporting promptly.
     * \~spanish TODOS los pasos se intentan aunque uno falle, y lo que vuelve es
     * el PRIMER fallo.  Parar en el primero dejaria la maquina con parte de
     * nuestros selectores todavia armados, que es peor que el error que
     * estariamos reportando puntualmente. \~ */
    rc = ops->write(IA32_PERF_GLOBAL_CTRL, 0, ops->ctx);
    if (rc != OK && first == OK) {
        first = rc;
    }

    for (i = 0; i < st->saved; ++i) {
        /* \~english Switch off before touching the count: the same rule as when
         * arming, and for the same reason.
         * \~spanish Apagar el interruptor antes de tocar la cuenta: la misma
         * regla que al armar, y por lo mismo. \~ */
        rc = ops->write(IA32_PERFEVTSEL0 + i, 0, ops->ctx);
        if (rc != OK && first == OK) {
            first = rc;
        }
        rc = ops->write(st->counter_msr + i, st->pmc[i], ops->ctx);
        if (rc != OK && first == OK) {
            first = rc;
        }
        rc = ops->write(IA32_PERFEVTSEL0 + i, st->evtsel[i], ops->ctx);
        if (rc != OK && first == OK) {
            first = rc;
        }
    }

    rc = ops->write(IA32_PERF_GLOBAL_CTRL, st->global_ctrl, ops->ctx);
    if (rc != OK && first == OK) {
        first = rc;
    }
    return first;
}
