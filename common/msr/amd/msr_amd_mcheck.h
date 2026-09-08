/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_mcheck.h
 * @brief Comprobacion de maquina
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

#ifndef VXP_COMMON_MSR_AMD_MCHECK_H
#define VXP_COMMON_MSR_AMD_MCHECK_H

/** Global Machine Check Capabilities */
#define AMD_MCG_CAP 0x179u

/** Global Machine Check Status */
#define AMD_MCG_STAT 0x17Au

/** Global Machine Check Exception Reporting Control */
#define AMD_MCG_CTL 0x17Bu

/** MCA Interrupt Configuration */
#define AMD_McaIntrCfg 0xC0000410u

/** Machine Check Exception Redirection */
#define AMD_McExcepRedir 0xC0010022u


#endif /* VXP_COMMON_MSR_AMD_MCHECK_H */
