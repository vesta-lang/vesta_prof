/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_ring.c
 * @brief
 * \~english The ring's overflow policy, exercised without a driver.
 * \~spanish La politica de desbordamiento del anillo, ejercitada sin driver.
 * \~
 *
 * \~english
 * WHY THIS CAN BE TESTED HERE AT ALL.  The policy has no dependency on the
 * system: the data area is supplied by the caller and the whole state lives in
 * `ring_producer`.  So a make-believe ring in an ordinary array exercises the
 * SAME code that runs inside the interrupt handler -- which is the point of
 * `common/` not including any OS header.
 *
 * WHAT IS CHECKED, and it is the part that only shows up when it is full:
 *
 *   - that when it does not fit, what is dropped is the NEW one and nothing
 *     already published is touched;
 *   - that the loss is counted and PLACED IN TIME, and published once there is
 *     room;
 *   - that no record ends up straddling the end of the area;
 *   - that crossing the watermark throttles and says so.
 *
 * \~spanish
 * POR QUE ESTO SE PUEDE PROBAR AQUI.  La politica no depende del sistema: el
 * area de datos la pone el llamante y el estado vive entero en `ring_producer`.
 * Asi que un anillo de mentira en un array corriente ejercita el MISMO codigo
 * que corre dentro del manejador de interrupcion -- que es para lo que sirve que
 * `common/` no incluya ninguna cabecera del sistema.
 *
 * QUE SE COMPRUEBA, y es lo que solo aparece cuando esta lleno:
 *
 *   - que al no caber se pierde lo NUEVO y no se toca nada ya publicado;
 *   - que la perdida se cuenta y se SITUA EN EL TIEMPO, y se publica en cuanto
 *     hay sitio;
 *   - que ningun registro queda a caballo del final del area;
 *   - que al cruzar la marca de agua se estrangula y se dice.
 */

#include "ring/produce.h"

#include <stdio.h>

static int failures = 0;

static void check(int cond, const char *what) {
    if (!cond) {
        printf("FAIL: %s\n", what);
        failures += 1;
    }
}

/* \~english 64 KiB: the smallest the policy accepts, because a record can
 * describe up to 65535 bytes and there must be room for one.
 * \~spanish 64 KiB: lo minimo que la politica acepta, porque un registro puede
 * describir hasta 65535 bytes y tiene que caber uno. \~ */
#define CAP (64u * 1024u)

static ring_header g_hdr;
static u8 g_data[CAP];
static ring_producer g_p;

/**
 * @brief
 * \~english Writes one record of `payload` payload bytes.
 * \~spanish Escribe un registro con `payload` bytes de carga.
 * @return \~english non-zero if it fit \~spanish distinto de cero si cupo \~
 */
static int put(u16 payload, u64 tsc, u8 fill) {
    u16 total = (u16)(payload + sizeof(abi_record));
    u8 *at = (u8 *)ring_reserve(&g_p, RING_KIND_FIRST_SAMPLE, total, tsc);
    u16 i;
    if (at == 0) {
        return 0;
    }
    for (i = 0; i < payload; ++i) {
        at[i] = fill;
    }
    ring_commit(&g_p);
    return 1;
}

/**
 * @brief
 * \~english Walks the ring from `tail` to `head`, checking it decodes.
 * \~spanish Recorre el anillo de `tail` a `head` comprobando que decodifica.
 *
 * \~english
 * This is the check that matters: if a record straddled the end, or a length
 * were wrong, the walk would not land exactly on `head`.  It also proves the
 * consumer can skip a kind it does not know -- padding included -- using only
 * `length`.
 *
 * \~spanish
 * Esta es la comprobacion que importa: si un registro quedara a caballo del
 * final, o una longitud estuviera mal, el recorrido no caeria exactamente en
 * `head`.  Ademas demuestra que el consumidor puede saltar un tipo que no
 * conoce -- el relleno incluido -- usando solo `length`.
 *
 * @return \~english how many records were walked \~spanish cuantos registros se recorrieron \~
 */
