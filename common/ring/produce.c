/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file produce.c
 * @brief
 * \~english The ring's policy when it fills up.
 * \~spanish La politica del anillo cuando se llena.
 * \~
 *
 * \~english
 * No dependency on the system: the data area is supplied by the caller and the
 * state lives in `ring_producer`.  That is what allows filling a ring from a
 * user-space test and checking that the policy does what it says, instead of
 * finding out by loading a driver.
 *
 * \~spanish
 * Sin dependencias del sistema: el area de datos la pone el llamante y el estado
 * vive en `ring_producer`.  Es lo que permite llenar un anillo desde una prueba
 * de usuario y comprobar que la politica hace lo que dice, en vez de averiguarlo
 * cargando un driver.
 */

#include "produce.h"

/**
 * @brief
 * \~english The publication barrier.
 * \~spanish La barrera de publicacion.
 * \~
 *
 * \~english
 * The producer writes the payload and ONLY THEN advances the head.  If the
 * compiler reordered those two, the consumer could see an index promising bytes
 * that are not written yet.
 *
 * On x86-64 a COMPILER barrier is enough: the processor does not reorder one
 * store with another store, so no instruction is needed.  That it is free
 * matters, because this sits in the interrupt handler.
 *
 * It lives here with a name rather than scattered through the code: the day
 * there is a port to a weakly ordered architecture, this is the only place that
 * changes.
 *
 * \~spanish
 * El productor escribe la carga y SOLO DESPUES avanza la cabeza.  Si el
 * compilador reordenara esas dos cosas, el consumidor podria ver un indice que
 * promete bytes que aun no estan escritos.
 *
 * En x86-64 basta con una barrera de COMPILADOR: el procesador no reordena una
 * escritura con otra escritura, asi que no hace falta ninguna instruccion.  Que
 * sea gratis importa, porque esto esta en el manejador de interrupcion.
 *
 * Va aqui con nombre en vez de suelto por el codigo: el dia que haya un puerto a
 * una arquitectura con ordenacion debil, este es el unico sitio que cambia.
 */
#if defined(_MSC_VER)
#include <intrin.h>
#define PUBLISH_BARRIER() _ReadWriteBarrier()
#else
#define PUBLISH_BARRIER() __asm__ __volatile__("" ::: "memory")
#endif

/**
 * @brief
 * \~english Is it a power of two, and non-zero?
 * \~spanish ¿Es potencia de dos, y distinto de cero?
 * \~
 *
 * \~english
 * A power of two has exactly one bit set, so subtracting one flips that bit off
 * and turns every lower bit on: the AND of the two is zero only in that case.
 *
 * \~spanish
 * Una potencia de dos tiene exactamente un bit puesto, asi que restarle uno
 * apaga ese bit y enciende todos los de debajo: el AND de los dos es cero solo
 * en ese caso.
 */
static int is_power_of_two(u64 v) {
    return v != 0u && (v & (v - 1u)) == 0u;
}

/**
 * @brief
 * \~english Copies `n` bytes.  There is no libc in the kernel.
 * \~spanish Copia `n` bytes.  En el kernel no hay biblioteca estandar.
 */
static void copy_bytes(u8 *to, const u8 *from, usize n) {
    usize i;
    for (i = 0; i < n; ++i) {
        to[i] = from[i];
    }
}

status ring_producer_init(ring_producer *p, ring_header *ring, u8 *data,
                          u64 capacity, u32 cpu, u64 period) {
    usize i;

    if (p == 0 || ring == 0 || data == 0) {
        return ERR_INVALID;
    }
    /* \~english The position comes from `index & mask`, so with any other size
     * the mask would not cover the area.  It is checked rather than assumed:
     * the mistake would not give a failure, it would give writes outside the
     * ring.
     * \~spanish La posicion sale de `index & mask`, asi que con cualquier otro
     * tamano la mascara no cubriria el area.  Se comprueba en vez de suponerse:
     * el error no daria un fallo, daria escrituras fuera del anillo. \~ */
    if (!is_power_of_two(capacity)) {
        return ERR_INVALID;
    }
    /* \~english And the largest describable record must fit, or there would be
     * legal sizes in which `ring_reserve` could never succeed.
     * \~spanish Y tiene que caber el registro mas grande que se puede describir,
     * o habria tamanos legales en los que `ring_reserve` no podria terminar
     * nunca. \~ */
    if (capacity < 0x10000u) {
        return ERR_INVALID;
    }

    p->ring = ring;
    p->data = data;
    p->lost_records = 0;
    p->lost_bytes = 0;
    p->lost_tsc_first = 0;
    p->lost_tsc_last = 0;
    p->period = period;
    p->throttled = 0;

    ring->sized.size = (u32)sizeof(ring_header);
    ring->mask = (u32)(capacity - 1u);
    ring->flags = 0;
    ring->cpu = cpu;
    /* \~english Three quarters.  This does NOT come from measuring anything --
     * there is no real producer yet -- so it is a starting point and not a
     * defensible number: it leaves room for the burst that arrives while the
     * throttling takes effect.
     * \~spanish Tres cuartos.  Esto NO sale de medir nada -- todavia no hay
     * productor real --, asi que es un punto de partida y no un numero
     * defendible: deja sitio para la rafaga que llega mientras el estrangulado
     * surte efecto. \~ */
    ring->watermark = (u32)(capacity - capacity / 4u);
    ring->_pad = 0;
    ring->head = 0;
    ring->tail = 0;
    for (i = 0; i < sizeof ring->_producer_pad; ++i) {
        ring->_producer_pad[i] = 0;
    }
    for (i = 0; i < sizeof ring->_consumer_pad; ++i) {
        ring->_consumer_pad[i] = 0;
    }
    return OK;
}

