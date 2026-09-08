/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_misc.h
 * @brief Hojas que no caen en ninguna familia
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_amd_cpuid_index.py
 *
 * a partir del manual de AMD, que no viaja en el repositorio.
 *
 * ALCANCE: **SOLO AMD**.  Los de Intel estan en `common/cpuid/intel/`, y no se
 * mezclan porque el rango extendido lo reparten los dos: Intel llega a
 * `80000008H` y de ahi en adelante manda AMD.
 *
 * Los nombres son los del manual de AMD -- CamelCase, no MAYUSCULAS --, para
 * que se pueda buscar en el la misma cadena que hay aqui.  El prefijo `AMD_`
 * dice de que manual salio cada macro.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef VXP_COMMON_CPUID_AMD_MISC_H
#define VXP_COMMON_CPUID_AMD_MISC_H

/* ---- Fn00000000, EAX ---- */

/** EAX bits 31:0 */
#define AMD_CPUID_00000000_EAX_LFuncStd_SHIFT 0u
#define AMD_CPUID_00000000_EAX_LFuncStd_MASK 0xFFFFFFFFu

/* ---- Fn00000000, EBX ---- */

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000000_EBX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000000_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn00000000, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000000_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000000_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn00000000, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000000_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000000_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn00000001, EAX ---- */

/** EAX bits 31:28 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_EAX_RESERVED_31_28_SHIFT 28u
#define AMD_CPUID_00000001_EAX_RESERVED_31_28_MASK 0xFu

/** EAX bits 27:20 */
#define AMD_CPUID_00000001_EAX_ExtFamily_SHIFT 20u
#define AMD_CPUID_00000001_EAX_ExtFamily_MASK 0xFFu

/** EAX bits 19:16 */
#define AMD_CPUID_00000001_EAX_ExtModel_SHIFT 16u
#define AMD_CPUID_00000001_EAX_ExtModel_MASK 0xFu

/** EAX bits 15:12 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_EAX_RESERVED_15_12_SHIFT 12u
#define AMD_CPUID_00000001_EAX_RESERVED_15_12_MASK 0xFu

/** EAX bits 11:8 */
#define AMD_CPUID_00000001_EAX_BaseFamily_SHIFT 8u
#define AMD_CPUID_00000001_EAX_BaseFamily_MASK 0xFu

/** EAX bits 7:4 */
#define AMD_CPUID_00000001_EAX_BaseModel_SHIFT 4u
#define AMD_CPUID_00000001_EAX_BaseModel_MASK 0xFu

/** EAX bits 3:0 */
#define AMD_CPUID_00000001_EAX_Stepping_SHIFT 0u
#define AMD_CPUID_00000001_EAX_Stepping_MASK 0xFu

/* ---- Fn00000001, EBX ---- */

/** EBX bits 31:24 */
#define AMD_CPUID_00000001_EBX_LocalApicId_SHIFT 24u
#define AMD_CPUID_00000001_EBX_LocalApicId_MASK 0xFFu

/** EBX bits 23:16 */
#define AMD_CPUID_00000001_EBX_LogicalProcessor_SHIFT 16u
#define AMD_CPUID_00000001_EBX_LogicalProcessor_MASK 0xFFu

/** EBX bits 15:8 */
#define AMD_CPUID_00000001_EBX_CLFlush_SHIFT 8u
#define AMD_CPUID_00000001_EBX_CLFlush_MASK 0xFFu

/** EBX bits 7:0 */
#define AMD_CPUID_00000001_EBX_8BitBrandId_SHIFT 0u
#define AMD_CPUID_00000001_EBX_8BitBrandId_MASK 0xFFu

/* ---- Fn00000001, ECX ---- */

/** ECX bit 31 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_ECX_RESERVED_31_31 31u

/** ECX bit 30 */
#define AMD_CPUID_00000001_ECX_RDRAND 30u

/** ECX bit 29 */
#define AMD_CPUID_00000001_ECX_F16C 29u

/** ECX bit 28 */
#define AMD_CPUID_00000001_ECX_AVX 28u

/** ECX bit 27 */
#define AMD_CPUID_00000001_ECX_OSXSAVE 27u

/** ECX bit 26 */
#define AMD_CPUID_00000001_ECX_XSAVE 26u

/** ECX bit 25 */
#define AMD_CPUID_00000001_ECX_AES 25u

/** ECX bit 24 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_ECX_RESERVED_24_24 24u

/** ECX bit 23 */
#define AMD_CPUID_00000001_ECX_POPCNT 23u

/** ECX bit 21 */
#define AMD_CPUID_00000001_ECX_x2APIC 21u

/** ECX bit 20 */
#define AMD_CPUID_00000001_ECX_SSE42 20u

/** ECX bit 19 */
#define AMD_CPUID_00000001_ECX_SSE41 19u

