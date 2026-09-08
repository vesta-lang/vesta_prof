/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_features.h
 * @brief Mapas de bits de caracteristicas
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

#ifndef VXP_COMMON_CPUID_AMD_FEATURES_H
#define VXP_COMMON_CPUID_AMD_FEATURES_H

/* ---- Fn80000001, EAX ---- */

/** EAX bits 31:0 */
#define AMD_CPUID_80000001_EAX_Family_SHIFT 0u
#define AMD_CPUID_80000001_EAX_Family_MASK 0xFFFFFFFFu

/* ---- Fn80000001, EBX ---- */

/** EBX bits 31:28 */
#define AMD_CPUID_80000001_EBX_PkgType_SHIFT 28u
#define AMD_CPUID_80000001_EBX_PkgType_MASK 0xFu

/** EBX bits 27:16 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_EBX_RESERVED_27_16_SHIFT 16u
#define AMD_CPUID_80000001_EBX_RESERVED_27_16_MASK 0xFFFu

/** EBX bits 15:0 */
#define AMD_CPUID_80000001_EBX_BrandId_SHIFT 0u
#define AMD_CPUID_80000001_EBX_BrandId_MASK 0xFFFFu

/* ---- Fn80000001, ECX ---- */

/** ECX bit 31 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_ECX_RESERVED_31_31 31u

/** ECX bit 30 */
#define AMD_CPUID_80000001_ECX_AddrMaskExt 30u

/** ECX bit 29 */
#define AMD_CPUID_80000001_ECX_MONITORX 29u

/** ECX bit 28 */
#define AMD_CPUID_80000001_ECX_PerfCtrExtLLC 28u

/** ECX bit 27 */
#define AMD_CPUID_80000001_ECX_PerfTsc 27u

/** ECX bit 26 */
#define AMD_CPUID_80000001_ECX_DataBkptExt 26u

/** ECX bit 25 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_ECX_RESERVED_25_25 25u

/** ECX bit 24 */
#define AMD_CPUID_80000001_ECX_PerfCtrExtNB 24u

/** ECX bit 23 */
#define AMD_CPUID_80000001_ECX_PerfCtrExtCore 23u

/** ECX bit 22 */
#define AMD_CPUID_80000001_ECX_TopologyExtensio 22u

/** ECX bit 21 */
#define AMD_CPUID_80000001_ECX_TBM 21u

/** ECX bit 20 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_ECX_RESERVED_20_20 20u

/** ECX bit 19 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_ECX_RESERVED_19_19 19u

/** ECX bit 18 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_ECX_RESERVED_18_18 18u

/** ECX bit 17 */
#define AMD_CPUID_80000001_ECX_TCE 17u

/** ECX bit 16 */
#define AMD_CPUID_80000001_ECX_FMA4 16u

/** ECX bit 15 */
#define AMD_CPUID_80000001_ECX_LWP 15u

/** ECX bit 14 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_ECX_RESERVED_14_14 14u

/** ECX bit 13 */
#define AMD_CPUID_80000001_ECX_WDT 13u

/** ECX bit 12 */
#define AMD_CPUID_80000001_ECX_SKINIT 12u

/** ECX bit 11 */
#define AMD_CPUID_80000001_ECX_XOP 11u

/** ECX bit 10 */
#define AMD_CPUID_80000001_ECX_IBS 10u

/** ECX bit 9 */
#define AMD_CPUID_80000001_ECX_OSVW 9u

/** ECX bit 8 */
#define AMD_CPUID_80000001_ECX_3DNowPrefetch 8u

/** ECX bit 7 */
#define AMD_CPUID_80000001_ECX_MisAlignSse 7u

/** ECX bit 6 */
#define AMD_CPUID_80000001_ECX_SSE4A 6u

/** ECX bit 5 */
#define AMD_CPUID_80000001_ECX_ABM 5u

/** ECX bit 4 */
#define AMD_CPUID_80000001_ECX_AltMovCr8 4u

