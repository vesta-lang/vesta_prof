/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_security.h
 * @brief Enclaves, cerrojo de claves y configuracion de plataforma
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

#ifndef VXP_COMMON_CPUID_SECURITY_H
#define VXP_COMMON_CPUID_SECURITY_H

/* ---- hoja 12H -- Sub-Leaf Encoding Type EAX[3:0] = 0001b ---- */

/** EAX bits 31:12 */
#define CPUID_12_EAX_EPC_SECTION_ADDR_31_12_SHIFT 12u
#define CPUID_12_EAX_EPC_SECTION_ADDR_31_12_MASK 0xFFFFFu

/** EAX bits 11:4 -- RESERVADO, no se toca */
#define CPUID_12_EAX_RESERVED_11_4_SHIFT 4u
#define CPUID_12_EAX_RESERVED_11_4_MASK 0xFFu

/** EAX bits 3:0 */
#define CPUID_12_EAX_SUB_LEAF_TYPE_SHIFT 0u
#define CPUID_12_EAX_SUB_LEAF_TYPE_MASK 0xFu

/** EBX bits 31:20 -- RESERVADO, no se toca */
#define CPUID_12_EBX_RESERVED_31_20_SHIFT 20u
#define CPUID_12_EBX_RESERVED_31_20_MASK 0xFFFu

/** EBX bits 19:0 */
#define CPUID_12_EBX_EPC_SECTION_ADDR_51_32_SHIFT 0u
#define CPUID_12_EBX_EPC_SECTION_ADDR_51_32_MASK 0xFFFFFu

/** ECX bits 31:12 */
#define CPUID_12_ECX_EPC_SECTION_SIZE_31_12_SHIFT 12u
#define CPUID_12_ECX_EPC_SECTION_SIZE_31_12_MASK 0xFFFFFu

/** ECX bits 11:4 -- RESERVADO, no se toca */
#define CPUID_12_ECX_RESERVED_11_4_SHIFT 4u
#define CPUID_12_ECX_RESERVED_11_4_MASK 0xFFu

/** ECX bits 3:0 */
#define CPUID_12_ECX_EPC_SECTION_PROPERTY_SHIFT 0u
#define CPUID_12_ECX_EPC_SECTION_PROPERTY_MASK 0xFu

/** EDX bits 31:20 -- RESERVADO, no se toca */
#define CPUID_12_EDX_RESERVED_31_20_SHIFT 20u
#define CPUID_12_EDX_RESERVED_31_20_MASK 0xFFFu

/** EDX bits 19:0 */
#define CPUID_12_EDX_EPC_SECTION_SIZE_51_32_SHIFT 0u
#define CPUID_12_EDX_EPC_SECTION_SIZE_51_32_MASK 0xFFFFFu

/* ---- hoja 12H, subhoja 00H -- Intel(R) SGX Main Sub-Leaf ---- */

/** EAX bits 31:12 -- RESERVADO, no se toca */
#define CPUID_12_00_EAX_RESERVED_31_12_SHIFT 12u
#define CPUID_12_00_EAX_RESERVED_31_12_MASK 0xFFFFFu

/** EAX bit 11 */
#define CPUID_12_00_EAX_EDECCSSA 11u

/** EAX bit 10 */
#define CPUID_12_00_EAX_EUPDATESVN 10u

/** EAX bits 9:8 -- RESERVADO, no se toca */
#define CPUID_12_00_EAX_RESERVED_9_8_SHIFT 8u
#define CPUID_12_00_EAX_RESERVED_9_8_MASK 0x3u

/** EAX bit 7 */
#define CPUID_12_00_EAX_EVERIFYREPORT2 7u

/** EAX bits 6:2 -- RESERVADO, no se toca */
#define CPUID_12_00_EAX_RESERVED_6_2_SHIFT 2u
#define CPUID_12_00_EAX_RESERVED_6_2_MASK 0x1Fu

/** EAX bit 1 */
#define CPUID_12_00_EAX_SGX2 1u

/** EAX bit 0 */
#define CPUID_12_00_EAX_SGX1 0u

/** EBX bits 31:0 */
#define CPUID_12_00_EBX_MISCSELECT_SHIFT 0u
#define CPUID_12_00_EBX_MISCSELECT_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_12_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_12_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:16 -- RESERVADO, no se toca */
#define CPUID_12_00_EDX_RESERVED_31_16_SHIFT 16u
#define CPUID_12_00_EDX_RESERVED_31_16_MASK 0xFFFFu

/** EDX bits 15:8 */
#define CPUID_12_00_EDX_MAX_ENCLAVE_SIZE_64_SHIFT 8u
#define CPUID_12_00_EDX_MAX_ENCLAVE_SIZE_64_MASK 0xFFu

/** EDX bits 7:0 */
#define CPUID_12_00_EDX_MAX_ENCLAVE_SIZE_NOT_64_SHIFT 0u
#define CPUID_12_00_EDX_MAX_ENCLAVE_SIZE_NOT_64_MASK 0xFFu

/* ---- hoja 12H, subhoja 01H -- Intel(R) SGX Attributes ---- */

