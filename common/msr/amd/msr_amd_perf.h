/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_perf.h
 * @brief Contadores de rendimiento, ramas y depuracion
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_amd_msr_index.py
 *
 * a partir del Processor Programming Reference de AMD, que no viaja en el
 * repositorio.
 *
 * ALCANCE: **SOLO AMD**.  Los MSR de Intel estan en `common/msr/intel/`, y no
 * se mezclan porque los dos espacios se solapan sin coincidir: `0xC0000080` es
 * `IA32_EFER` para Intel y `EFER` para AMD -- el mismo registro con dos nombres
 * y dos fuentes --, mientras que `0xC0011030` solo existe aqui.
 *
 * Los nombres son los del PPR, para que se pueda buscar en el la misma cadena
 * que hay aqui.  Solo direcciones: los campos de bits de lo que se decodifique
 * se escriben a mano.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef VXP_COMMON_MSR_AMD_PERF_H
#define VXP_COMMON_MSR_AMD_PERF_H

/** Max Performance Frequency Clock Count */
#define AMD_MPERF 0xE7u

/** Actual Performance Frequency Clock Count */
#define AMD_APERF 0xE8u

/** Debug Control */
#define AMD_DBG_CTL_MSR 0x1D9u

/** Last Branch From IP */
#define AMD_BR_FROM 0x1DBu

/** Last Branch To IP */
#define AMD_BR_TO 0x1DCu

/** Last Exception From IP */
#define AMD_LastExcpFromIp 0x1DDu

/** Last Exception To IP */
#define AMD_LastExcpToIp 0x1DEu

/** LVT Performance Monitor */
#define AMD_PerformanceCounterLvtEntry 0x834u

/** Monitoring Event Select */
#define AMD_QM_EVTSEL 0xC8Du

/** QOS L3 Counter */
#define AMD_QM_CTR 0xC8Eu

/** Read-Only Max Performance Frequency Clock Count */
#define AMD_MPerfReadOnly 0xC00000E7u

/** Read-Only Actual Performance Frequency Clock Count */
#define AMD_APerfReadOnly 0xC00000E8u

/** Instructions Retired Performance Count */
#define AMD_IRPerfCount 0xC00000E9u

/** Last Branch Stack Select */
#define AMD_LastBranchStackSelect 0xC000010Eu

/** Debug Extension Control */
#define AMD_DebugExtnCtl 0xC000010Fu

/** Performance Counter Global Status */
#define AMD_PerfCntrGlobalStatus 0xC0000300u

/** Performance Counter Global Control */
#define AMD_PerfCntrGlobalCtl 0xC0000301u

/** Performance Counter Global Status Clear */
#define AMD_PerfCntrGlobalStatusClr 0xC0000302u

/** Performance Counter Global Status Set */
#define AMD_PerfCntrGlobalStatusSet 0xC0000303u

/** Performance Event Select 0 */
#define AMD_PERF_LEGACY_CTL0 0xC0010000u

/** Performance Event Select 1 */
#define AMD_PERF_LEGACY_CTL1 0xC0010001u

/** Performance Event Select 2 */
#define AMD_PERF_LEGACY_CTL2 0xC0010002u

/** Performance Event Select 3 */
#define AMD_PERF_LEGACY_CTL3 0xC0010003u

/** Performance Event Select 0 */
#define AMD_PERF_CTL0 0xC0010200u

/** Performance Event Select 1 */
#define AMD_PERF_CTL1 0xC0010202u

/** Performance Event Select 2 */
#define AMD_PERF_CTL2 0xC0010204u

/** Performance Event Select 3 */
#define AMD_PERF_CTL3 0xC0010206u

/** Performance Event Select 4 */
#define AMD_PERF_CTL4 0xC0010208u

/** Performance Event Select 5 */
#define AMD_PERF_CTL5 0xC001020Au


#endif /* VXP_COMMON_MSR_AMD_PERF_H */