/** ECX bit 3 */
#define AMD_CPUID_80000001_ECX_ExtApicSpace 3u

/** ECX bit 2 */
#define AMD_CPUID_80000001_ECX_SVM 2u

/** ECX bit 1 */
#define AMD_CPUID_80000001_ECX_CmpLegacy 1u

/** ECX bit 0 */
#define AMD_CPUID_80000001_ECX_LahfSahf 0u

/* ---- Fn80000001, EDX ---- */

/** EDX bit 31 */
#define AMD_CPUID_80000001_EDX_3DNow 31u

/** EDX bits 31:0 */
#define AMD_CPUID_80000001_EDX_ProcName_SHIFT 0u
#define AMD_CPUID_80000001_EDX_ProcName_MASK 0xFFFFFFFFu

/** EDX bit 30 */
#define AMD_CPUID_80000001_EDX_3DNowExt 30u

/** EDX bit 29 */
#define AMD_CPUID_80000001_EDX_LM 29u

/** EDX bit 28 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_EDX_RESERVED_28_28 28u

/** EDX bit 27 */
#define AMD_CPUID_80000001_EDX_RDTSCP 27u

/** EDX bit 26 */
#define AMD_CPUID_80000001_EDX_Page1GB 26u

/** EDX bit 25 */
#define AMD_CPUID_80000001_EDX_FFXSR 25u

/** EDX bit 24 */
#define AMD_CPUID_80000001_EDX_FXSR 24u

/** EDX bit 23 */
#define AMD_CPUID_80000001_EDX_MMX 23u

/** EDX bit 22 */
#define AMD_CPUID_80000001_EDX_MmxExt 22u

/** EDX bit 21 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_EDX_RESERVED_21_21 21u

/** EDX bit 20 */
#define AMD_CPUID_80000001_EDX_NX 20u

/** EDX bits 19:18 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_EDX_RESERVED_19_18_SHIFT 18u
#define AMD_CPUID_80000001_EDX_RESERVED_19_18_MASK 0x3u

/** EDX bit 17 */
#define AMD_CPUID_80000001_EDX_PSE36 17u

/** EDX bit 16 */
#define AMD_CPUID_80000001_EDX_PAT 16u

/** EDX bit 15 */
#define AMD_CPUID_80000001_EDX_CMOV 15u

/** EDX bit 14 */
#define AMD_CPUID_80000001_EDX_MCA 14u

/** EDX bit 13 */
#define AMD_CPUID_80000001_EDX_PGE 13u

/** EDX bit 12 */
#define AMD_CPUID_80000001_EDX_MTRR 12u

/** EDX bit 11 */
#define AMD_CPUID_80000001_EDX_SysCallSysRet 11u

/** EDX bit 10 -- RESERVADO, no se toca */
#define AMD_CPUID_80000001_EDX_RESERVED_10_10 10u

/** EDX bit 9 */
#define AMD_CPUID_80000001_EDX_APIC 9u

/** EDX bit 8 */
#define AMD_CPUID_80000001_EDX_CMPXCHG8B 8u

/** EDX bit 7 */
#define AMD_CPUID_80000001_EDX_MCE 7u

/** EDX bit 6 */
#define AMD_CPUID_80000001_EDX_PAE 6u

/** EDX bit 5 */
#define AMD_CPUID_80000001_EDX_MSR 5u

/** EDX bit 4 */
#define AMD_CPUID_80000001_EDX_TSC 4u

/** EDX bit 3 */
#define AMD_CPUID_80000001_EDX_PSE 3u

/** EDX bit 2 */
#define AMD_CPUID_80000001_EDX_DE 2u

/** EDX bit 1 */
#define AMD_CPUID_80000001_EDX_VME 1u

/** EDX bit 0 */
#define AMD_CPUID_80000001_EDX_FPU 0u

/* ---- Fn80000019, EAX ---- */

/** EAX bits 31:28 */
#define AMD_CPUID_80000019_EAX_L1DTlb1GAssoc_SHIFT 28u
#define AMD_CPUID_80000019_EAX_L1DTlb1GAssoc_MASK 0xFu

