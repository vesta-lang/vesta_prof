/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_rdt.h
 * @brief Reparto y vigilancia de recursos compartidos
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

#ifndef VXP_COMMON_CPUID_RDT_H
#define VXP_COMMON_CPUID_RDT_H

/* ---- hoja 0FH, subhoja 00H -- Intel(R) RDT Monitoring Main Sub-Leaf ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_0F_00_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_0F_00_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_0F_00_EBX_MAX_RMID_SHIFT 0u
#define CPUID_0F_00_EBX_MAX_RMID_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_0F_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_0F_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:2 -- RESERVADO, no se toca */
#define CPUID_0F_00_EDX_RESERVED_31_2_SHIFT 2u
#define CPUID_0F_00_EDX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** EDX bit 1 */
#define CPUID_0F_00_EDX_L3_MON 1u

/** EDX bit 0 -- RESERVADO, no se toca */
#define CPUID_0F_00_EDX_RESERVED_0_0 0u

/* ---- hoja 0FH, subhoja 01H -- L3 Cache Intel(R) Resource Director Technology Monitoring ---- */

/** EAX bits 31:11 -- RESERVADO, no se toca */
#define CPUID_0F_01_EAX_RESERVED_31_11_SHIFT 11u
#define CPUID_0F_01_EAX_RESERVED_31_11_MASK 0x1FFFFFu

/** EAX bit 10 */
#define CPUID_0F_01_EAX_IO_RDT_MBM 10u

/** EAX bit 9 */
#define CPUID_0F_01_EAX_IO_RDT_CMT 9u

/** EAX bit 8 */
#define CPUID_0F_01_EAX_RDT_M_OVF 8u

/** EAX bits 7:0 */
#define CPUID_0F_01_EAX_CTR_WIDTH_SHIFT 0u
#define CPUID_0F_01_EAX_CTR_WIDTH_MASK 0xFFu

/** EBX bits 31:0 */
#define CPUID_0F_01_EBX_CONV_FACTOR_SHIFT 0u
#define CPUID_0F_01_EBX_CONV_FACTOR_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_0F_01_ECX_MAX_RMID_L3_SHIFT 0u
#define CPUID_0F_01_ECX_MAX_RMID_L3_MASK 0xFFFFFFFFu

/** EDX bits 31:3 -- RESERVADO, no se toca */
#define CPUID_0F_01_EDX_RESERVED_31_3_SHIFT 3u
#define CPUID_0F_01_EDX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** EDX bit 2 */
#define CPUID_0F_01_EDX_MBM_L3_LOCAL 2u

/** EDX bit 1 */
#define CPUID_0F_01_EDX_MBM_L3_TOTAL 1u

/** EDX bit 0 */
#define CPUID_0F_01_EDX_CMT_L3_OCCUP 0u

/* ---- hoja 10H, subhoja 00H -- Intel(R) RDT Allocation Main Sub-Leaf ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_00_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_00_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:7 -- RESERVADO, no se toca */
#define CPUID_10_00_EBX_RESERVED_31_7_SHIFT 7u
#define CPUID_10_00_EBX_RESERVED_31_7_MASK 0x1FFFFFFu

/** EBX bit 6 */
#define CPUID_10_00_EBX_RESOURCE_PRIORITY 6u

/** EBX bit 5 */
#define CPUID_10_00_EBX_CBA 5u

/** EBX bit 4 -- RESERVADO, no se toca */
#define CPUID_10_00_EBX_RESERVED_4_4 4u

/** EBX bit 3 */
#define CPUID_10_00_EBX_MBA 3u

/** EBX bit 2 */
#define CPUID_10_00_EBX_CAT_L2 2u

/** EBX bit 1 */
#define CPUID_10_00_EBX_CAT_L3 1u

/** EBX bit 0 -- RESERVADO, no se toca */
#define CPUID_10_00_EBX_RESERVED_0_0 0u

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_00_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_00_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 10H, subhoja 01H -- L3 Cache Allocation Technology ---- */

