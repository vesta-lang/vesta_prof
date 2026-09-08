/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_features.h
 * @brief Los mapas de bits de caracteristicas, y el estado extendido
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

#ifndef VXP_COMMON_CPUID_FEATURES_H
#define VXP_COMMON_CPUID_FEATURES_H

/* ---- hoja 01H -- Version and Features ---- */

/** EAX bits 31:28 -- RESERVADO, no se toca */
#define CPUID_01_EAX_RESERVED_31_28_SHIFT 28u
#define CPUID_01_EAX_RESERVED_31_28_MASK 0xFu

/** EAX bits 27:20 */
#define CPUID_01_EAX_EXTENDED_FAMILY_ID_SHIFT 20u
#define CPUID_01_EAX_EXTENDED_FAMILY_ID_MASK 0xFFu

/** EAX bits 19:16 */
#define CPUID_01_EAX_EXTENDED_MODEL_ID_SHIFT 16u
#define CPUID_01_EAX_EXTENDED_MODEL_ID_MASK 0xFu

/** EAX bits 15:14 -- RESERVADO, no se toca */
#define CPUID_01_EAX_RESERVED_15_14_SHIFT 14u
#define CPUID_01_EAX_RESERVED_15_14_MASK 0x3u

/** EAX bits 13:12 */
#define CPUID_01_EAX_PROCESSOR_TYPE_SHIFT 12u
#define CPUID_01_EAX_PROCESSOR_TYPE_MASK 0x3u

/** EAX bits 11:8 */
#define CPUID_01_EAX_FAMILY_ID_SHIFT 8u
#define CPUID_01_EAX_FAMILY_ID_MASK 0xFu

/** EAX bits 7:4 */
#define CPUID_01_EAX_MODEL_ID_SHIFT 4u
#define CPUID_01_EAX_MODEL_ID_MASK 0xFu

/** EAX bits 3:0 */
#define CPUID_01_EAX_STEPPING_ID_SHIFT 0u
#define CPUID_01_EAX_STEPPING_ID_MASK 0xFu

/** EBX bits 31:24 */
#define CPUID_01_EBX_INITIAL_APIC_ID_SHIFT 24u
#define CPUID_01_EBX_INITIAL_APIC_ID_MASK 0xFFu

/** EBX bits 23:16 */
#define CPUID_01_EBX_APIC_ID_SPACE_SHIFT 16u
#define CPUID_01_EBX_APIC_ID_SPACE_MASK 0xFFu

/** EBX bits 15:8 */
#define CPUID_01_EBX_CLFLUSH_LINE_SIZE_SHIFT 8u
#define CPUID_01_EBX_CLFLUSH_LINE_SIZE_MASK 0xFFu

/** EBX bits 7:0 */
#define CPUID_01_EBX_BRAND_INDEX_SHIFT 0u
#define CPUID_01_EBX_BRAND_INDEX_MASK 0xFFu

/** ECX bit 30 */
#define CPUID_01_ECX_RDRAND 30u

/** ECX bit 29 */
#define CPUID_01_ECX_F16C 29u

/** ECX bit 28 */
#define CPUID_01_ECX_AVX 28u

/** ECX bit 27 */
#define CPUID_01_ECX_OSXSAVE 27u

/** ECX bit 26 */
#define CPUID_01_ECX_XSAVE 26u

/** ECX bit 25 */
#define CPUID_01_ECX_AESNI 25u

/** ECX bit 24 */
#define CPUID_01_ECX_TSC_DEADLINE 24u

/** ECX bit 23 */
#define CPUID_01_ECX_POPCNT 23u

/** ECX bit 22 */
#define CPUID_01_ECX_MOVBE 22u

/** ECX bit 21 */
#define CPUID_01_ECX_X2APIC 21u

/** ECX bit 20 */
#define CPUID_01_ECX_SSE4_2 20u

/** ECX bit 19 */
#define CPUID_01_ECX_SSE4_1 19u

/** ECX bit 18 */
#define CPUID_01_ECX_DCA 18u

/** ECX bit 17 */
#define CPUID_01_ECX_PCID 17u

