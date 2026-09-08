/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_power.h
 * @brief Frecuencia, energia, control termico y base de tiempo
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_cpuid_index.py
 *
 * a partir del volcado de texto del manual de Intel, que no viaja en el
 * repositorio.
 *
 * POR QUE SE GENERA.  Un numero de bit escrito de memoria falla callado: se
 * pregunta por el bit equivocado, sale cero, y se concluye que la maquina no
 * tiene algo que si tiene.  No da un error -- da una capacidad perdida.
 *
 * Los nombres son los del manual, para que se pueda buscar en el la misma
 * cadena que hay aqui.  Un campo de un bit sale como su POSICION; uno de varios,
 * como DESPLAZAMIENTO y MASCARA.
 *
 * ALCANCE: **SOLO INTEL**, y conviene no confundirlo con "completo".  Sale del
 * manual de Intel, asi que describe lo que Intel documenta -- y el rango
 * extendido de Intel acaba en 80000008H.  Por encima de ahi hay hojas que
 * existen y que son de AMD:
 *
 *     8000001BH   IBS, el muestreo preciso de AMD
 *     8000001DH   topologia de cache
 *     8000001EH   extensiones de topologia: hermanos SMT, nodo
 *     80000021H   tamano del parche de microcodigo, predictor de retorno
 *
 * Que no esten aqui NO significa que no existan: significa que su fuente es
 * otra, el manual de AMD, y que hay que tratarla igual que esta -- no de
 * memoria.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef VXP_COMMON_CPUID_POWER_H
#define VXP_COMMON_CPUID_POWER_H

/* ---- hoja 05H -- MONITOR and MWAIT Features ---- */

/** EAX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_05_EAX_RESERVED_31_16_SHIFT 16u
#define CPUID_05_EAX_RESERVED_31_16_MASK 0xFFFFu

/** EAX bits 15:0 */
#define CPUID_05_EAX_SMALLEST_MONITOR_LINE_SIZE_SHIFT 0u
#define CPUID_05_EAX_SMALLEST_MONITOR_LINE_SIZE_MASK 0xFFFFu

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_05_EBX_RESERVED_31_16_SHIFT 16u
#define CPUID_05_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:0 */
#define CPUID_05_EBX_LARGEST_MONITOR_LINE_SIZE_SHIFT 0u
#define CPUID_05_EBX_LARGEST_MONITOR_LINE_SIZE_MASK 0xFFFFu

/** ECX bits 31:2 -- RESERVADO, no se toca */
#define CPUID_05_ECX_RESERVED_31_2_SHIFT 2u
#define CPUID_05_ECX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** ECX bit 1 */
#define CPUID_05_ECX_INTERRUPT_AS_BREAK_EVENT 1u

/** ECX bit 0 */
#define CPUID_05_ECX_MONITOR_MWAIT_EXTENSIONS 0u

/** EDX bits 31:28 */
#define CPUID_05_EDX_C7_SUB_STATES_SHIFT 28u
#define CPUID_05_EDX_C7_SUB_STATES_MASK 0xFu

/** EDX bits 27:24 */
#define CPUID_05_EDX_C6_SUB_STATES_SHIFT 24u
#define CPUID_05_EDX_C6_SUB_STATES_MASK 0xFu

/** EDX bits 23:20 */
#define CPUID_05_EDX_C5_SUB_STATES_SHIFT 20u
#define CPUID_05_EDX_C5_SUB_STATES_MASK 0xFu

/** EDX bits 19:16 */
#define CPUID_05_EDX_C4_SUB_STATES_SHIFT 16u
#define CPUID_05_EDX_C4_SUB_STATES_MASK 0xFu

/** EDX bits 15:12 */
#define CPUID_05_EDX_C3_SUB_STATES_SHIFT 12u
#define CPUID_05_EDX_C3_SUB_STATES_MASK 0xFu