/** EAX bits 31:5 -- RESERVADO, no se toca */
#define CPUID_10_01_EAX_RESERVED_31_5_SHIFT 5u
#define CPUID_10_01_EAX_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX bits 4:0 */
#define CPUID_10_01_EAX_CAT_L3_BITMASK_LENGTH_SHIFT 0u
#define CPUID_10_01_EAX_CAT_L3_BITMASK_LENGTH_MASK 0x1Fu

/** EBX bits 31:0 */
#define CPUID_10_01_EBX_CAT_L3_CONTENTION_SHIFT 0u
#define CPUID_10_01_EBX_CAT_L3_CONTENTION_MASK 0xFFFFFFFFu

/** ECX bits 31:4 -- RESERVADO, no se toca */
#define CPUID_10_01_ECX_RESERVED_31_4_SHIFT 4u
#define CPUID_10_01_ECX_RESERVED_31_4_MASK 0xFFFFFFFu

/** ECX bit 3 */
#define CPUID_10_01_ECX_CAT_L3_NONCONTIG 3u

/** ECX bit 2 */
#define CPUID_10_01_ECX_CAT_L3_CDP 2u

/** ECX bit 1 */
#define CPUID_10_01_ECX_CAT_L3_NONCPU 1u

/** ECX bit 0 -- RESERVADO, no se toca */
#define CPUID_10_01_ECX_RESERVED_0_0 0u

/** EDX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_10_01_EDX_RESERVED_31_16_SHIFT 16u
#define CPUID_10_01_EDX_RESERVED_31_16_MASK 0xFFFFu

/** EDX bits 15:0 */
#define CPUID_10_01_EDX_CAT_L3_MAX_CLOS_SHIFT 0u
#define CPUID_10_01_EDX_CAT_L3_MAX_CLOS_MASK 0xFFFFu

/* ---- hoja 10H, subhoja 02H -- L2 Cache Allocation Technology ---- */

/** EAX bits 31:5 -- RESERVADO, no se toca */
#define CPUID_10_02_EAX_RESERVED_31_5_SHIFT 5u
#define CPUID_10_02_EAX_RESERVED_31_5_MASK 0x7FFFFFFu

/** EAX bits 4:0 */
#define CPUID_10_02_EAX_CAT_L2_BITMASK_LENGTH_SHIFT 0u
#define CPUID_10_02_EAX_CAT_L2_BITMASK_LENGTH_MASK 0x1Fu

/** EBX bits 31:0 */
#define CPUID_10_02_EBX_CAT_L2_CONTENTION_SHIFT 0u
#define CPUID_10_02_EBX_CAT_L2_CONTENTION_MASK 0xFFFFFFFFu

/** ECX bits 31:4 -- RESERVADO, no se toca */
#define CPUID_10_02_ECX_RESERVED_31_4_SHIFT 4u
#define CPUID_10_02_ECX_RESERVED_31_4_MASK 0xFFFFFFFu

/** ECX bit 3 */
#define CPUID_10_02_ECX_CAT_L2_NONCONTIG 3u

/** ECX bit 2 */
#define CPUID_10_02_ECX_CAT_L2_CDP 2u

/** ECX bits 1:0 -- RESERVADO, no se toca */
#define CPUID_10_02_ECX_RESERVED_1_0_SHIFT 0u
#define CPUID_10_02_ECX_RESERVED_1_0_MASK 0x3u

/** EDX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_10_02_EDX_RESERVED_31_16_SHIFT 16u
#define CPUID_10_02_EDX_RESERVED_31_16_MASK 0xFFFFu

/** EDX bits 15:0 */
#define CPUID_10_02_EDX_CAT_L2_MAX_CLOS_SHIFT 0u
#define CPUID_10_02_EDX_CAT_L2_MAX_CLOS_MASK 0xFFFFu

