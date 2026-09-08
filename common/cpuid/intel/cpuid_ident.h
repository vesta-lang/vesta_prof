/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpuid_ident.h
 * @brief Identificacion: fabricante, marca y descriptores
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

#ifndef VXP_COMMON_CPUID_IDENT_H
#define VXP_COMMON_CPUID_IDENT_H

/* ---- hoja 00H -- Maximum Input for Basic CPUID and Vendor ID ---- */

/** EAX bits 31:0 */
#define CPUID_00_EAX_MAX_LEAF_SHIFT 0u
#define CPUID_00_EAX_MAX_LEAF_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_00_EBX_VENDOR_ID_1_SHIFT 0u
#define CPUID_00_EBX_VENDOR_ID_1_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_00_ECX_VENDOR_ID_2_SHIFT 0u
#define CPUID_00_ECX_VENDOR_ID_2_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_00_EDX_VENDOR_ID_3_SHIFT 0u
#define CPUID_00_EDX_VENDOR_ID_3_MASK 0xFFFFFFFFu

/* ---- hoja 02H -- TLB/Cache/Prefetch Information ---- */

/** EAX bits 31:24 */
#define CPUID_02_EAX_DESCRIPTOR_3_SHIFT 24u
#define CPUID_02_EAX_DESCRIPTOR_3_MASK 0xFFu

/** EAX bits 23:16 */
#define CPUID_02_EAX_DESCRIPTOR_2_SHIFT 16u
#define CPUID_02_EAX_DESCRIPTOR_2_MASK 0xFFu

/** EAX bits 15:8 */
#define CPUID_02_EAX_DESCRIPTOR_1_SHIFT 8u
#define CPUID_02_EAX_DESCRIPTOR_1_MASK 0xFFu

/** EAX bits 7:0 -- RESERVADO, no se toca */
#define CPUID_02_EAX_RESERVED_7_0_SHIFT 0u
#define CPUID_02_EAX_RESERVED_7_0_MASK 0xFFu

/** EBX bits 31:24 */
#define CPUID_02_EBX_DESCRIPTOR_7_SHIFT 24u
#define CPUID_02_EBX_DESCRIPTOR_7_MASK 0xFFu

/** EBX bits 23:16 */
#define CPUID_02_EBX_DESCRIPTOR_6_SHIFT 16u
#define CPUID_02_EBX_DESCRIPTOR_6_MASK 0xFFu

/** EBX bits 15:8 */
#define CPUID_02_EBX_DESCRIPTOR_5_SHIFT 8u
#define CPUID_02_EBX_DESCRIPTOR_5_MASK 0xFFu

/** EBX bits 7:0 */
#define CPUID_02_EBX_DESCRIPTOR_4_SHIFT 0u
#define CPUID_02_EBX_DESCRIPTOR_4_MASK 0xFFu

/** ECX bits 31:24 */
#define CPUID_02_ECX_DESCRIPTOR_11_SHIFT 24u
#define CPUID_02_ECX_DESCRIPTOR_11_MASK 0xFFu

/** ECX bits 23:16 */
#define CPUID_02_ECX_DESCRIPTOR_10_SHIFT 16u
#define CPUID_02_ECX_DESCRIPTOR_10_MASK 0xFFu

/** ECX bits 15:8 */
#define CPUID_02_ECX_DESCRIPTOR_9_SHIFT 8u
#define CPUID_02_ECX_DESCRIPTOR_9_MASK 0xFFu

/** ECX bits 7:0 */
#define CPUID_02_ECX_DESCRIPTOR_8_SHIFT 0u
#define CPUID_02_ECX_DESCRIPTOR_8_MASK 0xFFu

/** EDX bits 31:24 */
#define CPUID_02_EDX_DESCRIPTOR_15_SHIFT 24u
#define CPUID_02_EDX_DESCRIPTOR_15_MASK 0xFFu

/** EDX bits 23:16 */
#define CPUID_02_EDX_DESCRIPTOR_14_SHIFT 16u
#define CPUID_02_EDX_DESCRIPTOR_14_MASK 0xFFu

/** EDX bits 15:8 */
#define CPUID_02_EDX_DESCRIPTOR_13_SHIFT 8u
#define CPUID_02_EDX_DESCRIPTOR_13_MASK 0xFFu

/** EDX bits 7:0 */
#define CPUID_02_EDX_DESCRIPTOR_12_SHIFT 0u
#define CPUID_02_EDX_DESCRIPTOR_12_MASK 0xFFu

/* ---- hoja 03H -- Processor Serial Number ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_03_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_03_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_03_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_03_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_03_ECX_PSN_31_0_SHIFT 0u
#define CPUID_03_ECX_PSN_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_03_EDX_PSN_63_32_SHIFT 0u
#define CPUID_03_EDX_PSN_63_32_MASK 0xFFFFFFFFu

/* ---- hoja 17H, subhoja 00H -- Main Sub-Leaf ---- */

/** EAX bits 31:0 */
#define CPUID_17_00_EAX_MAX_SOCID_INDEX_SHIFT 0u
#define CPUID_17_00_EAX_MAX_SOCID_INDEX_MASK 0xFFFFFFFFu

