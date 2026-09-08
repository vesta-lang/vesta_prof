/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file ring.h
 * @brief
 * \~english The shape of the sample ring.
 * \~spanish La forma del anillo de muestras.
 * \~
 *
 * \~english
 * WHAT A RING IS HERE.  A fixed block of memory written from one end and read
 * from the other, where the end wraps around to the beginning.  It exists
 * because the two sides run at wildly different speeds and cannot wait for each
 * other: the writer is the interrupt handler the processor invokes when a
 * counter overflows, and the reader is an ordinary program.  The ring is the
 * buffer that lets a handler which must finish in microseconds hand work to a
 * reader that may not run for milliseconds.
 *
 * ONE RING PER LOGICAL PROCESSOR.  That makes the producer UNIQUE, which is
 * what removes the atomic compare-and-swap from the head and stops a cache line
 * from bouncing between every core at the hottest point there is.  A cache line
 * is the unit the processor moves between cores -- 64 bytes -- and when two
 * cores write to the same one, each write invalidates the other's copy and both
 * pay for the traffic.
 *
 * It also makes loss LOCAL: "core 3 lost 40%" can be investigated, "the machine
 * lost 4%" cannot.  Global ordering is not given up: every sample carries its
 * timestamp and its core, and whoever drains them merges.
 *
 * WHEN IT FILLS, THE NEW SAMPLE IS DROPPED.  The other two ways out were
 * rejected for different reasons:
 *
 *   drop the old       with variable-length records, advancing the tail means
 *                      PARSING the ring inside the interrupt handler, over
 *                      records the reader may be looking at
 *   stall the producer not possible as such: the producer is the interrupt
 *                      handler, and waiting there for a user-space reader that
 *                      may be descheduled is not slowness, it is a hang
 *
 * Dropping the new one buys the property that matters inside an interrupt
 * handler: **a published byte is never rewritten**.  The reader decodes in
 * place and the correctness argument fits in one sentence.
 *
 * Its defect is real -- it loses during the burst, which is exactly when the
 * interesting thing happens -- so two pieces come with it: the `LOST` record is
 * PLACED IN TIME, and crossing the watermark THROTTLES.  See `RING_KIND_LOST`
 * and `RING_KIND_THROTTLE`.
 *
 * \~spanish
 * QUE ES AQUI UN ANILLO.  Un bloque fijo de memoria que se escribe por un
 * extremo y se lee por el otro, y donde el final da la vuelta al principio.
 * Existe porque los dos lados van a velocidades muy distintas y no pueden
 * esperarse: quien escribe es el manejador de interrupcion que el procesador
 * invoca cuando un contador se desborda, y quien lee es un programa corriente.
 * El anillo es el bufer que permite a un manejador que debe terminar en
 * microsegundos pasarle trabajo a un lector que quiza no corra hasta dentro de
 * milisegundos.
 *
 * UN ANILLO POR PROCESADOR LOGICO.  Eso hace al productor UNICO, que es lo que
 * quita el intercambio atomico de la cabeza y evita que una linea de cache
 * rebote entre todos los nucleos en el sitio mas caliente que hay.  Una linea
 * de cache es la unidad que el procesador mueve entre nucleos -- 64 bytes -- y
 * cuando dos nucleos escriben en la misma, cada escritura invalida la copia del
 * otro y los dos pagan el trafico.
 *
 * Ademas vuelve la perdida LOCAL: "el nucleo 3 perdio el 40%" se puede
 * investigar, "la maquina perdio el 4%" no.  No se renuncia al orden global:
 * cada muestra lleva su marca de tiempo y su nucleo, y quien vacia los mezcla.
 *
 * AL LLENARSE SE PIERDE LO NUEVO.  Las otras dos salidas se descartaron por
 * motivos distintos:
 *
 *   perder lo viejo     con registros de longitud variable, avanzar la cola
 *                       obliga a PARSEAR el anillo dentro del manejador de
 *                       interrupcion, sobre registros que el lector puede estar
 *                       mirando
 *   frenar al productor imposible tal cual: el productor es el manejador de la
 *                       interrupcion, y esperar ahi a un lector de usuario que
 *                       puede estar desalojado no es lentitud, es un cuelgue
 *
 * Perder lo nuevo compra la propiedad que importa dentro de un manejador de
 * interrupcion: **nunca se reescribe un byte ya publicado**.  El lector
 * decodifica en sitio y el argumento de correccion cabe en una frase.
 *
 * Su defecto es real -- pierde durante la rafaga, o sea justo cuando pasa lo
 * interesante -- y por eso van con el dos piezas: el `LOST` va SITUADO en el
 * tiempo, y al cruzar la marca de agua se ESTRANGULA.  Ver `RING_KIND_LOST` y
 * `RING_KIND_THROTTLE`.
 * \~
 *
 * \~english
 * NOT IN `include/` YET, deliberately.  This header will become the contract
 * between the driver and user space, and `include/` is MIT precisely so third
 * party tools can read it.  That makes it a one-way door: afterwards it only
 * admits APPENDING fields at the end -- that is what `abi_sized` is for -- and
 * adding record kinds -- that is what `abi_record.length` is for.  Reordering a
 * field, changing an index width or the meaning of a published kind, no.
 *
 * And this shape comes from reasoning, not from use: there is no producer yet.
 * It moves to `include/` once one has exercised it.
 *
 * \~spanish
 * TODAVIA NO EN `include/`, a proposito.  Esta cabecera acabara siendo el
 * contrato entre el driver y el lado de usuario, y `include/` es MIT
 * precisamente para que herramientas de terceros puedan leerlo.  Eso la
 * convierte en una puerta de un solo sentido: despues solo admite ANADIR
 * campos por el final -- para eso esta `abi_sized` -- y anadir tipos de
 * registro -- para eso esta `abi_record.length` --.  Reordenar un campo,
 * cambiar el ancho de un indice o el significado de un tipo ya publicado, no.
 *
 * Y esta forma sale de razonar, no de usarla: todavia no hay ningun productor.
 * Sube a `include/` cuando uno la haya ejercitado.
 * \~
 *
 * \~english The shape, once.  Labels are field names, so it reads the same in
 *           both languages.
 * \~spanish La forma, una vez.  Las etiquetas son nombres de campo, asi que se
 *           lee igual en los dos idiomas.
 * \~
 *
 *      data[0]                                              data[mask]
 *        |                                                        |
 *        v                                                        v
 *        +---------+--------------------------+-------------------+
 *        |  (free) |    published, unread     |      (free)       |
 *        +---------+--------------------------+-------------------+
 *                  ^                          ^
 *              tail & mask                head & mask
 *
 *        head - tail  =  ring_used()      <- no special case: the indices
 *        head, tail   ->  forever            never wrap, only the position
 *                                            is masked
 *
 *  \~english  And one record, which is what `head` advances by:
 *  \~spanish  Y un registro, que es lo que avanza `head`:
 *  \~
 *
 *        +--------+--------+---------------------------+
 *        |  kind  | length |          payload          |
 *        |  2 B   |  2 B   |      length - 4 bytes     |
 *        +--------+--------+---------------------------+
 *        <------------------ length ------------------->
 *
 *  \~english  `length` counts the header, so advancing is one addition and not
 *             an addition plus a correction somebody will forget.
 *  \~spanish  `length` cuenta la cabecera, asi que avanzar es una suma y no una
 *             suma con una correccion que alguien olvidara.
 *  \~
 *
 * @see doc/sampling.md
 */