/** EAX bits 27:16 */
#define AMD_CPUID_80000019_EAX_L1DTlb1GSize_SHIFT 16u
#define AMD_CPUID_80000019_EAX_L1DTlb1GSize_MASK 0xFFFu

/** EAX bits 15:12 */
#define AMD_CPUID_80000019_EAX_L1ITlb1GAssoc_SHIFT 12u
#define AMD_CPUID_80000019_EAX_L1ITlb1GAssoc_MASK 0xFu

/** EAX bits 11:0 */
#define AMD_CPUID_80000019_EAX_L1ITlb1GSize_SHIFT 0u
#define AMD_CPUID_80000019_EAX_L1ITlb1GSize_MASK 0xFFFu

/* ---- Fn80000019, EBX ---- */

/** EBX bits 31:28 */
#define AMD_CPUID_80000019_EBX_L2DTlb1GAssoc_SHIFT 28u
#define AMD_CPUID_80000019_EBX_L2DTlb1GAssoc_MASK 0xFu

/** EBX bits 27:16 */
#define AMD_CPUID_80000019_EBX_L2DTlb1GSize_SHIFT 16u
#define AMD_CPUID_80000019_EBX_L2DTlb1GSize_MASK 0xFFFu

/** EBX bits 15:12 */
#define AMD_CPUID_80000019_EBX_L2ITlb1GAssoc_SHIFT 12u
#define AMD_CPUID_80000019_EBX_L2ITlb1GAssoc_MASK 0xFu

/** EBX bits 11:0 */
#define AMD_CPUID_80000019_EBX_L2ITlb1GSize_SHIFT 0u
#define AMD_CPUID_80000019_EBX_L2ITlb1GSize_MASK 0xFFFu

/* ---- Fn80000019, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000019_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000019_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000019, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000019_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000019_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn8000001A, EAX ---- */

/** EAX bits 31:3 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001A_EAX_RESERVED_31_3_SHIFT 3u
#define AMD_CPUID_8000001A_EAX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** EAX bit 2 */
#define AMD_CPUID_8000001A_EAX_FP256 2u

/** EAX bit 1 */
#define AMD_CPUID_8000001A_EAX_MOVU 1u

/** EAX bit 0 */
#define AMD_CPUID_8000001A_EAX_FP128 0u

/* ---- Fn8000001A, EBX ---- */

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001A_EBX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_8000001A_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn8000001A, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001A_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_8000001A_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn8000001A, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001A_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_8000001A_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn8000001C, EAX ---- */

/** EAX bit 31 */
#define AMD_CPUID_8000001C_EAX_LwpInt 31u

/** EAX bit 30 */
#define AMD_CPUID_8000001C_EAX_LwpPTSC 30u

/** EAX bit 29 */
#define AMD_CPUID_8000001C_EAX_LwpCont 29u

/** EAX bits 28:7 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001C_EAX_RESERVED_28_7_SHIFT 7u
#define AMD_CPUID_8000001C_EAX_RESERVED_28_7_MASK 0x3FFFFFu

/** EAX bit 6 */
#define AMD_CPUID_8000001C_EAX_LwpRNH 6u

/** EAX bit 5 */
#define AMD_CPUID_8000001C_EAX_LwpCNH 5u

/** EAX bit 4 */
#define AMD_CPUID_8000001C_EAX_LwpDME 4u

/** EAX bit 3 */
#define AMD_CPUID_8000001C_EAX_LwpBRE 3u

/** EAX bit 2 */
#define AMD_CPUID_8000001C_EAX_LwpIRE 2u

/** EAX bit 1 */
#define AMD_CPUID_8000001C_EAX_LwpVAL 1u

/** EAX bit 0 */
#define AMD_CPUID_8000001C_EAX_LwpAvail 0u

/* ---- Fn8000001C, EBX ---- */

