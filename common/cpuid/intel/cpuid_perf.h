/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_perf.h
 * @brief Rendimiento: PMU, Intel PT, LBR y la extension del perfmon
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

#ifndef VXP_COMMON_CPUID_PERF_H
#define VXP_COMMON_CPUID_PERF_H

/* ---- hoja 0AH -- Architectural Performance Monitoring ---- */

/** EAX bits 31:24 */
#define CPUID_0A_EAX_EVENT_ENUM_LENGTH_SHIFT 24u
#define CPUID_0A_EAX_EVENT_ENUM_LENGTH_MASK 0xFFu

/** EAX bits 23:16 */
#define CPUID_0A_EAX_GP_CTR_WIDTH_SHIFT 16u
#define CPUID_0A_EAX_GP_CTR_WIDTH_MASK 0xFFu

/** EAX bits 15:8 */
#define CPUID_0A_EAX_NUM_GP_CTRS_SHIFT 8u
#define CPUID_0A_EAX_NUM_GP_CTRS_MASK 0xFFu

/** EAX bits 7:0 */
#define CPUID_0A_EAX_VERSION_SHIFT 0u
#define CPUID_0A_EAX_VERSION_MASK 0xFFu

/** EBX bits 31:13 -- RESERVADO, no se toca */
#define CPUID_0A_EBX_RESERVED_31_13_SHIFT 13u
#define CPUID_0A_EBX_RESERVED_31_13_MASK 0x7FFFFu

/** EBX bit 12 */
#define CPUID_0A_EBX_LBR_INSERTS_NA 12u

/** EBX bit 11 */
#define CPUID_0A_EBX_RETIRING_NA 11u

/** EBX bit 10 */
#define CPUID_0A_EBX_FRONTEND_NA 10u

/** EBX bit 9 */
#define CPUID_0A_EBX_BADSPEC_NA 9u

/** EBX bit 8 */
#define CPUID_0A_EBX_BACKEND_NA 8u

/** EBX bit 7 */
#define CPUID_0A_EBX_SLOTS_NA 7u

/** EBX bit 6 */
#define CPUID_0A_EBX_BR_MISPRED_RET_NA 6u

/** EBX bit 5 */
#define CPUID_0A_EBX_BR_INSTR_RET_NA 5u

/** EBX bit 4 */
#define CPUID_0A_EBX_LLC_MISSES_NA 4u

/** EBX bit 3 */
#define CPUID_0A_EBX_LLC_CYC_NA 3u

/** EBX bit 2 */
#define CPUID_0A_EBX_REF_CYC_NA 2u

/** EBX bit 1 */
#define CPUID_0A_EBX_INTR_RET_NA 1u

/** EBX bit 0 */
#define CPUID_0A_EBX_CORE_CYC_NA 0u

/** ECX bits 31:0 */
#define CPUID_0A_ECX_FIXED_CTR_MASK_SHIFT 0u
#define CPUID_0A_ECX_FIXED_CTR_MASK_MASK 0xFFFFFFFFu

/** EDX bits 31:20 -- RESERVADO, no se toca */
#define CPUID_0A_EDX_RESERVED_31_20_SHIFT 20u
#define CPUID_0A_EDX_RESERVED_31_20_MASK 0xFFFu

/** EDX bits 19:16 */
#define CPUID_0A_EDX_SLOTS_PER_CYC_SHIFT 16u
#define CPUID_0A_EDX_SLOTS_PER_CYC_MASK 0xFu

/** EDX bit 15 */
#define CPUID_0A_EDX_ANYTHREAD_DEPRECATION 15u

/** EDX bits 14:13 -- RESERVADO, no se toca */
#define CPUID_0A_EDX_RESERVED_14_13_SHIFT 13u
#define CPUID_0A_EDX_RESERVED_14_13_MASK 0x3u

/** EDX bits 12:5 */
#define CPUID_0A_EDX_FIXED_CTR_WIDTH_SHIFT 5u
#define CPUID_0A_EDX_FIXED_CTR_WIDTH_MASK 0xFFu

/** EDX bits 4:0 */
#define CPUID_0A_EDX_NUM_FIXED_CTR_SHIFT 0u
#define CPUID_0A_EDX_NUM_FIXED_CTR_MASK 0x1Fu