#ifndef VXP_COMMON_RING_H
#define VXP_COMMON_RING_H

#include "vxp_abi.h"
#include "vxp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 *  \~english  The records the ring generates about itself.
 *  \~spanish  Los registros que el propio anillo genera.
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * @brief
 * \~english Padding up to the end of the ring.
 * \~spanish Relleno hasta el final del anillo.
 * \~
 *
 * \~english
 * NO RECORD STRADDLES THE END.  When what comes next does not fit whole before
 * the limit, one of these is published filling what is left and the record
 * starts again from the beginning.
 *
 * The reason is the reader: splitting a record in two pieces would force it to
 * copy and reassemble before looking at it, and that throws away what dropping
 * the new one bought -- decoding in place, without copying.  The price is a few
 * wasted bytes per lap.
 *
 * \~spanish
 * NINGUN REGISTRO QUEDA A CABALLO DEL FINAL.  Cuando lo que viene no cabe
 * entero antes del limite, se publica uno de estos ocupando lo que queda y el
 * registro empieza de nuevo por el principio.
 *
 * El motivo es el lector: partir un registro en dos trozos le obligaria a
 * copiar y recomponer antes de mirarlo, y eso tira lo que se compro con perder
 * lo nuevo -- decodificar en sitio, sin copiar.  El precio son unos pocos bytes
 * desperdiciados por vuelta.
 */
#define RING_KIND_PADDING 1u

