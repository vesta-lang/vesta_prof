/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file probe/pmu_caps.h
 * @brief
 * \~english What THIS logical processor's PMU can do, and how certainly.
 * \~spanish Que sabe hacer el PMU de ESTE procesador logico, y con que certeza.
 * \~
 *
 * \~english
 * THE PMU is the performance monitoring unit: a handful of counters inside the
 * processor that count events -- cycles, retired instructions, cache misses --
 * and can raise an interrupt when they overflow.  That interrupt is what turns
 * counting into sampling.
 *
 * This is the profiler's first question and the one that orders everything else:
 * without knowing which counters exist, whether they are precise and whether
 * somebody else is using them, any number published afterwards is a guess with
 * formatting.
 *
 * TWO SOURCES THAT ARE NOT WORTH THE SAME.  CPUID is asked first because it is
 * cheap, but **under a hypervisor it is not a read of the silicon**: every
 * `cpuid` leaves through a VM exit and the answers are put there by the
 * hypervisor.  The MSRs are the second opinion, and specifically
 * `IA32_MISC_ENABLE` bit 12 is the authoritative word on PEBS above whatever
 * CPUID announces.
 *
 * Hence the `PMU_PEBS_MASKED_BY_CPUID` verdict, which at first sight looks like
 * an oddity and is the case that motivated writing this: **CPUID says there is
 * no Debug Store and the MSR says PEBS is there**.  Without separating the two
 * sources, that case gets filed as "this machine cannot" and one stops measuring
 * well out of believing an announcement.
 *
 * CERTAINTY TRAVELS INSIDE THE FACT.  Every value coming out of an MSR goes with
 * the `status` of ITS read.  That is not ceremony: a zero that was read and a
 * zero because it could not be read send you to different places -- one to carry
 * on, the other to find out why -- and a struct that conflates them makes it
 * impossible to tell them apart downstream.
 *
 * THIS MODULE DOES NOT SET AFFINITY.  It describes the processor where it is
 * called, and the caller decides where.  That is what lets the driver walk it
 * core by core -- which is how the difference between a P and an E shows -- with
 * `common/` not having to know such a thing as pinning a thread exists.
 *
 * \~spanish
 * EL PMU es la unidad de monitorizacion de rendimiento: un punado de contadores
 * dentro del procesador que cuentan eventos -- ciclos, instrucciones retiradas,
 * fallos de cache -- y pueden levantar una interrupcion al desbordarse.  Esa
 * interrupcion es lo que convierte contar en muestrear.
 *
 * Es la primera pregunta del perfilador y la que ordena todo lo demas: sin saber
 * que contadores hay, si son precisos y si alguien mas los esta usando,
 * cualquier numero que se publique despues es una suposicion con formato.
 *
 * DOS FUENTES QUE NO VALEN LO MISMO.  CPUID se pregunta primero porque es
 * barato, pero **bajo un hipervisor no es una lectura del silicio**: cada
 * `cpuid` sale por una salida de VM y las respuestas las pone el hipervisor.
 * Los MSR son la segunda opinion, y en concreto `IA32_MISC_ENABLE` bit 12 es la
 * palabra autorizada sobre PEBS por encima de lo que anuncie CPUID.
 *
 * De ahi el veredicto `PMU_PEBS_MASKED_BY_CPUID`, que a primera vista parece una
 * rareza y es el caso que motivo escribir esto: **CPUID dice que no hay Debug
 * Store y el MSR dice que PEBS si esta**.  Sin separar las dos fuentes, ese caso
 * se archiva como "esta maquina no puede" y se deja de medir bien por creerse un
 * anuncio.
 *
 * LA CERTEZA VIAJA DENTRO DEL HECHO.  Cada valor que sale de un MSR va con el
 * `status` de SU lectura.  No es ceremonia: un cero leido y un cero por no haber
 * podido leer mandan a sitios distintos -- uno a seguir, el otro a buscar por
 * que --, y un struct que los confunda hace imposible distinguirlos aguas abajo.
 *
 * ESTE MODULO NO FIJA AFINIDAD.  Describe el procesador donde se le llame, y
 * quien llama decide donde.  Es lo que permite que el driver lo recorra nucleo a
 * nucleo -- que es como se ve la diferencia entre un P y un E -- sin que
 * `common/` tenga que saber que existe tal cosa como fijar un hilo.
 */