static u32 walk(u32 *padding_seen, u32 *lost_seen, u32 *throttle_seen) {
    u64 at = g_hdr.tail;
    u32 n = 0;

    *padding_seen = 0;
    *lost_seen = 0;
    *throttle_seen = 0;

    while (at < g_hdr.head) {
        u64 off = at & g_hdr.mask;
        abi_record h;
        usize i;

        for (i = 0; i < sizeof h; ++i) {
            ((u8 *)&h)[i] = g_data[off + i];
        }
        if (h.length < sizeof(abi_record)) {
            printf("FAIL: registro de longitud %u en %llu\n",
                   (unsigned)h.length, (unsigned long long)at);
            failures += 1;
            return n;
        }
        /* \~english Nothing may cross the end of the area.
         * \~spanish Nada puede cruzar el final del area. \~ */
        if (off + h.length > (u64)g_hdr.mask + 1u) {
            printf("FAIL: registro a caballo del final en %llu (+%u)\n",
                   (unsigned long long)at, (unsigned)h.length);
            failures += 1;
            return n;
        }
        if (h.kind == RING_KIND_PADDING) {
            *padding_seen += 1;
        } else if (h.kind == RING_KIND_LOST) {
            *lost_seen += 1;
        } else if (h.kind == RING_KIND_THROTTLE) {
            *throttle_seen += 1;
        }
        at += h.length;
        n += 1;
    }
    check(at == g_hdr.head, "el recorrido cae exactamente en head");
    return n;
}

/** \~english Nothing published is ever rewritten. \~spanish Nunca se reescribe
 *  nada publicado. \~ */
static void test_drops_the_new_one(void) {
    u32 pad, lost, thr;
    u64 head_when_full;
    int fit;
    u32 written = 0;

    check(ring_producer_init(&g_p, &g_hdr, g_data, CAP, 3, 1000) == OK,
          "el productor arranca");
    check(g_hdr.cpu == 3, "el anillo sabe de que nucleo es");

    /* \~english Fill it without ever draining.
     * \~spanish Llenarlo sin vaciar nunca. \~ */
    while (put(1020, 100 + written, 0xAA)) {
        written += 1;
    }
    head_when_full = g_hdr.head;
    check(written > 0, "cupo algo antes de llenarse");
    check(g_p.lost_records == 1, "el primero que no cupo se conto");

    /* \~english More attempts: they must all be lost, and `head` must not move
     * a single byte.  That is "a published byte is never rewritten".
     * \~spanish Mas intentos: todos deben perderse, y `head` no puede moverse ni
     * un byte.  Eso es "nunca se reescribe un byte ya publicado". \~ */
    fit = put(1020, 500, 0xBB);
    check(!fit, "con el anillo lleno no entra nada mas");
    check(g_hdr.head == head_when_full, "y head no se ha movido");
    check(g_p.lost_records == 2, "la segunda perdida tambien se conto");
    check(g_p.lost_tsc_first < g_p.lost_tsc_last,
          "la perdida cubre una VENTANA, no un instante");

    (void)walk(&pad, &lost, &thr);
    printf("  lleno con %u registros, %u de relleno\n", written, pad);
}

/** \~english The loss is published once there is room. \~spanish La perdida se
 *  publica en cuanto hay sitio. \~ */
static void test_lost_is_published(void) {
    u32 pad, lost, thr;
    u32 before = g_p.lost_records;

    check(before > 0, "hay perdida acumulada de la prueba anterior");
    check(ring_flush_lost(&g_p) == 0, "sin sitio, el LOST no se publica");

    /* \~english The consumer drains everything: now there is room.
     * \~spanish El consumidor vacia todo: ahora hay sitio. \~ */
    g_hdr.tail = g_hdr.head;
    check(ring_flush_lost(&g_p) == 1, "con sitio, el LOST se publica");
    check(g_p.lost_records == 0, "y la cuenta se pone a cero");

    (void)walk(&pad, &lost, &thr);
    check(lost == 1, "hay exactamente un registro LOST en el anillo");
}

/** \~english Crossing the watermark throttles. \~spanish Al cruzar la marca de
 *  agua se estrangula. \~ */