/** EBX bits 31:17 -- RESERVADO, no se toca */
#define CPUID_17_00_EBX_RESERVED_31_17_SHIFT 17u
#define CPUID_17_00_EBX_RESERVED_31_17_MASK 0x7FFFu

/** EBX bit 16 */
#define CPUID_17_00_EBX_IS_VENDOR_SCHEME 16u

/** EBX bits 15:0 */
#define CPUID_17_00_EBX_SOC_VENDOR_ID_SHIFT 0u
#define CPUID_17_00_EBX_SOC_VENDOR_ID_MASK 0xFFFFu

/** ECX bits 31:0 */
#define CPUID_17_00_ECX_PROJECT_ID_SHIFT 0u
#define CPUID_17_00_ECX_PROJECT_ID_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_17_00_EDX_STEPPING_ID_SHIFT 0u
#define CPUID_17_00_EDX_STEPPING_ID_MASK 0xFFFFFFFFu

/* ---- hoja 17H, subhoja 01H -- Vendor Brand String Sub-Leaf (Bytes 0 to 15) ---- */

/** EAX bits 31:0 */
#define CPUID_17_01_EAX_VENDOR_BRAND_STRING_BYTES_0_to_3_SHIFT 0u
#define CPUID_17_01_EAX_VENDOR_BRAND_STRING_BYTES_0_to_3_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_17_01_EBX_VENDOR_BRAND_STRING_BYTES_4_to_7_SHIFT 0u
#define CPUID_17_01_EBX_VENDOR_BRAND_STRING_BYTES_4_to_7_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_17_01_ECX_VENDOR_BRAND_STRING_BYTES_8_to_11_SHIFT 0u
#define CPUID_17_01_ECX_VENDOR_BRAND_STRING_BYTES_8_to_11_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_17_01_EDX_VENDOR_BRAND_STRING_BYTES_12_to_15_SHIFT 0u
#define CPUID_17_01_EDX_VENDOR_BRAND_STRING_BYTES_12_to_15_MASK 0xFFFFFFFFu

/* ---- hoja 17H, subhoja 02H -- Vendor Brand String Sub-Leaf (Bytes 16 to 31) ---- */

/** EAX bits 31:0 */
#define CPUID_17_02_EAX_VENDOR_BRAND_STRING_BYTES_16_to_19_SHIFT 0u
#define CPUID_17_02_EAX_VENDOR_BRAND_STRING_BYTES_16_to_19_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_17_02_EBX_VENDOR_BRAND_STRING_BYTES_20_to_23_SHIFT 0u
#define CPUID_17_02_EBX_VENDOR_BRAND_STRING_BYTES_20_to_23_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_17_02_ECX_VENDOR_BRAND_STRING_BYTES_24_to_27_SHIFT 0u
#define CPUID_17_02_ECX_VENDOR_BRAND_STRING_BYTES_24_to_27_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_17_02_EDX_VENDOR_BRAND_STRING_BYTES_28_to_31_SHIFT 0u
#define CPUID_17_02_EDX_VENDOR_BRAND_STRING_BYTES_28_to_31_MASK 0xFFFFFFFFu

/* ---- hoja 17H, subhoja 03H -- Vendor Brand String Sub-Leaf (Bytes 32 to 47) ---- */

/** EAX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_17_03_EAX_RESERVED_31_0_SHIFT 0u
#define CPUID_17_03_EAX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EAX bits 31:0 */
#define CPUID_17_03_EAX_VENDOR_BRAND_STRING_BYTES_32_to_35_SHIFT 0u
#define CPUID_17_03_EAX_VENDOR_BRAND_STRING_BYTES_32_to_35_MASK 0xFFFFFFFFu

/** EBX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_17_03_EBX_RESERVED_31_0_SHIFT 0u
#define CPUID_17_03_EBX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EBX bits 31:0 */
#define CPUID_17_03_EBX_VENDOR_BRAND_STRING_BYTES_36_to_39_SHIFT 0u
#define CPUID_17_03_EBX_VENDOR_BRAND_STRING_BYTES_36_to_39_MASK 0xFFFFFFFFu

/** ECX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_17_03_ECX_RESERVED_31_0_SHIFT 0u
#define CPUID_17_03_ECX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** ECX bits 31:0 */
#define CPUID_17_03_ECX_VENDOR_BRAND_STRING_BYTES_40_to_43_SHIFT 0u
#define CPUID_17_03_ECX_VENDOR_BRAND_STRING_BYTES_40_to_43_MASK 0xFFFFFFFFu

/** EDX bits 31:0 -- RESERVADO, no se toca */
#define CPUID_17_03_EDX_RESERVED_31_0_SHIFT 0u
#define CPUID_17_03_EDX_RESERVED_31_0_MASK 0xFFFFFFFFu

/** EDX bits 31:0 */
#define CPUID_17_03_EDX_VENDOR_BRAND_STRING_BYTES_44_to_47_SHIFT 0u
#define CPUID_17_03_EDX_VENDOR_BRAND_STRING_BYTES_44_to_47_MASK 0xFFFFFFFFu


#endif /* VXP_COMMON_CPUID_IDENT_H */