#ifndef VXP_COMMON_PROBE_PMU_CAPS_H
#define VXP_COMMON_PROBE_PMU_CAPS_H

#include "vxp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief
 *  \~english Core class, from CPUID.1A EAX[31:24].
 *  \~spanish Clase de nucleo, de CPUID.1A EAX[31:24]. \~ */
typedef enum core_class {
    /**< \~english not a hybrid part, or it does not say
     *   \~spanish no es una pieza hibrida, o no lo dice \~ */
    CORE_CLASS_UNKNOWN = 0,
    CORE_CLASS_ATOM = 0x20, /**< E-core */
    CORE_CLASS_CORE = 0x40  /**< P-core */
} core_class;

/** @brief
 *  \~english Vendor, which decides which MSRs make sense.
 *  \~spanish Fabricante, que decide que MSR tienen sentido. \~ */
typedef enum cpu_vendor {
    CPU_VENDOR_UNKNOWN = 0,
    CPU_VENDOR_INTEL = 1,
    CPU_VENDOR_AMD = 2
} cpu_vendor;

/**
 * @brief
 * \~english What can be said about PEBS, and on what grounds.
 * \~spanish Que se puede decir de PEBS, y con que fundamento.
 * \~
 *
 * \~english
 * PEBS -- Precise Event Based Sampling -- is the mechanism by which the hardware
 * itself writes the record, instead of the interrupt handler reading the state
 * afterwards.  That is what removes the skid: the address in the record is the
 * instruction that caused the event, not one somewhere nearby.
 *
 * The five are different answers, and the temptation to merge the last two into
 * a "no" is exactly the mistake this module exists to avoid.
 *
 * \~spanish
 * PEBS -- Precise Event Based Sampling -- es el mecanismo por el que el propio
 * hardware escribe el registro, en vez de que el manejador de la interrupcion
 * lea el estado despues.  Eso es lo que quita la desviacion: la direccion del
 * registro es la instruccion que causo el evento, no una de por ahi cerca.
 *
 * Los cinco son respuestas distintas, y la tentacion de juntar las dos ultimas
 * en un "no" es exactamente el error que este modulo existe para evitar.
 */
typedef enum pebs_verdict {
    /**< \~english could not be determined: the read is missing
     *   \~spanish no se pudo determinar: falta la lectura \~ */
    PMU_PEBS_UNKNOWN = 0,
    /**< \~english CPUID and MSR agree that yes
     *   \~spanish CPUID y MSR de acuerdo en que si \~ */
    PMU_PEBS_AVAILABLE,
    /**< \~english CPUID says no, the MSR says yes
     *   \~spanish CPUID dice que no, el MSR dice que si \~ */
    PMU_PEBS_MASKED_BY_CPUID,
    /**< \~english the MSR says no.  This one IS a no
     *   \~spanish el MSR dice que no.  Esto si es un no \~ */
    PMU_PEBS_UNAVAILABLE,
    /**< \~english not Intel: on AMD the mechanism is IBS
     *   \~spanish no es Intel: en AMD el mecanismo es IBS \~ */
    PMU_PEBS_NOT_APPLICABLE
} pebs_verdict;

/**
 * @brief
 * \~english An MSR value with the status of its read attached.
 * \~spanish Un valor de MSR con el estado de su lectura pegado.
 * \~
 *
 * \~english
 * They travel together and not as two loose fields so that one cannot be read
 * without the other by accident.  `value` only means something if `rc == OK`.
 *
 * \~spanish
 * Van juntos y no en dos campos sueltos para que no se pueda leer uno sin el
 * otro por descuido.  `value` solo significa algo si `rc == OK`.
 */
typedef struct msr_value {
    /**< \~english what was read; meaningless if `rc != OK`
     *   \~spanish lo leido; sin sentido si `rc != OK` \~ */
    u64 value;
    /**< \~english how the read went \~spanish como fue la lectura \~ */
    status rc;
} msr_value;