static void test_throttle(void) {
    u32 pad, lost, thr;
    u64 period;
    u32 written = 0;

    check(ring_producer_init(&g_p, &g_hdr, g_data, CAP, 0, 1000) == OK,
          "el productor rearranca");
    check(ring_throttle(&g_p, 1) == 1000,
          "vacio no se estrangula");

    while (!ring_over_watermark(&g_hdr) && put(1020, 200 + written, 0xCC)) {
        written += 1;
    }
    check(ring_over_watermark(&g_hdr), "se cruzo la marca de agua");

    period = ring_throttle(&g_p, 999);
    check(period == 2000, "el periodo se doblo");
    check(g_p.throttled == 1, "y quedo contado");

    (void)walk(&pad, &lost, &thr);
    check(thr == 1, "hay un registro THROTTLE en el anillo");
    printf("  marca de agua a %u de %u bytes\n", (unsigned)g_hdr.watermark,
           (unsigned)CAP);
}

/**
 * @brief
 * \~english Forces the wrap, which is where padding lives.
 * \~spanish Fuerza la vuelta, que es donde vive el relleno.
 * \~
 *
 * \~english
 * THIS TEST EXISTS BECAUSE THE OTHERS DID NOT REACH IT.  With 1024-byte records
 * in a 65536-byte ring nothing ever straddles the end -- the size divides
 * exactly -- so the padding path ran zero times and a bug in it would have gone
 * unnoticed.  A size that does NOT divide the capacity is what makes the record
 * land across the limit.
 *
 * It drains as it goes, so it goes round many times instead of filling up:
 * here what is being checked is the wrap, not the loss.
 *
 * \~spanish
 * ESTA PRUEBA EXISTE PORQUE LAS OTRAS NO LLEGABAN AQUI.  Con registros de 1024
 * bytes en un anillo de 65536 nunca queda ninguno a caballo del final -- el
 * tamano divide exacto --, asi que el camino del relleno se ejecutaba cero veces
 * y un fallo en el habria pasado desapercibido.  Un tamano que NO divide la
 * capacidad es lo que hace que un registro caiga cruzando el limite.
 *
 * Va vaciando sobre la marcha, asi que da muchas vueltas en vez de llenarse:
 * aqui lo que se comprueba es la vuelta, no la perdida.
 */
static void test_wrap_pads(void) {
    u32 pad, lost, thr;
    u32 i;
    u32 fitted = 0;

    check(ring_producer_init(&g_p, &g_hdr, g_data, CAP, 0, 1000) == OK,
          "el productor rearranca para la vuelta");

    /* \~english 1000 is not a divisor of 65536, so the offset drifts and sooner
     * or later a record lands across the end.
     * \~spanish 1000 no divide a 65536, asi que el desplazamiento se va
     * corriendo y antes o despues un registro cae cruzando el final. \~ */
    for (i = 0; i < 500u; ++i) {
        if (put(996, 1000 + i, (u8)i)) {
            fitted += 1;
        }
        /* \~english Drain everything on each turn: the ring never fills, so the
         * only reason to pad is the wrap.
         * \~spanish Vaciar todo en cada vuelta: el anillo no se llena nunca, asi
         * que el unico motivo para rellenar es la vuelta. \~ */
        g_hdr.tail = g_hdr.head;
    }

    check(fitted == 500u, "vaciando en cada vuelta, cabe siempre");
    check(g_p.lost_records == 0, "y no se pierde nada");

    /* \~english The walk cannot check what was already drained, so what is
     * verified is the counter the producer kept: the ring went round several
     * times and therefore padded.
     * \~spanish El recorrido no puede comprobar lo ya vaciado, asi que lo que se
     * verifica es la cuenta que llevo el productor: el anillo dio varias vueltas
     * y por tanto rellenó. \~ */
    check(g_hdr.head > (u64)CAP * 5u, "dio varias vueltas");

    (void)walk(&pad, &lost, &thr);
    printf("  %u vueltas completas\n", (unsigned)(g_hdr.head / CAP));
}