/**
 * @brief
 * \~english What was lost, PLACED IN TIME.
 * \~spanish Lo que se perdio, SITUADO EN EL TIEMPO.
 * \~
 *
 * \~english
 * A total says "you lost 4%".  A placed one says "and all of it fell in 12 ms
 * of core 3", which is the difference between a profile that can be read and
 * one that cannot: without the interval, the reader cannot tell whether the
 * loss landed on what they were looking at.
 *
 * It accumulates WHILE there is no room and is published as soon as there is.
 * That is why it carries two timestamps and not one: it describes a WINDOW, not
 * an instant.
 *
 * \~spanish
 * Un total dice "perdiste el 4%".  Uno situado dice "y todo cayo en 12 ms del
 * nucleo 3", que es la diferencia entre un perfil que se puede leer y uno que
 * no: sin el intervalo, quien lo lea no puede saber si la perdida cayo sobre lo
 * que estaba mirando.
 *
 * Se acumula MIENTRAS no hay sitio y se publica en cuanto lo hay.  Por eso
 * lleva dos marcas de tiempo y no una: describe una VENTANA, no un instante.
 */
typedef struct ring_lost {
    abi_record header; /**< \~english `RING_KIND_LOST` \~spanish `RING_KIND_LOST` \~ */
    u32 records;       /**< \~english records that could not be written \~spanish registros que no se pudieron escribir \~ */
    u64 bytes;         /**< \~english bytes they would have taken \~spanish bytes que habrian ocupado \~ */
    u64 tsc_first;     /**< \~english stamp of the first one lost \~spanish marca del primero que se perdio \~ */
    u64 tsc_last;      /**< \~english stamp of the last one \~spanish marca del ultimo \~ */
} ring_lost;

#define RING_KIND_LOST 2u

/**
 * @brief
 * \~english The period was raised to stop losing samples.
 * \~spanish El periodo se subio para dejar de perder muestras.
 * \~
 *
 * \~english
 * THE PERIOD is how many events pass between one sample and the next: the
 * counter is loaded so it overflows after that many, and the overflow is what
 * raises the interrupt.  Raising it means sampling less often.
 *
 * On crossing the watermark the producer THROTTLES instead of going on losing,
 * and it is the only realizable form of "stall the producer": nobody is waited
 * for, samples are simply taken less often.
 *
 * And it is strictly better than losing, which is the reason it exists: a
 * throttled window has a KNOWN scale factor -- you know what to multiply by --
 * whereas a loss leaves a hole nobody knows the contents of.  That is why both
 * periods are published and not just the new one.
 *
 * \~spanish
 * EL PERIODO es cuantos eventos pasan entre una muestra y la siguiente: el
 * contador se carga para que se desborde al cabo de esos, y el desbordamiento
 * es lo que levanta la interrupcion.  Subirlo significa muestrear menos a
 * menudo.
 *
 * Al cruzar la marca de agua el productor ESTRANGULA en vez de seguir
 * perdiendo, y es la unica forma realizable de "frenar al productor": no se
 * espera a nadie, simplemente se muestrea menos.
 *
 * Y es estrictamente mejor que perder, que es la razon de que exista: una
 * ventana estrangulada tiene un factor de escala CONOCIDO -- se sabe por cuanto
 * multiplicar -- mientras que una perdida deja un agujero del que nadie sabe
 * que contenia.  Por eso se publican los dos periodos y no solo el nuevo.
 */
typedef struct ring_throttle_rec {
    abi_record header; /**< \~english `RING_KIND_THROTTLE` \~spanish `RING_KIND_THROTTLE` \~ */
    u64 tsc;           /**< \~english when it was decided \~spanish cuando se decidio \~ */
    u64 period_before; /**< \~english the period there was \~spanish el periodo que habia \~ */
    u64 period_after;  /**< \~english the one that remains \~spanish el que queda \~ */
} ring_throttle_rec;

#define RING_KIND_THROTTLE 3u

/**
 * \~english
 * SAMPLE kinds start here.  Below this, what the ring tells about itself; a
 * reader therefore knows that a kind smaller than this belongs to the transport
 * and is not data about the profiled program.
 *
 * \~spanish
 * Los tipos de las MUESTRAS empiezan aqui.  Por debajo, lo que el anillo se
 * cuenta a si mismo; asi un lector sabe que un tipo menor que esto es del
 * transporte y no un dato del programa perfilado.
 */
#define RING_KIND_FIRST_SAMPLE 16u