/** ECX bits 18:14 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_ECX_RESERVED_18_14_SHIFT 14u
#define AMD_CPUID_00000001_ECX_RESERVED_18_14_MASK 0x1Fu

/** ECX bit 13 */
#define AMD_CPUID_00000001_ECX_CMPXCHG16B 13u

/** ECX bit 12 */
#define AMD_CPUID_00000001_ECX_FMA 12u

/** ECX bits 11:10 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_ECX_RESERVED_11_10_SHIFT 10u
#define AMD_CPUID_00000001_ECX_RESERVED_11_10_MASK 0x3u

/** ECX bit 9 */
#define AMD_CPUID_00000001_ECX_SSSE3 9u

/** ECX bits 8:4 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_ECX_RESERVED_8_4_SHIFT 4u
#define AMD_CPUID_00000001_ECX_RESERVED_8_4_MASK 0x1Fu

/** ECX bit 3 */
#define AMD_CPUID_00000001_ECX_MONITOR 3u

/** ECX bit 2 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_ECX_RESERVED_2_2 2u

/** ECX bit 1 */
#define AMD_CPUID_00000001_ECX_PCLMULQDQ 1u

/** ECX bit 0 */
#define AMD_CPUID_00000001_ECX_SSE3 0u

/* ---- Fn00000001, EDX ---- */

/** EDX bits 31:29 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_EDX_RESERVED_31_29_SHIFT 29u
#define AMD_CPUID_00000001_EDX_RESERVED_31_29_MASK 0x7u

/** EDX bit 28 */
#define AMD_CPUID_00000001_EDX_HTT 28u

/** EDX bit 27 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_EDX_RESERVED_27_27 27u

/** EDX bit 26 */
#define AMD_CPUID_00000001_EDX_SSE2 26u

/** EDX bit 25 */
#define AMD_CPUID_00000001_EDX_SSE 25u

/** EDX bit 24 */
#define AMD_CPUID_00000001_EDX_FXSR 24u

/** EDX bit 23 */
#define AMD_CPUID_00000001_EDX_MMX 23u

/** EDX bits 22:20 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_EDX_RESERVED_22_20_SHIFT 20u
#define AMD_CPUID_00000001_EDX_RESERVED_22_20_MASK 0x7u

/** EDX bit 19 */
#define AMD_CPUID_00000001_EDX_CLFSH 19u

/** EDX bit 18 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_EDX_RESERVED_18_18 18u

/** EDX bit 17 */
#define AMD_CPUID_00000001_EDX_PSE36 17u

/** EDX bit 16 */
#define AMD_CPUID_00000001_EDX_PAT 16u

/** EDX bit 15 */
#define AMD_CPUID_00000001_EDX_CMOV 15u

/** EDX bit 14 */
#define AMD_CPUID_00000001_EDX_MCA 14u

/** EDX bit 13 */
#define AMD_CPUID_00000001_EDX_PGE 13u

/** EDX bit 12 */
#define AMD_CPUID_00000001_EDX_MTRR 12u

/** EDX bit 11 */
#define AMD_CPUID_00000001_EDX_SysEnterSysExit 11u

/** EDX bit 10 -- RESERVADO, no se toca */
#define AMD_CPUID_00000001_EDX_RESERVED_10_10 10u

/** EDX bit 9 */
#define AMD_CPUID_00000001_EDX_APIC 9u

/** EDX bit 8 */
#define AMD_CPUID_00000001_EDX_CMPXCHG8B 8u

/** EDX bit 7 */
#define AMD_CPUID_00000001_EDX_MCE 7u

/** EDX bit 6 */
#define AMD_CPUID_00000001_EDX_PAE 6u

/** EDX bit 5 */
#define AMD_CPUID_00000001_EDX_MSR 5u

/** EDX bit 4 */
#define AMD_CPUID_00000001_EDX_TSC 4u

/** EDX bit 3 */
#define AMD_CPUID_00000001_EDX_PSE 3u

/** EDX bit 2 */
#define AMD_CPUID_00000001_EDX_DE 2u

/** EDX bit 1 */
#define AMD_CPUID_00000001_EDX_VME 1u

/** EDX bit 0 */
#define AMD_CPUID_00000001_EDX_FPU 0u

/* ---- Fn00000005, EAX ---- */

/** EAX bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_00000005_EAX_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_00000005_EAX_RESERVED_31_16_MASK 0xFFFFu

/** EAX bits 15:0 */
#define AMD_CPUID_00000005_EAX_MonLineSizeMin_SHIFT 0u
#define AMD_CPUID_00000005_EAX_MonLineSizeMin_MASK 0xFFFFu

/* ---- Fn00000005, EBX ---- */

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_00000005_EBX_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_00000005_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:0 */
#define AMD_CPUID_00000005_EBX_MonLineSizeMax_SHIFT 0u
#define AMD_CPUID_00000005_EBX_MonLineSizeMax_MASK 0xFFFFu

