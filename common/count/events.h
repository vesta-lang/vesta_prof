/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file count/events.h
 * @brief
 * \~english The architectural events, as data with their encoding.
 * \~spanish Los eventos arquitectonicos, como datos y con su codificacion.
 * \~
 *
 * \~english
 * WHAT AN ARCHITECTURAL EVENT IS.  A processor counts hundreds of things, and
 * almost all of them are named differently on each microarchitecture: the code
 * that counts cache misses on one part counts something else on the next.  Intel
 * carves out a handful and PROMISES they will keep their meaning across
 * generations; those are the architectural ones, and they are the only ones a
 * portable profiler can program without a table per part.
 *
 * WHY THE ENCODING IS A NUMBER HERE AND A STRING NOWHERE.  The pair (event,
 * umask) is what gets written into the selector to ask for the event.  The
 * capability report used to carry it as text, to print it, and this file was
 * about to carry it again as numbers, to program it.  Two copies of the same
 * fact drift: the day one gets corrected, the other keeps lying, and nothing
 * fails -- the counter simply counts something else.  So there is ONE table, it
 * holds numbers, and the report formats them.
 *
 * THIRTEEN, NOT SEVEN.  The seven everybody knows are the classic ones; the
 * manual enumerates six more (the full topdown breakdown and the LBR
 * insertions).  Six of those thirteen have NO encoding here, and that is
 * deliberate: topdown is not read through an event selector but through the
 * metrics register, and the LBR insertions are microarchitecture-specific.
 * Writing a plausible number in those slots would be exactly the mistake this
 * table exists to avoid.  `ARCH_EVENT_ENCODED` says which ones can be armed.
 *
 * AVAILABILITY IS NOT IN THIS TABLE, and cannot be.  Which of the thirteen a
 * given part actually has is answered by `CPUID.0AH`, per processor, and on a
 * hybrid part a P core and an E core do not answer the same.  This table says
 * what the events ARE; `arch_event_present` asks whether this processor has one.
 *
 * \~spanish
 * QUE ES UN EVENTO ARQUITECTONICO.  Un procesador cuenta cientos de cosas, y
 * casi todas se llaman distinto en cada microarquitectura: el codigo que cuenta
 * fallos de cache en una pieza cuenta otra cosa en la siguiente.  Intel aparta
 * un punado y PROMETE que conservaran su significado entre generaciones; esos
 * son los arquitectonicos, y son los unicos que un perfilador portable puede
 * programar sin una tabla por pieza.
 *
 * POR QUE LA CODIFICACION ES UN NUMERO AQUI Y UNA CADENA EN NINGUN SITIO.  La
 * pareja (evento, umask) es lo que se escribe en el selector para pedir el
 * evento.  El informe de capacidades la llevaba como texto, para imprimirla, y
 * este fichero estaba a punto de llevarla otra vez como numeros, para
 * programarla.  Dos copias del mismo hecho se separan: el dia que se corrige
 * una, la otra sigue mintiendo, y no falla nada -- simplemente el contador
 * cuenta otra cosa.  Asi que hay UNA tabla, tiene numeros, y el informe los
 * formatea.
 *
 * TRECE, NO SIETE.  Los siete que todo el mundo conoce son los clasicos; el
 * manual enumera seis mas (el reparto de topdown al completo y las inserciones
 * de LBR).  Seis de esos trece NO llevan codificacion aqui, y es deliberado: el
 * topdown no se lee por un selector de evento sino por el registro de metricas,
 * y las inserciones de LBR son propias de cada microarquitectura.  Escribir un
 * numero plausible en esas ranuras seria justo el fallo que esta tabla existe
 * para no cometer.  `ARCH_EVENT_ENCODED` dice cuales se pueden armar.
 *
 * LA DISPONIBILIDAD NO ESTA EN ESTA TABLA, ni puede estarlo.  Cuales de los
 * trece tiene una pieza concreta lo responde `CPUID.0AH`, por procesador, y en
 * una pieza hibrida un nucleo P y uno E no responden lo mismo.  Esta tabla dice
 * que SON los eventos; `arch_event_present` pregunta si este procesador tiene
 * uno.
 */

#ifndef VXP_COMMON_COUNT_EVENTS_H
#define VXP_COMMON_COUNT_EVENTS_H

#include "vxp_base.h"

