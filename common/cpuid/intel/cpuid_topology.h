/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_topology.h
 * @brief Topologia: caches, TLB, hilos hermanos y clase de nucleo
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

#ifndef VXP_COMMON_CPUID_TOPOLOGY_H
#define VXP_COMMON_CPUID_TOPOLOGY_H

/* ---- hoja 04H -- Deterministic Cache Parameters ---- */

/** EAX bits 31:26 */
#define CPUID_04_EAX_MAX_CORES_ADDRESSABLE_IDS_PKG_SHIFT 26u
#define CPUID_04_EAX_MAX_CORES_ADDRESSABLE_IDS_PKG_MASK 0x3Fu

/** EAX bits 25:14 */
#define CPUID_04_EAX_MAX_LP_ADDRESSABLE_IDS_SHIFT 14u
#define CPUID_04_EAX_MAX_LP_ADDRESSABLE_IDS_MASK 0xFFFu

/** EAX bits 13:10 -- RESERVADO, no se toca */
#define CPUID_04_EAX_RESERVED_13_10_SHIFT 10u
#define CPUID_04_EAX_RESERVED_13_10_MASK 0xFu

/** EAX bit 9 */
#define CPUID_04_EAX_FULLY_ASSOC 9u

/** EAX bit 8 */
#define CPUID_04_EAX_SELF_INITIALIZING_CACHE 8u

/** EAX bits 7:5 */
#define CPUID_04_EAX_CACHE_LEVEL_SHIFT 5u
#define CPUID_04_EAX_CACHE_LEVEL_MASK 0x7u

/** EAX bits 4:0 */
#define CPUID_04_EAX_CACHE_TYPE_SHIFT 0u
#define CPUID_04_EAX_CACHE_TYPE_MASK 0x1Fu

/** EBX bits 31:22 */
#define CPUID_04_EBX_NUM_WAYS_SHIFT 22u
#define CPUID_04_EBX_NUM_WAYS_MASK 0x3FFu

/** EBX bits 21:12 */
#define CPUID_04_EBX_PHYS_LINE_PARTITIONS_SHIFT 12u
#define CPUID_04_EBX_PHYS_LINE_PARTITIONS_MASK 0x3FFu

/** EBX bits 11:0 */
#define CPUID_04_EBX_LINE_SIZE_SHIFT 0u
#define CPUID_04_EBX_LINE_SIZE_MASK 0xFFFu

/** ECX bits 31:0 */
#define CPUID_04_ECX_NUM_SETS_SHIFT 0u
#define CPUID_04_ECX_NUM_SETS_MASK 0xFFFFFFFFu

/** EDX bits 31:3 -- RESERVADO, no se toca */
#define CPUID_04_EDX_RESERVED_31_3_SHIFT 3u
#define CPUID_04_EDX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** EDX bit 2 */
#define CPUID_04_EDX_COMPLEX_CACHE_INDEXING 2u

/** EDX bit 1 */
#define CPUID_04_EDX_INCLUSIVE_CACHE 1u

/** EDX bit 0 */
#define CPUID_04_EDX_NOT_LWR_CACHE_FLUSH 0u

/* ---- hoja 0BH -- ECX >= 0 ---- */

/** EAX bits 31:5 -- RESERVADO, no se toca */
#define CPUID_0B_EAX_RESERVED_31_5_SHIFT 5u
#define CPUID_0B_EAX_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX bits 4:0 */
#define CPUID_0B_EAX_SHIFT_COUNT_SHIFT 0u
#define CPUID_0B_EAX_SHIFT_COUNT_MASK 0x1Fu

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_0B_EBX_RESERVED_31_16_SHIFT 16u
#define CPUID_0B_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:0 */
#define CPUID_0B_EBX_NEXT_LEVEL_NUM_LP_SHIFT 0u
#define CPUID_0B_EBX_NEXT_LEVEL_NUM_LP_MASK 0xFFFFu

/** ECX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_0B_ECX_RESERVED_31_16_SHIFT 16u
#define CPUID_0B_ECX_RESERVED_31_16_MASK 0xFFFFu

/** ECX bits 15:8 */
#define CPUID_0B_ECX_DOMAIN_TYPE_SHIFT 8u
#define CPUID_0B_ECX_DOMAIN_TYPE_MASK 0xFFu

/** ECX bits 7:0 */
#define CPUID_0B_ECX_LEVEL_NUM_SHIFT 0u
#define CPUID_0B_ECX_LEVEL_NUM_MASK 0xFFu

/** EDX bits 31:0 */
#define CPUID_0B_EDX_X2APIC_ID_SHIFT 0u
#define CPUID_0B_EDX_X2APIC_ID_MASK 0xFFFFFFFFu

/* ---- hoja 18H, subhoja 00H -- Main Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_18_00_EAX_MAX_SUBLEAF_SHIFT 0u
#define CPUID_18_00_EAX_MAX_SUBLEAF_MASK 0xFFFFFFFFu

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_18_00_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_18_00_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:16 */
#define CPUID_18_00_EBX_NUM_WAYS_SHIFT 16u
#define CPUID_18_00_EBX_NUM_WAYS_MASK 0xFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_18_00_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_18_00_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 15:11 -- RESERVADO, no se toca */
#define CPUID_18_00_EBX_RESERVED_15_11_SHIFT 11u
#define CPUID_18_00_EBX_RESERVED_15_11_MASK 0x1Fu

