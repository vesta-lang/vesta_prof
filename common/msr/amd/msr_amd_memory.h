/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_memory.h
 * @brief Memoria: MTRR, tipos de pagina y ventanas
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

#ifndef VXP_COMMON_MSR_AMD_MEMORY_H
#define VXP_COMMON_MSR_AMD_MEMORY_H

/** MTRR Capabilities */
#define AMD_MTRRcap 0xFEu

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_64K 0x250u

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_16K_0 0x258u

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_16K_1 0x259u

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_0 0x268u

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_1 0x269u

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_2 0x26Au

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_3 0x26Bu

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_4 0x26Cu

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_5 0x26Du

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_6 0x26Eu

/** Fixed-Size MTRRs */
#define AMD_MtrrFix_4K_7 0x26Fu

/** Page Attribute Table */
#define AMD_PAT 0x277u

/** MTRR Default Memory Type */
#define AMD_MTRRdefType 0x2FFu

/** Prefetch Control */
#define AMD_PrefetchControl 0xC0000108u

/** System Configuration */
#define AMD_SYS_CFG 0xC0010010u

/** Top Of Memory */
#define AMD_TOP_MEM 0xC001001Au

/** Top Of Memory 2 */
#define AMD_TOM2 0xC001001Du

/** MMIO Configuration Base Address */
#define AMD_MmioCfgBaseAddr 0xC0010058u

/** Virtual TOM */
#define AMD_VIRTUAL_TOM 0xC0010135u


#endif /* VXP_COMMON_MSR_AMD_MEMORY_H */