/* ---- hoja 14H, subhoja 00H -- Intel(R) PT Main Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_14_00_EAX_MAX_SUBLEAF_SHIFT 0u
#define CPUID_14_00_EAX_MAX_SUBLEAF_MASK 0xFFFFFFFFu

/** EBX bits 31:10 -- RESERVADO, no se toca */
#define CPUID_14_00_EBX_RESERVED_31_10_SHIFT 10u
#define CPUID_14_00_EBX_RESERVED_31_10_MASK 0x3FFFFFu

/** EBX bit 9 */
#define CPUID_14_00_EBX_PTTT 9u

/** EBX bit 8 */
#define CPUID_14_00_EBX_TNT_DIS 8u

/** EBX bit 7 */
#define CPUID_14_00_EBX_EVENT_TRACE 7u

/** EBX bit 6 */
#define CPUID_14_00_EBX_PMI_PRESERVE 6u

/** EBX bit 5 */
#define CPUID_14_00_EBX_PWR_EVT_TRACE 5u

/** EBX bit 4 */
#define CPUID_14_00_EBX_PTWRITE 4u

/** EBX bit 3 */
#define CPUID_14_00_EBX_MTC 3u

/** EBX bit 2 */
#define CPUID_14_00_EBX_IP_FILTER 2u

/** EBX bit 1 */
#define CPUID_14_00_EBX_CYC_ACC 1u

/** EBX bit 0 */
#define CPUID_14_00_EBX_CR3_FILTER 0u

/** ECX bit 31 */
#define CPUID_14_00_ECX_LIP 31u

/** ECX bits 30:4 -- RESERVADO, no se toca */
#define CPUID_14_00_ECX_RESERVED_30_4_SHIFT 4u
#define CPUID_14_00_ECX_RESERVED_30_4_MASK 0x7FFFFFFu

/** ECX bit 3 */
#define CPUID_14_00_ECX_TRACE_TRANSPORT_SUBSYSTEM 3u

/** ECX bit 2 */
#define CPUID_14_00_ECX_SNGL_RNG_OUT 2u

/** ECX bit 1 */
#define CPUID_14_00_ECX_MENTRY 1u

/** ECX bit 0 */
#define CPUID_14_00_ECX_TOPAOUT 0u

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_14_00_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_14_00_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 14H, subhoja 01H -- Feature Information Sub-Leaf ---- */

/** EAX bits 31:16 */
#define CPUID_14_01_EAX_MTC_RATE_SHIFT 16u
#define CPUID_14_01_EAX_MTC_RATE_MASK 0xFFFFu

/** EAX bits 15:11 -- RESERVADO, no se toca */
#define CPUID_14_01_EAX_RESERVED_15_11_SHIFT 11u
#define CPUID_14_01_EAX_RESERVED_15_11_MASK 0x1Fu

/** EAX bits 10:8 */
#define CPUID_14_01_EAX_TRIGGER_CFG_CNT_SHIFT 8u
#define CPUID_14_01_EAX_TRIGGER_CFG_CNT_MASK 0x7u

/** EAX bits 7:3 -- RESERVADO, no se toca */
#define CPUID_14_01_EAX_RESERVED_7_3_SHIFT 3u
#define CPUID_14_01_EAX_RESERVED_7_3_MASK 0x1Fu

/** EAX bits 2:0 */
#define CPUID_14_01_EAX_RANGECNT_SHIFT 0u
#define CPUID_14_01_EAX_RANGECNT_MASK 0x7u

/** EBX bits 31:16 */
#define CPUID_14_01_EBX_PSB_RATE_SHIFT 16u
#define CPUID_14_01_EBX_PSB_RATE_MASK 0xFFFFu

/** EBX bits 15:0 */
#define CPUID_14_01_EBX_CYC_THRESHOLDS_SHIFT 0u
#define CPUID_14_01_EBX_CYC_THRESHOLDS_MASK 0xFFFFu

/** ECX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_14_01_ECX_RESERVED_31_16_SHIFT 16u
#define CPUID_14_01_ECX_RESERVED_31_16_MASK 0xFFFFu

/** ECX bit 15 */
#define CPUID_14_01_ECX_TRIGGER_DR_MATCH 15u

/** ECX bits 14:2 -- RESERVADO, no se toca */
#define CPUID_14_01_ECX_RESERVED_14_2_SHIFT 2u
#define CPUID_14_01_ECX_RESERVED_14_2_MASK 0x1FFFu

/** ECX bit 1 */
#define CPUID_14_01_ECX_TRIGGER_PAUSE 1u

/** ECX bit 0 */
#define CPUID_14_01_ECX_ICNT 0u

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_14_01_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_14_01_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 1CH -- Last Branch Records (LBR) Information ---- */