/** @brief
 *  \~english The pair (event, umask) is known: this event can be armed.
 *  \~spanish La pareja (evento, umask) se conoce: este evento se puede
 *            armar. \~ */
#define ARCH_EVENT_ENCODED 0x01u

/**
 * @brief
 * \~english How many the manual enumerates, and the order of `CPUID.0AH:EBX`.
 * \~spanish Cuantos enumera el manual, y el orden de `CPUID.0AH:EBX`.
 * \~
 *
 * \~english
 * The position in the table IS the bit position in EBX.  That is not a
 * convenience: it is how the processor names them, and reordering the table
 * would silently ask about the wrong event.
 *
 * \~spanish
 * La posicion en la tabla ES la posicion del bit en EBX.  No es una comodidad:
 * es como los nombra el procesador, y reordenar la tabla preguntaria en silencio
 * por el evento equivocado.
 */
#define ARCH_EVENT_MAX 13

/**
 * @brief
 * \~english One architectural event: what it is called and how it is asked for.
 * \~spanish Un evento arquitectonico: como se llama y como se pide.
 */
typedef struct arch_event {
    const char *name; /**< \~english short name, for a column \~spanish nombre corto, para una columna \~ */
    const char *text; /**< \~english what it counts, in prose \~spanish que cuenta, en prosa \~ */
    u8 event;         /**< \~english Event Select: selector bits 7:0 \~spanish Event Select: bits 7:0 del selector \~ */
    u8 umask;         /**< \~english Unit Mask: selector bits 15:8 \~spanish Unit Mask: bits 15:8 del selector \~ */
    u8 flags;         /**< `ARCH_EVENT_*` */
    u8 _pad;
} arch_event;

/** @brief
 *  \~english The thirteen, in EBX's bit order.
 *  \~spanish Los trece, en el orden de los bits de EBX. \~ */
extern const arch_event arch_events[ARCH_EVENT_MAX];

/**
 * @brief
 * \~english Does THIS processor have event number `i`?
 * \~spanish ¿Tiene ESTE procesador el evento numero `i`?
 * \~
 *
 * @param listed \~english `CPUID.0AH:EAX[31:24]`: how many bits the map has \~spanish `CPUID.0AH:EAX[31:24]`: cuantos bits tiene el mapa \~
 * @param absent \~english `CPUID.0AH:EBX`: bit SET = NOT available \~spanish `CPUID.0AH:EBX`: bit PUESTO = NO disponible \~
 * @param i      \~english which event, in table order \~spanish que evento, en el orden de la tabla \~
 * @return \~english non-zero if it can be counted \~spanish distinto de cero si se puede contar \~
 *
 * \~english
 * TWO CONDITIONS, AND THE FIRST ONE IS THE ONE THAT GETS FORGOTTEN.  The
 * inverted map is famous -- a bit SET means NOT available, which reads backwards
 * the first three times.  What gets missed is that the map only has as many bits
 * as `EAX[31:24]` says: on this machine that is seven, so events 7 to 12 are not
 * absent, they are simply NOT ENUMERATED, and their EBX bit is zero for the same
 * reason any bit beyond the end is zero.  Reading the map alone would conclude
 * the part has topdown slots.
 *
 *      EAX[31:24] = 7          EBX = ....0000000
 *                                       |_____|
 *                                       these seven are the map
 *      event 7 (SLOTS):  EBX bit 7 is zero, but it is OUTSIDE the map
 *                        -> not enumerated, NOT "available"
 *
 * \~spanish
 * DOS CONDICIONES, Y LA QUE SE OLVIDA ES LA PRIMERA.  El mapa invertido es
 * famoso -- un bit PUESTO quiere decir NO disponible, que se lee al reves las
 * tres primeras veces --.  Lo que se escapa es que el mapa solo tiene tantos
 * bits como diga `EAX[31:24]`: en esta maquina son siete, asi que los eventos 7
 * a 12 no estan ausentes, es que NO ESTAN ENUMERADOS, y su bit de EBX vale cero
 * por la misma razon por la que vale cero cualquier bit pasado el final.  Mirar
 * solo el mapa concluiria que la pieza tiene ranuras de topdown.
 */
static inline int arch_event_present(u32 listed, u32 absent, u32 i) {
    if (i >= listed) {
        return 0;
    }
    return ((absent >> i) & 1u) == 0u;
}

#endif /* VXP_COMMON_COUNT_EVENTS_H */