/**
 * @brief
 * \~english The wrap, checked record by record without draining.
 * \~spanish La vuelta, comprobada registro a registro sin vaciar.
 * \~
 *
 * \~english
 * The previous one proves it does not break; this one proves the padding is
 * THERE and is well formed, because the walk crosses the whole ring and must
 * land exactly on `head`.  If padding carried a wrong length -- zero, for
 * instance -- the walk would derail here.
 *
 * \~spanish
 * La anterior demuestra que no se rompe; esta demuestra que el relleno ESTA y
 * esta bien formado, porque el recorrido cruza el anillo entero y tiene que caer
 * exactamente en `head`.  Si el relleno llevara una longitud mal -- cero, por
 * ejemplo --, el recorrido descarrilaria aqui.
 */
static void test_padding_is_walkable(void) {
    u32 pad, lost, thr;
    u32 n;
    u32 written = 0;

    check(ring_producer_init(&g_p, &g_hdr, g_data, CAP, 0, 1000) == OK,
          "el productor rearranca para el relleno");

    /* \~english THE PADDING NEEDS ROOM THAT IS NOT CONTIGUOUS, and that is the
     * subtlety: with the ring FULL there is no padding, there is loss.  So it
     * is filled up to near the end, half of it is drained, and writing goes on
     * -- now there is plenty of free space but not before the limit, which is
     * exactly the case padding exists for.
     *
     * 6000 bytes: 65536 / 6000 leaves a remainder of 5536, so the next one does
     * not fit before the end.
     *
     * \~spanish EL RELLENO NECESITA SITIO QUE NO ES CONTIGUO, y esa es la
     * sutileza: con el anillo LLENO no hay relleno, hay perdida.  Asi que se
     * llena hasta cerca del final, se vacia la mitad, y se sigue escribiendo --
     * ahora sobra sitio pero no antes del limite, que es justo el caso para el
     * que existe el relleno.
     *
     * 6000 bytes: 65536 / 6000 deja un resto de 5536, asi que el siguiente no
     * cabe antes del final. \~ */
    for (written = 0; written < 10u; ++written) {
        check(put(5996, 2000 + written, 0xDD) != 0,
              "los diez primeros de 6000 B caben");
    }
    check(g_hdr.head == 60000u, "van 60000 bytes, faltan 5536 para el final");

    /* \~english The consumer takes five: now there is room, just not there.
     * \~spanish El consumidor se lleva cinco: ahora hay sitio, pero no ahi. \~ */
    g_hdr.tail = 30000u;
    check(put(5996, 3000, 0xEE) != 0, "el siguiente cabe, dando la vuelta");
    written += 1;

    n = walk(&pad, &lost, &thr);
    check(pad == 1u, "hubo exactamente un relleno");
    /* \~english Five were drained, so the walk sees six records plus the
     * padding.  If the padding carried a wrong length the walk would not land
     * on `head` and `walk` would have already said so.
     * \~spanish Se vaciaron cinco, asi que el recorrido ve seis registros mas el
     * relleno.  Si el relleno llevara una longitud mal, el recorrido no caeria
     * en `head` y `walk` ya lo habria dicho. \~ */
    check(n == 6u + pad, "el recorrido ve los registros vivos y el relleno");
    printf("  %u registros de 6000 B, %u relleno(s), recorridos %u\n", written,
           pad, n);
}

/** \~english A size that is not a power of two is refused. \~spanish Un tamano
 *  que no es potencia de dos se rechaza. \~ */
static void test_bad_capacity(void) {
    check(ring_producer_init(&g_p, &g_hdr, g_data, CAP - 1u, 0, 1) ==
                  ERR_INVALID,
          "un tamano que no es potencia de dos se rechaza");
    check(ring_producer_init(&g_p, &g_hdr, g_data, 4096u, 0, 1) == ERR_INVALID,
          "un anillo donde no cabe un registro maximo se rechaza");
    check(ring_producer_init(&g_p, 0, g_data, CAP, 0, 1) == ERR_INVALID,
          "un destino nulo se rechaza");
}

int main(void) {
    test_drops_the_new_one();
    test_lost_is_published();
    test_throttle();
    test_wrap_pads();
    test_padding_is_walkable();
    test_bad_capacity();

    if (failures != 0) {
        printf("%d comprobaciones fallaron\n", failures);
        return 1;
    }
    printf("test_ring: OK\n");
    return 0;
}
