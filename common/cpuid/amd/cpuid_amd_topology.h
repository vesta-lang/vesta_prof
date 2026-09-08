/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_topology.h
 * @brief Topologia: cache, hermanos SMT y nodo
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

#ifndef VXP_COMMON_CPUID_AMD_TOPOLOGY_H
#define VXP_COMMON_CPUID_AMD_TOPOLOGY_H

/* ---- Fn80000005, EAX ---- */

/** EAX bits 31:24 */
#define AMD_CPUID_80000005_EAX_L1DTlb2and4MAssoc_SHIFT 24u
#define AMD_CPUID_80000005_EAX_L1DTlb2and4MAssoc_MASK 0xFFu

/** EAX bits 23:16 */
#define AMD_CPUID_80000005_EAX_L1DTlb2and4MSize_SHIFT 16u
#define AMD_CPUID_80000005_EAX_L1DTlb2and4MSize_MASK 0xFFu

/** EAX bits 15:8 */
#define AMD_CPUID_80000005_EAX_L1ITlb2and4MAssoc_SHIFT 8u
#define AMD_CPUID_80000005_EAX_L1ITlb2and4MAssoc_MASK 0xFFu

/** EAX bits 7:0 */
#define AMD_CPUID_80000005_EAX_L1ITlb2and4MSize_SHIFT 0u
#define AMD_CPUID_80000005_EAX_L1ITlb2and4MSize_MASK 0xFFu

/* ---- Fn80000005, EBX ---- */

/** EBX bits 31:24 */
#define AMD_CPUID_80000005_EBX_L1DTlb4KAssoc_SHIFT 24u
#define AMD_CPUID_80000005_EBX_L1DTlb4KAssoc_MASK 0xFFu

/** EBX bits 23:16 */
#define AMD_CPUID_80000005_EBX_L1DTlb4KSize_SHIFT 16u
#define AMD_CPUID_80000005_EBX_L1DTlb4KSize_MASK 0xFFu

/** EBX bits 15:8 */
#define AMD_CPUID_80000005_EBX_L1ITlb4KAssoc_SHIFT 8u
#define AMD_CPUID_80000005_EBX_L1ITlb4KAssoc_MASK 0xFFu

/** EBX bits 7:0 */
#define AMD_CPUID_80000005_EBX_L1ITlb4KSize_SHIFT 0u
#define AMD_CPUID_80000005_EBX_L1ITlb4KSize_MASK 0xFFu

/* ---- Fn80000005, ECX ---- */

/** ECX bits 31:24 */
#define AMD_CPUID_80000005_ECX_L1DcSize_SHIFT 24u
#define AMD_CPUID_80000005_ECX_L1DcSize_MASK 0xFFu

/** ECX bits 23:16 */
#define AMD_CPUID_80000005_ECX_L1DcAssoc_SHIFT 16u
#define AMD_CPUID_80000005_ECX_L1DcAssoc_MASK 0xFFu

/** ECX bits 15:8 */
#define AMD_CPUID_80000005_ECX_L1DcLinesPerTag_SHIFT 8u
#define AMD_CPUID_80000005_ECX_L1DcLinesPerTag_MASK 0xFFu

/** ECX bits 7:0 */
#define AMD_CPUID_80000005_ECX_L1DcLineSize_SHIFT 0u
#define AMD_CPUID_80000005_ECX_L1DcLineSize_MASK 0xFFu

/* ---- Fn80000005, EDX ---- */

/** EDX bits 31:24 */
#define AMD_CPUID_80000005_EDX_L1IcSize_SHIFT 24u
#define AMD_CPUID_80000005_EDX_L1IcSize_MASK 0xFFu

/** EDX bits 23:16 */
#define AMD_CPUID_80000005_EDX_L1IcAssoc_SHIFT 16u
#define AMD_CPUID_80000005_EDX_L1IcAssoc_MASK 0xFFu

/** EDX bits 15:8 */
#define AMD_CPUID_80000005_EDX_L1IcLinesPerTag_SHIFT 8u
#define AMD_CPUID_80000005_EDX_L1IcLinesPerTag_MASK 0xFFu

/** EDX bits 7:0 */
#define AMD_CPUID_80000005_EDX_L1IcLineSize_SHIFT 0u
#define AMD_CPUID_80000005_EDX_L1IcLineSize_MASK 0xFFu

/* ---- Fn80000006, EAX ---- */

/** EAX bits 31:28 */
#define AMD_CPUID_80000006_EAX_L2DTlb2and4MAssoc_SHIFT 28u
#define AMD_CPUID_80000006_EAX_L2DTlb2and4MAssoc_MASK 0xFu

/** EAX bits 27:16 */
#define AMD_CPUID_80000006_EAX_L2DTlb2and4MSize_SHIFT 16u
#define AMD_CPUID_80000006_EAX_L2DTlb2and4MSize_MASK 0xFFFu

/** EAX bits 15:12 */
#define AMD_CPUID_80000006_EAX_L2ITlb2and4MAssoc_SHIFT 12u
#define AMD_CPUID_80000006_EAX_L2ITlb2and4MAssoc_MASK 0xFu