/** EDX bits 11:8 */
#define CPUID_05_EDX_C2_SUB_STATES_SHIFT 8u
#define CPUID_05_EDX_C2_SUB_STATES_MASK 0xFu

/** EDX bits 7:4 */
#define CPUID_05_EDX_C1_SUB_STATES_SHIFT 4u
#define CPUID_05_EDX_C1_SUB_STATES_MASK 0xFu

/** EDX bits 3:0 */
#define CPUID_05_EDX_C0_SUB_STATES_SHIFT 0u
#define CPUID_05_EDX_C0_SUB_STATES_MASK 0xFu

/* ---- hoja 06H -- Thermal and Power Management Features ---- */

/** EAX bits 31:24 -- RESERVADO, no se toca */
#define CPUID_06_EAX_RESERVED_31_24_SHIFT 24u
#define CPUID_06_EAX_RESERVED_31_24_MASK 0xFFu

/** EAX bit 23 */
#define CPUID_06_EAX_THREAD_DIRECTOR 23u

/** EAX bit 22 */
#define CPUID_06_EAX_HWP_CTL 22u

/** EAX bit 21 -- RESERVADO, no se toca */
#define CPUID_06_EAX_RESERVED_21_21 21u

/** EAX bit 20 */
#define CPUID_06_EAX_HWP_REQUEST_IGNORE_IDLE 20u

/** EAX bit 19 */
#define CPUID_06_EAX_HW_FEEDBACK 19u

/** EAX bit 18 */
#define CPUID_06_EAX_HWP_REQUEST_FAST_ACCESS 18u

/** EAX bit 17 */
#define CPUID_06_EAX_FLEXIBLE_HWP 17u

/** EAX bit 16 */
#define CPUID_06_EAX_HWP_PECI_OVERRIDE 16u

/** EAX bit 15 */
#define CPUID_06_EAX_HWP_CAP 15u

/** EAX bit 14 */
#define CPUID_06_EAX_TURBO_BOOST_MAX 14u

/** EAX bit 13 */
#define CPUID_06_EAX_HDC 13u

/** EAX bit 12 -- RESERVADO, no se toca */
#define CPUID_06_EAX_RESERVED_12_12 12u

/** EAX bit 11 */
#define CPUID_06_EAX_HWP_REQUEST_PKG 11u

/** EAX bit 10 */
#define CPUID_06_EAX_HWP_EPP 10u

/** EAX bit 9 */
#define CPUID_06_EAX_HWP_ACTIVITY_WINDOW 9u

/** EAX bit 8 */
#define CPUID_06_EAX_HWP_INTERRUPT 8u

/** EAX bit 7 */
#define CPUID_06_EAX_HWP 7u

/** EAX bit 6 */
#define CPUID_06_EAX_PKG_THERM_MGMT 6u

/** EAX bit 5 */
#define CPUID_06_EAX_EXT_CLOCK_MOD 5u

/** EAX bit 4 */
#define CPUID_06_EAX_POWER_LIMIT_NOTIFY 4u

/** EAX bit 3 -- RESERVADO, no se toca */
#define CPUID_06_EAX_RESERVED_3_3 3u

/** EAX bit 2 */
#define CPUID_06_EAX_ALWAYS_RUNNING_APIC_TIMER 2u

/** EAX bit 1 */
#define CPUID_06_EAX_TURBO_BOOST 1u

/** EAX bit 0 */
#define CPUID_06_EAX_DIGITAL_TEMP_SENSOR 0u

/** EBX bits 31:4 -- RESERVADO, no se toca */
#define CPUID_06_EBX_RESERVED_31_4_SHIFT 4u
#define CPUID_06_EBX_RESERVED_31_4_MASK 0xFFFFFFFu

/** EBX bits 3:0 */
#define CPUID_06_EBX_DTS_NUM_INT_THRESHOLDS_SHIFT 0u
#define CPUID_06_EBX_DTS_NUM_INT_THRESHOLDS_MASK 0xFu

