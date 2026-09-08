/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file produce.h
 * @brief
 * \~english The side that WRITES into the ring, and the policy for when it does
 *           not fit.
 * \~spanish El lado que ESCRIBE en el anillo, y la politica de cuando no cabe.
 * \~
 *
 * \~english
 * WHERE THIS RUNS.  Inside the interrupt handler the processor invokes when a
 * performance counter overflows, millions of times, at an interrupt level where
 * most of the operating system is off limits.  Three properties follow from
 * that and govern everything here:
 *
 *   - **nobody is waited for**.  The producer cannot block: on the other side
 *     there is a user-space consumer that may be descheduled;
 *   - **a published byte is never rewritten**, which is what lets the consumer
 *     decode in place without copying or validating;
 *   - **no memory is allocated**, not here and nowhere after start-up.
 *
 * IT IS TESTED WITHOUT A DRIVER.  The whole state lives in these structures and
 * the data area is supplied by the caller, so filling a make-believe ring from
 * a user-space test exercises the SAME policy that runs in the kernel.  See
 * `tests/test_ring.c`.
 *
 * \~spanish
 * DONDE CORRE ESTO.  Dentro del manejador de interrupcion que el procesador
 * invoca cuando un contador de rendimiento se desborda, millones de veces, a un
 * nivel de interrupcion donde la mayor parte del sistema operativo esta vedada.
 * De ahi salen las tres propiedades que lo gobiernan todo:
 *
 *   - **no se espera a nadie**.  El productor no puede bloquearse: al otro lado
 *     hay un consumidor de usuario que puede estar desalojado;
 *   - **no se reescribe un byte ya publicado**, que es lo que permite al
 *     consumidor decodificar en sitio sin copiar ni validar;
 *   - **no se reserva memoria**, ni aqui ni en ningun sitio despues del
 *     arranque.
 *
 * SE PRUEBA SIN DRIVER.  El estado vive entero en estas estructuras y el area
 * de datos la pone el llamante, asi que llenar un anillo de mentira desde una
 * prueba de usuario ejercita la MISMA politica que corre en el kernel.  Ver
 * `tests/test_ring.c`.
 */

#ifndef VXP_COMMON_RING_PRODUCE_H
#define VXP_COMMON_RING_PRODUCE_H

#include "ring.h"
#include "vxp_base.h"

/**
 * @brief
 * \~english The producer's private state.
 * \~spanish El estado privado del productor.
 * \~
 *
 * \~english
 * What was lost accumulates HERE and not in the shared header, and that is not
 * a detail: while a record does not fit, neither does the `LOST` record that
 * counts it, so the tally has to live somewhere that is not the ring.  Besides,
 * nobody on the other side reads this, and putting it in the shared area would
 * be giving away writes to a cache line the consumer looks at.
 *
 * \~spanish
 * Lo perdido se acumula AQUI y no en la cabecera compartida, y no es un
 * detalle: mientras no cabe un registro tampoco cabe el `LOST` que lo cuenta,
 * asi que hay que llevar la cuenta en algun sitio que no sea el anillo.  Ademas
 * esto no lo lee nadie del otro lado, y ponerlo en la zona compartida seria
 * regalar escrituras a una linea de cache que el consumidor mira.
 */
typedef struct ring_producer {
    ring_header *ring; /**< \~english the shared header \~spanish la cabecera compartida \~ */
    u8 *data;          /**< \~english the data area, `mask + 1` bytes \~spanish el area de datos, `mask + 1` bytes \~ */

    /** \~english What did not fit, waiting for room to report it.
     *  \~spanish Lo que no cupo, esperando a que haya sitio para contarlo. \~ */
    u32 lost_records;   /**< \~english how many records were lost \~spanish cuantos registros se perdieron \~ */
    u64 lost_bytes;     /**< \~english how many bytes they would have taken \~spanish cuantos bytes habrian ocupado \~ */
    u64 lost_tsc_first; /**< \~english stamp of the first \~spanish marca del primero \~ */
    u64 lost_tsc_last;  /**< \~english stamp of the last \~spanish marca del ultimo \~ */

    u64 period;    /**< \~english the period in force \~spanish el periodo vigente \~ */
    u64 throttled; /**< \~english how many times it was raised \~spanish cuantas veces se subio \~ */
} ring_producer;

/**
 * @brief
 * \~english Gets the producer ready over an already reserved ring.
 * \~spanish Deja el productor listo sobre un anillo ya reservado.
 * \~
 *
 * @param p        \~english the producer \~spanish el productor \~
 * @param ring     \~english the shared header \~spanish la cabecera compartida \~
 * @param data     \~english the data area, `capacity` bytes \~spanish el area de datos, de `capacity` bytes \~
 * @param capacity \~english area size; must be a POWER OF TWO \~spanish tamano del area; tiene que ser POTENCIA DE DOS \~
 * @param cpu      \~english this ring's logical processor \~spanish el procesador logico de este anillo \~
 * @param period   \~english the initial sampling period \~spanish el periodo de muestreo inicial \~
 *
 * @return
 * \~english `ERR_INVALID` if `capacity` is not a power of two or cannot hold a
 *           maximum-sized record; `OK` if it is ready.
 * \~spanish `ERR_INVALID` si `capacity` no es potencia de dos o no cabe un
 *           registro maximo; `OK` si quedo listo.
 * \~
 *
 * \~english
 * The power of two is not a preference: the position comes from `index & mask`,
 * and with any other size the mask would not cover the area.
 *
 * \~spanish
 * La potencia de dos no es una preferencia: la posicion sale de `index & mask`,
 * y con cualquier otro tamano la mascara no cubriria el area.
 */