/* ---- Fn00000005, ECX ---- */

/** ECX bits 31:2 -- RESERVADO, no se toca */
#define AMD_CPUID_00000005_ECX_RESERVED_31_2_SHIFT 2u
#define AMD_CPUID_00000005_ECX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** ECX bit 1 */
#define AMD_CPUID_00000005_ECX_IBE 1u

/** ECX bit 0 */
#define AMD_CPUID_00000005_ECX_EMX 0u

/* ---- Fn00000006, EAX ---- */

/** EAX bits 31:3 -- RESERVADO, no se toca */
#define AMD_CPUID_00000006_EAX_RESERVED_31_3_SHIFT 3u
#define AMD_CPUID_00000006_EAX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** EAX bit 2 */
#define AMD_CPUID_00000006_EAX_ARAT 2u

/** EAX bits 1:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000006_EAX_RESERVED_1_0_SHIFT 0u
#define AMD_CPUID_00000006_EAX_RESERVED_1_0_MASK 0x3u

/* ---- Fn00000006, ECX ---- */

/** ECX bits 31:1 -- RESERVADO, no se toca */
#define AMD_CPUID_00000006_ECX_RESERVED_31_1_SHIFT 1u
#define AMD_CPUID_00000006_ECX_RESERVED_31_1_MASK 0x7FFFFFFFu

/** ECX bit 0 */
#define AMD_CPUID_00000006_ECX_EffFreq 0u

/* ---- Fn00000007, EAX_x0 ---- */

/** EAX_x0 bits 31:0 */
#define AMD_CPUID_00000007_EAX_x0_MaxSubFn_SHIFT 0u
#define AMD_CPUID_00000007_EAX_x0_MaxSubFn_MASK 0xFFFFFFFFu

/* ---- Fn00000007, EAX_x1 ---- */

/** EAX_x1 bits 31:24 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EAX_x1_RESERVED_31_24_SHIFT 24u
#define AMD_CPUID_00000007_EAX_x1_RESERVED_31_24_MASK 0xFFu

/** EAX_x1 bit 23 */
#define AMD_CPUID_00000007_EAX_x1_AVX_IFMA 23u

/** EAX_x1 bits 22:19 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EAX_x1_RESERVED_22_19_SHIFT 19u
#define AMD_CPUID_00000007_EAX_x1_RESERVED_22_19_MASK 0xFu

/** EAX_x1 bit 18 */
#define AMD_CPUID_00000007_EAX_x1_LKGS 18u

/** EAX_x1 bit 17 */
#define AMD_CPUID_00000007_EAX_x1_FRED 17u

/** EAX_x1 bits 16:6 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EAX_x1_RESERVED_16_6_SHIFT 6u
#define AMD_CPUID_00000007_EAX_x1_RESERVED_16_6_MASK 0x7FFu

/** EAX_x1 bit 5 */
#define AMD_CPUID_00000007_EAX_x1_AVX512_BF16 5u

/** EAX_x1 bit 4 */
#define AMD_CPUID_00000007_EAX_x1_AVX_VNNI 4u

/** EAX_x1 bits 3:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EAX_x1_RESERVED_3_0_SHIFT 0u
#define AMD_CPUID_00000007_EAX_x1_RESERVED_3_0_MASK 0xFu

/* ---- Fn00000007, EBX_x0 ---- */

/** EBX_x0 bit 31 */
#define AMD_CPUID_00000007_EBX_x0_AVX512VL 31u

/** EBX_x0 bit 30 */
#define AMD_CPUID_00000007_EBX_x0_AVX512BW 30u

/** EBX_x0 bit 29 */
#define AMD_CPUID_00000007_EBX_x0_SHA 29u

/** EBX_x0 bit 28 */
#define AMD_CPUID_00000007_EBX_x0_AVX512CD 28u

/** EBX_x0 bits 27:25 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x0_RESERVED_27_25_SHIFT 25u
#define AMD_CPUID_00000007_EBX_x0_RESERVED_27_25_MASK 0x7u

/** EBX_x0 bit 24 */
#define AMD_CPUID_00000007_EBX_x0_CLWB 24u

/** EBX_x0 bit 23 */
#define AMD_CPUID_00000007_EBX_x0_CLFLUSHOPT 23u

/** EBX_x0 bit 22 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x0_RESERVED_22_22 22u

/** EBX_x0 bit 21 */
#define AMD_CPUID_00000007_EBX_x0_AVX512_IFMA 21u

/** EBX_x0 bit 20 */
#define AMD_CPUID_00000007_EBX_x0_SMAP 20u

/** EBX_x0 bit 19 */
#define AMD_CPUID_00000007_EBX_x0_ADX 19u