/** EAX bits 11:0 */
#define AMD_CPUID_80000006_EAX_L2ITlb2and4MSize_SHIFT 0u
#define AMD_CPUID_80000006_EAX_L2ITlb2and4MSize_MASK 0xFFFu

/* ---- Fn80000006, EBX ---- */

/** EBX bits 31:28 */
#define AMD_CPUID_80000006_EBX_L2DTlb4KAssoc_SHIFT 28u
#define AMD_CPUID_80000006_EBX_L2DTlb4KAssoc_MASK 0xFu

/** EBX bits 27:16 */
#define AMD_CPUID_80000006_EBX_L2DTlb4KSize_SHIFT 16u
#define AMD_CPUID_80000006_EBX_L2DTlb4KSize_MASK 0xFFFu

/** EBX bits 15:12 */
#define AMD_CPUID_80000006_EBX_L2ITlb4KAssoc_SHIFT 12u
#define AMD_CPUID_80000006_EBX_L2ITlb4KAssoc_MASK 0xFu

/** EBX bits 11:0 */
#define AMD_CPUID_80000006_EBX_L2ITlb4KSize_SHIFT 0u
#define AMD_CPUID_80000006_EBX_L2ITlb4KSize_MASK 0xFFFu

/* ---- Fn80000006, ECX ---- */

/** ECX bits 31:16 */
#define AMD_CPUID_80000006_ECX_L2Size_SHIFT 16u
#define AMD_CPUID_80000006_ECX_L2Size_MASK 0xFFFFu

/** ECX bits 15:12 */
#define AMD_CPUID_80000006_ECX_L2Assoc_SHIFT 12u
#define AMD_CPUID_80000006_ECX_L2Assoc_MASK 0xFu

/** ECX bits 11:8 */
#define AMD_CPUID_80000006_ECX_L2LinesPerTag_SHIFT 8u
#define AMD_CPUID_80000006_ECX_L2LinesPerTag_MASK 0xFu

/** ECX bits 7:0 */
#define AMD_CPUID_80000006_ECX_L2LineSize_SHIFT 0u
#define AMD_CPUID_80000006_ECX_L2LineSize_MASK 0xFFu

/* ---- Fn80000006, EDX ---- */

/** EDX bits 31:18 */
#define AMD_CPUID_80000006_EDX_L3Size_SHIFT 18u
#define AMD_CPUID_80000006_EDX_L3Size_MASK 0x3FFFu

/** EDX bits 17:16 -- RESERVADO, no se toca */
#define AMD_CPUID_80000006_EDX_RESERVED_17_16_SHIFT 16u
#define AMD_CPUID_80000006_EDX_RESERVED_17_16_MASK 0x3u

/** EDX bits 15:12 */
#define AMD_CPUID_80000006_EDX_L3Assoc_SHIFT 12u
#define AMD_CPUID_80000006_EDX_L3Assoc_MASK 0xFu

/** EDX bits 11:8 */
#define AMD_CPUID_80000006_EDX_L3LinesPerTag_SHIFT 8u
#define AMD_CPUID_80000006_EDX_L3LinesPerTag_MASK 0xFu

/** EDX bits 7:0 */
#define AMD_CPUID_80000006_EDX_L3LineSize_SHIFT 0u
#define AMD_CPUID_80000006_EDX_L3LineSize_MASK 0xFFu

/* ---- Fn8000001D, EAX ---- */

/** EAX bits 31:26 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001D_EAX_RESERVED_31_26_SHIFT 26u
#define AMD_CPUID_8000001D_EAX_RESERVED_31_26_MASK 0x3Fu

/** EAX bits 25:14 */
#define AMD_CPUID_8000001D_EAX_NumSharingCache_SHIFT 14u
#define AMD_CPUID_8000001D_EAX_NumSharingCache_MASK 0xFFFu

/** EAX bits 13:10 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001D_EAX_RESERVED_13_10_SHIFT 10u
#define AMD_CPUID_8000001D_EAX_RESERVED_13_10_MASK 0xFu

/** EAX bit 9 */
#define AMD_CPUID_8000001D_EAX_FullyAssociative 9u

/** EAX bit 8 */
#define AMD_CPUID_8000001D_EAX_SelfInitialization 8u

/** EAX bits 7:5 */
#define AMD_CPUID_8000001D_EAX_CacheLevel_SHIFT 5u
#define AMD_CPUID_8000001D_EAX_CacheLevel_MASK 0x7u

/** EAX bits 4:0 */
#define AMD_CPUID_8000001D_EAX_CacheType_SHIFT 0u
#define AMD_CPUID_8000001D_EAX_CacheType_MASK 0x1Fu

/* ---- Fn8000001D, EBX ---- */

/** EBX bits 31:22 */
#define AMD_CPUID_8000001D_EBX_CacheNumWays_SHIFT 22u
#define AMD_CPUID_8000001D_EBX_CacheNumWays_MASK 0x3FFu

/** EBX bits 21:12 */
#define AMD_CPUID_8000001D_EBX_CachePhysPartitions_SHIFT 12u
#define AMD_CPUID_8000001D_EBX_CachePhysPartitions_MASK 0x3FFu

