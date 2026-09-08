/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_extended.h
 * @brief Rango extendido (8000_0000H)
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

#ifndef VXP_COMMON_CPUID_EXTENDED_H
#define VXP_COMMON_CPUID_EXTENDED_H

/* ---- hoja 80000000H -- Maximum Input Value for Extended Function CPUID ---- */

/** EAX bits 31:0 */
#define CPUID_80000000_EAX_MAX_EXTENDED_LEAF_SHIFT 0u
#define CPUID_80000000_EAX_MAX_EXTENDED_LEAF_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000000_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000000_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000000_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000000_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000000_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000000_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 80000001H -- Extended Processor Signature and Feature Bits ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000001_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000001_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000001_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000001_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:9 -- RESERVADO, no se toca */
#define CPUID_80000001_ECX_RESERVED_31_9_SHIFT 9u
#define CPUID_80000001_ECX_RESERVED_31_9_MASK 0x7FFFFFu

/** ECX bit 8 */
#define CPUID_80000001_ECX_PREFETCHW 8u

/** ECX bits 7:6 -- RESERVADO, no se toca */
#define CPUID_80000001_ECX_RESERVED_7_6_SHIFT 6u
#define CPUID_80000001_ECX_RESERVED_7_6_MASK 0x3u

/** ECX bit 5 */
#define CPUID_80000001_ECX_LZCNT 5u

/** ECX bits 4:1 -- RESERVADO, no se toca */
#define CPUID_80000001_ECX_RESERVED_4_1_SHIFT 1u
#define CPUID_80000001_ECX_RESERVED_4_1_MASK 0xFu

/** ECX bit 0 */
#define CPUID_80000001_ECX_LAHF_SAHF_64 0u

/** EDX bits 31:30 -- RESERVADO, no se toca */
#define CPUID_80000001_EDX_RESERVED_31_30_SHIFT 30u
#define CPUID_80000001_EDX_RESERVED_31_30_MASK 0x3u

/** EDX bit 29 */
#define CPUID_80000001_EDX_INTEL64 29u

/** EDX bit 28 -- RESERVADO, no se toca */
#define CPUID_80000001_EDX_RESERVED_28_28 28u

/** EDX bit 27 */
#define CPUID_80000001_EDX_RDTSCP 27u

/** EDX bit 26 */
#define CPUID_80000001_EDX_PAGE_1GB 26u

/** EDX bits 25:21 -- RESERVADO, no se toca */
#define CPUID_80000001_EDX_RESERVED_25_21_SHIFT 21u
#define CPUID_80000001_EDX_RESERVED_25_21_MASK 0x1Fu

/** EDX bit 20 */
#define CPUID_80000001_EDX_EXECUTE_DIS 20u

/** EDX bits 19:12 -- RESERVADO, no se toca */
#define CPUID_80000001_EDX_RESERVED_19_12_SHIFT 12u
#define CPUID_80000001_EDX_RESERVED_19_12_MASK 0xFFu

/** EDX bit 11 */
#define CPUID_80000001_EDX_SYSCALL_SYSRET_64 11u

/** EDX bits 10:0 -- RESERVADO, no se toca */
#define CPUID_80000001_EDX_RESERVED_10_0_SHIFT 0u
#define CPUID_80000001_EDX_RESERVED_10_0_MASK 0x7FFu

/* ---- hoja 80000002H -- Processor Brand String (Bytes 0 to 15) ---- */

/** EAX bits 31:0 */
#define CPUID_80000002_EAX_BRAND_NAME_0_SHIFT 0u
#define CPUID_80000002_EAX_BRAND_NAME_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_80000002_EBX_BRAND_NAME_1_SHIFT 0u
#define CPUID_80000002_EBX_BRAND_NAME_1_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_80000002_ECX_BRAND_NAME_2_SHIFT 0u
#define CPUID_80000002_ECX_BRAND_NAME_2_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_80000002_EDX_BRAND_NAME_3_SHIFT 0u
#define CPUID_80000002_EDX_BRAND_NAME_3_MASK 0xFFFFFFFFu

/* ---- hoja 80000003H -- Processor brand string (Bytes 16 to 31) ---- */

/** EAX bits 31:0 */
#define CPUID_80000003_EAX_BRAND_NAME_4_SHIFT 0u
#define CPUID_80000003_EAX_BRAND_NAME_4_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_80000003_EBX_BRAND_NAME_5_SHIFT 0u
#define CPUID_80000003_EBX_BRAND_NAME_5_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_80000003_ECX_BRAND_NAME_6_SHIFT 0u
#define CPUID_80000003_ECX_BRAND_NAME_6_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_80000003_EDX_BRAND_NAME_7_SHIFT 0u
#define CPUID_80000003_EDX_BRAND_NAME_7_MASK 0xFFFFFFFFu

/* ---- hoja 80000004H -- Processor brand string (Bytes 32 to 47) ---- */

