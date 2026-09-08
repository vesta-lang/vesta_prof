/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_security.h
 * @brief Cifrado de memoria, mitigaciones y aislamiento
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

#ifndef VXP_COMMON_CPUID_AMD_SECURITY_H
#define VXP_COMMON_CPUID_AMD_SECURITY_H

/* ---- Fn8000001F, EAX ---- */

/** EAX bit 31 */
#define AMD_CPUID_8000001F_EAX_IbpbOnEntry 31u

/** EAX bit 30 */
#define AMD_CPUID_8000001F_EAX_HvInUseWrAllowed 30u

/** EAX bit 29 */
#define AMD_CPUID_8000001F_EAX_NestedVirtSnpMsr 29u

/** EAX bit 28 */
#define AMD_CPUID_8000001F_EAX_SvsmCommPageMsr 28u

/** EAX bit 27 */
#define AMD_CPUID_8000001F_EAX_AllowedSevFeatures 27u

/** EAX bit 26 */
#define AMD_CPUID_8000001F_EAX_SecureAvic 26u

/** EAX bit 25 */
#define AMD_CPUID_8000001F_EAX_SmtProtection 25u

/** EAX bit 24 */
#define AMD_CPUID_8000001F_EAX_VmsaRegProt 24u

/** EAX bit 23 */
#define AMD_CPUID_8000001F_EAX_SegmentedRmp 23u

/** EAX bit 22 */
#define AMD_CPUID_8000001F_EAX_GuestInterceptCtl 22u

/** EAX bit 21 */
#define AMD_CPUID_8000001F_EAX_RMPREAD 21u

/** EAX bit 20 */
#define AMD_CPUID_8000001F_EAX_PmcVirtGuestCtl 20u

/** EAX bit 19 */
#define AMD_CPUID_8000001F_EAX_IbsVirtGuestCtl 19u

/** EAX bit 18 */
#define AMD_CPUID_8000001F_EAX_VirtualTomMsr 18u

/** EAX bit 17 */
#define AMD_CPUID_8000001F_EAX_VmgexitParameter 17u

/** EAX bit 16 */
#define AMD_CPUID_8000001F_EAX_VTE 16u

/** EAX bit 15 */
#define AMD_CPUID_8000001F_EAX_PreventHostIbs 15u

/** EAX bit 14 */
#define AMD_CPUID_8000001F_EAX_DebugVirt 14u

/** EAX bit 13 */
#define AMD_CPUID_8000001F_EAX_AlternateInjection 13u

/** EAX bit 12 */
#define AMD_CPUID_8000001F_EAX_RestrictedInjection 12u

/** EAX bit 11 */
#define AMD_CPUID_8000001F_EAX_64BitHost 11u

/** EAX bit 10 */
#define AMD_CPUID_8000001F_EAX_HwEnfCacheCoh 10u

/** EAX bit 9 */
#define AMD_CPUID_8000001F_EAX_TscAuxVirtualization 9u

/** EAX bit 8 */
#define AMD_CPUID_8000001F_EAX_SecureTsc 8u

/** EAX bit 7 */
#define AMD_CPUID_8000001F_EAX_VmplSSS 7u

/** EAX bit 6 */
#define AMD_CPUID_8000001F_EAX_RMPQUERY 6u

/** EAX bit 5 */
#define AMD_CPUID_8000001F_EAX_VMPL 5u

/** EAX bit 4 -- el manual lo llama `SEV-SNP` */
#define AMD_CPUID_8000001F_EAX_SEV_SNP 4u

/** EAX bit 3 -- el manual lo llama `SEV-ES` */
#define AMD_CPUID_8000001F_EAX_SEV_ES 3u

/** EAX bit 2 */
#define AMD_CPUID_8000001F_EAX_PageFlushMsr 2u

/** EAX bit 1 */
#define AMD_CPUID_8000001F_EAX_SEV 1u

/** EAX bit 0 */
#define AMD_CPUID_8000001F_EAX_SME 0u

/* ---- Fn8000001F, EBX ---- */

/** EBX bit 31 */
#define AMD_CPUID_8000001F_EBX_COHERENCY_SFW_NO 31u