/** ECX bit 16 -- RESERVADO, no se toca */
#define CPUID_01_ECX_RESERVED_16_16 16u

/** ECX bit 15 */
#define CPUID_01_ECX_PERF_CAPABILITIES 15u

/** ECX bit 14 */
#define CPUID_01_ECX_XTPR_UPDATE_CONTROL 14u

/** ECX bit 13 */
#define CPUID_01_ECX_CMPXCHG16B 13u

/** ECX bit 12 */
#define CPUID_01_ECX_FMA 12u

/** ECX bit 11 */
#define CPUID_01_ECX_DEBUG_INTERFACE 11u

/** ECX bit 10 */
#define CPUID_01_ECX_L1_CONTEXT_ID 10u

/** ECX bit 9 */
#define CPUID_01_ECX_SSSE3 9u

/** ECX bit 8 */
#define CPUID_01_ECX_TM2 8u

/** ECX bit 7 */
#define CPUID_01_ECX_EIST 7u

/** ECX bit 6 */
#define CPUID_01_ECX_SMX 6u

/** ECX bit 5 */
#define CPUID_01_ECX_VMX 5u

/** ECX bit 4 */
#define CPUID_01_ECX_DS_CPL 4u

/** ECX bit 3 */
#define CPUID_01_ECX_MONITOR 3u

/** ECX bit 2 */
#define CPUID_01_ECX_DTES64 2u

/** ECX bit 1 */
#define CPUID_01_ECX_PCLMULQDQ 1u

/** ECX bit 0 */
#define CPUID_01_ECX_SSE3 0u

/** EDX bit 31 */
#define CPUID_01_EDX_PBE 31u

/** EDX bit 30 -- RESERVADO, no se toca */
#define CPUID_01_EDX_RESERVED_30_30 30u

/** EDX bit 29 */
#define CPUID_01_EDX_TM 29u

/** EDX bit 28 */
#define CPUID_01_EDX_HTT 28u

/** EDX bit 27 */
#define CPUID_01_EDX_SELF_SNOOP 27u

/** EDX bit 26 */
#define CPUID_01_EDX_SSE2 26u

/** EDX bit 25 */
#define CPUID_01_EDX_SSE 25u

/** EDX bit 24 */
#define CPUID_01_EDX_FXSR 24u

/** EDX bit 23 */
#define CPUID_01_EDX_MMX 23u

/** EDX bit 22 */
#define CPUID_01_EDX_ACPI 22u

/** EDX bit 21 */
#define CPUID_01_EDX_DS 21u

/** EDX bit 20 -- RESERVADO, no se toca */
#define CPUID_01_EDX_RESERVED_20_20 20u

/** EDX bit 19 */
#define CPUID_01_EDX_CLFLUSH 19u

/** EDX bit 18 */
#define CPUID_01_EDX_PSN 18u

/** EDX bit 17 */
#define CPUID_01_EDX_PSE_36 17u

/** EDX bit 16 */
#define CPUID_01_EDX_PAT 16u

/** EDX bit 15 */
#define CPUID_01_EDX_CMOV 15u

/** EDX bit 14 */
#define CPUID_01_EDX_MCA 14u

/** EDX bit 13 */
#define CPUID_01_EDX_PGE 13u

/** EDX bit 12 */
#define CPUID_01_EDX_MTRR 12u

/** EDX bit 11 */
#define CPUID_01_EDX_SEP 11u

/** EDX bit 10 -- RESERVADO, no se toca */
#define CPUID_01_EDX_RESERVED_10_10 10u

/** EDX bit 9 */
#define CPUID_01_EDX_APIC 9u

/** EDX bit 8 */
#define CPUID_01_EDX_CMPXCHG8B 8u

/** EDX bit 7 */
#define CPUID_01_EDX_MCE 7u

/** EDX bit 6 */
#define CPUID_01_EDX_PAE 6u

/** EDX bit 5 */
#define CPUID_01_EDX_MSR 5u

/** EDX bit 4 */
#define CPUID_01_EDX_TSC 4u

/** EDX bit 3 */
#define CPUID_01_EDX_PSE 3u

/** EDX bit 2 */
#define CPUID_01_EDX_DE 2u

