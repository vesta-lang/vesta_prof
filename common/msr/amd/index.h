/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file index.h
 * @brief Todos los MSR del PPR de AMD, por familia.
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

#ifndef VXP_COMMON_MSR_AMD_INDEX_H
#define VXP_COMMON_MSR_AMD_INDEX_H

#include "msr_amd_ibs.h"
#include "msr_amd_perf.h"
#include "msr_amd_mcheck.h"
#include "msr_amd_apic.h"
#include "msr_amd_power.h"
#include "msr_amd_memory.h"
#include "msr_amd_virt.h"
#include "msr_amd_security.h"
#include "msr_amd_platform.h"


#endif /* VXP_COMMON_MSR_AMD_INDEX_H */