/**
 * @brief
 * \~english Everything known about one logical processor's PMU.
 * \~spanish Todo lo que se sabe del PMU de un procesador logico.
 * \~
 *
 * \~english
 * TRANSPARENT on purpose.  The directory's rule is "opaque by default", and the
 * exception is what only carries data: there is no invariant to protect here --
 * it is the result of a measurement, and whoever receives it has to be able to
 * read it field by field to dump it.  See `common/README.md`.
 *
 * \~spanish
 * TRANSPARENTE a proposito.  La regla del directorio es "opaco por defecto", y
 * la excepcion es lo que solo transporta datos: aqui no hay invariante que
 * proteger -- es el resultado de una medida, y quien lo recibe tiene que poder
 * leer campo a campo para volcarlo.  Ver `common/README.md`.
 */
typedef struct pmu_caps {
    /* --- \~english where this measurement came from
     *     \~spanish de donde salio esta medida \~ ------------------------- */
    u32 cpu_index; /**< \~english logical processor the detection ran on \~spanish procesador logico donde se ejecuto la deteccion \~ */
    u32 core_type; /**< \~english `core_class`; 0 if the part is not hybrid \~spanish `core_class`; 0 si la pieza no es hibrida \~ */
    u32 vendor;    /**< `cpu_vendor` */
    u32 family;    /**< \~english displayed family, extended already added \~spanish familia mostrada, ya sumada la extendida \~ */
    u32 model;     /**< \~english displayed model, already composed with the extended one \~spanish modelo mostrado, ya compuesto con el extendido \~ */
    u32 stepping;

    /* --- \~english architectural PMU, from CPUID.0A
     *     \~spanish PMU arquitectonico, de CPUID.0A \~ -------------------- */
    u32 pmu_version;        /**< \~english 0 = no architectural PMU \~spanish 0 = no hay PMU arquitectonico \~ */
    u32 gp_counters;        /**< \~english general purpose counters per thread \~spanish contadores de proposito general por hilo \~ */
    u32 gp_width;           /**< \~english bits of each one \~spanish bits de cada uno \~ */
    u32 fixed_counters;     /**< \~english fixed counters \~spanish contadores fijos \~ */
    u32 fixed_width;        /**< \~english bits of each one \~spanish bits de cada uno \~ */
    u32 arch_events_listed; /**< \~english how many bits the EBX map enumerates \~spanish cuantos bits enumera el mapa de EBX \~ */
    u32 arch_events_absent; /**< \~english EBX map: bit SET = NOT available \~spanish mapa de EBX: bit PUESTO = NO disponible \~ */
    u32 fixed_ctr_bitmap;   /**< \~english ECX; from PMU v5 onwards \~spanish ECX; desde PMU v5 \~ */

    /* --- \~english CPUID flags  \~spanish banderas de CPUID \~ ----------- */
    u8 has_ds;            /**< \~english Debug Store: without it there is no PEBS \~spanish Debug Store: sin el, no hay PEBS \~ */
    u8 has_pdcm;          /**< \~english IA32_PERF_CAPABILITIES is readable \~spanish IA32_PERF_CAPABILITIES es legible \~ */
    u8 has_hypervisor;    /**< \~english something underneath: CPUID stops being trustworthy \~spanish hay algo debajo: CPUID deja de ser fiable \~ */
    u8 has_hybrid;        /**< \~english the part mixes P and E cores \~spanish la pieza mezcla nucleos P y E \~ */
    u8 has_invariant_tsc; /**< \~english the TSC does not change pace \~spanish el TSC no cambia de ritmo \~ */
    u8 has_ibs;           /**< \~english AMD: Instruction Based Sampling \~spanish AMD: Instruction Based Sampling \~ */
    u8 has_hw_feedback;   /**< \~english APERF and MPERF exist \~spanish existen APERF y MPERF \~ */
    u8 has_rdtscp;        /**< \~english `rdtscp` exists, and with it IA32_TSC_AUX \~spanish existe `rdtscp`, y con el IA32_TSC_AUX \~ */

    /* --- \~english what CPUID cannot contradict: the MSRs
     *     \~spanish lo que CPUID no puede desmentir: los MSR \~ ----------- */
    msr_value misc_enable;       /**< IA32_MISC_ENABLE */
    msr_value perf_capabilities; /**< \~english IA32_PERF_CAPABILITIES, if PDCM \~spanish IA32_PERF_CAPABILITIES, si hay PDCM \~ */
    msr_value debugctl;          /**< \~english IA32_DEBUGCTL: the LBR hangs off here \~spanish IA32_DEBUGCTL: de aqui cuelga el LBR \~ */
    msr_value fixed_ctr_ctrl;    /**< IA32_FIXED_CTR_CTRL */
    msr_value perf_global_ctrl;  /**< IA32_PERF_GLOBAL_CTRL */

    /* --- \~english derived from IA32_MISC_ENABLE
     *     \~spanish lo derivado de IA32_MISC_ENABLE \~ ------------------- */
    u8 perfmon_available;    /**< \~english bit 7: monitoring is possible \~spanish bit 7: se puede monitorizar \~ */
    u8 bts_unavailable;      /**< \~english bit 11: no Branch Trace Store \~spanish bit 11: sin Branch Trace Store \~ */
    u8 pebs_unavailable_bit; /**< \~english bit 12: the authoritative word on PEBS \~spanish bit 12: la palabra autorizada sobre PEBS \~ */

    /* --- \~english derived from IA32_PERF_CAPABILITIES
     *
     * It is decoded WHOLE and not only the field that was needed, because
     * reading a register and keeping one bit is how you arrive at loose
     * conclusions: that BTS and PEBS BOTH fall is an explanation -- both hang
     * off the Debug Store -- whereas "bit 12 says no" is an orphan fact.
     *
     *     \~spanish lo derivado de IA32_PERF_CAPABILITIES
     *
     * Se decodifica ENTERO y no solo el campo que hacia falta, porque leer un
     * registro y quedarse con un bit es como se llega a conclusiones sueltas:
     * que BTS y PEBS caigan LOS DOS es una explicacion -- los dos cuelgan del
     * Debug Store --, mientras que "el bit 12 dice que no" es un dato huerfano.
     *     \~ ------------------------------------------------------------- */
    u8 lbr_format;         /**< \~english bits 5:0.  0 = no LBR \~spanish bits 5:0.  0 = no hay LBR \~ */
    u8 pebs_record_format; /**< \~english bits 11:8.  NOT the low bits \~spanish bits 11:8.  NO son los bits bajos \~ */
    u8 pebs_trap;          /**< bit 6 */
    u8 pebs_arch_regs;     /**< bit 7 */
    u8 smm_freeze;         /**< bit 12 */
    u8 full_width_write;   /**< \~english bit 13: full width counters \~spanish bit 13: contadores de ancho completo \~ */
    u8 pebs_baseline;      /**< bit 14 */
    u8 perf_metrics;       /**< bit 15 */
    u8 pebs_output_pt;     /**< \~english bit 16: PEBS can go out through Intel PT \~spanish bit 16: PEBS puede salir por Intel PT \~ */

    u8 verdict; /**< `pebs_verdict` */

    /**
     * @brief
     * \~english Somebody else is measuring RIGHT NOW on this core.
     * \~spanish Alguien mas esta midiendo AHORA en este nucleo.
     * \~
     *
     * \~english
     * It matters because the PMU is a global resource and on Windows there is no
     * arbiter for a third party: if the system has counters armed and we program
     * on top, both of us read numbers that are not right.
     *
     * IT IS NOT DEDUCED FROM `PERF_GLOBAL_CTRL`, though it looks like it.  That
     * register is a gate, and on this machine it reads wide open while the fixed
     * counters are stopped.  A counter counts if its bit is in the gate **and
     * also** its selector has `EN` set; the fixed ones, if `FIXED_CTR_CTRL` is
     * not zero.  The first version looked only at the gate and published a false
     * fact.
     *
     * \~spanish
     * Importa porque el PMU es un recurso global y en Windows no hay arbitro
     * para un tercero: si el sistema tiene contadores armados y nosotros
     * programamos encima, los dos leemos numeros que no son.
     *
     * NO SE DEDUCE DE `PERF_GLOBAL_CTRL`, aunque lo parezca.  Ese registro es una
     * compuerta, y en esta maquina se lee con todo abierto mientras los
     * contadores fijos estan parados.  Un contador cuenta si su bit esta en la
     * compuerta **y ademas** su selector tiene el `EN` puesto; los fijos, si
     * `FIXED_CTR_CTRL` no es cero.  La primera version miro solo la compuerta y
     * publico un hecho falso.
     */
    u8 counters_in_use;

    /** \~english Map of armed selectors: bit i = `PERFEVTSELi` has `EN`.
     *  \~spanish Mapa de selectores armados: bit i = `PERFEVTSELi` tiene el
     *            `EN`. \~ */
    u32 gp_enabled_mask;

    /** \~english The raw selectors, for the report.  Only the ones that exist.
     *  \~spanish Los selectores en crudo, para el informe.  Solo los que
     *            existen. \~ */
    msr_value evtsel[8];
} pmu_caps;