/**
 * @brief
 * \~english Records that one entry did not fit.
 * \~spanish Anota que un registro no cupo.
 */
static void note_lost(ring_producer *p, u16 length, u64 tsc) {
    if (p->lost_records == 0) {
        p->lost_tsc_first = tsc;
    }
    p->lost_tsc_last = tsc;
    p->lost_records += 1;
    p->lost_bytes += length;
}

/**
 * @brief
 * \~english Writes a record header at `off`, returns where the payload goes.
 * \~spanish Escribe una cabecera de registro en `off` y devuelve donde va la carga.
 */
static void *put_header(ring_producer *p, u64 off, u16 kind, u16 length) {
    u8 *at = p->data + (off & p->ring->mask);
    abi_record h;

    h.kind = kind;
    h.length = length;
    /* \~english Byte by byte and not by struct assignment: the destination need
     * not be aligned.
     * \~spanish Byte a byte y no por asignacion de estructura: el destino no
     * tiene por que estar alineado. \~ */
    copy_bytes(at, (const u8 *)&h, sizeof h);
    return at + sizeof h;
}

void *ring_reserve(ring_producer *p, u16 kind, u16 length, u64 tsc) {
    u64 capacity = ring_capacity(p->ring);
    u64 head = p->ring->head;
    u64 offset = head & p->ring->mask;
    u64 to_end = capacity - offset;
    u64 need = length;

    if (length < sizeof(abi_record)) {
        return 0;
    }

    /* \~english NO RECORD STRADDLES THE END.  If it does not fit whole before
     * the limit, what is left is padded and the record starts from the
     * beginning.
     * \~spanish NINGUN REGISTRO A CABALLO DEL FINAL.  Si no cabe entero antes
     * del limite, lo que queda se rellena y el registro empieza por el
     * principio.
     * \~
     *
     *                    head & mask              data[mask]
     *                         |                        |
     *                         v                        v
     *      +------------------+------------------------+
     *      |    ...           |    <-- to_end -->      |
     *      +------------------+------------------------+
     *                         |<-- length -->|
     *                          does NOT fit
     *
     *      \~english  so instead:
     *      \~spanish  asi que en su lugar:
     *      \~
     *      +------------------+------------------------+
     *      |    ...           | PADDING, length=to_end |
     *      +------------------+------------------------+
     *      +--------------+
     *      | the record   |  <- \~english from data[0] \~spanish desde data[0] \~
     *      +--------------+
     *
     * \~english The padding counts as occupancy, so both must be asked for
     * TOGETHER: publishing the padding and then not fitting the record would
     * fill the ring with holes without saying anything was lost.
     * \~spanish El relleno cuenta como ocupacion, asi que hay que pedir los dos
     * JUNTOS: publicar el relleno y luego no caber el registro llenaria el
     * anillo de huecos sin decir que se perdio nada. \~ */
    if (to_end < need) {
        need += to_end;
    }

    if (ring_free(p->ring) < need) {
        /* \~english It does not fit.  The NEW one is dropped -- this one --
         * never what is already published.
         * \~spanish No cabe.  Se pierde lo NUEVO -- este --, nunca lo ya
         * publicado. \~ */
        note_lost(p, length, tsc);
        return 0;
    }

    if (to_end < (u64)length) {
        /* \~english The padding carries its own length so the consumer skips it
         * the way it skips any record it does not know.  It always fits in
         * `u16`: padding only happens when `to_end < length`, and `length` is a
         * `u16`, so `to_end` is smaller still.
         * \~spanish El relleno lleva su propia longitud para que el consumidor
         * lo salte como salta cualquier registro que no conozca.  Siempre cabe
         * en `u16`: solo se rellena cuando `to_end < length`, y `length` es un
         * `u16`, asi que `to_end` es aun menor. \~ */
        (void)put_header(p, head, RING_KIND_PADDING, (u16)to_end);
        PUBLISH_BARRIER();
        p->ring->head = head + to_end;
        head = p->ring->head;
    }

    return put_header(p, head, kind, length);
}