/** EDX bit 1 */
#define CPUID_01_EDX_VME 1u

/** EDX bit 0 */
#define CPUID_01_EDX_FPU 0u

/* ---- hoja 07H, subhoja 00H -- Structured Extended Feature Flags Main Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_07_00_EAX_MAX_SUBLEAF_SHIFT 0u
#define CPUID_07_00_EAX_MAX_SUBLEAF_MASK 0xFFFFFFFFu

/** EBX bit 31 */
#define CPUID_07_00_EBX_AVX512VL 31u

/** EBX bit 30 */
#define CPUID_07_00_EBX_AVX512BW 30u

/** EBX bit 29 */
#define CPUID_07_00_EBX_SHA 29u

/** EBX bit 28 */
#define CPUID_07_00_EBX_AVX512CD 28u

/** EBX bit 27 */
#define CPUID_07_00_EBX_AVX512ER 27u

/** EBX bit 26 */
#define CPUID_07_00_EBX_AVX512PF 26u

/** EBX bit 25 */
#define CPUID_07_00_EBX_INTEL_PROC_TRACE 25u

/** EBX bit 24 */
#define CPUID_07_00_EBX_CLWB 24u

/** EBX bit 23 */
#define CPUID_07_00_EBX_CLFLUSHOPT 23u

/** EBX bit 22 -- RESERVADO, no se toca */
#define CPUID_07_00_EBX_RESERVED_22_22 22u

/** EBX bit 21 */
#define CPUID_07_00_EBX_AVX512_IFMA 21u

/** EBX bit 20 */
#define CPUID_07_00_EBX_SMAP 20u

/** EBX bit 19 */
#define CPUID_07_00_EBX_ADX 19u

/** EBX bit 18 */
#define CPUID_07_00_EBX_RDSEED 18u

/** EBX bit 17 */
#define CPUID_07_00_EBX_AVX512DQ 17u

/** EBX bit 16 */
#define CPUID_07_00_EBX_AVX512F 16u

/** EBX bit 15 */
#define CPUID_07_00_EBX_RDT_A 15u

/** EBX bit 14 */
#define CPUID_07_00_EBX_MPX 14u

/** EBX bit 13 */
#define CPUID_07_00_EBX_FCS_FDS_DEPRECATION 13u

/** EBX bit 12 */
#define CPUID_07_00_EBX_RDT_M 12u

/** EBX bit 11 */
#define CPUID_07_00_EBX_RTM 11u

/** EBX bit 10 */
#define CPUID_07_00_EBX_INVPCID 10u

/** EBX bit 9 */
#define CPUID_07_00_EBX_ENH_REP_MOVSB_STOSB 9u

/** EBX bit 8 */
#define CPUID_07_00_EBX_BMI2 8u

/** EBX bit 7 */
#define CPUID_07_00_EBX_SMEP 7u

/** EBX bit 6 */
#define CPUID_07_00_EBX_FDP_EXCPTN_ONLY 6u

/** EBX bit 5 */
#define CPUID_07_00_EBX_AVX2 5u

/** EBX bit 4 */
#define CPUID_07_00_EBX_HLE 4u

/** EBX bit 3 */
#define CPUID_07_00_EBX_BMI1 3u

/** EBX bit 2 */
#define CPUID_07_00_EBX_SGX 2u

/** EBX bit 1 */
#define CPUID_07_00_EBX_TSC_ADJUST 1u

/** EBX bit 0 */
#define CPUID_07_00_EBX_FSGSBASE 0u

/** ECX bit 31 */
#define CPUID_07_00_ECX_PKS 31u

/** ECX bit 30 */
#define CPUID_07_00_ECX_SGX_LC 30u

/** ECX bit 29 */
#define CPUID_07_00_ECX_ENQCMD 29u

/** ECX bit 28 */
#define CPUID_07_00_ECX_MOVDIR64B 28u

/** ECX bit 27 */
#define CPUID_07_00_ECX_MOVDIRI 27u

/** ECX bit 26 -- RESERVADO, no se toca */
#define CPUID_07_00_ECX_RESERVED_26_26 26u

/** ECX bit 25 */
#define CPUID_07_00_ECX_CLDEMOTE 25u