/** ECX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_06_ECX_RESERVED_31_16_SHIFT 16u
#define CPUID_06_ECX_RESERVED_31_16_MASK 0xFFFFu

/** ECX bits 15:8 */
#define CPUID_06_ECX_HW_FEEDBACK_NUM_CLASSES_SHIFT 8u
#define CPUID_06_ECX_HW_FEEDBACK_NUM_CLASSES_MASK 0xFFu

/** ECX bits 7:4 -- RESERVADO, no se toca */
#define CPUID_06_ECX_RESERVED_7_4_SHIFT 4u
#define CPUID_06_ECX_RESERVED_7_4_MASK 0xFu

/** ECX bit 3 */
#define CPUID_06_ECX_ENERGY_PERF_BIAS 3u

/** ECX bits 2:1 -- RESERVADO, no se toca */
#define CPUID_06_ECX_RESERVED_2_1_SHIFT 1u
#define CPUID_06_ECX_RESERVED_2_1_MASK 0x3u

/** ECX bit 0 */
#define CPUID_06_ECX_HW_FEEDBACK_CAP 0u

/** EDX bits 31:16 */
#define CPUID_06_EDX_HW_FEEDBACK_TABLE_INDEX_SHIFT 16u
#define CPUID_06_EDX_HW_FEEDBACK_TABLE_INDEX_MASK 0xFFFFu

/** EDX bits 15:12 -- RESERVADO, no se toca */
#define CPUID_06_EDX_RESERVED_15_12_SHIFT 12u
#define CPUID_06_EDX_RESERVED_15_12_MASK 0xFu

/** EDX bits 11:8 */
#define CPUID_06_EDX_HW_FEEDBACK_TABLE_SIZE_SHIFT 8u
#define CPUID_06_EDX_HW_FEEDBACK_TABLE_SIZE_MASK 0xFu

/** EDX bits 7:0 */
#define CPUID_06_EDX_HW_FEEDBACK_CAPS_SHIFT 0u
#define CPUID_06_EDX_HW_FEEDBACK_CAPS_MASK 0xFFu

/* ---- hoja 15H -- Time Stamp Counter and Nominal Core Crystal Clock ---- */

/** EAX bits 31:0 */
#define CPUID_15_EAX_DENOMINATOR_SHIFT 0u
#define CPUID_15_EAX_DENOMINATOR_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_15_EBX_NUMERATOR_SHIFT 0u
#define CPUID_15_EBX_NUMERATOR_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_15_ECX_NOMINAL_ART_FREQUENCY_SHIFT 0u
#define CPUID_15_ECX_NOMINAL_ART_FREQUENCY_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_15_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_15_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 16H -- Processor Frequency Information ---- */

/** EAX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_16_EAX_RESERVED_31_16_SHIFT 16u
#define CPUID_16_EAX_RESERVED_31_16_MASK 0xFFFFu

/** EAX bits 15:0 */
#define CPUID_16_EAX_PROCESSOR_BASE_FREQUENCY_SHIFT 0u
#define CPUID_16_EAX_PROCESSOR_BASE_FREQUENCY_MASK 0xFFFFu

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_16_EBX_RESERVED_31_16_SHIFT 16u
#define CPUID_16_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:0 */
#define CPUID_16_EBX_MAXIMUM_FREQUENCY_SHIFT 0u
#define CPUID_16_EBX_MAXIMUM_FREQUENCY_MASK 0xFFFFu

/** ECX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_16_ECX_RESERVED_31_16_SHIFT 16u
#define CPUID_16_ECX_RESERVED_31_16_MASK 0xFFFFu

/** ECX bits 15:0 */
#define CPUID_16_ECX_BUS_FREQUENCY_SHIFT 0u
#define CPUID_16_ECX_BUS_FREQUENCY_MASK 0xFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_16_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_16_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_POWER_H */