void ring_commit(ring_producer *p) {
    const u8 *at = p->data + (p->ring->head & p->ring->mask);
    u16 length = 0;

    /* \~english The length is re-read from the header just written instead of
     * being carried along in the producer: that way there are not two places
     * that could disagree about how big the record being published is.
     * \~spanish La longitud se relee de la cabecera recien escrita en vez de
     * arrastrarla en el productor: asi no hay dos sitios que puedan discrepar
     * sobre cuanto ocupa el registro que se esta publicando. \~ */
    copy_bytes((u8 *)&length, at + sizeof(u16), sizeof length);

    /* \~english The payload must be visible BEFORE the index that promises it.
     * \~spanish La carga tiene que ser visible ANTES que el indice que la
     * promete. \~ */
    PUBLISH_BARRIER();
    p->ring->head += length;
}

int ring_flush_lost(ring_producer *p) {
    ring_lost *rec;
    void *at;

    if (p->lost_records == 0) {
        return 0;
    }
    at = ring_reserve(p, RING_KIND_LOST, (u16)sizeof(ring_lost),
                      p->lost_tsc_last);
    if (at == 0) {
        /* \~english Not even the casualty report fits.  It stays accumulated:
         * counting it later is correct, and `ring_reserve` has just added this
         * attempt to the tally, which is also true -- it did not fit.
         * \~spanish Sigue sin caber ni el parte de bajas.  Se queda acumulado:
         * contarlo mas tarde es correcto, y `ring_reserve` acaba de sumar este
         * intento a la cuenta, que tambien es cierto -- no cupo. \~ */
        return 0;
    }
    /* \~english `at` points behind the header; the record starts before it.
     * \~spanish `at` apunta detras de la cabecera; el registro empieza antes. \~ */
    rec = (ring_lost *)(void *)((u8 *)at - sizeof(abi_record));
    rec->records = p->lost_records;
    rec->bytes = p->lost_bytes;
    rec->tsc_first = p->lost_tsc_first;
    rec->tsc_last = p->lost_tsc_last;
    ring_commit(p);

    p->lost_records = 0;
    p->lost_bytes = 0;
    p->lost_tsc_first = 0;
    p->lost_tsc_last = 0;
    return 1;
}

u64 ring_throttle(ring_producer *p, u64 tsc) {
    ring_throttle_rec *rec;
    void *at;
    u64 before;

    if (!ring_over_watermark(p->ring)) {
        return p->period;
    }

    /* \~english It DOUBLES.  Like the watermark, this comes from reasoning and
     * not from measuring: doubling converges fast and does not need to know how
     * much excess there is, which is exactly what cannot be known inside an
     * interrupt handler.  When there is a real producer, this is among the first
     * things to check against data.
     * \~spanish Se DOBLA.  Como la marca de agua, esto sale de razonar y no de
     * medir: doblar converge rapido y no necesita saber cuanto sobra, que es
     * justo lo que no se puede saber dentro de un manejador de interrupcion.
     * Cuando haya un productor de verdad, esto es de lo primero que hay que
     * contrastar con datos. \~ */
    before = p->period;
    p->period *= 2u;
    p->throttled += 1;

    at = ring_reserve(p, RING_KIND_THROTTLE, (u16)sizeof(ring_throttle_rec),
                      tsc);
    if (at != 0) {
        rec = (ring_throttle_rec *)(void *)((u8 *)at - sizeof(abi_record));
        rec->tsc = tsc;
        rec->period_before = before;
        rec->period_after = p->period;
        ring_commit(p);
    }
    /* \~english If not even that fit, the period goes up ANYWAY.  Throttling
     * without being able to report it is worse than reporting it, but much
     * better than going on losing for want of somewhere to write it down.
     * \~spanish Si ni eso cupo, el periodo sube IGUAL.  Estrangular sin poder
     * contarlo es peor que contarlo, pero mucho mejor que seguir perdiendo por
     * no tener donde anotarlo. \~ */
    return p->period;
}