/** ECX bit 24 */
#define CPUID_07_00_ECX_BUS_LOCK_DETECT 24u

/** ECX bit 23 */
#define CPUID_07_00_ECX_KEY_LOCKER 23u

/** ECX bit 22 */
#define CPUID_07_00_ECX_RDPID 22u

/** ECX bits 21:17 */
#define CPUID_07_00_ECX_MPX_MAWAU_SHIFT 17u
#define CPUID_07_00_ECX_MPX_MAWAU_MASK 0x1Fu

/** ECX bit 16 */
#define CPUID_07_00_ECX_LA57 16u

/** ECX bit 15 -- RESERVADO, no se toca */
#define CPUID_07_00_ECX_RESERVED_15_15 15u

/** ECX bit 14 */
#define CPUID_07_00_ECX_AVX512_VPOPCNTDQ 14u

/** ECX bit 13 */
#define CPUID_07_00_ECX_TME_EN 13u

/** ECX bit 12 */
#define CPUID_07_00_ECX_AVX512_BITALG 12u

/** ECX bit 11 */
#define CPUID_07_00_ECX_AVX512_VNNI 11u

/** ECX bit 10 */
#define CPUID_07_00_ECX_VPCLMULQDQ 10u

/** ECX bit 9 */
#define CPUID_07_00_ECX_VAES 9u

/** ECX bit 8 */
#define CPUID_07_00_ECX_GFNI 8u

/** ECX bit 7 */
#define CPUID_07_00_ECX_CET_SS 7u

/** ECX bit 6 */
#define CPUID_07_00_ECX_AVX512_VBMI2 6u

/** ECX bit 5 */
#define CPUID_07_00_ECX_WAITPKG 5u

/** ECX bit 4 */
#define CPUID_07_00_ECX_OSPKE 4u

/** ECX bit 3 */
#define CPUID_07_00_ECX_PKU 3u

/** ECX bit 2 */
#define CPUID_07_00_ECX_UMIP 2u

/** ECX bit 1 */
#define CPUID_07_00_ECX_AVX512_VBMI 1u

/** ECX bit 0 */
#define CPUID_07_00_ECX_PREFETCHWT1 0u

/** EDX bit 31 */
#define CPUID_07_00_EDX_SPEC_CTRL_SSBD 31u

/** EDX bit 30 */
#define CPUID_07_00_EDX_CORE_CAPABILITIES 30u

/** EDX bit 29 */
#define CPUID_07_00_EDX_ARCH_CAPABILITIES 29u

/** EDX bit 28 */
#define CPUID_07_00_EDX_L1D_FLUSH_INTERFACE 28u

/** EDX bit 27 */
#define CPUID_07_00_EDX_SPEC_CTRL_ST_PREDICTORS 27u

/** EDX bit 26 */
#define CPUID_07_00_EDX_IBRS_IBPB 26u

/** EDX bit 25 */
#define CPUID_07_00_EDX_AMX_INT8 25u

/** EDX bit 24 */
#define CPUID_07_00_EDX_AMX_TILE 24u

/** EDX bit 23 */
#define CPUID_07_00_EDX_AVX512_FP16 23u

/** EDX bit 22 */
#define CPUID_07_00_EDX_AMX_BF16 22u

/** EDX bit 21 -- RESERVADO, no se toca */
#define CPUID_07_00_EDX_RESERVED_21_21 21u

/** EDX bit 20 */
#define CPUID_07_00_EDX_CET_IBT 20u

/** EDX bit 19 */
#define CPUID_07_00_EDX_ARCH_LBRS 19u

/** EDX bit 18 */
#define CPUID_07_00_EDX_PCONFIG 18u

/** EDX bit 17 -- RESERVADO, no se toca */
#define CPUID_07_00_EDX_RESERVED_17_17 17u

/** EDX bit 16 */
#define CPUID_07_00_EDX_TSXLDTRK 16u

/** EDX bit 15 */
#define CPUID_07_00_EDX_HYBRID 15u

/** EDX bit 14 */
#define CPUID_07_00_EDX_SERIALIZE 14u

/** EDX bit 13 */
#define CPUID_07_00_EDX_RTM_FORCE_ABORT 13u

