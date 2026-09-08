/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_virt.h
 * @brief Virtualizacion y modo de gestion del sistema
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

#ifndef VXP_COMMON_MSR_AMD_VIRT_H
#define VXP_COMMON_MSR_AMD_VIRT_H

/** Time Stamp Counter Ratio */
#define AMD_TscRateMsr 0xC0000104u

/** IO Trap Control */
#define AMD_SMI_ON_IO_TRAP_CTL_STS 0xC0010054u

/** SMI Trigger IO Cycle */
#define AMD_SmiTrigIoCycle 0xC0010056u

/** SMM Base Address */
#define AMD_SMM_BASE 0xC0010111u

/** SMM TSeg Base Address */
#define AMD_SMMAddr 0xC0010112u

/** SMM TSeg Mask */
#define AMD_SMMMask 0xC0010113u

/** Virtual Machine Control */
#define AMD_VM_CR 0xC0010114u

/** Virtual Machine Host Save Physical Address */
#define AMD_VM_HSAVE_PA 0xC0010117u

/** Guest TSC Frequency */
#define AMD_GUEST_TSC_FREQ 0xC0010134u


#endif /* VXP_COMMON_MSR_AMD_VIRT_H */
