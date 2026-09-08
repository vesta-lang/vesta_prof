/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_power.h
 * @brief Energia, control termico y estados de rendimiento
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

#ifndef VXP_COMMON_CPUID_AMD_POWER_H
#define VXP_COMMON_CPUID_AMD_POWER_H

/* ---- Fn80000007, EAX ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000007_EAX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000007_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000007, EBX ---- */

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_80000007_EBX_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_80000007_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bit 15 */
#define AMD_CPUID_80000007_EBX_MCA_QUERY_MSR 15u

/** EBX bits 14:4 -- RESERVADO, no se toca */
#define AMD_CPUID_80000007_EBX_RESERVED_14_4_SHIFT 4u
#define AMD_CPUID_80000007_EBX_RESERVED_14_4_MASK 0x7FFu

/** EBX bit 3 */
#define AMD_CPUID_80000007_EBX_ScalableMca 3u

/** EBX bit 2 */
#define AMD_CPUID_80000007_EBX_HWA 2u

/** EBX bit 1 */
#define AMD_CPUID_80000007_EBX_SUCCOR 1u

/** EBX bit 0 */
#define AMD_CPUID_80000007_EBX_McaOverflowRecov 0u

/* ---- Fn80000007, ECX ---- */

/** ECX bits 31:0 */
#define AMD_CPUID_80000007_ECX_CpuPwrSampleTimeRatio_SHIFT 0u
#define AMD_CPUID_80000007_ECX_CpuPwrSampleTimeRatio_MASK 0xFFFFFFFFu

/* ---- Fn80000007, EDX ---- */

/** EDX bits 31:17 -- RESERVADO, no se toca */
#define AMD_CPUID_80000007_EDX_RESERVED_31_17_SHIFT 17u
#define AMD_CPUID_80000007_EDX_RESERVED_31_17_MASK 0x7FFFu

/** EDX bit 16 */
#define AMD_CPUID_80000007_EDX_CppcPerfPriority 16u

/** EDX bits 15:13 -- RESERVADO, no se toca */
#define AMD_CPUID_80000007_EDX_RESERVED_15_13_SHIFT 13u
#define AMD_CPUID_80000007_EDX_RESERVED_15_13_MASK 0x7u

/** EDX bit 12 */
#define AMD_CPUID_80000007_EDX_ProcPowerReporting 12u

/** EDX bit 11 */
#define AMD_CPUID_80000007_EDX_ProcFeedbackInterface 11u

/** EDX bit 10 */
#define AMD_CPUID_80000007_EDX_EffFreqRO 10u

/** EDX bit 9 */
#define AMD_CPUID_80000007_EDX_CPB 9u

/** EDX bit 8 */
#define AMD_CPUID_80000007_EDX_TscInvariant 8u

/** EDX bit 7 */
#define AMD_CPUID_80000007_EDX_HwPstate 7u

/** EDX bit 6 */
#define AMD_CPUID_80000007_EDX_100MHzSteps 6u

/** EDX bit 5 -- RESERVADO, no se toca */
#define AMD_CPUID_80000007_EDX_RESERVED_5_5 5u

/** EDX bit 4 */
#define AMD_CPUID_80000007_EDX_TM 4u

/** EDX bit 3 */
#define AMD_CPUID_80000007_EDX_TTP 3u

/** EDX bit 2 */
#define AMD_CPUID_80000007_EDX_VID 2u

/** EDX bit 1 */
#define AMD_CPUID_80000007_EDX_FID 1u

/** EDX bit 0 */
#define AMD_CPUID_80000007_EDX_TS 0u

/* ---- Fn80000008, EAX ---- */

/** EAX bits 31:24 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_EAX_RESERVED_31_24_SHIFT 24u
#define AMD_CPUID_80000008_EAX_RESERVED_31_24_MASK 0xFFu

/** EAX bits 23:16 */
#define AMD_CPUID_80000008_EAX_GuestPhysAddrSize_SHIFT 16u
#define AMD_CPUID_80000008_EAX_GuestPhysAddrSize_MASK 0xFFu

/** EAX bits 15:8 */
#define AMD_CPUID_80000008_EAX_LinAddrSize_SHIFT 8u
#define AMD_CPUID_80000008_EAX_LinAddrSize_MASK 0xFFu

/** EAX bits 7:0 */
#define AMD_CPUID_80000008_EAX_PhysAddrSize_SHIFT 0u
#define AMD_CPUID_80000008_EAX_PhysAddrSize_MASK 0xFFu

/* ---- Fn80000008, EBX ---- */

/** EBX bit 31 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_EBX_RESERVED_31_31 31u

/** EBX bit 30 */
#define AMD_CPUID_80000008_EBX_IBPB_RET 30u