/** EBX_x0 bit 18 */
#define AMD_CPUID_00000007_EBX_x0_RDSEED 18u

/** EBX_x0 bit 17 */
#define AMD_CPUID_00000007_EBX_x0_AVX512DQ 17u

/** EBX_x0 bit 16 */
#define AMD_CPUID_00000007_EBX_x0_AVX512F 16u

/** EBX_x0 bit 15 */
#define AMD_CPUID_00000007_EBX_x0_PQE 15u

/** EBX_x0 bits 14:13 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x0_RESERVED_14_13_SHIFT 13u
#define AMD_CPUID_00000007_EBX_x0_RESERVED_14_13_MASK 0x3u

/** EBX_x0 bit 12 */
#define AMD_CPUID_00000007_EBX_x0_PQM 12u

/** EBX_x0 bit 11 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x0_RESERVED_11_11 11u

/** EBX_x0 bit 10 */
#define AMD_CPUID_00000007_EBX_x0_INVPCID 10u

/** EBX_x0 bit 9 */
#define AMD_CPUID_00000007_EBX_x0_ERMS 9u

/** EBX_x0 bit 8 */
#define AMD_CPUID_00000007_EBX_x0_BMI2 8u

/** EBX_x0 bit 7 */
#define AMD_CPUID_00000007_EBX_x0_SMEP 7u

/** EBX_x0 bit 6 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x0_RESERVED_6_6 6u

/** EBX_x0 bit 5 */
#define AMD_CPUID_00000007_EBX_x0_AVX2 5u

/** EBX_x0 bit 4 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x0_RESERVED_4_4 4u

/** EBX_x0 bit 3 */
#define AMD_CPUID_00000007_EBX_x0_BMI1 3u

/** EBX_x0 bit 2 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x0_RESERVED_2_2 2u

/** EBX_x0 bit 1 */
#define AMD_CPUID_00000007_EBX_x0_TSCADJUST 1u

/** EBX_x0 bit 0 */
#define AMD_CPUID_00000007_EBX_x0_FSGSBASE 0u

/* ---- Fn00000007, EBX_x1 ---- */

/** EBX_x1 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EBX_x1_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000007_EBX_x1_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn00000007, ECX_x0 ---- */

/** ECX_x0 bits 31:29 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_31_29_SHIFT 29u
#define AMD_CPUID_00000007_ECX_x0_RESERVED_31_29_MASK 0x7u

/** ECX_x0 bit 28 */
#define AMD_CPUID_00000007_ECX_x0_MOVDIR64B 28u

/** ECX_x0 bit 27 */
#define AMD_CPUID_00000007_ECX_x0_MOVDIRI 27u

/** ECX_x0 bits 26:25 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_26_25_SHIFT 25u
#define AMD_CPUID_00000007_ECX_x0_RESERVED_26_25_MASK 0x3u

/** ECX_x0 bit 24 */
#define AMD_CPUID_00000007_ECX_x0_BUSLOCKTRAP 24u

/** ECX_x0 bit 23 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_23_23 23u

/** ECX_x0 bit 22 */
#define AMD_CPUID_00000007_ECX_x0_RDPID 22u

/** ECX_x0 bits 21:17 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_21_17_SHIFT 17u
#define AMD_CPUID_00000007_ECX_x0_RESERVED_21_17_MASK 0x1Fu

/** ECX_x0 bit 16 */
#define AMD_CPUID_00000007_ECX_x0_LA57 16u

/** ECX_x0 bit 15 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_15_15 15u

/** ECX_x0 bit 14 */
#define AMD_CPUID_00000007_ECX_x0_AVX512_VPOPC 14u

/** ECX_x0 bit 13 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_13_13 13u

/** ECX_x0 bit 12 */
#define AMD_CPUID_00000007_ECX_x0_AVX512_BITALG 12u

/** ECX_x0 bit 11 */
#define AMD_CPUID_00000007_ECX_x0_AVX512_VNNI 11u

/** ECX_x0 bit 10 */
#define AMD_CPUID_00000007_ECX_x0_VPCMULQDQ 10u

/** ECX_x0 bit 9 */
#define AMD_CPUID_00000007_ECX_x0_VAES 9u

/** ECX_x0 bit 8 */
#define AMD_CPUID_00000007_ECX_x0_GFNI 8u

/** ECX_x0 bit 7 */
#define AMD_CPUID_00000007_ECX_x0_CET_SS 7u

/** ECX_x0 bit 6 */
#define AMD_CPUID_00000007_ECX_x0_AVX512_VBMI2 6u

/** ECX_x0 bit 5 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_5_5 5u

/** ECX_x0 bit 4 */
#define AMD_CPUID_00000007_ECX_x0_OSPKE 4u

