/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_perf.h
 * @brief Monitorizacion de rendimiento y sus extensiones
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

#ifndef VXP_COMMON_CPUID_AMD_PERF_H
#define VXP_COMMON_CPUID_AMD_PERF_H

/* ---- Fn8000000A, EAX ---- */

/** EAX bits 31:8 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_EAX_RESERVED_31_8_SHIFT 8u
#define AMD_CPUID_8000000A_EAX_RESERVED_31_8_MASK 0xFFFFFFu

/** EAX bits 7:0 */
#define AMD_CPUID_8000000A_EAX_SvmRev_SHIFT 0u
#define AMD_CPUID_8000000A_EAX_SvmRev_MASK 0xFFu

/* ---- Fn8000000A, EBX ---- */

/** EBX bits 31:0 */
#define AMD_CPUID_8000000A_EBX_NASID_SHIFT 0u
#define AMD_CPUID_8000000A_EBX_NASID_MASK 0xFFFFFFFFu

/* ---- Fn8000000A, ECX ---- */

/** ECX bits 31:7 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_ECX_RESERVED_31_7_SHIFT 7u
#define AMD_CPUID_8000000A_ECX_RESERVED_31_7_MASK 0x1FFFFFFu

/** ECX bit 6 */
#define AMD_CPUID_8000000A_ECX_x2AVIC_EXT 6u

/** ECX bit 5 */
#define AMD_CPUID_8000000A_ECX_EnhancedTlbi 5u

/** ECX bit 4 */
#define AMD_CPUID_8000000A_ECX_PML 4u

/** ECX bits 3:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_ECX_RESERVED_3_0_SHIFT 0u
#define AMD_CPUID_8000000A_ECX_RESERVED_3_0_MASK 0xFu

/* ---- Fn8000000A, EDX ---- */

/** EDX bit 31 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_EDX_RESERVED_31_31 31u

/** EDX bit 30 */
#define AMD_CPUID_8000000A_EDX_IdleHltIntercept 30u

/** EDX bit 29 */
#define AMD_CPUID_8000000A_EDX_BusLockThreshold 29u

/** EDX bit 28 */
#define AMD_CPUID_8000000A_EDX_NestedVirtVmcbAddrChk 28u

/** EDX bit 27 */
#define AMD_CPUID_8000000A_EDX_ExtLvtAvicAccessChg 27u

/** EDX bit 26 */
#define AMD_CPUID_8000000A_EDX_IbsVirt 26u

/** EDX bit 25 */
#define AMD_CPUID_8000000A_EDX_VNMI 25u

/** EDX bit 24 */
#define AMD_CPUID_8000000A_EDX_TlbiCtl 24u

/** EDX bit 23 */
#define AMD_CPUID_8000000A_EDX_HOST_MCE_OVERRIDE 23u

/** EDX bit 22 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_EDX_RESERVED_22_22 22u

/** EDX bit 21 */
#define AMD_CPUID_8000000A_EDX_ROGPT 21u

/** EDX bit 20 */
#define AMD_CPUID_8000000A_EDX_SpecCtrl 20u

/** EDX bit 19 */
#define AMD_CPUID_8000000A_EDX_SSSCheck 19u

/** EDX bit 18 */
#define AMD_CPUID_8000000A_EDX_x2AVIC 18u

/** EDX bit 17 */
#define AMD_CPUID_8000000A_EDX_GMET 17u

/** EDX bit 16 */
#define AMD_CPUID_8000000A_EDX_VGIF 16u

/** EDX bit 15 */
#define AMD_CPUID_8000000A_EDX_VMSAVEvirt 15u

/** EDX bit 14 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_EDX_RESERVED_14_14 14u

/** EDX bit 13 */
#define AMD_CPUID_8000000A_EDX_AVIC 13u

/** EDX bit 12 */
#define AMD_CPUID_8000000A_EDX_PauseFilterThreshold 12u

/** EDX bit 11 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_EDX_RESERVED_11_11 11u

/** EDX bit 10 */
#define AMD_CPUID_8000000A_EDX_PauseFilter 10u

/** EDX bit 9 -- RESERVADO, no se toca */
#define AMD_CPUID_8000000A_EDX_RESERVED_9_9 9u

/** EDX bit 8 */
#define AMD_CPUID_8000000A_EDX_PmcVirt 8u

/** EDX bit 7 */
#define AMD_CPUID_8000000A_EDX_DecodeAssists 7u

/** EDX bit 6 */
#define AMD_CPUID_8000000A_EDX_FlushByAsid 6u

/** EDX bit 5 */
#define AMD_CPUID_8000000A_EDX_VmcbClean 5u

/** EDX bit 4 */
#define AMD_CPUID_8000000A_EDX_TscRateMsr 4u

/** EDX bit 3 */
#define AMD_CPUID_8000000A_EDX_NRIPS 3u

/** EDX bit 2 */
#define AMD_CPUID_8000000A_EDX_SVML 2u

/** EDX bit 1 */
#define AMD_CPUID_8000000A_EDX_LbrVirt 1u

/** EDX bit 0 */
#define AMD_CPUID_8000000A_EDX_NP 0u

/* ---- Fn80000022, EAX ---- */

/** EAX bits 31:3 -- RESERVADO, no se toca */
#define AMD_CPUID_80000022_EAX_RESERVED_31_3_SHIFT 3u
#define AMD_CPUID_80000022_EAX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** EAX bit 2 */
#define AMD_CPUID_80000022_EAX_LbrAndPmcFreeze 2u

/** EAX bit 1 */
#define AMD_CPUID_80000022_EAX_LbrStack 1u

/** EAX bit 0 */
#define AMD_CPUID_80000022_EAX_PerfMonV2 0u

/* ---- Fn80000022, EBX ---- */

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_80000022_EBX_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_80000022_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:10 */
#define AMD_CPUID_80000022_EBX_NumPerfCtrNB_SHIFT 10u
#define AMD_CPUID_80000022_EBX_NumPerfCtrNB_MASK 0x3Fu

/** EBX bits 9:4 */
#define AMD_CPUID_80000022_EBX_LbrStackSize_SHIFT 4u
#define AMD_CPUID_80000022_EBX_LbrStackSize_MASK 0x3Fu

/** EBX bits 3:0 */
#define AMD_CPUID_80000022_EBX_NumPerfCtrCore_SHIFT 0u
#define AMD_CPUID_80000022_EBX_NumPerfCtrCore_MASK 0xFu

/* ---- Fn80000022, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000022_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000022_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000022, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000022_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000022_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_AMD_PERF_H */