/** EDX bit 12 -- RESERVADO, no se toca */
#define CPUID_07_00_EDX_RESERVED_12_12 12u

/** EDX bit 11 */
#define CPUID_07_00_EDX_RTM_ALWAYS_ABORT 11u

/** EDX bit 10 */
#define CPUID_07_00_EDX_MD_CLEAR 10u

/** EDX bit 9 */
#define CPUID_07_00_EDX_MCU_OPT_CTRL 9u

/** EDX bit 8 */
#define CPUID_07_00_EDX_AVX512_VP2INTERSECT 8u

/** EDX bits 7:6 -- RESERVADO, no se toca */
#define CPUID_07_00_EDX_RESERVED_7_6_SHIFT 6u
#define CPUID_07_00_EDX_RESERVED_7_6_MASK 0x3u

/** EDX bit 5 */
#define CPUID_07_00_EDX_UINTR 5u

/** EDX bit 4 */
#define CPUID_07_00_EDX_FAST_SHORT_REP_MOVSB 4u

/** EDX bit 3 */
#define CPUID_07_00_EDX_AVX512_4FMAPS 3u

/** EDX bit 2 */
#define CPUID_07_00_EDX_AVX512_4VNNIW 2u

/** EDX bit 1 */
#define CPUID_07_00_EDX_SGX_KEYS 1u

/** EDX bit 0 -- RESERVADO, no se toca */
#define CPUID_07_00_EDX_RESERVED_0_0 0u

/* ---- hoja 07H, subhoja 01H -- Structured Extended Feature Sub-Leaf 1 ---- */

/** EAX bit 31 -- RESERVADO, no se toca */
#define CPUID_07_01_EAX_RESERVED_31_31 31u

/** EAX bit 30 */
#define CPUID_07_01_EAX_INVD_DISABLE_POST_BIOS_DONE 30u

/** EAX bits 29:28 -- RESERVADO, no se toca */
#define CPUID_07_01_EAX_RESERVED_29_28_SHIFT 28u
#define CPUID_07_01_EAX_RESERVED_29_28_MASK 0x3u

/** EAX bit 27 */
#define CPUID_07_01_EAX_MSRLIST 27u

/** EAX bit 26 */
#define CPUID_07_01_EAX_LAM 26u

/** EAX bits 25:24 -- RESERVADO, no se toca */
#define CPUID_07_01_EAX_RESERVED_25_24_SHIFT 24u
#define CPUID_07_01_EAX_RESERVED_25_24_MASK 0x3u

/** EAX bit 23 */
#define CPUID_07_01_EAX_AVX_IFMA 23u

/** EAX bit 22 */
#define CPUID_07_01_EAX_HRESET 22u

/** EAX bit 21 */
#define CPUID_07_01_EAX_AMX_FP16 21u

/** EAX bit 20 -- RESERVADO, no se toca */
#define CPUID_07_01_EAX_RESERVED_20_20 20u

/** EAX bit 19 */
#define CPUID_07_01_EAX_WRMSRNS 19u

/** EAX bit 18 */
#define CPUID_07_01_EAX_LKGS 18u

/** EAX bit 17 */
#define CPUID_07_01_EAX_FRED 17u

/** EAX bits 16:13 -- RESERVADO, no se toca */
#define CPUID_07_01_EAX_RESERVED_16_13_SHIFT 13u
#define CPUID_07_01_EAX_RESERVED_16_13_MASK 0xFu

/** EAX bit 12 */
#define CPUID_07_01_EAX_FAST_REP_CMPSB_SCASB 12u

/** EAX bit 11 */
#define CPUID_07_01_EAX_FAST_REP_STOSB 11u

/** EAX bit 10 */
#define CPUID_07_01_EAX_FAST_REP_MOVSB 10u

/** EAX bit 9 -- RESERVADO, no se toca */
#define CPUID_07_01_EAX_RESERVED_9_9 9u

/** EAX bit 8 */
#define CPUID_07_01_EAX_ARCH_PERFMON_EXT 8u

/** EAX bit 7 */
#define CPUID_07_01_EAX_CMPCCXADD 7u