/** ECX_x0 bit 3 */
#define AMD_CPUID_00000007_ECX_x0_PKU 3u

/** ECX_x0 bit 2 */
#define AMD_CPUID_00000007_ECX_x0_UMIP 2u

/** ECX_x0 bit 1 */
#define AMD_CPUID_00000007_ECX_x0_AVX512_VBMI 1u

/** ECX_x0 bit 0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x0_RESERVED_0_0 0u

/* ---- Fn00000007, ECX_x1 ---- */

/** ECX_x1 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_ECX_x1_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000007_ECX_x1_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn00000007, EDX_x0 ---- */

/** EDX_x0 bits 31:29 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EDX_x0_RESERVED_31_29_SHIFT 29u
#define AMD_CPUID_00000007_EDX_x0_RESERVED_31_29_MASK 0x7u

/** EDX_x0 bit 28 */
#define AMD_CPUID_00000007_EDX_x0_L1D_FLUSH 28u

/** EDX_x0 bits 27:24 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EDX_x0_RESERVED_27_24_SHIFT 24u
#define AMD_CPUID_00000007_EDX_x0_RESERVED_27_24_MASK 0xFu

/** EDX_x0 bit 23 */
#define AMD_CPUID_00000007_EDX_x0_AVX512_FP16 23u

/** EDX_x0 bits 22:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EDX_x0_RESERVED_22_0_SHIFT 0u
#define AMD_CPUID_00000007_EDX_x0_RESERVED_22_0_MASK 0x7FFFFFu

/* ---- Fn00000007, EDX_x1 ---- */

/** EDX_x1 bits 31:6 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EDX_x1_RESERVED_31_6_SHIFT 6u
#define AMD_CPUID_00000007_EDX_x1_RESERVED_31_6_MASK 0x3FFFFFFu

/** EDX_x1 bit 5 */
#define AMD_CPUID_00000007_EDX_x1_AVX_NE_CONVERT 5u

/** EDX_x1 bit 4 */
#define AMD_CPUID_00000007_EDX_x1_AVX_VNNI_INT8 4u

/** EDX_x1 bits 3:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000007_EDX_x1_RESERVED_3_0_SHIFT 0u
#define AMD_CPUID_00000007_EDX_x1_RESERVED_3_0_MASK 0xFu

/* ---- Fn0000000B, EAX_x0 ---- */

/** EAX_x0 bits 31:5 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000B_EAX_x0_RESERVED_31_5_SHIFT 5u
#define AMD_CPUID_0000000B_EAX_x0_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX_x0 bits 4:0 */
#define AMD_CPUID_0000000B_EAX_x0_ThreadMaskWidth_SHIFT 0u
#define AMD_CPUID_0000000B_EAX_x0_ThreadMaskWidth_MASK 0x1Fu

/* ---- Fn0000000B, EAX_x1 ---- */

/** EAX_x1 bits 31:5 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000B_EAX_x1_RESERVED_31_5_SHIFT 5u
#define AMD_CPUID_0000000B_EAX_x1_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX_x1 bits 4:0 */
#define AMD_CPUID_0000000B_EAX_x1_CoreMaskWidth_SHIFT 0u
#define AMD_CPUID_0000000B_EAX_x1_CoreMaskWidth_MASK 0x1Fu

/* ---- Fn0000000B, EBX_x0 ---- */

/** EBX_x0 bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000B_EBX_x0_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_0000000B_EBX_x0_RESERVED_31_16_MASK 0xFFFFu

/** EBX_x0 bits 15:0 */
#define AMD_CPUID_0000000B_EBX_x0_NumLogProc_SHIFT 0u
#define AMD_CPUID_0000000B_EBX_x0_NumLogProc_MASK 0xFFFFu

/* ---- Fn0000000B, EBX_x1 ---- */

/** EBX_x1 bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000B_EBX_x1_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_0000000B_EBX_x1_RESERVED_31_16_MASK 0xFFFFu

/** EBX_x1 bits 15:0 */
#define AMD_CPUID_0000000B_EBX_x1_NumLogCores_SHIFT 0u
#define AMD_CPUID_0000000B_EBX_x1_NumLogCores_MASK 0xFFFFu

/* ---- Fn0000000B, ECX_x0 ---- */

/** ECX_x0 bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000B_ECX_x0_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_0000000B_ECX_x0_RESERVED_31_16_MASK 0xFFFFu

/** ECX_x0 bits 15:8 */
#define AMD_CPUID_0000000B_ECX_x0_HierarchyLevel_SHIFT 8u
#define AMD_CPUID_0000000B_ECX_x0_HierarchyLevel_MASK 0xFFu

/** ECX_x0 bits 7:0 */
#define AMD_CPUID_0000000B_ECX_x0_InputEcx_SHIFT 0u
#define AMD_CPUID_0000000B_ECX_x0_InputEcx_MASK 0xFFu

