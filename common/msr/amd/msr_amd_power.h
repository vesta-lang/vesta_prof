/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_power.h
 * @brief Estados de rendimiento, energia y control termico
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

#ifndef VXP_COMMON_MSR_AMD_POWER_H
#define VXP_COMMON_MSR_AMD_POWER_H

/** Hardware Configuration */
#define AMD_HWCR 0xC0010015u

/** P-state Current Limit */
#define AMD_PStateCurLim 0xC0010061u

/** P-state Control */
#define AMD_PStateCtl 0xC0010062u

/** P-state Status */
#define AMD_PStateStat 0xC0010063u

/** C-state Base Address */
#define AMD_CStateBaseAddr 0xC0010073u

/** Core Energy Status */
#define AMD_CORE_ENERGY_STAT 0xC001029Au

/** CPPC Capability 1 */
#define AMD_CppcCapability1 0xC00102B0u

/** CPPC Enable */
#define AMD_CppcEnable 0xC00102B1u

/** CPPC Capability 2 */
#define AMD_CppcCapability2 0xC00102B2u

/** CPPC Request */
#define AMD_CppcRequest 0xC00102B3u

/** CPPC Status */
#define AMD_CppcStatus 0xC00102B4u

/** Thermal and Power Management CPUID Features */
#define AMD_CPUID_PWR_THERM 0xC0011003u


#endif /* VXP_COMMON_MSR_AMD_POWER_H */