/** EAX bit 31 */
#define CPUID_1C_EAX_IP_VALUES_CONTAIN_LIP 31u

/** EAX bit 30 */
#define CPUID_1C_EAX_DEEP_C_STATE_RESET 30u

/** EAX bits 29:8 -- RESERVADO, no se toca */
#define CPUID_1C_EAX_RESERVED_29_8_SHIFT 8u
#define CPUID_1C_EAX_RESERVED_29_8_MASK 0x3FFFFFu

/** EAX bits 7:0 */
#define CPUID_1C_EAX_LBR_DEPTH_VALUES_SHIFT 0u
#define CPUID_1C_EAX_LBR_DEPTH_VALUES_MASK 0xFFu

/** EBX bits 31:3 -- RESERVADO, no se toca */
#define CPUID_1C_EBX_RESERVED_31_3_SHIFT 3u
#define CPUID_1C_EBX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** EBX bit 2 */
#define CPUID_1C_EBX_CALL_STACK_MODE 2u

/** EBX bit 1 */
#define CPUID_1C_EBX_BRANCH_FILTERING 1u

/** EBX bit 0 */
#define CPUID_1C_EBX_CPL_FILTERING 0u

/** ECX bits 31:20 -- RESERVADO, no se toca */
#define CPUID_1C_ECX_RESERVED_31_20_SHIFT 20u
#define CPUID_1C_ECX_RESERVED_31_20_MASK 0xFFFu

/** ECX bits 19:16 */
#define CPUID_1C_ECX_EVENT_LOGGING_BITMAP_SHIFT 16u
#define CPUID_1C_ECX_EVENT_LOGGING_BITMAP_MASK 0xFu

/** ECX bits 15:3 -- RESERVADO, no se toca */
#define CPUID_1C_ECX_RESERVED_15_3_SHIFT 3u
#define CPUID_1C_ECX_RESERVED_15_3_MASK 0x1FFFu

/** ECX bit 2 */
#define CPUID_1C_ECX_BRANCH_TYPE_FIELD_SUPPORTED 2u

/** ECX bit 1 */
#define CPUID_1C_ECX_TIMED_LBRS 1u

/** ECX bit 0 */
#define CPUID_1C_ECX_MISPREDICT_BIT 0u

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1C_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_1C_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 23H, subhoja 00H -- Main Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_23_00_EAX_SUBLEAF_MASK_SHIFT 0u
#define CPUID_23_00_EAX_SUBLEAF_MASK_MASK 0xFFFFFFFFu

/** EBX bits 31:2 -- RESERVADO, no se toca */
#define CPUID_23_00_EBX_RESERVED_31_2_SHIFT 2u
#define CPUID_23_00_EBX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** EBX bit 1 */
#define CPUID_23_00_EBX_EQ 1u

/** EBX bit 0 */
#define CPUID_23_00_EBX_UNITMASK2 0u

/** ECX bits 31:8 -- RESERVADO, no se toca */
#define CPUID_23_00_ECX_RESERVED_31_8_SHIFT 8u
#define CPUID_23_00_ECX_RESERVED_31_8_MASK 0xFFFFFFu

/** ECX bits 7:0 */
#define CPUID_23_00_ECX_SLOTS_PER_CYC_SHIFT 0u
#define CPUID_23_00_ECX_SLOTS_PER_CYC_MASK 0xFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_00_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_00_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 23H, subhoja 01H -- Counter Information Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_23_01_EAX_GP_COUNTERS_SHIFT 0u
#define CPUID_23_01_EAX_GP_COUNTERS_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_23_01_EBX_FIXED_COUNTERS_SHIFT 0u
#define CPUID_23_01_EBX_FIXED_COUNTERS_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_01_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_01_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_01_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_01_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 23H, subhoja 02H -- Bitmap of Auto Counter Reload Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_23_02_EAX_ACR_GP_RELOAD_SHIFT 0u
#define CPUID_23_02_EAX_ACR_GP_RELOAD_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_23_02_EBX_ACR_FIXED_RELOAD_SHIFT 0u
#define CPUID_23_02_EBX_ACR_FIXED_RELOAD_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_23_02_ECX_ACR_GP_TRIGGER_SHIFT 0u
#define CPUID_23_02_ECX_ACR_GP_TRIGGER_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_23_02_EDX_ACR_FIXED_TRIGGER_SHIFT 0u
#define CPUID_23_02_EDX_ACR_FIXED_TRIGGER_MASK 0xFFFFFFFFu