/** EBX bits 30:16 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001F_EBX_RESERVED_30_16_SHIFT 16u
#define AMD_CPUID_8000001F_EBX_RESERVED_30_16_MASK 0x7FFFu

/** EBX bits 15:12 */
#define AMD_CPUID_8000001F_EBX_NumVMPL_SHIFT 12u
#define AMD_CPUID_8000001F_EBX_NumVMPL_MASK 0xFu

/** EBX bits 11:6 */
#define AMD_CPUID_8000001F_EBX_PhysAddrReduction_SHIFT 6u
#define AMD_CPUID_8000001F_EBX_PhysAddrReduction_MASK 0x3Fu

/** EBX bits 5:0 */
#define AMD_CPUID_8000001F_EBX_CbitPosition_SHIFT 0u
#define AMD_CPUID_8000001F_EBX_CbitPosition_MASK 0x3Fu

/* ---- Fn8000001F, ECX ---- */

/** ECX bits 31:0 */
#define AMD_CPUID_8000001F_ECX_NumEncryptedGuests_SHIFT 0u
#define AMD_CPUID_8000001F_ECX_NumEncryptedGuests_MASK 0xFFFFFFFFu

/* ---- Fn8000001F, EDX ---- */

/** EDX bits 31:0 */
#define AMD_CPUID_8000001F_EDX_MinSevNoEsAsid_SHIFT 0u
#define AMD_CPUID_8000001F_EDX_MinSevNoEsAsid_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EAX_x0 ---- */

/** EAX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EAX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_EAX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EAX_x1 ---- */

/** EAX_x1 bits 31:0 */
#define AMD_CPUID_80000020_EAX_x1_BW_LEN_SHIFT 0u
#define AMD_CPUID_80000020_EAX_x1_BW_LEN_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EAX_x2 ---- */

/** EAX_x2 bits 31:0 */
#define AMD_CPUID_80000020_EAX_x2_BW_LEN_SHIFT 0u
#define AMD_CPUID_80000020_EAX_x2_BW_LEN_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EAX_x3 ---- */

/** EAX_x3 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EAX_x3_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_EAX_x3_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EBX_x0 ---- */

/** EBX_x0 bits 31:7 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EBX_x0_RESERVED_31_7_SHIFT 7u
#define AMD_CPUID_80000020_EBX_x0_RESERVED_31_7_MASK 0x1FFFFFFu

/** EBX_x0 bit 6 */
#define AMD_CPUID_80000020_EBX_x0_SDCIAE 6u

/** EBX_x0 bit 5 */
#define AMD_CPUID_80000020_EBX_x0_ABMC 5u

/** EBX_x0 bit 4 */
#define AMD_CPUID_80000020_EBX_x0_L3RR 4u

/** EBX_x0 bit 3 */
#define AMD_CPUID_80000020_EBX_x0_BMEC 3u

/** EBX_x0 bit 2 */
#define AMD_CPUID_80000020_EBX_x0_L3SMBE 2u

/** EBX_x0 bit 1 */
#define AMD_CPUID_80000020_EBX_x0_L3MBE 1u

/** EBX_x0 bit 0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EBX_x0_RESERVED_0_0 0u

/* ---- Fn80000020, EBX_x1 ---- */

/** EBX_x1 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EBX_x1_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_EBX_x1_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EBX_x2 ---- */

/** EBX_x2 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EBX_x2_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_EBX_x2_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EBX_x3 ---- */

/** EBX_x3 bits 31:7 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EBX_x3_RESERVED_31_7_SHIFT 7u
#define AMD_CPUID_80000020_EBX_x3_RESERVED_31_7_MASK 0x1FFFFFFu

/** EBX_x3 bits 31:8 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EBX_x3_RESERVED_31_8_SHIFT 8u
#define AMD_CPUID_80000020_EBX_x3_RESERVED_31_8_MASK 0xFFFFFFu

/** EBX_x3 bits 7:0 */
#define AMD_CPUID_80000020_EBX_x3_EVT_NUM_SHIFT 0u
#define AMD_CPUID_80000020_EBX_x3_EVT_NUM_MASK 0xFFu

/** EBX_x3 bit 6 */
#define AMD_CPUID_80000020_EBX_x3_L3CacheVicMon 6u

