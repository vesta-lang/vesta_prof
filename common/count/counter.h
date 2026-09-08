/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file count/counter.h
 * @brief
 * \~english Arming, reading and disarming the general-purpose counters.
 * \~spanish Armar, leer y desarmar los contadores de proposito general.
 * \~
 *
 * \~english
 * COUNTING IS NOT SAMPLING, and this file only does the first.  Counting is
 * "how many instructions retired between here and there": one number per
 * processor, exact, with no interrupt in the middle.  Sampling is "where were we
 * every N events", which needs the counter to overflow and interrupt, and that
 * is another file.  Counting comes first because it is the floor: if the number
 * is not right, no sample built on top of it is either.
 *
 * WHAT A COUNTER IS.  Three registers per counter, and they only mean something
 * together:
 *
 *      IA32_PERFEVTSELx   WHAT is counted, for whom, and whether it is running
 *      IA32_PMCx          the count itself
 *      IA32_PERF_GLOBAL_CTRL  one bit per counter, ANDed with the selector's
 *
 * The last one is the reason there is a global register at all: it is the ONE
 * write that starts or stops everything at once, so that several counters
 * measure exactly the same window instead of a slightly different one each.
 *
 *      counting happens  <=>  GLOBAL_CTRL bit i   AND   PERFEVTSELi.EN
 *
 * THE STATE IS SAVED AND PUT BACK.  The PMU is one resource for the whole
 * machine and there is no arbiter that will stop us stepping on somebody: what
 * we can do is not leave anything moved.  `counter_state` is what was there
 * before, and `counter_disarm` writes it back.  It is also why arming refuses
 * when someone else already has counters running -- overwriting them would give
 * both of us numbers that are not right, silently.
 *
 * IT KNOWS NO SYSTEM.  The MSR read and write come in through `msr_ops`, the
 * same seam `probe/dump.c` uses: inside the kernel they are the real
 * instruction, in the tests a plain array.  That is what allows checking the
 * arm/disarm sequence -- the part where a wrong ORDER does not fail, it just
 * counts something else -- without loading a driver.
 *
 * \~spanish
 * CONTAR NO ES MUESTREAR, y este fichero solo hace lo primero.  Contar es
 * "cuantas instrucciones se retiraron entre aqui y alli": un numero por
 * procesador, exacto, sin ninguna interrupcion por medio.  Muestrear es "donde
 * estabamos cada N eventos", que necesita que el contador se desborde e
 * interrumpa, y eso es otro fichero.  Contar va primero porque es el suelo: si
 * el numero no es correcto, ninguna muestra construida encima lo es.
 *
 * QUE ES UN CONTADOR.  Tres registros por contador, y solo significan algo
 * juntos:
 *
 *      IA32_PERFEVTSELx   QUE se cuenta, de quien, y si esta en marcha
 *      IA32_PMCx          la cuenta en si
 *      IA32_PERF_GLOBAL_CTRL  un bit por contador, en Y logica con el del
 *                             selector
 *
 * El ultimo es la razon de que exista un registro global: es la UNICA escritura
 * que arranca o para todo a la vez, para que varios contadores midan exactamente
 * la misma ventana y no cada uno una ligeramente distinta.
 *
 *      cuenta  <=>  bit i de GLOBAL_CTRL   Y   PERFEVTSELi.EN
 *
 * EL ESTADO SE GUARDA Y SE DEVUELVE.  El PMU es un recurso de la maquina entera
 * y no hay arbitro que impida pisar a alguien: lo que si podemos es no dejar
 * nada movido.  `counter_state` es lo que habia antes, y `counter_disarm` lo
 * vuelve a escribir.  Es tambien el motivo de que armar se niegue cuando ya hay
 * contadores de otro en marcha -- sobreescribirlos nos daria a los dos numeros
 * que no son, en silencio.
 *
 * NO CONOCE NINGUN SISTEMA.  La lectura y la escritura de MSR entran por
 * `msr_ops`, la misma costura que usa `probe/dump.c`: dentro del kernel son la
 * instruccion de verdad, en las pruebas un array corriente.  Es lo que permite
 * comprobar la secuencia de armado y desarmado -- la parte donde un ORDEN
 * equivocado no falla, simplemente cuenta otra cosa -- sin cargar un driver.
 */