/* -------------------------------------------------------------------------
 *  \~english  The control block.
 *  \~spanish  El bloque de control.
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * @brief
 * \~english How far apart what each side writes must sit.
 * \~spanish Cuanto hay que separar lo que escribe cada lado.
 * \~
 *
 * \~english
 * `head` is written by the producer and `tail` by the consumer.  In the same
 * cache line, each advance by one INVALIDATES the other's line, and that is
 * paid inside the interrupt handler, which is where it can least be paid.
 * Sixty-four bytes is the line on every current x86-64; more costs nothing,
 * less costs on every sample.
 *
 * \~spanish
 * `head` la escribe el productor y `tail` el consumidor.  En la misma linea de
 * cache, cada avance de uno INVALIDA la linea del otro, y eso se paga dentro
 * del manejador de interrupcion, que es donde menos se puede pagar.  Sesenta y
 * cuatro bytes es la linea en todo x86-64 actual; de mas no cuesta nada, de
 * menos cuesta en cada muestra.
 */
#define RING_CACHE_LINE 64u

/**
 * \~english The ring may overwrite the old: RESERVED, not implemented.
 * \~spanish El anillo puede sobrescribir lo viejo: RESERVADO, sin implementar.
 * \~
 *
 * \~english
 * The flight recorder case -- "let it run and when it blows up dump the last N
 * ms" -- is legitimate and will want it.  Reserving the bit today costs
 * nothing; adding it later would break the published contract.
 *
 * \~spanish
 * El caso de la grabadora de vuelo -- "dejalo correr y cuando reviente vuelca
 * los ultimos N ms" -- es legitimo y lo va a querer.  Reservar el bit hoy no
 * cuesta nada; anadirlo despues seria romper el contrato publicado.
 */
#define RING_FLAG_OVERWRITE 0x00000001u

/**
 * @brief
 * \~english The ring header, shared between the driver and user space.
 * \~spanish La cabecera del anillo, compartida entre el driver y el usuario.
 * \~
 *
 * \~english
 * THE INDICES DO NOT WRAP: they grow forever and the position comes from
 * `index & mask`.  That way "how much is there" is a subtraction -- `head -
 * tail` -- with no special case, and full and empty cannot be confused, which
 * is the classic bug of a ring with wrapped indices.  At 64 bits they do not
 * overflow: writing one gigabyte per second they would take five hundred years.
 *
 * THE SIZE IS A POWER OF TWO so a mask replaces the modulo.  `mask` is
 * published rather than `size` because the mask is what is USED on the hot
 * path, and deriving it on every sample would pay a subtraction per sample to
 * save four bytes once.
 *
 * THE PROTOCOL, which is what makes this work without locks:
 *
 *   producer  writes the record at `data + (head & mask)`, and ONLY THEN
 *             publishes by advancing `head`.  The data write must become
 *             visible before the index write -- a publication barrier -- or the
 *             consumer can see an index promising bytes that are not there yet;
 *   consumer  reads `head`, and ONLY THEN looks at the bytes.  Same barrier the
 *             other way round.  When done, it advances `tail`.
 *
 * One producer per ring, which is what lets `head` be advanced with a plain
 * store instead of an atomic compare-and-swap.
 *
 * \~spanish
 * LOS INDICES NO SE ENVUELVEN: crecen para siempre y la posicion sale de
 * `index & mask`.  Asi "cuanto hay" es una resta -- `head - tail` -- sin ningun
 * caso especial, y lleno y vacio no se confunden, que es el error clasico de un
 * anillo con indices envueltos.  A 64 bits no se desbordan: escribiendo un
 * gigabyte por segundo tardarian quinientos anos.
 *
 * EL TAMANO ES POTENCIA DE DOS para que la mascara sustituya al modulo.  Se
 * publica `mask` y no `size` porque la mascara es lo que se USA en el camino
 * caliente, y derivarla en cada muestra seria pagar una resta por muestra para
 * ahorrar cuatro bytes una vez.
 *
 * EL PROTOCOLO, que es lo que hace que esto funcione sin cerrojos:
 *
 *   productor  escribe el registro en `data + (head & mask)`, y SOLO DESPUES
 *              publica avanzando `head`.  La escritura del dato tiene que ser
 *              visible antes que la del indice -- barrera de publicacion --, o
 *              el consumidor puede ver un indice que promete bytes que aun no
 *              estan;
 *   consumidor lee `head`, y SOLO DESPUES mira los bytes.  Misma barrera al
 *              reves.  Cuando termina, avanza `tail`.
 *
 * Un solo productor por anillo, que es lo que permite avanzar `head` con una
 * escritura simple y no con un intercambio atomico.
 */