/** EBX_x3 bit 5 */
#define AMD_CPUID_80000020_EBX_x3_L3CacheRmtSlowBwFIllMon 5u

/* ---- Fn80000020, ECX_x0 ---- */

/** ECX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_ECX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_ECX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, ECX_x1 ---- */

/** ECX_x1 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_ECX_x1_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_ECX_x1_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, ECX_x2 ---- */

/** ECX_x2 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_ECX_x2_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_ECX_x2_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, ECX_x3 ---- */

/** ECX_x3 bit 4 */
#define AMD_CPUID_80000020_ECX_x3_L3CacheLclSlowBwFIllMon 4u

/** ECX_x3 bit 3 */
#define AMD_CPUID_80000020_ECX_x3_L3CacheRmtBwNtWrMon 3u

/** ECX_x3 bit 2 */
#define AMD_CPUID_80000020_ECX_x3_L3CacheLclBwNtWrMon 2u

/** ECX_x3 bit 1 */
#define AMD_CPUID_80000020_ECX_x3_L3CacheRmtBwFillMon 1u

/** ECX_x3 bit 0 */
#define AMD_CPUID_80000020_ECX_x3_L3CacheLclBwFillMon 0u

/* ---- Fn80000020, EDX_x0 ---- */

/** EDX_x0 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EDX_x0_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_EDX_x0_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EDX_x1 ---- */

/** EDX_x1 bits 31:0 */
#define AMD_CPUID_80000020_EDX_x1_COS_MAX_SHIFT 0u
#define AMD_CPUID_80000020_EDX_x1_COS_MAX_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EDX_x2 ---- */

/** EDX_x2 bits 31:0 */
#define AMD_CPUID_80000020_EDX_x2_COS_MAX_SHIFT 0u
#define AMD_CPUID_80000020_EDX_x2_COS_MAX_MASK 0xFFFFFFFFu

/* ---- Fn80000020, EDX_x3 ---- */

/** EDX_x3 bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000020_EDX_x3_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000020_EDX_x3_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000021, EAX ---- */

/** EAX bit 31 */
#define AMD_CPUID_80000021_EAX_SRSO_MSR_FIX 31u

/** EAX bit 30 */
#define AMD_CPUID_80000021_EAX_SRSO_USER_KERNEL_NO 30u

/** EAX bit 29 */
#define AMD_CPUID_80000021_EAX_SRSO_NO 29u

/** EAX bit 28 */
#define AMD_CPUID_80000021_EAX_IBPB_BRTYPE 28u

/** EAX bit 27 */
#define AMD_CPUID_80000021_EAX_SBPB 27u

/** EAX bits 26:25 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_EAX_RESERVED_26_25_SHIFT 25u
#define AMD_CPUID_80000021_EAX_RESERVED_26_25_MASK 0x3u

/** EAX bit 24 */
#define AMD_CPUID_80000021_EAX_ERAPS 24u

/** EAX bit 23 */
#define AMD_CPUID_80000021_EAX_AVX512_BMM 23u

/** EAX bit 22 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_EAX_RESERVED_22_22 22u

/** EAX bit 21 */
#define AMD_CPUID_80000021_EAX_FP512_DOWNGRADE 21u

/** EAX bit 20 */
#define AMD_CPUID_80000021_EAX_PREFETCHI 20u

/** EAX bit 19 */
#define AMD_CPUID_80000021_EAX_FAST_REP_SCASB 19u

/** EAX bit 18 */
#define AMD_CPUID_80000021_EAX_EPSF 18u

/** EAX bit 17 */
#define AMD_CPUID_80000021_EAX_CpuidUserDis 17u

/** EAX bit 16 */
#define AMD_CPUID_80000021_EAX_Opcode0F017Reclaim 16u

/** EAX bit 15 */
#define AMD_CPUID_80000021_EAX_AMD_ERMSB 15u

/** EAX bit 14 */
#define AMD_CPUID_80000021_EAX_L2TlbSizeX32 14u

/** EAX bit 13 */
#define AMD_CPUID_80000021_EAX_PrefetchCtlMsr 13u

