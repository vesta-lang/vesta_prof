/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file index.h
 * @brief Todos los campos de CPUID de AMD, por familia.
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

#ifndef VXP_COMMON_CPUID_AMD_INDEX_H
#define VXP_COMMON_CPUID_AMD_INDEX_H

#include "cpuid_amd_ibs.h"
#include "cpuid_amd_perf.h"
#include "cpuid_amd_topology.h"
#include "cpuid_amd_power.h"
#include "cpuid_amd_security.h"
#include "cpuid_amd_features.h"
#include "cpuid_amd_ident.h"
#include "cpuid_amd_misc.h"


#endif /* VXP_COMMON_CPUID_AMD_INDEX_H */