/** EAX bits 31:0 */
#define CPUID_80000004_EAX_BRAND_NAME_8_SHIFT 0u
#define CPUID_80000004_EAX_BRAND_NAME_8_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_80000004_EBX_BRAND_NAME_9_SHIFT 0u
#define CPUID_80000004_EBX_BRAND_NAME_9_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_80000004_ECX_BRAND_NAME_10_SHIFT 0u
#define CPUID_80000004_ECX_BRAND_NAME_10_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_80000004_EDX_BRAND_NAME_11_SHIFT 0u
#define CPUID_80000004_EDX_BRAND_NAME_11_MASK 0xFFFFFFFFu

/* ---- hoja 80000005H -- Reserved ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000005_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000005_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000005_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000005_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000005_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000005_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000005_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000005_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/* ---- hoja 80000006H -- Extended Function CPUID Information ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000006_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000006_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000006_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000006_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:16 */
#define CPUID_80000006_ECX_L2_SIZE_SHIFT 16u
#define CPUID_80000006_ECX_L2_SIZE_MASK 0xFFFFu

/** ECX bits 15:12 */
#define CPUID_80000006_ECX_L2_ASSOC_SHIFT 12u
#define CPUID_80000006_ECX_L2_ASSOC_MASK 0xFu

/** ECX bits 11:8 -- RESERVADO, no se toca */
#define CPUID_80000006_ECX_RESERVED_11_8_SHIFT 8u
#define CPUID_80000006_ECX_RESERVED_11_8_MASK 0xFu

/** ECX bits 7:0 */
#define CPUID_80000006_ECX_L2_LINE_SIZE_SHIFT 0u
#define CPUID_80000006_ECX_L2_LINE_SIZE_MASK 0xFFu

/** EDX bits 31:8 -- RESERVADO, no se toca */
#define CPUID_80000006_EDX_RESERVED_31_8_SHIFT 8u
#define CPUID_80000006_EDX_RESERVED_31_8_MASK 0xFFFFFFu

/** EDX bits 7:0 -- RESERVADO, no se toca */
#define CPUID_80000006_EDX_RESERVED_7_0_SHIFT 0u
#define CPUID_80000006_EDX_RESERVED_7_0_MASK 0xFFu

/* ---- hoja 80000007H -- Extended Function CPUID Information 1 ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000007_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000007_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000007_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000007_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000007_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000007_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:9 -- RESERVADO, no se toca */
#define CPUID_80000007_EDX_RESERVED_31_9_SHIFT 9u
#define CPUID_80000007_EDX_RESERVED_31_9_MASK 0x7FFFFFu

/** EDX bit 8 */
#define CPUID_80000007_EDX_TSC_INVARIANT 8u

/** EDX bits 7:0 -- RESERVADO, no se toca */
#define CPUID_80000007_EDX_RESERVED_7_0_SHIFT 0u
#define CPUID_80000007_EDX_RESERVED_7_0_MASK 0xFFu

/* ---- hoja 80000008H -- Extended Function CPUID Information 2 ---- */

/** EAX bits 31:24 -- RESERVADO, no se toca */
#define CPUID_80000008_EAX_RESERVED_31_24_SHIFT 24u
#define CPUID_80000008_EAX_RESERVED_31_24_MASK 0xFFu

/** EAX bits 23:16 */
#define CPUID_80000008_EAX_GUEST_PHYS_ADDR_SIZE_SHIFT 16u
#define CPUID_80000008_EAX_GUEST_PHYS_ADDR_SIZE_MASK 0xFFu

/** EAX bits 15:8 */
#define CPUID_80000008_EAX_LIN_ADDR_SIZE_SHIFT 8u
#define CPUID_80000008_EAX_LIN_ADDR_SIZE_MASK 0xFFu

/** EAX bits 7:0 */
#define CPUID_80000008_EAX_PHYS_ADDR_SIZE_SHIFT 0u
#define CPUID_80000008_EAX_PHYS_ADDR_SIZE_MASK 0xFFu

/** EBX bits 31:10 -- RESERVADO, no se toca */
#define CPUID_80000008_EBX_RESERVED_31_10_SHIFT 10u
#define CPUID_80000008_EBX_RESERVED_31_10_MASK 0x3FFFFFu

/** EBX bit 9 */
#define CPUID_80000008_EBX_WBNOINVD 9u

/** EBX bits 8:0 -- RESERVADO, no se toca */
#define CPUID_80000008_EBX_RESERVED_8_0_SHIFT 0u
#define CPUID_80000008_EBX_RESERVED_8_0_MASK 0x1FFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000008_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000008_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_80000008_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_80000008_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/*
 * Filas del manual que REMITEN A OTRA TABLA en vez de
 * definir un campo.  Se listan para que no parezca que
 * esos registros no estan documentados.
 *
 *   hoja 80000008H EAX[31:5]  -> by...
 *   hoja 80000008H ECX[15:0]  -> holds...
 */

#endif /* VXP_COMMON_CPUID_EXTENDED_H */