typedef struct ring_header {
    abi_sized sized; /**< \~english size of THIS struct, to survive versions \~spanish el tamano de ESTA estructura, para sobrevivir a versiones \~ */
    u32 mask;        /**< \~english data area size minus one; a power of two \~spanish tamano del area de datos menos uno; potencia de dos \~ */
    u32 flags;       /**< \~english `RING_FLAG_*` \~spanish `RING_FLAG_*` \~ */
    u32 cpu;         /**< \~english the logical processor this ring belongs to \~spanish el procesador logico al que pertenece este anillo \~ */
    u32 watermark;   /**< \~english used bytes from which it throttles \~spanish bytes ocupados a partir de los cuales se estrangula \~ */
    u32 _pad;

    /** \~english The producer writes here, and nobody else.
     *  \~spanish El productor escribe aqui, y nadie mas. \~ */
    u8 _producer_pad[RING_CACHE_LINE];
    u64 head; /**< \~english bytes published since forever \~spanish bytes publicados desde siempre \~ */

    /** \~english The consumer writes here, and nobody else.
     *  \~spanish El consumidor escribe aqui, y nadie mas. \~ */
    u8 _consumer_pad[RING_CACHE_LINE];
    u64 tail; /**< \~english bytes consumed since forever \~spanish bytes consumidos desde siempre \~ */
} ring_header;

/**
 * @brief
 * \~english How many bytes are there to read.
 * \~spanish Cuantos bytes hay por leer.
 */
static inline u64 ring_used(const ring_header *r) {
    /* \~english No special case and no comparison: the indices do not wrap, so
     * the subtraction always gives what is there.  That is why they grow
     * forever.
     * \~spanish Sin caso especial y sin comparar: los indices no se envuelven,
     * asi que la resta siempre da lo que hay.  Es el motivo de que crezcan para
     * siempre. \~ */
    return r->head - r->tail;
}

/**
 * @brief
 * \~english How many bytes still fit.
 * \~spanish Cuantos bytes caben todavia.
 */
static inline u64 ring_free(const ring_header *r) {
    return (u64)r->mask + 1u - ring_used(r);
}

/**
 * @brief
 * \~english Is it time to throttle?
 * \~spanish ¿Toca estrangular?
 */
static inline int ring_over_watermark(const ring_header *r) {
    return ring_used(r) >= (u64)r->watermark;
}

/**
 * @brief
 * \~english The data area size, in bytes.
 * \~spanish El tamano del area de datos, en bytes.
 * \~
 *
 * \~english
 * A function and not a field because `mask` already says it: two fields derived
 * from one another are two chances for them to disagree.
 *
 * \~spanish
 * Va como funcion y no como campo porque `mask` ya lo dice: dos campos que se
 * derivan uno del otro son dos oportunidades de que discrepen.
 */
static inline u64 ring_capacity(const ring_header *r) {
    return (u64)r->mask + 1u;
}

/* \~english The header crosses the boundary, so its shape is pinned HERE.  If
 * somebody adds a field in the middle, this stops compiling instead of
 * producing a driver and a consumer that read different fields from the same
 * place.
 * \~spanish La cabecera cruza la frontera, asi que su forma se fija AQUI.  Si
 * alguien anade un campo en medio, esto deja de compilar en vez de producir un
 * driver y un consumidor que leen campos distintos del mismo sitio. \~ */
ABI_ASSERT_FIELD(ring_header, sized, 0);
ABI_ASSERT_FIELD(ring_header, mask, 4);
ABI_ASSERT_FIELD(ring_header, flags, 8);
ABI_ASSERT_FIELD(ring_header, cpu, 12);
ABI_ASSERT_FIELD(ring_header, watermark, 16);

STATIC_ASSERT(sizeof(ring_lost) == 32, "ring_lost must be 32 bytes");
STATIC_ASSERT(sizeof(ring_throttle_rec) == 32,
              "ring_throttle_rec must be 32 bytes");

/* \~english And the kinds the transport reserves must not invade the sample
 * range.
 * \~spanish Y los tipos que se reserva el transporte no pueden invadir los de
 * muestra. \~ */
STATIC_ASSERT(RING_KIND_THROTTLE < RING_KIND_FIRST_SAMPLE,
              "transport kinds must stay below the sample range");

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_COMMON_RING_H */