#ifndef VXP_COMMON_COUNT_COUNTER_H
#define VXP_COMMON_COUNT_COUNTER_H

#include "vxp_base.h"

#include "count/events.h"
#include "probe/pmu_caps.h"

/**
 * @brief
 * \~english The layout of `IA32_PERFEVTSELx`, from SDM Figure 22-1.
 * \~spanish El reparto de `IA32_PERFEVTSELx`, de la figura 22-1 del manual.
 * \~
 *
 * @code
 *   63    32 31    24 23 22 21 20 19 18 17 16 15       8 7        0
 *  +--------+--------+--+--+--+--+--+--+--+--+----------+----------+
 *  |reserved| CMASK  |IN|EN|AN|IN|PC| E|OS|US|  UMASK   |  EVENT   |
 *  |        |        | V|  | Y| T|  |  |  | R|          |  SELECT  |
 *  +--------+--------+--+--+--+--+--+--+--+--+----------+----------+
 * @endcode
 *
 * \~english
 * EVENT and UMASK are the pair that names the event -- they come from
 * `count/events.h` and nowhere else.  USR and OS say at which privilege level
 * the event counts, and they are what separates "what the program did" from
 * "what the kernel did on its behalf".  EN is the switch, and INT is the one
 * that turns counting into sampling: with it set, the overflow raises an
 * interrupt through the local APIC.  It is not used here.
 *
 * \~spanish
 * EVENT y UMASK son la pareja que nombra el evento -- salen de `count/events.h`
 * y de ningun otro sitio.  USR y OS dicen en que nivel de privilegio cuenta el
 * evento, y son lo que separa "lo que hizo el programa" de "lo que hizo el
 * nucleo por su cuenta".  EN es el interruptor, e INT es el que convierte contar
 * en muestrear: con el puesto, el desbordamiento levanta una interrupcion por el
 * APIC local.  Aqui no se usa.
 */
#define PERFEVTSEL_EVENT_SHIFT 0u
#define PERFEVTSEL_UMASK_SHIFT 8u
#define PERFEVTSEL_USR (1u << 16)   /**< \~english count at CPL 1, 2 or 3 \~spanish cuenta en CPL 1, 2 o 3 \~ */
#define PERFEVTSEL_OS (1u << 17)    /**< \~english count at CPL 0 \~spanish cuenta en CPL 0 \~ */
#define PERFEVTSEL_EDGE (1u << 18)  /**< \~english count transitions, not time \~spanish cuenta transiciones, no tiempo \~ */
#define PERFEVTSEL_PC (1u << 19)    /**< \~english pin control; reserved since Sandy Bridge \~spanish control de patilla; reservado desde Sandy Bridge \~ */
#define PERFEVTSEL_INT (1u << 20)   /**< \~english interrupt on overflow: this is sampling \~spanish interrumpe al desbordar: esto es muestrear \~ */
#define PERFEVTSEL_ANY (1u << 21)   /**< \~english count both threads of the core \~spanish cuenta los dos hilos del nucleo \~ */
#define PERFEVTSEL_EN (1u << 22)    /**< \~english the switch \~spanish el interruptor \~ */
#define PERFEVTSEL_INV (1u << 23)   /**< \~english invert the counter mask comparison \~spanish invierte la comparacion de la mascara \~ */
#define PERFEVTSEL_CMASK_SHIFT 24u

/**
 * @brief
 * \~english How many counters this module will program at most.
 * \~spanish Cuantos contadores programa este modulo como mucho.
 * \~
 *
 * \~english
 * The architecture allows up to eight general-purpose ones per logical
 * processor.  How many there really are is said by `CPUID.0AH:EAX[15:8]`, per
 * processor, and it is NOT always eight: on this machine it is six, because
 * there is a hypervisor underneath that keeps some for itself.  The number is
 * asked for, never assumed; this constant only sizes the arrays.
 *
 * \~spanish
 * La arquitectura permite hasta ocho de proposito general por procesador logico.
 * Cuantos hay de verdad lo dice `CPUID.0AH:EAX[15:8]`, por procesador, y NO
 * siempre son ocho: en esta maquina son seis, porque hay un hipervisor debajo
 * que se queda algunos.  El numero se pregunta, nunca se supone; esta constante
 * solo dimensiona los arrays.
 */