/** EAX bit 6 */
#define CPUID_07_01_EAX_LASS 6u

/** EAX bit 5 */
#define CPUID_07_01_EAX_AVX512_BF16 5u

/** EAX bit 4 */
#define CPUID_07_01_EAX_AVX_VNNI 4u

/** EAX bit 3 -- RESERVADO, no se toca */
#define CPUID_07_01_EAX_RESERVED_3_3 3u

/** EAX bit 2 */
#define CPUID_07_01_EAX_SM4 2u

/** EAX bit 1 */
#define CPUID_07_01_EAX_SM3 1u

/** EAX bit 0 */
#define CPUID_07_01_EAX_SHA512 0u

/** EBX bits 31:4 -- RESERVADO, no se toca */
#define CPUID_07_01_EBX_RESERVED_31_4_SHIFT 4u
#define CPUID_07_01_EBX_RESERVED_31_4_MASK 0xFFFFFFFu

/** EBX bit 3 */
#define CPUID_07_01_EBX_CPUIDMAXVAL_LIM_RMV 3u

/** EBX bit 2 -- RESERVADO, no se toca */
#define CPUID_07_01_EBX_RESERVED_2_2 2u

/** EBX bit 1 */
#define CPUID_07_01_EBX_PBNDKB 1u

/** EBX bit 0 */
#define CPUID_07_01_EBX_PPIN 0u

/** ECX bits 31:2 -- RESERVADO, no se toca */
#define CPUID_07_01_ECX_RESERVED_31_2_SHIFT 2u
#define CPUID_07_01_ECX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** ECX bit 1 */
#define CPUID_07_01_ECX_RDT_A_ASYM 1u

/** ECX bit 0 */
#define CPUID_07_01_ECX_RDT_M_ASYM 0u

/** EDX bits 31:25 -- RESERVADO, no se toca */
#define CPUID_07_01_EDX_RESERVED_31_25_SHIFT 25u
#define CPUID_07_01_EDX_RESERVED_31_25_MASK 0x7Fu

/** EDX bit 24 */
#define CPUID_07_01_EDX_SLSM 24u

/** EDX bit 23 */
#define CPUID_07_01_EDX_MWAIT 23u

/** EDX bits 21:20 -- RESERVADO, no se toca */
#define CPUID_07_01_EDX_RESERVED_21_20_SHIFT 20u
#define CPUID_07_01_EDX_RESERVED_21_20_MASK 0x3u

/** EDX bit 19 */
#define CPUID_07_01_EDX_AVX10 19u

/** EDX bit 18 */
#define CPUID_07_01_EDX_CET_SSS 18u

/** EDX bit 17 */
#define CPUID_07_01_EDX_UIRET_UIF 17u

/** EDX bits 16:15 -- RESERVADO, no se toca */
#define CPUID_07_01_EDX_RESERVED_16_15_SHIFT 15u
#define CPUID_07_01_EDX_RESERVED_16_15_MASK 0x3u

/** EDX bit 14 */
#define CPUID_07_01_EDX_PREFETCHI 14u

/** EDX bits 13:11 -- RESERVADO, no se toca */
#define CPUID_07_01_EDX_RESERVED_13_11_SHIFT 11u
#define CPUID_07_01_EDX_RESERVED_13_11_MASK 0x7u

/** EDX bit 10 */
#define CPUID_07_01_EDX_AVX_VNNI_INT16 10u

/** EDX bit 9 -- RESERVADO, no se toca */
#define CPUID_07_01_EDX_RESERVED_9_9 9u

/** EDX bit 8 */
#define CPUID_07_01_EDX_AMX_COMPLEX 8u

/** EDX bits 7:6 -- RESERVADO, no se toca */
#define CPUID_07_01_EDX_RESERVED_7_6_SHIFT 6u
#define CPUID_07_01_EDX_RESERVED_7_6_MASK 0x3u

/** EDX bit 5 */
#define CPUID_07_01_EDX_AVX_NE_CONVERT 5u

/** EDX bit 4 */
#define CPUID_07_01_EDX_AVX_VNNI_INT8 4u