/** EBX bit 29 */
#define AMD_CPUID_80000008_EBX_BTC_NO 29u

/** EBX bit 28 */
#define AMD_CPUID_80000008_EBX_PSFD 28u

/** EBX bit 27 */
#define AMD_CPUID_80000008_EBX_CPPC 27u

/** EBX bit 26 */
#define AMD_CPUID_80000008_EBX_SsbdNotRequired 26u

/** EBX bit 25 */
#define AMD_CPUID_80000008_EBX_SsbdVirtSpecCtrl 25u

/** EBX bit 24 */
#define AMD_CPUID_80000008_EBX_SSBD 24u

/** EBX bits 23:22 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_EBX_RESERVED_23_22_SHIFT 22u
#define AMD_CPUID_80000008_EBX_RESERVED_23_22_MASK 0x3u

/** EBX bit 21 */
#define AMD_CPUID_80000008_EBX_INVLPGBnestedPages 21u

/** EBX bit 20 */
#define AMD_CPUID_80000008_EBX_EferLmsleUnsupported 20u

/** EBX bit 19 */
#define AMD_CPUID_80000008_EBX_IbrsSameMode 19u

/** EBX bit 18 */
#define AMD_CPUID_80000008_EBX_IbrsPreferred 18u

/** EBX bit 17 */
#define AMD_CPUID_80000008_EBX_StibpAlwaysOn 17u

/** EBX bit 16 */
#define AMD_CPUID_80000008_EBX_IbrsAlwaysOn 16u

/** EBX bit 15 */
#define AMD_CPUID_80000008_EBX_STIBP 15u

/** EBX bit 14 */
#define AMD_CPUID_80000008_EBX_IBRS 14u

/** EBX bit 13 */
#define AMD_CPUID_80000008_EBX_INT_WBINVD 13u

/** EBX bit 12 */
#define AMD_CPUID_80000008_EBX_IBPB 12u

/** EBX bits 11:10 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_EBX_RESERVED_11_10_SHIFT 10u
#define AMD_CPUID_80000008_EBX_RESERVED_11_10_MASK 0x3u

/** EBX bit 9 */
#define AMD_CPUID_80000008_EBX_WBNOINVD 9u

/** EBX bit 8 */
#define AMD_CPUID_80000008_EBX_MCOMMIT 8u

/** EBX bit 7 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_EBX_RESERVED_7_7 7u

/** EBX bit 6 */
#define AMD_CPUID_80000008_EBX_BE 6u

/** EBX bit 5 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_EBX_RESERVED_5_5 5u

/** EBX bit 4 */
#define AMD_CPUID_80000008_EBX_RDPRU 4u

/** EBX bit 3 */
#define AMD_CPUID_80000008_EBX_INVLPGB 3u

/** EBX bit 2 */
#define AMD_CPUID_80000008_EBX_RstrFpErrPtrs 2u

/** EBX bit 1 */
#define AMD_CPUID_80000008_EBX_InstRetCntMsr 1u

/** EBX bit 0 */
#define AMD_CPUID_80000008_EBX_CLZERO 0u

/* ---- Fn80000008, ECX ---- */

/** ECX bits 31:18 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_ECX_RESERVED_31_18_SHIFT 18u
#define AMD_CPUID_80000008_ECX_RESERVED_31_18_MASK 0x3FFFu

/** ECX bits 17:16 */
#define AMD_CPUID_80000008_ECX_PerfTscSize_SHIFT 16u
#define AMD_CPUID_80000008_ECX_PerfTscSize_MASK 0x3u

/** ECX bits 15:12 */
#define AMD_CPUID_80000008_ECX_ApicIdSize_SHIFT 12u
#define AMD_CPUID_80000008_ECX_ApicIdSize_MASK 0xFu

/** ECX bits 11:8 -- RESERVADO, no se toca */
#define AMD_CPUID_80000008_ECX_RESERVED_11_8_SHIFT 8u
#define AMD_CPUID_80000008_ECX_RESERVED_11_8_MASK 0xFu

/** ECX bits 7:0 */
#define AMD_CPUID_80000008_ECX_NC_SHIFT 0u
#define AMD_CPUID_80000008_ECX_NC_MASK 0xFFu

/* ---- Fn80000008, EDX ---- */

/** EDX bits 31:16 */
#define AMD_CPUID_80000008_EDX_MaxRdpruID_SHIFT 16u
#define AMD_CPUID_80000008_EDX_MaxRdpruID_MASK 0xFFFFu

/** EDX bits 15:0 */
#define AMD_CPUID_80000008_EDX_InvlpgbCountMax_SHIFT 0u
#define AMD_CPUID_80000008_EDX_InvlpgbCountMax_MASK 0xFFFFu


#endif /* VXP_COMMON_CPUID_AMD_POWER_H */