/** EBX bits 31:24 */
#define AMD_CPUID_8000001C_EBX_LwpEventOffset_SHIFT 24u
#define AMD_CPUID_8000001C_EBX_LwpEventOffset_MASK 0xFFu

/** EBX bits 23:16 */
#define AMD_CPUID_8000001C_EBX_LwpMaxEvents_SHIFT 16u
#define AMD_CPUID_8000001C_EBX_LwpMaxEvents_MASK 0xFFu

/** EBX bits 15:8 */
#define AMD_CPUID_8000001C_EBX_LwpEventSize_SHIFT 8u
#define AMD_CPUID_8000001C_EBX_LwpEventSize_MASK 0xFFu

/** EBX bits 7:0 */
#define AMD_CPUID_8000001C_EBX_LwpCbSize_SHIFT 0u
#define AMD_CPUID_8000001C_EBX_LwpCbSize_MASK 0xFFu

/* ---- Fn8000001C, ECX ---- */

/** ECX bit 31 */
#define AMD_CPUID_8000001C_ECX_LwpCacheLatency 31u

/** ECX bit 30 */
#define AMD_CPUID_8000001C_ECX_LwpCacheLevels 30u

/** ECX bit 29 */
#define AMD_CPUID_8000001C_ECX_LwpIpFiltering 29u

/** ECX bit 28 */
#define AMD_CPUID_8000001C_ECX_LwpBranchPredict 28u

/** ECX bits 27:24 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001C_ECX_RESERVED_27_24_SHIFT 24u
#define AMD_CPUID_8000001C_ECX_RESERVED_27_24_MASK 0xFu

/** ECX bits 23:16 */
#define AMD_CPUID_8000001C_ECX_LwpMinBufferSize_SHIFT 16u
#define AMD_CPUID_8000001C_ECX_LwpMinBufferSize_MASK 0xFFu

/** ECX bits 15:9 */
#define AMD_CPUID_8000001C_ECX_LwpVersion_SHIFT 9u
#define AMD_CPUID_8000001C_ECX_LwpVersion_MASK 0x7Fu

/** ECX bits 8:6 */
#define AMD_CPUID_8000001C_ECX_LwpLatencyRnd_SHIFT 6u
#define AMD_CPUID_8000001C_ECX_LwpLatencyRnd_MASK 0x7u

/** ECX bit 5 */
#define AMD_CPUID_8000001C_ECX_LwpDataAddress 5u

/** ECX bits 4:0 */
#define AMD_CPUID_8000001C_ECX_LwpLatencyMax_SHIFT 0u
#define AMD_CPUID_8000001C_ECX_LwpLatencyMax_MASK 0x1Fu

/* ---- Fn8000001C, EDX ---- */

/** EDX bit 31 */
#define AMD_CPUID_8000001C_EDX_LwpInt 31u

/** EDX bit 30 */
#define AMD_CPUID_8000001C_EDX_LwpPTSC 30u

/** EDX bit 29 */
#define AMD_CPUID_8000001C_EDX_LwpCont 29u

/** EDX bits 28:7 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001C_EDX_RESERVED_28_7_SHIFT 7u
#define AMD_CPUID_8000001C_EDX_RESERVED_28_7_MASK 0x3FFFFFu

/** EDX bit 6 */
#define AMD_CPUID_8000001C_EDX_LwpRNH 6u

/** EDX bit 5 */
#define AMD_CPUID_8000001C_EDX_LwpCNH 5u

/** EDX bit 4 */
#define AMD_CPUID_8000001C_EDX_LwpDME 4u

/** EDX bit 3 */
#define AMD_CPUID_8000001C_EDX_LwpBRE 3u

/** EDX bit 2 */
#define AMD_CPUID_8000001C_EDX_LwpIRE 2u

/** EDX bit 1 */
#define AMD_CPUID_8000001C_EDX_LwpVAL 1u

/** EDX bit 0 */
#define AMD_CPUID_8000001C_EDX_LwpAvail 0u


#endif /* VXP_COMMON_CPUID_AMD_FEATURES_H */