/** EDX bits 3:0 -- RESERVADO, no se toca */
#define CPUID_07_01_EDX_RESERVED_3_0_SHIFT 0u
#define CPUID_07_01_EDX_RESERVED_3_0_MASK 0xFu

/* ---- hoja 07H, subhoja 02H -- Structured Extended Feature Sub-Leaf 2 ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_07_02_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_07_02_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_07_02_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_07_02_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_07_02_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_07_02_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:8 -- RESERVADO, no se toca */
#define CPUID_07_02_EDX_RESERVED_31_8_SHIFT 8u
#define CPUID_07_02_EDX_RESERVED_31_8_MASK 0xFFFFFFu

/** EDX bit 7 */
#define CPUID_07_02_EDX_MONITOR_MITG_NO 7u

/** EDX bit 6 */
#define CPUID_07_02_EDX_UC_LOCK_DISABLE 6u

/** EDX bit 5 */
#define CPUID_07_02_EDX_MCDT_NO 5u

/** EDX bit 4 */
#define CPUID_07_02_EDX_BHI_CTRL 4u

/** EDX bit 3 */
#define CPUID_07_02_EDX_DDPD_U 3u

/** EDX bit 2 */
#define CPUID_07_02_EDX_RRSBA_CTRL 2u

/** EDX bit 1 */
#define CPUID_07_02_EDX_IPRED_CTRL 1u

/** EDX bit 0 */
#define CPUID_07_02_EDX_PSFD 0u

/* ---- hoja 0DH, subhoja 00H -- Processor Extended State Main Sub-Leaf ---- */

/** EAX bits 31:19 -- RESERVADO, no se toca */
#define CPUID_0D_00_EAX_RESERVED_31_19_SHIFT 19u
#define CPUID_0D_00_EAX_RESERVED_31_19_MASK 0x1FFFu

/** EAX bit 18 */
#define CPUID_0D_00_EAX_AMX_TILEDATA 18u

/** EAX bit 17 */
#define CPUID_0D_00_EAX_AMX_TILECFG 17u

/** EAX bits 16:10 -- RESERVADO, no se toca */
#define CPUID_0D_00_EAX_NA_16_10_SHIFT 10u
#define CPUID_0D_00_EAX_NA_16_10_MASK 0x7Fu

/** EAX bit 9 */
#define CPUID_0D_00_EAX_PKRU 9u

/** EAX bit 8 -- RESERVADO, no se toca */
#define CPUID_0D_00_EAX_NA_8_8 8u

/** EAX bit 7 */
#define CPUID_0D_00_EAX_AVX512_HI16_ZMM 7u

/** EAX bit 6 */
#define CPUID_0D_00_EAX_AVX512_ZMM_HI256 6u

/** EAX bit 5 */
#define CPUID_0D_00_EAX_AVX512_OPMASK 5u

/** EAX bit 4 */
#define CPUID_0D_00_EAX_MPX_BNDCSR 4u

/** EAX bit 3 */
#define CPUID_0D_00_EAX_MPX_BNDREGS 3u

/** EAX bit 2 */
#define CPUID_0D_00_EAX_AVX 2u

/** EAX bit 1 */
#define CPUID_0D_00_EAX_SSE 1u

/** EAX bit 0 */
#define CPUID_0D_00_EAX_X87 0u

/** EBX bits 31:0 */
#define CPUID_0D_00_EBX_XSAVE_BYTES_ENABLED_FEATURES_SHIFT 0u
#define CPUID_0D_00_EBX_XSAVE_BYTES_ENABLED_FEATURES_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_0D_00_ECX_XSAVE_BYTES_SUPPORTED_FEATURES_SHIFT 0u
#define CPUID_0D_00_ECX_XSAVE_BYTES_SUPPORTED_FEATURES_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_0D_00_EDX_VALID_XCR0_UPPER_32_SHIFT 0u
#define CPUID_0D_00_EDX_VALID_XCR0_UPPER_32_MASK 0xFFFFFFFFu

/* ---- hoja 0DH, subhoja 01H -- Feature and Supervisor State Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_0D_01_EAX_COMP_SIZE_SHIFT 0u
#define CPUID_0D_01_EAX_COMP_SIZE_MASK 0xFFFFFFFFu

/** EAX bits 31:5 -- RESERVADO, no se toca */
#define CPUID_0D_01_EAX_RESERVED_31_5_SHIFT 5u
#define CPUID_0D_01_EAX_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX bit 4 */
#define CPUID_0D_01_EAX_XFD 4u