/** EAX bit 12 */
#define AMD_CPUID_80000021_EAX_PreciseRetirePmc2Manual 12u

/** EAX bit 11 */
#define AMD_CPUID_80000021_EAX_FastShortRepeCmpsb 11u

/** EAX bit 10 */
#define AMD_CPUID_80000021_EAX_FastShortRepStosb 10u

/** EAX bit 9 */
#define AMD_CPUID_80000021_EAX_NoSmmCtlMSR 9u

/** EAX bit 8 */
#define AMD_CPUID_80000021_EAX_AutomaticIBRS 8u

/** EAX bit 7 */
#define AMD_CPUID_80000021_EAX_UpperAddressIgnore 7u

/** EAX bit 6 */
#define AMD_CPUID_80000021_EAX_NullSelectClearsBase 6u

/** EAX bits 5:4 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_EAX_RESERVED_5_4_SHIFT 4u
#define AMD_CPUID_80000021_EAX_RESERVED_5_4_MASK 0x3u

/** EAX bit 3 */
#define AMD_CPUID_80000021_EAX_SmmPgCfgLock 3u

/** EAX bit 2 */
#define AMD_CPUID_80000021_EAX_LFenceAlwaysSerializing 2u

/** EAX bit 1 */
#define AMD_CPUID_80000021_EAX_FsGsBaseWriteNotSerializing 1u

/** EAX bit 0 */
#define AMD_CPUID_80000021_EAX_NoNestedDataBp 0u

/* ---- Fn80000021, EBX ---- */

/** EBX bits 31:24 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_EBX_RESERVED_31_24_SHIFT 24u
#define AMD_CPUID_80000021_EBX_RESERVED_31_24_MASK 0xFFu

/** EBX bits 23:16 */
#define AMD_CPUID_80000021_EBX_RapSize_SHIFT 16u
#define AMD_CPUID_80000021_EBX_RapSize_MASK 0xFFu

/** EBX bits 15:0 */
#define AMD_CPUID_80000021_EBX_MicrocodePatchSize_SHIFT 0u
#define AMD_CPUID_80000021_EBX_MicrocodePatchSize_MASK 0xFFFFu

/* ---- Fn80000021, ECX ---- */

/** ECX bits 31:9 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_ECX_RESERVED_31_9_SHIFT 9u
#define AMD_CPUID_80000021_ECX_RESERVED_31_9_MASK 0x7FFFFFu

/** ECX bit 8 */
#define AMD_CPUID_80000021_ECX_BtbCtxIsolation 8u

/** ECX bits 7:6 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_ECX_RESERVED_7_6_SHIFT 6u
#define AMD_CPUID_80000021_ECX_RESERVED_7_6_MASK 0x3u

/** ECX bit 5 */
#define AMD_CPUID_80000021_ECX_VERW_CLEAR 5u

/** ECX bits 4:3 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_ECX_RESERVED_4_3_SHIFT 3u
#define AMD_CPUID_80000021_ECX_RESERVED_4_3_MASK 0x3u

/** ECX bit 2 */
#define AMD_CPUID_80000021_ECX_TSA_L1_NO 2u

/** ECX bit 1 */
#define AMD_CPUID_80000021_ECX_TSA_SQ_NO 1u

/** ECX bit 0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000021_ECX_RESERVED_0_0 0u

/* ---- Fn80000023, EAX ---- */

/** EAX bits 31:1 -- RESERVADO, no se toca */
#define AMD_CPUID_80000023_EAX_RESERVED_31_1_SHIFT 1u
#define AMD_CPUID_80000023_EAX_RESERVED_31_1_MASK 0x7FFFFFFFu

/** EAX bit 0 */
#define AMD_CPUID_80000023_EAX_MemHmk 0u

/* ---- Fn80000023, EBX ---- */

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_80000023_EBX_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_80000023_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:0 */
#define AMD_CPUID_80000023_EBX_MaxMemHmkEncrKeyID_SHIFT 0u
#define AMD_CPUID_80000023_EBX_MaxMemHmkEncrKeyID_MASK 0xFFFFu

/* ---- Fn80000023, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000023_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000023_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000023, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000023_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000023_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_AMD_SECURITY_H */
