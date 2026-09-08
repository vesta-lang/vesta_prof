/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_platform.h
 * @brief Plataforma: llamada al sistema, bases y marca de tiempo
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

#ifndef VXP_COMMON_MSR_AMD_PLATFORM_H
#define VXP_COMMON_MSR_AMD_PLATFORM_H

/** Time Stamp Counter */
#define AMD_TSC 0x10u

/** Cluster ID */
#define AMD_EBL_CR_POWERON 0x2Au

/** Time Stamp Counter Adjustment */
#define AMD_TSC_ADJUST 0x3Bu

/** Patch Level */
#define AMD_PATCH_LEVEL 0x8Bu

/** SYSENTER CS */
#define AMD_SYSENTER_CS 0x174u

/** SYSENTER ESP */
#define AMD_SYSENTER_ESP 0x175u

/** SYSENTER EIP */
#define AMD_SYSENTER_EIP 0x176u

/** Extended Supervisor State */
#define AMD_XSS 0xDA0u

/** Extended Feature Enable */
#define AMD_EFER 0xC0000080u

/** SYSCALL Target Address */
#define AMD_STAR 0xC0000081u

/** Long Mode SYSCALL Target Address */
#define AMD_STAR64 0xC0000082u

/** Compatibility Mode SYSCALL Target Address */
#define AMD_STARCOMPAT 0xC0000083u

/** SYSCALL Flag Mask */
#define AMD_SYSCALL_FLAG_MASK 0xC0000084u

/** FS Base */
#define AMD_FS_BASE 0xC0000100u

/** GS Base */
#define AMD_GS_BASE 0xC0000101u

/** Kernel GS Base */
#define AMD_KernelGSbase 0xC0000102u

/** Auxiliary Time Stamp Counter */
#define AMD_TSC_AUX 0xC0000103u

/** Patch Loader */
#define AMD_PATCH_LOADER 0xC0010020u

/** IGNNE */
#define AMD_IGNNE 0xC0010115u

/** SVM Lock Key */
#define AMD_SvmLockKey 0xC0010118u

/** Address Mask For DR1 Breakpoint */
#define AMD_DR1_ADDR_MASK 0xC0011019u

/** Address Mask For DR2 Breakpoint */
#define AMD_DR2_ADDR_MASK 0xC001101Au

/** Address Mask For DR3 Breakpoint */
#define AMD_DR3_ADDR_MASK 0xC001101Bu

/** Address Mask For DR0 Breakpoints */
#define AMD_DR0_ADDR_MASK 0xC0011027u


#endif /* VXP_COMMON_MSR_AMD_PLATFORM_H */