/** EAX bit 3 */
#define CPUID_0D_01_EAX_XSAVES 3u

/** EAX bit 2 */
#define CPUID_0D_01_EAX_XGETBV1 2u

/** EAX bit 1 */
#define CPUID_0D_01_EAX_XSAVEC 1u

/** EAX bit 0 */
#define CPUID_0D_01_EAX_XSAVEOPT 0u

/** EBX bits 31:0 */
#define CPUID_0D_01_EBX_COMP_OFFSET_SHIFT 0u
#define CPUID_0D_01_EBX_COMP_OFFSET_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_0D_01_EBX_XSAVES_BYTES_ENABLED_FEATURES_SHIFT 0u
#define CPUID_0D_01_EBX_XSAVES_BYTES_ENABLED_FEATURES_MASK 0xFFFFFFFFu

/** ECX bits 31:19 -- RESERVADO, no se toca */
#define CPUID_0D_01_ECX_RESERVED_31_19_SHIFT 19u
#define CPUID_0D_01_ECX_RESERVED_31_19_MASK 0x1FFFu

/** ECX bits 31:3 -- RESERVADO, no se toca */
#define CPUID_0D_01_ECX_RESERVED_31_3_SHIFT 3u
#define CPUID_0D_01_ECX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** ECX bits 18:17 -- RESERVADO, no se toca */
#define CPUID_0D_01_ECX_NA_18_17_SHIFT 17u
#define CPUID_0D_01_ECX_NA_18_17_MASK 0x3u

/** ECX bit 16 */
#define CPUID_0D_01_ECX_HWP 16u

/** ECX bit 15 */
#define CPUID_0D_01_ECX_LBR 15u

/** ECX bit 14 */
#define CPUID_0D_01_ECX_UINTR 14u

/** ECX bit 13 */
#define CPUID_0D_01_ECX_HDC 13u

/** ECX bit 12 */
#define CPUID_0D_01_ECX_CET_S 12u

/** ECX bit 11 */
#define CPUID_0D_01_ECX_CET_U 11u

/** ECX bit 10 */
#define CPUID_0D_01_ECX_PASID 10u

/** ECX bit 9 -- RESERVADO, no se toca */
#define CPUID_0D_01_ECX_RESERVED_9_9 9u

/** ECX bit 8 */
#define CPUID_0D_01_ECX_PT 8u

/** ECX bits 7:0 -- RESERVADO, no se toca */
#define CPUID_0D_01_ECX_NA_7_0_SHIFT 0u
#define CPUID_0D_01_ECX_NA_7_0_MASK 0xFFu

/** ECX bit 2 */
#define CPUID_0D_01_ECX_COMP_XFD 2u

/** ECX bit 1 */
#define CPUID_0D_01_ECX_COMP_64B_ALIGNED 1u

/** ECX bit 0 */
#define CPUID_0D_01_ECX_COMP_SUP 0u

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_0D_01_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_0D_01_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/*
 * Filas del manual que REMITEN A OTRA TABLA en vez de
 * definir un campo.  Se listan para que no parezca que
 * esos registros no estan documentados.
 *
 *   hoja 01H       EAX[31:0]  -> Version...
 *   hoja 01H       EBX[31:0]  -> Feature...
 *   hoja 01H       ECX[31:0]  -> Feature...
 *   hoja 01H       ECX[31:31]  -> Not...
 *   hoja 01H       EDX[31:0]  -> Feature...
 *   hoja 07H.00H   EBX[31:0]  -> Extended...
 *   hoja 07H.00H   ECX[31:0]  -> Extended...
 *   hoja 07H.00H   EDX[31:0]  -> Extended...
 *   hoja 07H.01H   EDX[22:22]  -> SEC-TEE-ATTESTATION...
 *   hoja 07H.02H   EDX[31:0]  -> Extended...
 */

#endif /* VXP_COMMON_CPUID_FEATURES_H */