/* ---- Fn0000000B, ECX_x1 ---- */

/** ECX_x1 bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000B_ECX_x1_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_0000000B_ECX_x1_RESERVED_31_16_MASK 0xFFFFu

/** ECX_x1 bits 15:8 */
#define AMD_CPUID_0000000B_ECX_x1_HierarchyLevel_SHIFT 8u
#define AMD_CPUID_0000000B_ECX_x1_HierarchyLevel_MASK 0xFFu

/** ECX_x1 bits 7:0 */
#define AMD_CPUID_0000000B_ECX_x1_InputEcx_SHIFT 0u
#define AMD_CPUID_0000000B_ECX_x1_InputEcx_MASK 0xFFu

/* ---- Fn0000000B, EDX_x0 ---- */

/** EDX_x0 bits 31:0 */
#define AMD_CPUID_0000000B_EDX_x0_x2APIC_ID_SHIFT 0u
#define AMD_CPUID_0000000B_EDX_x0_x2APIC_ID_MASK 0xFFFFFFFFu

/* ---- Fn0000000B, EDX_x1 ---- */

/** EDX_x1 bits 31:0 */
#define AMD_CPUID_0000000B_EDX_x1_x2APIC_ID_SHIFT 0u
#define AMD_CPUID_0000000B_EDX_x1_x2APIC_ID_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EAX_x0 ---- */

/** EAX_x0 bits 31:0 */
#define AMD_CPUID_0000000D_EAX_x0_XFeatureSupportedMask_SHIFT 0u
#define AMD_CPUID_0000000D_EAX_x0_XFeatureSupportedMask_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EAX_x1 ---- */

/** EAX_x1 bits 31:4 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_EAX_x1_RESERVED_31_4_SHIFT 4u
#define AMD_CPUID_0000000D_EAX_x1_RESERVED_31_4_MASK 0xFFFFFFFu

/** EAX_x1 bit 3 */
#define AMD_CPUID_0000000D_EAX_x1_XSAVES 3u

/** EAX_x1 bit 2 */
#define AMD_CPUID_0000000D_EAX_x1_XGETBV 2u

/** EAX_x1 bit 1 */
#define AMD_CPUID_0000000D_EAX_x1_XSAVEC 1u

/** EAX_x1 bit 0 */
#define AMD_CPUID_0000000D_EAX_x1_XSAVEOPT 0u

/* ---- Fn0000000D, EAX_x11 ---- */

/** EAX_x11 bits 31:0 */
#define AMD_CPUID_0000000D_EAX_x11_CetUserSize_SHIFT 0u
#define AMD_CPUID_0000000D_EAX_x11_CetUserSize_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EAX_x12 ---- */

/** EAX_x12 bits 31:0 */
#define AMD_CPUID_0000000D_EAX_x12_CetSupervisorSize_SHIFT 0u
#define AMD_CPUID_0000000D_EAX_x12_CetSupervisorSize_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EAX_x2 ---- */

/** EAX_x2 bits 31:0 */
#define AMD_CPUID_0000000D_EAX_x2_YmmSaveStateSize_SHIFT 0u
#define AMD_CPUID_0000000D_EAX_x2_YmmSaveStateSize_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EAX_x3E ---- */

/** EAX_x3E bits 31:0 */
#define AMD_CPUID_0000000D_EAX_x3E_LwpSaveStateSize_SHIFT 0u
#define AMD_CPUID_0000000D_EAX_x3E_LwpSaveStateSize_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EBX_x0 ---- */

/** EBX_x0 bits 31:0 */
#define AMD_CPUID_0000000D_EBX_x0_XFeatureEnabledSizeMax_SHIFT 0u
#define AMD_CPUID_0000000D_EBX_x0_XFeatureEnabledSizeMax_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EBX_x11 ---- */

/** EBX_x11 bits 31:0 */
#define AMD_CPUID_0000000D_EBX_x11_CetUserOffset_SHIFT 0u
#define AMD_CPUID_0000000D_EBX_x11_CetUserOffset_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EBX_x12 ---- */

/** EBX_x12 bits 31:0 */
#define AMD_CPUID_0000000D_EBX_x12_CetSupervisorOffset_SHIFT 0u
#define AMD_CPUID_0000000D_EBX_x12_CetSupervisorOffset_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EBX_x2 ---- */

/** EBX_x2 bits 31:0 */
#define AMD_CPUID_0000000D_EBX_x2_YmmSaveStateOffset_SHIFT 0u
#define AMD_CPUID_0000000D_EBX_x2_YmmSaveStateOffset_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EBX_x3E ---- */

/** EBX_x3E bits 31:0 */
#define AMD_CPUID_0000000D_EBX_x3E_LwpSaveStateOffset_SHIFT 0u
#define AMD_CPUID_0000000D_EBX_x3E_LwpSaveStateOffset_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, ECX_x0 ---- */

