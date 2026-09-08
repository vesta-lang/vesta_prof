/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_security.h
 * @brief Mitigaciones, pila de sombra e identificacion de pieza
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

#ifndef VXP_COMMON_MSR_AMD_SECURITY_H
#define VXP_COMMON_MSR_AMD_SECURITY_H

/** Speculative Control */
#define AMD_SPEC_CTRL 0x48u

/** Prediction Command */
#define AMD_PRED_CMD 0x49u

/** Flush Command */
#define AMD_FLUSH_CMD 0x10Bu

/** User CET */
#define AMD_U_CET 0x6A0u

/** Supervisor CET */
#define AMD_S_CET 0x6A2u

/** PL0 Shadow Stack Pointer */
#define AMD_PL0Ssp 0x6A4u

/** PL1 Shadow Stack Pointer */
#define AMD_PL1Ssp 0x6A5u

/** PL2 Shadow Stack Pointer */
#define AMD_PL2Ssp 0x6A6u

/** PL3 Shadow Stack Pointer */
#define AMD_PL3Ssp 0x6A7u

/** Interrupt SSP Table Address */
#define AMD_IstSspAddr 0x6A8u

/** OS Visible Work-around Length */
#define AMD_OSVW_ID_Length 0xC0010140u

/** OS Visible Work-around Status */
#define AMD_OSVW_Status 0xC0010141u

/** Protected Processor Inventory Number Control */
#define AMD_PPIN_CTL 0xC00102F0u

/** Protected Processor Inventory Number */
#define AMD_PPIN 0xC00102F1u


#endif /* VXP_COMMON_MSR_AMD_SECURITY_H */