status ring_producer_init(ring_producer *p, ring_header *ring, u8 *data,
                          u64 capacity, u32 cpu, u64 period);

/**
 * @brief
 * \~english Reserves room for a record and returns where its payload goes.
 * \~spanish Reserva sitio para un registro y devuelve donde va su carga.
 * \~
 *
 * @param p      \~english the producer \~spanish el productor \~
 * @param kind   \~english the kind, `RING_KIND_*` \~spanish el tipo, `RING_KIND_*` \~
 * @param length \~english TOTAL bytes, header included \~spanish bytes TOTALES, cabecera incluida \~
 * @param tsc    \~english the timestamp, to place the loss if it does not fit \~spanish la marca de tiempo, para situar la perdida si no cabe \~
 *
 * @return
 * \~english where to write the payload -- right behind the header -- or zero if
 *           it did not fit.  When it returns zero the loss is ALREADY recorded:
 *           the caller only has to not write.
 * \~spanish donde escribir la carga -- justo detras de la cabecera --, o cero si
 *           no cupo.  Cuando devuelve cero, la perdida YA quedo anotada: quien
 *           llama solo tiene que no escribir.
 * \~
 *
 * \~english
 * If what comes next does not fit whole before the end of the area, padding is
 * published up to the limit and the record starts from the beginning.  None is
 * ever left straddling.
 *
 * \~spanish
 * Si lo que viene no cabe entero antes del final del area, se publica un relleno
 * hasta el limite y el registro empieza por el principio.  Nunca queda uno a
 * caballo.
 */
void *ring_reserve(ring_producer *p, u16 kind, u16 length, u64 tsc);

/**
 * @brief
 * \~english Publishes what was reserved.
 * \~spanish Publica lo reservado.
 * \~
 *
 * \~english
 * Until this is called the consumer sees nothing: `ring_reserve` writes the
 * bytes and `ring_commit` makes them visible by advancing the head.  Splitting
 * it in two is what allows writing the payload IN PLACE, with no intermediate
 * buffer.
 *
 * \~spanish
 * Hasta que se llama a esto, el consumidor no ve nada: `ring_reserve` escribe
 * los bytes y `ring_commit` los hace visibles avanzando la cabeza.  Separarlo en
 * dos es lo que permite escribir la carga EN SITIO, sin un bufer intermedio.
 */
void ring_commit(ring_producer *p);

/**
 * @brief
 * \~english If there is accumulated loss and it now fits, publishes it.
 * \~spanish Si hay perdida acumulada y ahora cabe, la publica.
 * \~
 *
 * \~english
 * Called at the start of each sample.  The `LOST` record describes a WINDOW --
 * from the first loss to the last -- and not an instant, which is what lets a
 * reader tell whether the loss fell on what they were looking at.
 *
 * @return non-zero if it published one.
 *
 * \~spanish
 * Se llama al empezar cada muestra.  El `LOST` describe una VENTANA -- desde la
 * primera perdida hasta la ultima -- y no un instante, que es lo que permite a
 * quien lo lea saber si la perdida cayo sobre lo que estaba mirando.
 *
 * @return distinto de cero si publico uno.
 */
int ring_flush_lost(ring_producer *p);

/**
 * @brief
 * \~english Throttles if the watermark was crossed, and records it.
 * \~spanish Estrangula si se cruzo la marca de agua, y lo deja anotado.
 * \~
 *
 * \~english
 * THE WATERMARK is a fill level: past it, the ring is considered close enough
 * to full that acting is better than waiting.  On crossing it, sampling happens
 * LESS OFTEN instead of going on losing.  That is the only realizable form of
 * "stall the producer" -- nobody is waited for -- and it is strictly better than
 * losing: it leaves a known scale factor where a loss leaves a hole.
 *
 * @return the period that remains, the same as before if it was not needed.
 *
 * \~spanish
 * LA MARCA DE AGUA es un nivel de llenado: pasado el, se considera que el anillo
 * esta lo bastante cerca de lleno como para que actuar sea mejor que esperar.
 * Al cruzarla se muestrea MENOS a menudo en vez de seguir perdiendo.  Es la
 * unica forma realizable de "frenar al productor" -- no se espera a nadie -- y
 * es estrictamente mejor que perder: deja un factor de escala conocido donde una
 * perdida deja un agujero.
 *
 * @return el periodo que queda, igual al de antes si no hizo falta.
 */
u64 ring_throttle(ring_producer *p, u64 tsc);

#endif /* VXP_COMMON_RING_PRODUCE_H */