/* ---- hoja 10H, subhoja 03H -- Memory Bandwidth Allocation ---- */

/** EAX bits 31:12 -- RESERVADO, no se toca */
#define CPUID_10_03_EAX_RESERVED_31_12_SHIFT 12u
#define CPUID_10_03_EAX_RESERVED_31_12_MASK 0xFFFFFu

/** EAX bits 11:0 */
#define CPUID_10_03_EAX_MBA_MAX_SHIFT 0u
#define CPUID_10_03_EAX_MBA_MAX_MASK 0xFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_03_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_03_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:3 -- RESERVADO, no se toca */
#define CPUID_10_03_ECX_RESERVED_31_3_SHIFT 3u
#define CPUID_10_03_ECX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** ECX bit 2 */
#define CPUID_10_03_ECX_MBA_LINEAR 2u

/** ECX bit 1 -- RESERVADO, no se toca */
#define CPUID_10_03_ECX_RESERVED_1_1 1u

/** ECX bit 0 */
#define CPUID_10_03_ECX_PER_THREAD_MBA 0u

/** EDX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_10_03_EDX_RESERVED_31_16_SHIFT 16u
#define CPUID_10_03_EDX_RESERVED_31_16_MASK 0xFFFFu

/** EDX bits 15:0 */
#define CPUID_10_03_EDX_MBA_MAX_CLOS_SHIFT 0u
#define CPUID_10_03_EDX_MBA_MAX_CLOS_MASK 0xFFFFu

/* ---- hoja 10H, subhoja 05H -- Cache Bandwidth Allocation ---- */

/** EAX bits 31:12 -- RESERVADO, no se toca */
#define CPUID_10_05_EAX_RESERVED_31_12_SHIFT 12u
#define CPUID_10_05_EAX_RESERVED_31_12_MASK 0xFFFFFu

/** EAX bits 11:8 */
#define CPUID_10_05_EAX_BW_SCOPE_SHIFT 8u
#define CPUID_10_05_EAX_BW_SCOPE_MASK 0xFu

/** EAX bits 7:0 */
#define CPUID_10_05_EAX_CBA_MAX_LEVELS_SHIFT 0u
#define CPUID_10_05_EAX_CBA_MAX_LEVELS_MASK 0xFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_05_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_05_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:4 -- RESERVADO, no se toca */
#define CPUID_10_05_ECX_RESERVED_31_4_SHIFT 4u
#define CPUID_10_05_ECX_RESERVED_31_4_MASK 0xFFFFFFFu

/** ECX bit 3 */
#define CPUID_10_05_ECX_CBA_LINEAR 3u

/** ECX bits 2:0 -- RESERVADO, no se toca */
#define CPUID_10_05_ECX_RESERVED_2_0_SHIFT 0u
#define CPUID_10_05_ECX_RESERVED_2_0_MASK 0x7u

/** EDX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_10_05_EDX_RESERVED_31_16_SHIFT 16u
#define CPUID_10_05_EDX_RESERVED_31_16_MASK 0xFFFFu

/** EDX bits 15:0 */
#define CPUID_10_05_EDX_CBA_MAX_CLOS_SHIFT 0u
#define CPUID_10_05_EDX_CBA_MAX_CLOS_MASK 0xFFFFu

/* ---- hoja 10H, subhoja 06H -- Resource Priority Control ---- */

/** EAX bits 31:2 -- RESERVADO, no se toca */
#define CPUID_10_06_EAX_RESERVED_31_2_SHIFT 2u
#define CPUID_10_06_EAX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** EAX bit 1 */
#define CPUID_10_06_EAX_PACKAGE_ENABLE 1u

/** EAX bit 0 */
#define CPUID_10_06_EAX_THREAD_ENABLE 0u

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_06_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_06_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_06_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_06_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_10_06_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_10_06_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_RDT_H */
