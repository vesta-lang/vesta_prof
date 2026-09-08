/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_ibs.h
 * @brief Instruction-Based Sampling
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

#ifndef VXP_COMMON_CPUID_AMD_IBS_H
#define VXP_COMMON_CPUID_AMD_IBS_H

/* ---- Fn8000001B, EAX ---- */

/** EAX bits 31:20 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001B_EAX_RESERVED_31_20_SHIFT 20u
#define AMD_CPUID_8000001B_EAX_RESERVED_31_20_MASK 0xFFFu

/** EAX bit 19 */
#define AMD_CPUID_8000001B_EAX_IbsUpdtdDtlbStats 19u

/** EAX bit 18 */
#define AMD_CPUID_8000001B_EAX_IbsMemProfilerV1 18u

/** EAX bit 17 */
#define AMD_CPUID_8000001B_EAX_IbsBufferV1 17u

/** EAX bit 16 */
#define AMD_CPUID_8000001B_EAX_IbsStrmStAndRmtSocket 16u

/** EAX bit 15 */
#define AMD_CPUID_8000001B_EAX_IbsAddrBit63Filtering 15u

/** EAX bit 14 */
#define AMD_CPUID_8000001B_EAX_IbsFetchLatencyFiltering 14u

/** EAX bit 13 */
#define AMD_CPUID_8000001B_EAX_IbsDis 13u

/** EAX bit 12 */
#define AMD_CPUID_8000001B_EAX_IbsLoadLatencyFiltering 12u

/** EAX bit 11 */
#define AMD_CPUID_8000001B_EAX_IbsL3MissFiltering 11u

/** EAX bit 10 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001B_EAX_RESERVED_10_10 10u

/** EAX bit 9 */
#define AMD_CPUID_8000001B_EAX_IbsFetchCtlExtd 9u

/** EAX bit 8 */
#define AMD_CPUID_8000001B_EAX_OpBrnFuse 8u

/** EAX bit 7 */
#define AMD_CPUID_8000001B_EAX_RipInvalidChk 7u

/** EAX bit 6 */
#define AMD_CPUID_8000001B_EAX_OpCntExt 6u

/** EAX bit 5 */
#define AMD_CPUID_8000001B_EAX_BrnTrgt 5u

/** EAX bit 4 */
#define AMD_CPUID_8000001B_EAX_OpCnt 4u

/** EAX bit 3 */
#define AMD_CPUID_8000001B_EAX_RdWrOpCnt 3u

/** EAX bit 2 */
#define AMD_CPUID_8000001B_EAX_OpSam 2u

/** EAX bit 1 */
#define AMD_CPUID_8000001B_EAX_FetchSam 1u

/** EAX bit 0 */
#define AMD_CPUID_8000001B_EAX_IBSFFV 0u

/* ---- Fn8000001B, EBX ---- */

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001B_EBX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_8000001B_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn8000001B, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001B_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_8000001B_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn8000001B, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001B_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_8000001B_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_AMD_IBS_H */
