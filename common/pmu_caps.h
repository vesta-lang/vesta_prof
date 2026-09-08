/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file pmu_caps.h
 * @brief Que sabe hacer el PMU de ESTE procesador logico, y con que certeza.
 *
 * Es la primera pregunta del perfilador y la que ordena todo lo demas: sin
 * saber que contadores hay, si son precisos y si alguien mas los esta usando,
 * cualquier numero que se publique despues es una suposicion con formato.
 *
 * DOS FUENTES QUE NO VALEN LO MISMO.  CPUID se pregunta primero porque es
 * barato, pero **bajo un hipervisor no es una lectura del silicio**: cada
 * `cpuid` sale por una salida de VM y las respuestas las pone el hipervisor.
 * Los MSR son la segunda opinion, y en concreto `IA32_MISC_ENABLE` bit 12 es la
 * palabra autorizada sobre PEBS por encima de lo que anuncie CPUID.
 *
 * De ahi el veredicto `PMU_PEBS_MASKED_BY_CPUID`, que a primera vista parece
 * una rareza y es el caso que motivo escribir esto: **CPUID dice que no hay
 * Debug Store y el MSR dice que PEBS si esta**.  Sin separar las dos fuentes,
 * ese caso se archiva como "esta maquina no puede" y se deja de medir bien por
 * creerse un anuncio.
 *
 * LA CERTEZA VIAJA DENTRO DEL HECHO.  Cada valor que sale de un MSR va con el
 * `status` de SU lectura.  No es ceremonia: un cero leido y un cero por no
 * haber podido leer mandan a sitios distintos -- uno a seguir, el otro a buscar
 * por que --, y un struct que los confunda hace imposible distinguirlos aguas
 * abajo.
 *
 * ESTE MODULO NO FIJA AFINIDAD.  Describe el procesador donde se le llame, y
 * quien llama decide donde.  Es lo que permite que el driver lo recorra nucleo
 * a nucleo -- que es como se ve la diferencia entre un P y un E -- sin que
 * `common/` tenga que saber que existe tal cosa como fijar un hilo.
 */

#ifndef VXP_COMMON_PMU_CAPS_H
#define VXP_COMMON_PMU_CAPS_H

#include "vxp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Clase de nucleo, de CPUID.1A EAX[31:24]. */
typedef enum core_class {
    CORE_CLASS_UNKNOWN = 0, /**< no es una pieza hibrida, o no lo dice */
    CORE_CLASS_ATOM = 0x20, /**< E-core */
    CORE_CLASS_CORE = 0x40  /**< P-core */
} core_class;

/** @brief Fabricante, que decide que MSR tienen sentido. */
typedef enum cpu_vendor {
    CPU_VENDOR_UNKNOWN = 0,
    CPU_VENDOR_INTEL = 1,
    CPU_VENDOR_AMD = 2
} cpu_vendor;

/**
 * @brief Que se puede decir de PEBS, y con que fundamento.
 *
 * Los cuatro son respuestas distintas, y la tentacion de juntar las dos
 * ultimas en un "no" es exactamente el error que este modulo existe para
 * evitar.
 */
typedef enum pebs_verdict {
    PMU_PEBS_UNKNOWN = 0,        /**< no se pudo determinar: falta la lectura */
    PMU_PEBS_AVAILABLE,          /**< CPUID y MSR de acuerdo en que si        */
    PMU_PEBS_MASKED_BY_CPUID,    /**< CPUID dice que no, el MSR dice que si   */
    PMU_PEBS_UNAVAILABLE,        /**< el MSR dice que no.  Esto si es un no   */
    PMU_PEBS_NOT_APPLICABLE      /**< no es Intel: en AMD el mecanismo es IBS */
} pebs_verdict;

/**
 * @brief Un valor de MSR con el estado de su lectura pegado.
 *
 * Van juntos y no en dos campos sueltos para que no se pueda leer uno sin el
 * otro por descuido.  `value` solo significa algo si `rc == OK`.
 */
typedef struct msr_value {
    u64 value;  /**< lo leido; sin sentido si `rc != OK` */
    status rc;  /**< como fue la lectura */
} msr_value;

/**
 * @brief Todo lo que se sabe del PMU de un procesador logico.
 *
 * TRANSPARENTE a proposito.  La regla del directorio es "opaco por defecto",
 * y la excepcion es lo que solo transporta datos: aqui no hay invariante que
 * proteger -- es el resultado de una medida, y quien lo recibe tiene que poder
 * leer campo a campo para volcarlo.  Ver `common/README.md`.
 */