/**
 * @brief
 * \~english Fills `out` describing the logical processor it runs on.
 * \~spanish Rellena `out` describiendo el procesador logico donde se ejecute.
 * \~
 *
 * @param cpu_index \~english copied as-is into `out->cpu_index`; this module
 *                  does not know where it is running, it only notes it down
 *                  \~spanish se copia tal cual a `out->cpu_index`; este modulo
 *                  no sabe donde esta corriendo, solo lo apunta \~
 * @param out       \~english destination; zeroed entirely first \~spanish destino; se pone a cero entero antes de nada \~
 * @return \~english `OK` as long as `out` is not null \~spanish `OK` siempre que `out` no sea nulo \~
 *
 * \~english
 * IT DOES NOT RETURN AN ERROR BECAUSE A READ FAILED.  That an MSR will not be
 * read is a result, not a failure of the function: it is noted in its
 * `msr_value.rc` and the verdict takes it into account.  Returning an error here
 * would force the caller to choose between throwing away the whole report or
 * ignoring the code, and both are worse than reporting what could be known.
 *
 * \~spanish
 * NO DEVUELVE ERROR PORQUE UNA LECTURA FALLE.  Que un MSR no se deje leer es un
 * resultado, no un fallo de la funcion: queda anotado en su `msr_value.rc` y el
 * veredicto lo tiene en cuenta.  Devolver error aqui obligaria a quien llama a
 * elegir entre tirar el informe entero o ignorar el codigo, y las dos son peores
 * que informar de lo que si se pudo saber.
 */
