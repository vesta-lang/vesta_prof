/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amd_ident.h
 * @brief Identificacion y cadena de marca
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

#ifndef VXP_COMMON_CPUID_AMD_IDENT_H
#define VXP_COMMON_CPUID_AMD_IDENT_H

/* ---- Fn80000000, EAX ---- */

/** EAX bits 31:0 */
#define AMD_CPUID_80000000_EAX_LFuncExt_SHIFT 0u
#define AMD_CPUID_80000000_EAX_LFuncExt_MASK 0xFFFFFFFFu

/* ---- Fn80000000, EBX ---- */

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000000_EBX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000000_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000000, ECX ---- */

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000000_ECX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000000_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- Fn80000000, EDX ---- */

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define AMD_CPUID_80000000_EDX_RESERVED_31_0_SHIFT 0u
#define AMD_CPUID_80000000_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_AMD_IDENT_H */