/** EBX bits 11:0 */
#define AMD_CPUID_8000001D_EBX_CacheLineSize_SHIFT 0u
#define AMD_CPUID_8000001D_EBX_CacheLineSize_MASK 0xFFFu

/* ---- Fn8000001D, ECX ---- */

/** ECX bits 31:0 */
#define AMD_CPUID_8000001D_ECX_CacheNumSets_SHIFT 0u
#define AMD_CPUID_8000001D_ECX_CacheNumSets_MASK 0xFFFFFFFFu

/* ---- Fn8000001D, EDX ---- */

/** EDX bits 31:2 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001D_EDX_RESERVED_31_2_SHIFT 2u
#define AMD_CPUID_8000001D_EDX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** EDX bit 1 */
#define AMD_CPUID_8000001D_EDX_CacheInclusive 1u

/** EDX bit 0 */
#define AMD_CPUID_8000001D_EDX_WBINVD 0u

/* ---- Fn8000001E, EAX ---- */

/** EAX bits 31:0 */
#define AMD_CPUID_8000001E_EAX_ExtendedApicId_SHIFT 0u
#define AMD_CPUID_8000001E_EAX_ExtendedApicId_MASK 0xFFFFFFFFu

/* ---- Fn8000001E, EBX ---- */

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001E_EBX_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_8000001E_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:8 */
#define AMD_CPUID_8000001E_EBX_ThreadsPerComputeUnit_SHIFT 8u
#define AMD_CPUID_8000001E_EBX_ThreadsPerComputeUnit_MASK 0xFFu

/** EBX bits 7:0 */
#define AMD_CPUID_8000001E_EBX_ComputeUnitId_SHIFT 0u
#define AMD_CPUID_8000001E_EBX_ComputeUnitId_MASK 0xFFu

/* ---- Fn8000001E, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_8000001E_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_8000001E_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 10:8 */
#define AMD_CPUID_8000001E_ECX_NodesPerProcessor_SHIFT 8u
#define AMD_CPUID_8000001E_ECX_NodesPerProcessor_MASK 0x7u

/** ECX bits 7:0 */
#define AMD_CPUID_8000001E_ECX_NodeId_SHIFT 0u
#define AMD_CPUID_8000001E_ECX_NodeId_MASK 0xFFu

/* ---- Fn80000026, EAX ---- */

/** EAX bit 31 */
#define AMD_CPUID_80000026_EAX_AsymmetricTopology 31u

/** EAX bit 30 */
#define AMD_CPUID_80000026_EAX_HeterogeneousCores 30u

/** EAX bit 29 */
#define AMD_CPUID_80000026_EAX_EfficiencyRankingAvailable 29u

/** EAX bits 28:5 -- RESERVADO, no se toca */
#define AMD_CPUID_80000026_EAX_RESERVED_28_5_SHIFT 5u
#define AMD_CPUID_80000026_EAX_RESERVED_28_5_MASK 0xFFFFFFu

/** EAX bits 4:0 */
#define AMD_CPUID_80000026_EAX_MaskWidth_SHIFT 0u
#define AMD_CPUID_80000026_EAX_MaskWidth_MASK 0x1Fu

/* ---- Fn80000026, EBX ---- */

/** EBX bits 31:28 */
#define AMD_CPUID_80000026_EBX_CoreType_SHIFT 28u
#define AMD_CPUID_80000026_EBX_CoreType_MASK 0xFu

/** EBX bits 27:24 */
#define AMD_CPUID_80000026_EBX_NativeModelID_SHIFT 24u
#define AMD_CPUID_80000026_EBX_NativeModelID_MASK 0xFu

/** EBX bits 23:16 */
#define AMD_CPUID_80000026_EBX_PwrEfficiencyRanking_SHIFT 16u
#define AMD_CPUID_80000026_EBX_PwrEfficiencyRanking_MASK 0xFFu

/** EBX bits 15:0 */
#define AMD_CPUID_80000026_EBX_NumLogProc_SHIFT 0u
#define AMD_CPUID_80000026_EBX_NumLogProc_MASK 0xFFFFu

/* ---- Fn80000026, ECX ---- */

/** ECX bits 31:16 -- RESERVADO, no se toca */
#define AMD_CPUID_80000026_ECX_RESERVED_31_16_SHIFT 16u
#define AMD_CPUID_80000026_ECX_RESERVED_31_16_MASK 0xFFFFu

/** ECX bits 15:8 */
#define AMD_CPUID_80000026_ECX_LevelType_SHIFT 8u
#define AMD_CPUID_80000026_ECX_LevelType_MASK 0xFFu

/** ECX bits 7:0 */
#define AMD_CPUID_80000026_ECX_InputEcx_SHIFT 0u
#define AMD_CPUID_80000026_ECX_InputEcx_MASK 0xFFu

/* ---- Fn80000026, EDX ---- */

/** EDX bits 31:0 */
#define AMD_CPUID_80000026_EDX_ExtendedApicId_SHIFT 0u
#define AMD_CPUID_80000026_EDX_ExtendedApicId_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_AMD_TOPOLOGY_H */