/** ECX_x0 bits 31:0 */
#define AMD_CPUID_0000000D_ECX_x0_XFeatureSupportedSizeMax_SHIFT 0u
#define AMD_CPUID_0000000D_ECX_x0_XFeatureSupportedSizeMax_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, ECX_x1 ---- */

/** ECX_x1 bits 31:13 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_ECX_x1_RESERVED_31_13_SHIFT 13u
#define AMD_CPUID_0000000D_ECX_x1_RESERVED_31_13_MASK 0x7FFFFu

/** ECX_x1 bit 12 */
#define AMD_CPUID_0000000D_ECX_x1_CET_S 12u

/** ECX_x1 bit 11 */
#define AMD_CPUID_0000000D_ECX_x1_CET_U 11u

/** ECX_x1 bits 10:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_ECX_x1_RESERVED_10_0_SHIFT 0u
#define AMD_CPUID_0000000D_ECX_x1_RESERVED_10_0_MASK 0x7FFu

/* ---- Fn0000000D, ECX_x11 ---- */

/** ECX_x11 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_ECX_x11_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_ECX_x11_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX_x11 bit 0 -- el manual lo llama `U/S` */
#define AMD_CPUID_0000000D_ECX_x11_U_S 0u

/* ---- Fn0000000D, ECX_x12 ---- */

/** ECX_x12 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_ECX_x12_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_ECX_x12_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX_x12 bit 0 -- el manual lo llama `U/S` */
#define AMD_CPUID_0000000D_ECX_x12_U_S 0u

/* ---- Fn0000000D, ECX_x2 ---- */

/** ECX_x2 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_ECX_x2_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_ECX_x2_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, ECX_x3E ---- */

/** ECX_x3E bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_ECX_x3E_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_ECX_x3E_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EDX_x0 ---- */

/** EDX_x0 bits 31:0 */
#define AMD_CPUID_0000000D_EDX_x0_XFeatureSupportedMask_SHIFT 0u
#define AMD_CPUID_0000000D_EDX_x0_XFeatureSupportedMask_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EDX_x11 ---- */

/** EDX_x11 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_EDX_x11_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_EDX_x11_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EDX_x12 ---- */

/** EDX_x12 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_EDX_x12_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_EDX_x12_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EDX_x2 ---- */

/** EDX_x2 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_EDX_x2_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_EDX_x2_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn0000000D, EDX_x3E ---- */

/** EDX_x3E bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000D_EDX_x3E_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000D_EDX_x3E_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn0000000F, EAX_x0 ---- */

/** EAX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000F_EAX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000F_EAX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn0000000F, EAX_x1 ---- */

/** EAX_x1 bits 7:0 */
#define AMD_CPUID_0000000F_EAX_x1_CounterSize_SHIFT 0u
#define AMD_CPUID_0000000F_EAX_x1_CounterSize_MASK 0xFFu

/* ---- Fn0000000F, EBX_x0 ---- */

/** EBX_x0 bits 31:0 */
#define AMD_CPUID_0000000F_EBX_x0_Max_RMID_SHIFT 0u
#define AMD_CPUID_0000000F_EBX_x0_Max_RMID_MASK 0xFFFFFFFFu

/* ---- Fn0000000F, EBX_x1 ---- */

/** EBX_x1 bits 31:0 */
#define AMD_CPUID_0000000F_EBX_x1_ScaleFactor_SHIFT 0u
#define AMD_CPUID_0000000F_EBX_x1_ScaleFactor_MASK 0xFFFFFFFFu

/* ---- Fn0000000F, ECX_x0 ---- */

/** ECX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000F_ECX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_0000000F_ECX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX_x0 bits 31:2 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000F_ECX_x0_RESERVED_31_2_SHIFT 2u
#define AMD_CPUID_0000000F_ECX_x0_RESERVED_31_2_MASK 0x3FFFFFFFu

/* ---- Fn0000000F, ECX_x1 ---- */

/** ECX_x1 bits 31:0 */
#define AMD_CPUID_0000000F_ECX_x1_Max_RMID_SHIFT 0u
#define AMD_CPUID_0000000F_ECX_x1_Max_RMID_MASK 0xFFFFFFFFu

/** ECX_x1 bits 31:3 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000F_ECX_x1_RESERVED_31_3_SHIFT 3u
#define AMD_CPUID_0000000F_ECX_x1_RESERVED_31_3_MASK 0x1FFFFFFFu

/* ---- Fn0000000F, EDX_x0 ---- */

/** EDX_x0 bit 1 */
#define AMD_CPUID_0000000F_EDX_x0_L3CacheMon 1u

/** EDX_x0 bit 0 -- RESERVADO, no se toca */
#define AMD_CPUID_0000000F_EDX_x0_RESERVED_0_0 0u