/** EAX bits 31:0 */
#define CPUID_12_01_EAX_ECREATE_SECS_ATTRIBUTES_31_0_SHIFT 0u
#define CPUID_12_01_EAX_ECREATE_SECS_ATTRIBUTES_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_12_01_EBX_ECREATE_SECS_ATTRIBUTES_63_32_SHIFT 0u
#define CPUID_12_01_EBX_ECREATE_SECS_ATTRIBUTES_63_32_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_12_01_ECX_ECREATE_SECS_ATTRIBUTES_95_64_SHIFT 0u
#define CPUID_12_01_ECX_ECREATE_SECS_ATTRIBUTES_95_64_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_12_01_EDX_ECREATE_SECS_ATTRIBUTES_127_96_SHIFT 0u
#define CPUID_12_01_EDX_ECREATE_SECS_ATTRIBUTES_127_96_MASK 0xFFFFFFFFu

/* ---- hoja 19H -- Key Locker ---- */

/** EAX bits 31:3 -- RESERVADO, no se toca */
#define CPUID_19_EAX_RESERVED_31_3_SHIFT 3u
#define CPUID_19_EAX_RESERVED_31_3_MASK 0x1FFFFFFFu

/** EAX bit 2 */
#define CPUID_19_EAX_NO_DECRYPT_RESTRICT 2u

/** EAX bit 1 */
#define CPUID_19_EAX_NO_ENCRYPT_RESTRICT 1u

/** EAX bit 0 */
#define CPUID_19_EAX_CPL0_RESTRICT 0u

/** EBX bits 31:5 -- RESERVADO, no se toca */
#define CPUID_19_EBX_RESERVED_31_5_SHIFT 5u
#define CPUID_19_EBX_RESERVED_31_5_MASK 0x7FFFFFFu

/** EBX bit 4 */
#define CPUID_19_EBX_IWKEYBACKUP 4u

/** EBX bit 3 -- RESERVADO, no se toca */
#define CPUID_19_EBX_RESERVED_3_3 3u

/** EBX bit 2 */
#define CPUID_19_EBX_AES_WIDE 2u

/** EBX bit 1 -- RESERVADO, no se toca */
#define CPUID_19_EBX_RESERVED_1_1 1u

/** EBX bit 0 */
#define CPUID_19_EBX_AESKLE 0u

/** ECX bits 31:2 -- RESERVADO, no se toca */
#define CPUID_19_ECX_RESERVED_31_2_SHIFT 2u
#define CPUID_19_ECX_RESERVED_31_2_MASK 0x3FFFFFFFu

/** ECX bit 1 */
#define CPUID_19_ECX_RAND_IWKEY 1u

/** ECX bit 0 */
#define CPUID_19_ECX_NOBACKUP 0u

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_19_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_19_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 1BH -- Output Registers Format for All Sub-Leaves ---- */

/** EAX bits 31:12 -- RESERVADO, no se toca */
#define CPUID_1B_EAX_RESERVED_31_12_SHIFT 12u
#define CPUID_1B_EAX_RESERVED_31_12_MASK 0xFFFFFu

/** EAX bits 11:0 */
#define CPUID_1B_EAX_SUB_LEAF_TYPE_SHIFT 0u
#define CPUID_1B_EAX_SUB_LEAF_TYPE_MASK 0xFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1B_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_1B_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_1B_EBX_TARGET_IDENTIFIER_1_SHIFT 0u
#define CPUID_1B_EBX_TARGET_IDENTIFIER_1_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1B_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_1B_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_1B_ECX_TARGET_IDENTIFIER_2_SHIFT 0u
#define CPUID_1B_ECX_TARGET_IDENTIFIER_2_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_1B_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_1B_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_1B_EDX_TARGET_IDENTIFIER_3_SHIFT 0u
#define CPUID_1B_EDX_TARGET_IDENTIFIER_3_MASK 0xFFFFFFFFu

/* ---- hoja 20H, subhoja 00H -- Processor History Reset Sub-leaf ---- */

/** EAX bits 31:0 */
#define CPUID_20_00_EAX_MAX_SUBLEAF_SHIFT 0u
#define CPUID_20_00_EAX_MAX_SUBLEAF_MASK 0xFFFFFFFFu

/** EBX bits 31:1 -- RESERVADO, no se toca */
#define CPUID_20_00_EBX_RESERVED_31_1_SHIFT 1u
#define CPUID_20_00_EBX_RESERVED_31_1_MASK 0x7FFFFFFFu

/** EBX bit 0 */
#define CPUID_20_00_EBX_THREAD_DIRECTOR_HRESET 0u

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_20_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_20_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_20_00_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_20_00_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 24H, subhoja 00H -- Converged Vector ISA Main Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_24_00_EAX_MAX_SUBLEAF_SHIFT 0u
#define CPUID_24_00_EAX_MAX_SUBLEAF_MASK 0xFFFFFFFFu

/** EBX bits 31:19 -- RESERVADO, no se toca */
#define CPUID_24_00_EBX_RESERVED_31_19_SHIFT 19u
#define CPUID_24_00_EBX_RESERVED_31_19_MASK 0x1FFFu

/** EBX bits 18:16 -- RESERVADO, no se toca */
#define CPUID_24_00_EBX_RESERVED_18_16_SHIFT 16u
#define CPUID_24_00_EBX_RESERVED_18_16_MASK 0x7u

/** EBX bits 15:8 -- RESERVADO, no se toca */
#define CPUID_24_00_EBX_RESERVED_15_8_SHIFT 8u
#define CPUID_24_00_EBX_RESERVED_15_8_MASK 0xFFu

/** EBX bits 7:0 */
#define CPUID_24_00_EBX_VECTOR_ISA_VERSION_SHIFT 0u
#define CPUID_24_00_EBX_VECTOR_ISA_VERSION_MASK 0xFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_24_00_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_24_00_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_24_00_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_24_00_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_SECURITY_H */