/* ---- hoja 23H, subhoja 03H -- Architectural Performance Monitoring Events Bitmap Sub-Leaf ---- */

/** EAX bits 31:13 -- RESERVADO, no se toca */
#define CPUID_23_03_EAX_RESERVED_31_13_SHIFT 13u
#define CPUID_23_03_EAX_RESERVED_31_13_MASK 0x7FFFFu

/** EAX bit 12 */
#define CPUID_23_03_EAX_LBR_INSERTS 12u

/** EAX bit 11 */
#define CPUID_23_03_EAX_RETIRING 11u

/** EAX bit 10 */
#define CPUID_23_03_EAX_FRONTEND 10u

/** EAX bit 9 */
#define CPUID_23_03_EAX_BADSPEC 9u

/** EAX bit 8 */
#define CPUID_23_03_EAX_BACKEND 8u

/** EAX bit 7 */
#define CPUID_23_03_EAX_SLOTS 7u

/** EAX bit 6 */
#define CPUID_23_03_EAX_BR_MISPRED_RET 6u

/** EAX bit 5 */
#define CPUID_23_03_EAX_BR_INSTR_RET 5u

/** EAX bit 4 */
#define CPUID_23_03_EAX_LLC_MISSES 4u

/** EAX bit 3 */
#define CPUID_23_03_EAX_LLC_REF 3u

/** EAX bit 2 */
#define CPUID_23_03_EAX_REF_CYC 2u

/** EAX bit 1 */
#define CPUID_23_03_EAX_INSTR_RET 1u

/** EAX bit 0 */
#define CPUID_23_03_EAX_CORE_CYC 0u

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_03_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_03_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_03_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_03_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_03_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_03_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 23H, subhoja 04H -- PEBS Capabilities ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_04_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_04_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bit 31 -- RESERVADO, no se toca */
#define CPUID_23_04_EBX_RESERVED_31_31 31u

/** EBX bit 30 */
#define CPUID_23_04_EBX_AUX 30u

/** EBX bit 29 */
#define CPUID_23_04_EBX_GPR 29u

/** EBX bits 28:24 -- RESERVADO, no se toca */
#define CPUID_23_04_EBX_RESERVED_28_24_SHIFT 24u
#define CPUID_23_04_EBX_RESERVED_28_24_MASK 0x1Fu

/** EBX bits 23:16 */
#define CPUID_23_04_EBX_XER_SHIFT 16u
#define CPUID_23_04_EBX_XER_MASK 0xFFu

/** EBX bits 15:10 -- RESERVADO, no se toca */
#define CPUID_23_04_EBX_RESERVED_15_10_SHIFT 10u
#define CPUID_23_04_EBX_RESERVED_15_10_MASK 0x3Fu

/** EBX bits 9:8 */
#define CPUID_23_04_EBX_LBR_SHIFT 8u
#define CPUID_23_04_EBX_LBR_MASK 0x3u

/** EBX bit 7 -- RESERVADO, no se toca */
#define CPUID_23_04_EBX_RESERVED_7_7 7u

/** EBX bit 6 */
#define CPUID_23_04_EBX_CNTR_METRICS 6u

/** EBX bit 5 */
#define CPUID_23_04_EBX_CNTR_FIXED 5u

/** EBX bit 4 */
#define CPUID_23_04_EBX_CNTR_GP 4u

/** EBX bit 3 */
#define CPUID_23_04_EBX_ALLOW_IN_RECORD 3u

/** EBX bits 2:0 -- RESERVADO, no se toca */
#define CPUID_23_04_EBX_RESERVED_2_0_SHIFT 0u
#define CPUID_23_04_EBX_RESERVED_2_0_MASK 0x7u

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_04_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_04_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_23_04_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_23_04_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 23H, subhoja 05H -- Arch PEBS GP and Fixed Counters supported ---- */

/** EAX bits 31:0 */
#define CPUID_23_05_EAX_GP_PEBS_SHIFT 0u
#define CPUID_23_05_EAX_GP_PEBS_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_23_05_EBX_GP_PDIST_SHIFT 0u
#define CPUID_23_05_EBX_GP_PDIST_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_23_05_ECX_FIXED_PEBS_SHIFT 0u
#define CPUID_23_05_ECX_FIXED_PEBS_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_23_05_EDX_FIXED_PDIST_SHIFT 0u
#define CPUID_23_05_EDX_FIXED_PDIST_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_PERF_H */