/* ---- Fn0000000F, EDX_x1 ---- */

/** EDX_x1 bit 2 */
#define AMD_CPUID_0000000F_EDX_x1_L3CacheBWMonEvt1 2u

/** EDX_x1 bit 1 */
#define AMD_CPUID_0000000F_EDX_x1_L3CacheBWMonEvt0 1u

/** EDX_x1 bit 0 */
#define AMD_CPUID_0000000F_EDX_x1_L3CacheOccMon 0u

/* ---- Fn00000010, EAX_x0 ---- */

/** EAX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_EAX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000010_EAX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn00000010, EAX_x1 ---- */

/** EAX_x1 bits 31:5 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_EAX_x1_RESERVED_31_5_SHIFT 5u
#define AMD_CPUID_00000010_EAX_x1_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX_x1 bits 4:0 */
#define AMD_CPUID_00000010_EAX_x1_CBM_LEN_SHIFT 0u
#define AMD_CPUID_00000010_EAX_x1_CBM_LEN_MASK 0x1Fu

/* ---- Fn00000010, EBX_x0 ---- */

/** EBX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_EBX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000010_EBX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn00000010, EBX_x1 ---- */

/** EBX_x1 bits 31:0 */
#define AMD_CPUID_00000010_EBX_x1_L3ShareAllocMask_SHIFT 0u
#define AMD_CPUID_00000010_EBX_x1_L3ShareAllocMask_MASK 0xFFFFFFFFu

/** EBX_x1 bits 31:3 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_EBX_x1_RESERVED_31_3_SHIFT 3u
#define AMD_CPUID_00000010_EBX_x1_RESERVED_31_3_MASK 0x1FFFFFFFu

/* ---- Fn00000010, ECX_x0 ---- */

/** ECX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_ECX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_00000010_ECX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX_x0 bits 31:2 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_ECX_x0_RESERVED_31_2_SHIFT 2u
#define AMD_CPUID_00000010_ECX_x0_RESERVED_31_2_MASK 0x3FFFFFFFu

/* ---- Fn00000010, ECX_x1 ---- */

/** ECX_x1 bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_ECX_x1_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_00000010_ECX_x1_RESERVED_31_16_MASK 0xFFFFu

/** ECX_x1 bit 2 */
#define AMD_CPUID_00000010_ECX_x1_CDP 2u

/** ECX_x1 bits 1:0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_ECX_x1_RESERVED_1_0_SHIFT 0u
#define AMD_CPUID_00000010_ECX_x1_RESERVED_1_0_MASK 0x3u

/* ---- Fn00000010, EDX_x0 ---- */

/** EDX_x0 bit 1 */
#define AMD_CPUID_00000010_EDX_x0_L3Alloc 1u

/** EDX_x0 bit 0 -- RESERVADO, no se toca */
#define AMD_CPUID_00000010_EDX_x0_RESERVED_0_0 0u

/* ---- Fn00000010, EDX_x1 ---- */

/** EDX_x1 bits 15:0 */
#define AMD_CPUID_00000010_EDX_x1_COS_MAX_SHIFT 0u
#define AMD_CPUID_00000010_EDX_x1_COS_MAX_MASK 0xFFFFu

/* ---- Fn80000025, EAX ---- */

/** EAX bits 31:12 -- RESERVADO, no se toca */
#define AMD_CPUID_80000025_EAX_RESERVED_31_12_SHIFT 12u
#define AMD_CPUID_80000025_EAX_RESERVED_31_12_MASK 0xFFFFFu

/** EAX bits 11:6 */
#define AMD_CPUID_80000025_EAX_MaxRmpSegSize_SHIFT 6u
#define AMD_CPUID_80000025_EAX_MaxRmpSegSize_MASK 0x3Fu

/** EAX bits 5:0 */
#define AMD_CPUID_80000025_EAX_MinRmpSegSize_SHIFT 0u
#define AMD_CPUID_80000025_EAX_MinRmpSegSize_MASK 0x3Fu

/* ---- Fn80000025, EBX ---- */

/** EBX bits 31:11 -- RESERVADO, no se toca */
#define AMD_CPUID_80000025_EBX_RESERVED_31_11_SHIFT 11u
#define AMD_CPUID_80000025_EBX_RESERVED_31_11_MASK 0x1FFFFFu

/** EBX bit 10 */
#define AMD_CPUID_80000025_EBX_NumSegReduction 10u

/** EBX bits 9:0 */
#define AMD_CPUID_80000025_EBX_NumCachedSegments_SHIFT 0u
#define AMD_CPUID_80000025_EBX_NumCachedSegments_MASK 0x3FFu

/* ---- Fn80000025, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000025_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000025_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000025, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000025_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000025_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_AMD_MISC_H */