#define COUNTER_MAX 8u

/**
 * @brief
 * \~english How to read and write an MSR, and with what context.
 * \~spanish Como leer y escribir un MSR, y con que contexto.
 * \~
 *
 * \~english
 * It travels as ONE struct and not as two loose pointers because reading and
 * writing always go together here: a sequence that reads with one path and
 * writes with another is not a sequence, it is two halves of different
 * measurements.
 *
 * \~spanish
 * Viaja como UNA estructura y no como dos punteros sueltos porque leer y
 * escribir van siempre juntos aqui: una secuencia que lee por un camino y
 * escribe por otro no es una secuencia, son dos mitades de medidas distintas.
 */
typedef struct msr_ops {
    status (*read)(u32 addr, u64 *out, void *ctx);
    status (*write)(u32 addr, u64 value, void *ctx);
    void *ctx;
} msr_ops;

/**
 * @brief
 * \~english What was there before, and what we put in its place.
 * \~spanish Lo que habia antes, y lo que pusimos en su lugar.
 * \~
 *
 * \~english
 * `saved` and `armed` are DIFFERENT counts and the difference matters.  `saved`
 * is how many counters we managed to read before touching anything, and it is
 * what `counter_disarm` puts back -- if arming failed halfway, it is still the
 * right number to restore.  `armed` is how many we ended up programming, which
 * is at most as many as there were events to arm.
 *
 * \~spanish
 * `saved` y `armed` son cuentas DISTINTAS y la diferencia importa.  `saved` es
 * cuantos contadores conseguimos leer antes de tocar nada, y es lo que
 * `counter_disarm` devuelve a su sitio -- si el armado fallo a medias, sigue
 * siendo el numero correcto que restaurar.  `armed` es cuantos acabamos
 * programando, que son como mucho tantos como eventos hubiera que armar.
 */
typedef struct counter_state {
    u64 global_ctrl;           /**< \~english `IA32_PERF_GLOBAL_CTRL` as found \~spanish `IA32_PERF_GLOBAL_CTRL` tal como estaba \~ */
    u64 evtsel[COUNTER_MAX];   /**< \~english the selectors as found \~spanish los selectores tal como estaban \~ */
    u64 pmc[COUNTER_MAX];      /**< \~english the counts as found \~spanish las cuentas tal como estaban \~ */
    u32 saved;                 /**< \~english how many were read before touching \~spanish cuantos se leyeron antes de tocar \~ */
    u32 armed;                 /**< \~english how many were programmed \~spanish cuantos se programaron \~ */
    u32 qualifiers;            /**< \~english the `PERFEVTSEL_*` that were asked for \~spanish los `PERFEVTSEL_*` que se pidieron \~ */
    u32 counter_msr;           /**< \~english base of the counters: `IA32_PMC0` or its full-width alias \~spanish base de los contadores: `IA32_PMC0` o su alias de ancho completo \~ */
    u8 event[COUNTER_MAX];     /**< \~english index into `arch_events` per armed counter \~spanish indice en `arch_events` por contador armado \~ */
} counter_state;