/** EBX bits 10:8 */
#define CPUID_18_00_EBX_PARTITIONING_SHIFT 8u
#define CPUID_18_00_EBX_PARTITIONING_MASK 0x7u

/** EBX bits 7:4 -- RESERVADO, no se toca */
#define CPUID_18_00_EBX_RESERVED_7_4_SHIFT 4u
#define CPUID_18_00_EBX_RESERVED_7_4_MASK 0xFu

/** ECX bits 31:0 */
#define CPUID_18_00_ECX_NUM_SETS_SHIFT 0u
#define CPUID_18_00_ECX_NUM_SETS_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_18_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_18_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:26 -- RESERVADO, no se toca */
#define CPUID_18_00_EDX_RESERVED_31_26_SHIFT 26u
#define CPUID_18_00_EDX_RESERVED_31_26_MASK 0x3Fu

/** EDX bits 31:5 -- RESERVADO, no se toca */
#define CPUID_18_00_EDX_RESERVED_31_5_SHIFT 5u
#define CPUID_18_00_EDX_RESERVED_31_5_MASK 0x7FFFFFFu

/** EDX bits 25:14 */
#define CPUID_18_00_EDX_MAX_LP_ADDRESSABLE_IDS_SHIFT 14u
#define CPUID_18_00_EDX_MAX_LP_ADDRESSABLE_IDS_MASK 0xFFFu

/** EDX bits 13:9 -- RESERVADO, no se toca */
#define CPUID_18_00_EDX_RESERVED_13_9_SHIFT 9u
#define CPUID_18_00_EDX_RESERVED_13_9_MASK 0x1Fu

/** EDX bit 8 */
#define CPUID_18_00_EDX_FULLY_ASSOC 8u

/** EDX bits 7:5 */
#define CPUID_18_00_EDX_LEVEL_NUM_SHIFT 5u
#define CPUID_18_00_EDX_LEVEL_NUM_MASK 0x7u

/** EDX bits 4:0 */
#define CPUID_18_00_EDX_TYPE_SHIFT 0u
#define CPUID_18_00_EDX_TYPE_MASK 0x1Fu

/* ---- hoja 1AH -- Native Model ID Enumeration ---- */

/** EAX bits 31:24 */
#define CPUID_1A_EAX_CORE_TYPE_SHIFT 24u
#define CPUID_1A_EAX_CORE_TYPE_MASK 0xFFu

/** EAX bits 23:0 */
#define CPUID_1A_EAX_CORE_NATIVE_MODEL_ID_SHIFT 0u
#define CPUID_1A_EAX_CORE_NATIVE_MODEL_ID_MASK 0xFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1A_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_1A_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1A_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_1A_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1A_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_1A_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 1FH -- ECX >= 0 ---- */

/** EAX bits 31:5 -- RESERVADO, no se toca */
#define CPUID_1F_EAX_RESERVED_31_5_SHIFT 5u
#define CPUID_1F_EAX_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX bits 4:0 */
#define CPUID_1F_EAX_SHIFT_COUNT_SHIFT 0u
#define CPUID_1F_EAX_SHIFT_COUNT_MASK 0x1Fu

/** EBX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_1F_EBX_RESERVED_31_16_SHIFT 16u
#define CPUID_1F_EBX_RESERVED_31_16_MASK 0xFFFFu

/** EBX bits 15:0 */
#define CPUID_1F_EBX_NEXT_LEVEL_NUM_LP_SHIFT 0u
#define CPUID_1F_EBX_NEXT_LEVEL_NUM_LP_MASK 0xFFFFu

/** ECX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_1F_ECX_RESERVED_31_16_SHIFT 16u
#define CPUID_1F_ECX_RESERVED_31_16_MASK 0xFFFFu

/** ECX bits 15:8 */
#define CPUID_1F_ECX_DOMAIN_TYPE_SHIFT 8u
#define CPUID_1F_ECX_DOMAIN_TYPE_MASK 0xFFu

/** ECX bits 7:0 */
#define CPUID_1F_ECX_LEVEL_NUM_SHIFT 0u
#define CPUID_1F_ECX_LEVEL_NUM_MASK 0xFFu

/** EDX bits 31:0 */
#define CPUID_1F_EDX_X2APIC_ID_SHIFT 0u
#define CPUID_1F_EDX_X2APIC_ID_MASK 0xFFFFFFFFu

/*
 * Filas del manual que REMITEN A OTRA TABLA en vez de
 * definir un campo.  Se listan para que no parezca que
 * esos registros no estan documentados.
 *
 *   hoja 0BH       EBX[15:0]  -> as...
 *   hoja 18H.00H   EBX[0:0]  -> 4KB_ENTRIES...
 *   hoja 18H.00H   EBX[1:1]  -> 2MB_ENTRIES...
 *   hoja 18H.00H   EBX[2:2]  -> 4MB_ENTRIES...
 *   hoja 18H.00H   EBX[3:3]  -> 1GB_ENTRIES...
 *   hoja 1FH       EBX[15:0]  -> as...
 */

#endif /* VXP_COMMON_CPUID_TOPOLOGY_H */