MUST_CHECK status pmu_caps_detect(u32 cpu_index, pmu_caps *out);

/**
 * @brief
 * \~english The verdict on PEBS, as text, for the report.
 * \~spanish El veredicto sobre PEBS, en texto, para el informe.
 * @return \~english a static string; never null \~spanish una cadena estatica; nunca nulo \~
 */
const char *pmu_caps_verdict_name(u8 verdict);

/** @brief
 *  \~english The core class name, for the report.
 *  \~spanish El nombre de la clase de nucleo, para el informe. \~ */
const char *pmu_caps_core_class_name(u32 core_type);

/**
 * @brief
 * \~english Writes the readable report of `caps` into the caller's buffer.
 * \~spanish Escribe el informe legible de `caps` en el bufer que da el llamante.
 * \~
 *
 * @param caps    \~english what was detected \~spanish lo detectado \~
 * @param buf     \~english destination \~spanish destino \~
 * @param cap     \~english how much fits in `buf` \~spanish cuanto cabe en `buf` \~
 * @param written \~english bytes written; also set when it did not all fit \~spanish cuantos bytes se escribieron; se pone tambien si no cupo todo \~
 * @return \~english `OK`, or `ERR_NOSPACE` if the text did not fit whole \~spanish `OK`, o `ERR_NOSPACE` si el texto no cabia entero \~
 *
 * \~english
 * THE BUFFER IS SUPPLIED BY THE CALLER, like everything here: this directory
 * never allocates.  And the formatting is our own and not `snprintf` because
 * there is no standard library in the kernel -- besides which, that way exactly
 * the same code that runs inside is tested in user space.
 *
 * \~spanish
 * EL BUFER LO PONE QUIEN LLAMA, como todo aqui: este directorio no reserva
 * memoria nunca.  Y el formateo es propio y no `snprintf` porque en kernel no
 * hay biblioteca estandar -- ademas de que asi se prueba en espacio de usuario
 * exactamente el mismo codigo que corre dentro.
 */
MUST_CHECK status pmu_caps_format(const pmu_caps *caps, char *buf, usize cap,
                                  usize *written);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_COMMON_PROBE_PMU_CAPS_H */
