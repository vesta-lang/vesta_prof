/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_ibs.h
 * @brief Instruction-Based Sampling: el muestreo preciso de AMD
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

#ifndef VXP_COMMON_MSR_AMD_IBS_H
#define VXP_COMMON_MSR_AMD_IBS_H

/** IBS Fetch Control */
#define AMD_IBS_FETCH_CTL 0xC0011030u

/** IBS Fetch Linear Address */
#define AMD_IBS_FETCH_LINADDR 0xC0011031u

/** IBS Fetch Physical Address */
#define AMD_IBS_FETCH_PHYSADDR 0xC0011032u

/** IBS Execution Control */
#define AMD_IBS_OP_CTL 0xC0011033u

/** IBS Op RIP */
#define AMD_IBS_OP_RIP 0xC0011034u

/** IBS Op Data */
#define AMD_IBS_OP_DATA 0xC0011035u

/** IBS Op Data 2 */
#define AMD_IBS_OP_DATA2 0xC0011036u

/** IBS Op Data 3 */
#define AMD_IBS_OP_DATA3 0xC0011037u

/** IBS DC Linear Address */
#define AMD_IBS_DC_LINADDR 0xC0011038u

/** IBS DC Physical Address */
#define AMD_IBS_DC_PHYSADDR 0xC0011039u

/** IBS Control */
#define AMD_IBS_CTL 0xC001103Au

/** IBS Branch Target Address */
#define AMD_BP_IBSTGT_RIP 0xC001103Bu

/** IBS Fetch Control Extended */
#define AMD_IC_IBS_EXTD_CTL 0xC001103Cu


#endif /* VXP_COMMON_MSR_AMD_IBS_H */