/**
 * @brief
 * \~english Programs as many architectural events as there are counters.
 * \~spanish Programa tantos eventos arquitectonicos como contadores haya.
 * \~
 *
 * @param caps       \~english what this processor can do, already detected \~spanish lo que puede este procesador, ya detectado \~
 * @param qualifiers \~english `PERFEVTSEL_USR`, `PERFEVTSEL_OS`... ORed \~spanish `PERFEVTSEL_USR`, `PERFEVTSEL_OS`... en O logica \~
 * @param ops        \~english how to reach the MSRs \~spanish como llegar a los MSR \~
 * @param st         \~english where the previous state is left \~spanish donde queda el estado anterior \~
 * @return \~english `OK`; `ERR_UNSUPPORTED` if there is no usable PMU or somebody else is counting \~spanish `OK`; `ERR_UNSUPPORTED` si no hay PMU utilizable o ya cuenta otro \~
 *
 * \~english
 * MUST BE CALLED ON THE PROCESSOR IT IS PROGRAMMING.  An MSR write reaches the
 * logical processor executing it and no other; calling this without having
 * pinned first programs whichever core the scheduler felt like, and the caller
 * then reads a number belonging to somebody else.  The caller pins, because
 * pinning is the system's business and this file knows no system.
 *
 * IT REFUSES INSTEAD OF SHARING.  If `caps->counters_in_use` says somebody
 * already has selectors armed, nothing is touched and `ERR_UNSUPPORTED` comes
 * back.  Programming on top would not fail: both parties would simply read wrong
 * numbers, which is the failure mode this whole project exists to not have.
 *
 * \~spanish
 * HAY QUE LLAMARLA EN EL PROCESADOR QUE PROGRAMA.  Una escritura de MSR llega al
 * procesador logico que la ejecuta y a ningun otro; llamar a esto sin haberse
 * fijado antes programa el nucleo que al planificador le apeteciera, y el
 * llamante lee despues un numero que es de otro.  Fija el llamante, porque
 * fijarse es cosa del sistema y este fichero no conoce ninguno.
 *
 * SE NIEGA EN VEZ DE COMPARTIR.  Si `caps->counters_in_use` dice que ya hay
 * selectores armados de alguien, no se toca nada y vuelve `ERR_UNSUPPORTED`.
 * Programar encima no fallaria: simplemente los dos leeriamos numeros
 * equivocados, que es el modo de fallo que este proyecto entero existe para no
 * tener.
 */
status counter_arm(const pmu_caps *caps, u32 qualifiers, const msr_ops *ops,
                   counter_state *st);

/**
 * @brief
 * \~english Reads the armed counters, in the order they were armed.
 * \~spanish Lee los contadores armados, en el orden en que se armaron.
 * \~
 *
 * @param st  \~english what `counter_arm` left \~spanish lo que dejo `counter_arm` \~
 * @param ops \~english how to reach the MSRs \~spanish como llegar a los MSR \~
 * @param out \~english at least `st->armed` values \~spanish al menos `st->armed` valores \~
 * @return `OK` / `ERR_INVALID` / \~english whatever the read returned \~spanish lo que devolviera la lectura \~
 *
 * \~english
 * READING DOES NOT STOP THE COUNT, deliberately.  The counters keep running, so
 * reading twice gives two points of the same measurement instead of two
 * measurements.  Whoever wants a closed window stops first by disarming.
 *
 * \~spanish
 * LEER NO PARA LA CUENTA, a proposito.  Los contadores siguen corriendo, asi que
 * leer dos veces da dos puntos de la misma medida en vez de dos medidas.  Quien
 * quiera una ventana cerrada para antes desarmando.
 */
status counter_read(const counter_state *st, const msr_ops *ops, u64 *out);

/**
 * @brief
 * \~english Stops the counters and puts back what was there.
 * \~spanish Para los contadores y devuelve a su sitio lo que habia.
 * \~
 *
 * @param st  \~english what `counter_arm` left \~spanish lo que dejo `counter_arm` \~
 * @param ops \~english how to reach the MSRs \~spanish como llegar a los MSR \~
 * @return `OK` / `ERR_INVALID` / \~english whatever the write returned \~spanish lo que devolviera la escritura \~
 *
 * \~english
 * IT IS SAFE TO CALL AFTER A FAILED ARM: it restores `saved` counters, which is
 * exactly how many were read before anything was touched.  On a state that is
 * all zeros it does nothing, which is what a `DriverUnload` that never armed
 * needs.
 *
 * \~spanish
 * ES SEGURO LLAMARLA DESPUES DE UN ARMADO FALLIDO: restaura `saved` contadores,
 * que son exactamente los que se leyeron antes de tocar nada.  Sobre un estado a
 * ceros no hace nada, que es lo que necesita un `DriverUnload` que no llego a
 * armar.
 */
status counter_disarm(const counter_state *st, const msr_ops *ops);

#endif /* VXP_COMMON_COUNT_COUNTER_H */
