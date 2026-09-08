/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_amx.h
 * @brief Mosaicos de matrices
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_cpuid_index.py
 *
 * a partir del volcado de texto del manual de Intel, que no viaja en el
 * repositorio.
 *
 * POR QUE SE GENERA.  Un numero de bit escrito de memoria falla callado: se
 * pregunta por el bit equivocado, sale cero, y se concluye que la maquina no
 * tiene algo que si tiene.  No da un error -- da una capacidad perdida.
 *
 * Los nombres son los del manual, para que se pueda buscar en el la misma
 * cadena que hay aqui.  Un campo de un bit sale como su POSICION; uno de varios,
 * como DESPLAZAMIENTO y MASCARA.
 *
 * ALCANCE: **SOLO INTEL**, y conviene no confundirlo con "completo".  Sale del
 * manual de Intel, asi que describe lo que Intel documenta -- y el rango
 * extendido de Intel acaba en 80000008H.  Por encima de ahi hay hojas que
 * existen y que son de AMD:
 *
 *     8000001BH   IBS, el muestreo preciso de AMD
 *     8000001DH   topologia de cache
 *     8000001EH   extensiones de topologia: hermanos SMT, nodo
 *     80000021H   tamano del parche de microcodigo, predictor de retorno
 *
 * Que no esten aqui NO significa que no existan: significa que su fuente es
 * otra, el manual de AMD, y que hay que tratarla igual que esta -- no de
 * memoria.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef VXP_COMMON_CPUID_AMX_H
#define VXP_COMMON_CPUID_AMX_H

/* ---- hoja 1DH, subhoja 00H -- Tile Information Main Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_1D_00_EAX_MAX_PALETTE_SHIFT 0u
#define CPUID_1D_00_EAX_MAX_PALETTE_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1D_00_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_1D_00_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1D_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_1D_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1D_00_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_1D_00_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 1DH, subhoja 01H -- Tile Palette 1 ---- */

/** EAX bits 31:16 */
#define CPUID_1D_01_EAX_BYTES_PER_TILE_SHIFT 16u
#define CPUID_1D_01_EAX_BYTES_PER_TILE_MASK 0xFFFFu

/** EAX bits 15:0 */
#define CPUID_1D_01_EAX_TOTAL_TILE_BYTES_SHIFT 0u
#define CPUID_1D_01_EAX_TOTAL_TILE_BYTES_MASK 0xFFFFu

/** EBX bits 31:16 */
#define CPUID_1D_01_EBX_MAX_NAMES_SHIFT 16u
#define CPUID_1D_01_EBX_MAX_NAMES_MASK 0xFFFFu

/** EBX bits 15:0 */
#define CPUID_1D_01_EBX_BYTES_PER_ROW_SHIFT 0u
#define CPUID_1D_01_EBX_BYTES_PER_ROW_MASK 0xFFFFu

/** ECX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_1D_01_ECX_RESERVED_31_16_SHIFT 16u
#define CPUID_1D_01_ECX_RESERVED_31_16_MASK 0xFFFFu

/** ECX bits 15:0 */
#define CPUID_1D_01_ECX_MAX_ROWS_SHIFT 0u
#define CPUID_1D_01_ECX_MAX_ROWS_MASK 0xFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1D_01_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_1D_01_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 1EH, subhoja 00H -- TMUL       Information Main Leaf ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1E_00_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_1E_00_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:24 -- RESERVADO, no se toca */
#define CPUID_1E_00_EBX_RESERVED_31_24_SHIFT 24u
#define CPUID_1E_00_EBX_RESERVED_31_24_MASK 0xFFu

/** EBX bits 23:8 */
#define CPUID_1E_00_EBX_TMUL_MAXN_SHIFT 8u
#define CPUID_1E_00_EBX_TMUL_MAXN_MASK 0xFFFFu

/** EBX bits 7:0 */
#define CPUID_1E_00_EBX_TMUL_MAXK_SHIFT 0u
#define CPUID_1E_00_EBX_TMUL_MAXK_MASK 0xFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1E_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_1E_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1E_00_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_1E_00_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_AMX_H */