typedef struct pmu_caps {
    /* --- de donde salio esta medida ------------------------------------- */
    u32 cpu_index;   /**< procesador logico donde se ejecuto la deteccion */
    u32 core_type;   /**< `core_class`; 0 si la pieza no es hibrida       */
    u32 vendor;      /**< `cpu_vendor`                                    */
    u32 family;      /**< familia mostrada, ya sumada la extendida        */
    u32 model;       /**< modelo mostrado, ya compuesto con el extendido  */
    u32 stepping;

    /* --- PMU arquitectonico, de CPUID.0A -------------------------------- */
    u32 pmu_version;     /**< 0 = no hay PMU arquitectonico              */
    u32 gp_counters;     /**< contadores de proposito general por hilo   */
    u32 gp_width;        /**< bits de cada uno                           */
    u32 fixed_counters;  /**< contadores fijos                           */
    u32 fixed_width;     /**< bits de cada uno                           */
    u32 arch_events_listed;  /**< cuantos bits enumera el mapa de EBX    */
    u32 arch_events_absent;  /**< mapa de EBX: bit PUESTO = NO disponible */
    u32 fixed_ctr_bitmap;    /**< ECX; desde PMU v5                      */

    /* --- banderas de CPUID ---------------------------------------------- */
    u8 has_ds;            /**< Debug Store: sin el, no hay PEBS          */
    u8 has_pdcm;          /**< IA32_PERF_CAPABILITIES es legible         */
    u8 has_hypervisor;    /**< hay algo debajo: CPUID deja de ser fiable  */
    u8 has_hybrid;        /**< la pieza mezcla nucleos P y E             */
    u8 has_invariant_tsc; /**< el TSC no cambia de ritmo                 */
    u8 has_ibs;           /**< AMD: Instruction Based Sampling           */
    u8 _pad[2];

    /* --- lo que CPUID no puede desmentir: los MSR ------------------------ */
    msr_value misc_enable;      /**< IA32_MISC_ENABLE                     */
    msr_value perf_capabilities;/**< IA32_PERF_CAPABILITIES, si hay PDCM  */
    msr_value debugctl;         /**< IA32_DEBUGCTL: de aqui cuelga el LBR */
    msr_value fixed_ctr_ctrl;   /**< IA32_FIXED_CTR_CTRL                  */
    msr_value perf_global_ctrl; /**< IA32_PERF_GLOBAL_CTRL                */

    /* --- lo derivado, ya masticado -------------------------------------- */
    u8 pebs_unavailable_bit; /**< bit 12 de MISC_ENABLE; solo si su rc==OK */
    u8 pebs_record_format;   /**< PERF_CAPABILITIES[3:0]; 0 = no hay PEBS  */
    u8 verdict;              /**< `pebs_verdict`                           */

    /**
     * @brief Alguien mas esta midiendo AHORA en este nucleo.
     *
     * Importa porque el PMU es un recurso global y en Windows no hay arbitro
     * para un tercero: si el sistema tiene contadores armados y nosotros
     * programamos encima, los dos leemos numeros que no son.
     *
     * NO SE DEDUCE DE `PERF_GLOBAL_CTRL`, aunque lo parezca.  Ese registro es
     * una compuerta, y en esta maquina se lee con todo abierto mientras los
     * contadores fijos estan parados.  Un contador cuenta si su bit esta en la
     * compuerta **y ademas** su selector tiene el `EN` puesto; los fijos, si
     * `FIXED_CTR_CTRL` no es cero.  La primera version miro solo la compuerta y
     * publico un hecho falso.
     */
    u8 counters_in_use;

    /** Mapa de selectores armados: bit i = `PERFEVTSELi` tiene el `EN`. */
    u32 gp_enabled_mask;

    /** Los selectores en crudo, para el informe.  Solo los que existen. */
    msr_value evtsel[8];
} pmu_caps;

/**
 * @brief Rellena `out` describiendo el procesador logico donde se ejecute.
 *
 * @param cpu_index se copia tal cual a `out->cpu_index`; este modulo no sabe
 *                  donde esta corriendo, solo lo apunta.
 * @param out       destino; se pone a cero entero antes de nada.
 * @return `OK` siempre que `out` no sea nulo.
 *
 * NO DEVUELVE ERROR PORQUE UNA LECTURA FALLE.  Que un MSR no se deje leer es un
 * resultado, no un fallo de la funcion: queda anotado en su `msr_value.rc` y el
 * veredicto lo tiene en cuenta.  Devolver error aqui obligaria a quien llama a
 * elegir entre tirar el informe entero o ignorar el codigo, y las dos son
 * peores que informar de lo que si se pudo saber.
 */
MUST_CHECK status pmu_caps_detect(u32 cpu_index, pmu_caps *out);

/**
 * @brief El veredicto sobre PEBS, en texto, para el informe.
 * @return una cadena estatica; nunca nulo.
 */
const char *pmu_caps_verdict_name(u8 verdict);

/** @brief El nombre de la clase de nucleo, para el informe. */
const char *pmu_caps_core_class_name(u32 core_type);

/**
 * @brief Escribe el informe legible de `caps` en el bufer que da el llamante.
 *
 * @param caps    lo detectado.
 * @param buf     destino.
 * @param cap     cuanto cabe en `buf`.
 * @param written cuantos bytes se escribieron; se pone tambien si no cupo todo.
 * @return `OK`, o `ERR_NOSPACE` si el texto no cabia entero.
 *
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

#endif /* VXP_COMMON_PMU_CAPS_H */
