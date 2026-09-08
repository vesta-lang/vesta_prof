/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file table.c
 * @brief Los MSR de Amd, como datos recorribles.
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_amd_msr_index.py
 *
 * a partir del volcado de texto del manual, que no viaja en el repositorio.
 *
 * ES LA OTRA CARA DE LAS MACROS DE AL LADO, y sale de la misma pasada.  Las
 * macros sirven para preguntar por un campo que ya se conoce al escribir el
 * codigo; esta tabla, para recorrerlos todos sin saber de antemano cuales hay.
 * Una macro no existe en ejecucion: no se puede iterar ni imprimir su nombre.
 *
 * EL NOMBRE ES UN DESPLAZAMIENTO dentro del bloque de cadenas, no un puntero.
 * Un puntero por fila costaria ocho bytes y una reubicacion que el cargador
 * resuelve al arrancar, una por fila.  Ver `common/msr/table.h`.
 *
 * ES SU PROPIA UNIDAD DE TRADUCCION: quien no la referencie no la enlaza, asi
 * que tenerla en el arbol no le cuesta nada a quien no la use -- al driver,
 * sin ir mas lejos.
 */

#include "msr/table.h"

/* Los nombres, compartiendo los repetidos.  Array de bytes y no
 * literal de cadena: ver `Blob.lines` en tools/emit_table.py. */
static const char names[] = {
    /*      0 */ 'A','M','D','_','T','S','C', 0,
    /*      8 */ 'A','M','D','_','A','P','I','C','_','B','A','R', 0,
    /*     21 */ 'A','M','D','_','E','B','L','_','C','R','_','P','O','W','E','R','O','N', 0,
    /*     40 */ 'A','M','D','_','T','S','C','_','A','D','J','U','S','T', 0,
    /*     55 */ 'A','M','D','_','S','P','E','C','_','C','T','R','L', 0,
    /*     69 */ 'A','M','D','_','P','R','E','D','_','C','M','D', 0,
    /*     82 */ 'A','M','D','_','P','A','T','C','H','_','L','E','V','E','L', 0,
    /*     98 */ 'A','M','D','_','M','P','E','R','F', 0,
    /*    108 */ 'A','M','D','_','A','P','E','R','F', 0,
    /*    118 */ 'A','M','D','_','M','T','R','R','c','a','p', 0,
    /*    130 */ 'A','M','D','_','F','L','U','S','H','_','C','M','D', 0,
    /*    144 */ 'A','M','D','_','S','Y','S','E','N','T','E','R','_','C','S', 0,
    /*    160 */ 'A','M','D','_','S','Y','S','E','N','T','E','R','_','E','S','P', 0,
    /*    177 */ 'A','M','D','_','S','Y','S','E','N','T','E','R','_','E','I','P', 0,
    /*    194 */ 'A','M','D','_','M','C','G','_','C','A','P', 0,
    /*    206 */ 'A','M','D','_','M','C','G','_','S','T','A','T', 0,
    /*    219 */ 'A','M','D','_','M','C','G','_','C','T','L', 0,
    /*    231 */ 'A','M','D','_','D','B','G','_','C','T','L','_','M','S','R', 0,
    /*    247 */ 'A','M','D','_','B','R','_','F','R','O','M', 0,
    /*    259 */ 'A','M','D','_','B','R','_','T','O', 0,
    /*    269 */ 'A','M','D','_','L','a','s','t','E','x','c','p','F','r','o','m','I','p', 0,
    /*    288 */ 'A','M','D','_','L','a','s','t','E','x','c','p','T','o','I','p', 0,
    /*    305 */ 'A','M','D','_','M','t','r','r','F','i','x','_','6','4','K', 0,
    /*    321 */ 'A','M','D','_','M','t','r','r','F','i','x','_','1','6','K','_','0', 0,
    /*    339 */ 'A','M','D','_','M','t','r','r','F','i','x','_','1','6','K','_','1', 0,
    /*    357 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','0', 0,
    /*    374 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','1', 0,
    /*    391 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','2', 0,
    /*    408 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','3', 0,
    /*    425 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','4', 0,
    /*    442 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','5', 0,
    /*    459 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','6', 0,
    /*    476 */ 'A','M','D','_','M','t','r','r','F','i','x','_','4','K','_','7', 0,
    /*    493 */ 'A','M','D','_','P','A','T', 0,
    /*    501 */ 'A','M','D','_','M','T','R','R','d','e','f','T','y','p','e', 0,
    /*    517 */ 'A','M','D','_','U','_','C','E','T', 0,
    /*    527 */ 'A','M','D','_','S','_','C','E','T', 0,
    /*    537 */ 'A','M','D','_','P','L','0','S','s','p', 0,
    /*    548 */ 'A','M','D','_','P','L','1','S','s','p', 0,
    /*    559 */ 'A','M','D','_','P','L','2','S','s','p', 0,
    /*    570 */ 'A','M','D','_','P','L','3','S','s','p', 0,
    /*    581 */ 'A','M','D','_','I','s','t','S','s','p','A','d','d','r', 0,
    /*    596 */ 'A','M','D','_','A','P','I','C','_','I','D', 0,
    /*    608 */ 'A','M','D','_','A','p','i','c','V','e','r','s','i','o','n', 0,
    /*    624 */ 'A','M','D','_','T','P','R', 0,
    /*    632 */ 'A','M','D','_','A','r','b','i','t','r','a','t','i','o','n','P','r','i','o','r','i','t','y', 0,
    /*    656 */ 'A','M','D','_','P','r','o','c','e','s','s','o','r','P','r','i','o','r','i','t','y', 0,
    /*    678 */ 'A','M','D','_','E','O','I', 0,
    /*    686 */ 'A','M','D','_','L','D','R', 0,
    /*    694 */ 'A','M','D','_','S','V','R', 0,
    /*    702 */ 'A','M','D','_','E','S','R', 0,
    /*    710 */ 'A','M','D','_','I','n','t','e','r','r','u','p','t','C','o','m','m','a','n','d', 0,
    /*    731 */ 'A','M','D','_','T','i','m','e','r','L','v','t','E','n','t','r','y', 0,
    /*    749 */ 'A','M','D','_','T','h','e','r','m','a','l','L','v','t','E','n','t','r','y', 0,
    /*    769 */ 'A','M','D','_','P','e','r','f','o','r','m','a','n','c','e','C','o','u','n','t','e','r','L','v','t','E','n','t','r','y', 0,
    /*    800 */ 'A','M','D','_','E','r','r','o','r','L','v','t','E','n','t','r','y', 0,
    /*    818 */ 'A','M','D','_','T','i','m','e','r','I','n','i','t','i','a','l','C','o','u','n','t', 0,
    /*    840 */ 'A','M','D','_','T','i','m','e','r','C','u','r','r','e','n','t','C','o','u','n','t', 0,
    /*    862 */ 'A','M','D','_','T','i','m','e','r','D','i','v','i','d','e','C','o','n','f','i','g','u','r','a','t','i','o','n', 0,
    /*    891 */ 'A','M','D','_','S','e','l','f','I','P','I', 0,
    /*    903 */ 'A','M','D','_','E','x','t','e','n','d','e','d','A','p','i','c','F','e','a','t','u','r','e', 0,
    /*    927 */ 'A','M','D','_','E','x','t','e','n','d','e','d','A','p','i','c','C','o','n','t','r','o','l', 0,
    /*    951 */ 'A','M','D','_','S','p','e','c','i','f','i','c','E','n','d','O','f','I','n','t','e','r','r','u','p','t', 0,
    /*    978 */ 'A','M','D','_','I','n','t','e','r','r','u','p','t','E','n','a','b','l','e','0', 0,
    /*    999 */ 'A','M','D','_','Q','M','_','E','V','T','S','E','L', 0,
    /*   1013 */ 'A','M','D','_','Q','M','_','C','T','R', 0,
    /*   1024 */ 'A','M','D','_','X','S','S', 0,
    /*   1032 */ 'A','M','D','_','E','F','E','R', 0,
    /*   1041 */ 'A','M','D','_','S','T','A','R', 0,
    /*   1050 */ 'A','M','D','_','S','T','A','R','6','4', 0,
    /*   1061 */ 'A','M','D','_','S','T','A','R','C','O','M','P','A','T', 0,
    /*   1076 */ 'A','M','D','_','S','Y','S','C','A','L','L','_','F','L','A','G','_','M','A','S','K', 0,
    /*   1098 */ 'A','M','D','_','M','P','e','r','f','R','e','a','d','O','n','l','y', 0,
    /*   1116 */ 'A','M','D','_','A','P','e','r','f','R','e','a','d','O','n','l','y', 0,
    /*   1134 */ 'A','M','D','_','I','R','P','e','r','f','C','o','u','n','t', 0,
    /*   1150 */ 'A','M','D','_','F','S','_','B','A','S','E', 0,
    /*   1162 */ 'A','M','D','_','G','S','_','B','A','S','E', 0,
    /*   1174 */ 'A','M','D','_','K','e','r','n','e','l','G','S','b','a','s','e', 0,
    /*   1191 */ 'A','M','D','_','T','S','C','_','A','U','X', 0,
    /*   1203 */ 'A','M','D','_','T','s','c','R','a','t','e','M','s','r', 0,
    /*   1218 */ 'A','M','D','_','P','r','e','f','e','t','c','h','C','o','n','t','r','o','l', 0,
    /*   1238 */ 'A','M','D','_','L','a','s','t','B','r','a','n','c','h','S','t','a','c','k','S','e','l','e','c','t', 0,
    /*   1264 */ 'A','M','D','_','D','e','b','u','g','E','x','t','n','C','t','l', 0,
    /*   1281 */ 'A','M','D','_','P','e','r','f','C','n','t','r','G','l','o','b','a','l','S','t','a','t','u','s', 0,
    /*   1306 */ 'A','M','D','_','P','e','r','f','C','n','t','r','G','l','o','b','a','l','C','t','l', 0,
    /*   1328 */ 'A','M','D','_','P','e','r','f','C','n','t','r','G','l','o','b','a','l','S','t','a','t','u','s','C','l','r', 0,
    /*   1356 */ 'A','M','D','_','P','e','r','f','C','n','t','r','G','l','o','b','a','l','S','t','a','t','u','s','S','e','t', 0,
    /*   1384 */ 'A','M','D','_','M','c','a','I','n','t','r','C','f','g', 0,
    /*   1399 */ 'A','M','D','_','P','E','R','F','_','L','E','G','A','C','Y','_','C','T','L','0', 0,
    /*   1420 */ 'A','M','D','_','P','E','R','F','_','L','E','G','A','C','Y','_','C','T','L','1', 0,
    /*   1441 */ 'A','M','D','_','P','E','R','F','_','L','E','G','A','C','Y','_','C','T','L','2', 0,
    /*   1462 */ 'A','M','D','_','P','E','R','F','_','L','E','G','A','C','Y','_','C','T','L','3', 0,
    /*   1483 */ 'A','M','D','_','S','Y','S','_','C','F','G', 0,
    /*   1495 */ 'A','M','D','_','H','W','C','R', 0,
    /*   1504 */ 'A','M','D','_','T','O','P','_','M','E','M', 0,
    /*   1516 */ 'A','M','D','_','T','O','M','2', 0,
    /*   1525 */ 'A','M','D','_','P','A','T','C','H','_','L','O','A','D','E','R', 0,
    /*   1542 */ 'A','M','D','_','M','c','E','x','c','e','p','R','e','d','i','r', 0,
    /*   1559 */ 'A','M','D','_','S','M','I','_','O','N','_','I','O','_','T','R','A','P','_','C','T','L','_','S','T','S', 0,
    /*   1586 */ 'A','M','D','_','I','n','t','P','e','n','d', 0,
    /*   1598 */ 'A','M','D','_','S','m','i','T','r','i','g','I','o','C','y','c','l','e', 0,
    /*   1617 */ 'A','M','D','_','M','m','i','o','C','f','g','B','a','s','e','A','d','d','r', 0,
    /*   1637 */ 'A','M','D','_','P','S','t','a','t','e','C','u','r','L','i','m', 0,
    /*   1654 */ 'A','M','D','_','P','S','t','a','t','e','C','t','l', 0,
    /*   1668 */ 'A','M','D','_','P','S','t','a','t','e','S','t','a','t', 0,
    /*   1683 */ 'A','M','D','_','C','S','t','a','t','e','B','a','s','e','A','d','d','r', 0,
    /*   1702 */ 'A','M','D','_','S','M','M','_','B','A','S','E', 0,
    /*   1715 */ 'A','M','D','_','S','M','M','A','d','d','r', 0,
    /*   1727 */ 'A','M','D','_','S','M','M','M','a','s','k', 0,
    /*   1739 */ 'A','M','D','_','V','M','_','C','R', 0,
    /*   1749 */ 'A','M','D','_','I','G','N','N','E', 0,
    /*   1759 */ 'A','M','D','_','V','M','_','H','S','A','V','E','_','P','A', 0,
    /*   1775 */ 'A','M','D','_','S','v','m','L','o','c','k','K','e','y', 0,
    /*   1790 */ 'A','M','D','_','A','v','i','c','D','o','o','r','b','e','l','l', 0,
    /*   1807 */ 'A','M','D','_','G','U','E','S','T','_','T','S','C','_','F','R','E','Q', 0,
    /*   1826 */ 'A','M','D','_','V','I','R','T','U','A','L','_','T','O','M', 0,
    /*   1842 */ 'A','M','D','_','S','e','c','u','r','e','A','V','I','C', 0,
    /*   1857 */ 'A','M','D','_','O','S','V','W','_','I','D','_','L','e','n','g','t','h', 0,
    /*   1876 */ 'A','M','D','_','O','S','V','W','_','S','t','a','t','u','s', 0,
    /*   1892 */ 'A','M','D','_','P','E','R','F','_','C','T','L','0', 0,
    /*   1906 */ 'A','M','D','_','P','E','R','F','_','C','T','L','1', 0,
    /*   1920 */ 'A','M','D','_','P','E','R','F','_','C','T','L','2', 0,
    /*   1934 */ 'A','M','D','_','P','E','R','F','_','C','T','L','3', 0,
    /*   1948 */ 'A','M','D','_','P','E','R','F','_','C','T','L','4', 0,
    /*   1962 */ 'A','M','D','_','P','E','R','F','_','C','T','L','5', 0,
    /*   1976 */ 'A','M','D','_','C','O','R','E','_','E','N','E','R','G','Y','_','S','T','A','T', 0,
    /*   1997 */ 'A','M','D','_','C','p','p','c','C','a','p','a','b','i','l','i','t','y','1', 0,
    /*   2017 */ 'A','M','D','_','C','p','p','c','E','n','a','b','l','e', 0,
    /*   2032 */ 'A','M','D','_','C','p','p','c','C','a','p','a','b','i','l','i','t','y','2', 0,
    /*   2052 */ 'A','M','D','_','C','p','p','c','R','e','q','u','e','s','t', 0,
    /*   2068 */ 'A','M','D','_','C','p','p','c','S','t','a','t','u','s', 0,
    /*   2083 */ 'A','M','D','_','P','P','I','N','_','C','T','L', 0,
    /*   2096 */ 'A','M','D','_','P','P','I','N', 0,
    /*   2105 */ 'A','M','D','_','C','P','U','I','D','_','P','W','R','_','T','H','E','R','M', 0,
    /*   2125 */ 'A','M','D','_','D','R','1','_','A','D','D','R','_','M','A','S','K', 0,
    /*   2143 */ 'A','M','D','_','D','R','2','_','A','D','D','R','_','M','A','S','K', 0,
    /*   2161 */ 'A','M','D','_','D','R','3','_','A','D','D','R','_','M','A','S','K', 0,
    /*   2179 */ 'A','M','D','_','D','R','0','_','A','D','D','R','_','M','A','S','K', 0,
    /*   2197 */ 'A','M','D','_','I','B','S','_','F','E','T','C','H','_','C','T','L', 0,
    /*   2215 */ 'A','M','D','_','I','B','S','_','F','E','T','C','H','_','L','I','N','A','D','D','R', 0,
    /*   2237 */ 'A','M','D','_','I','B','S','_','F','E','T','C','H','_','P','H','Y','S','A','D','D','R', 0,
    /*   2260 */ 'A','M','D','_','I','B','S','_','O','P','_','C','T','L', 0,
    /*   2275 */ 'A','M','D','_','I','B','S','_','O','P','_','R','I','P', 0,
    /*   2290 */ 'A','M','D','_','I','B','S','_','O','P','_','D','A','T','A', 0,
    /*   2306 */ 'A','M','D','_','I','B','S','_','O','P','_','D','A','T','A','2', 0,
    /*   2323 */ 'A','M','D','_','I','B','S','_','O','P','_','D','A','T','A','3', 0,
    /*   2340 */ 'A','M','D','_','I','B','S','_','D','C','_','L','I','N','A','D','D','R', 0,
    /*   2359 */ 'A','M','D','_','I','B','S','_','D','C','_','P','H','Y','S','A','D','D','R', 0,
    /*   2379 */ 'A','M','D','_','I','B','S','_','C','T','L', 0,
    /*   2391 */ 'A','M','D','_','B','P','_','I','B','S','T','G','T','_','R','I','P', 0,
    /*   2409 */ 'A','M','D','_','I','C','_','I','B','S','_','E','X','T','D','_','C','T','L', 0,
};

static const msr_reg rows[] = {
    { 0x00000010u,      0u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TSC */
    { 0x0000001Bu,      8u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_APIC_BAR */
    { 0x0000002Au,     21u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_EBL_CR_POWERON */
    { 0x0000003Bu,     40u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TSC_ADJUST */
    { 0x00000048u,     55u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SPEC_CTRL */
    { 0x00000049u,     69u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PRED_CMD */
    { 0x0000008Bu,     82u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PATCH_LEVEL */
    { 0x000000E7u,     98u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MPERF */
    { 0x000000E8u,    108u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_APERF */
    { 0x000000FEu,    118u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MTRRcap */
    { 0x0000010Bu,    130u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_FLUSH_CMD */
    { 0x00000174u,    144u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SYSENTER_CS */
    { 0x00000175u,    160u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SYSENTER_ESP */
    { 0x00000176u,    177u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SYSENTER_EIP */
    { 0x00000179u,    194u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MCG_CAP */
    { 0x0000017Au,    206u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MCG_STAT */
    { 0x0000017Bu,    219u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MCG_CTL */
    { 0x000001D9u,    231u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_DBG_CTL_MSR */
    { 0x000001DBu,    247u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_BR_FROM */
    { 0x000001DCu,    259u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_BR_TO */
    { 0x000001DDu,    269u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_LastExcpFromIp */
    { 0x000001DEu,    288u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_LastExcpToIp */
    { 0x00000250u,    305u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_64K */
    { 0x00000258u,    321u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_16K_0 */
    { 0x00000259u,    339u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_16K_1 */
    { 0x00000268u,    357u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_0 */
    { 0x00000269u,    374u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_1 */
    { 0x0000026Au,    391u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_2 */
    { 0x0000026Bu,    408u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_3 */
    { 0x0000026Cu,    425u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_4 */
    { 0x0000026Du,    442u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_5 */
    { 0x0000026Eu,    459u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_6 */
    { 0x0000026Fu,    476u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MtrrFix_4K_7 */
    { 0x00000277u,    493u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PAT */
    { 0x000002FFu,    501u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MTRRdefType */
    { 0x000006A0u,    517u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_U_CET */
    { 0x000006A2u,    527u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_S_CET */
    { 0x000006A4u,    537u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PL0Ssp */
    { 0x000006A5u,    548u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PL1Ssp */
    { 0x000006A6u,    559u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PL2Ssp */
    { 0x000006A7u,    570u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PL3Ssp */
    { 0x000006A8u,    581u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IstSspAddr */
    { 0x00000802u,    596u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_APIC_ID */
    { 0x00000803u,    608u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ApicVersion */
    { 0x00000808u,    624u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TPR */
    { 0x00000809u,    632u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ArbitrationPriority */
    { 0x0000080Au,    656u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ProcessorPriority */
    { 0x0000080Bu,    678u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_EOI */
    { 0x0000080Du,    686u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_LDR */
    { 0x0000080Fu,    694u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SVR */
    { 0x00000828u,    702u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ESR */
    { 0x00000830u,    710u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_InterruptCommand */
    { 0x00000832u,    731u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TimerLvtEntry */
    { 0x00000833u,    749u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ThermalLvtEntry */
    { 0x00000834u,    769u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PerformanceCounterLvtEntry */
    { 0x00000837u,    800u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ErrorLvtEntry */
    { 0x00000838u,    818u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TimerInitialCount */
    { 0x00000839u,    840u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TimerCurrentCount */
    { 0x0000083Eu,    862u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TimerDivideConfiguration */
    { 0x0000083Fu,    891u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SelfIPI */
    { 0x00000840u,    903u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ExtendedApicFeature */
    { 0x00000841u,    927u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_ExtendedApicControl */
    { 0x00000842u,    951u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SpecificEndOfInterrupt */
    { 0x00000848u,    978u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_InterruptEnable0 */
    { 0x00000C8Du,    999u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_QM_EVTSEL */
    { 0x00000C8Eu,   1013u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_QM_CTR */
    { 0x00000DA0u,   1024u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_XSS */
    { 0xC0000080u,   1032u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_EFER */
    { 0xC0000081u,   1041u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_STAR */
    { 0xC0000082u,   1050u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_STAR64 */
    { 0xC0000083u,   1061u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_STARCOMPAT */
    { 0xC0000084u,   1076u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SYSCALL_FLAG_MASK */
    { 0xC00000E7u,   1098u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MPerfReadOnly */
    { 0xC00000E8u,   1116u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_APerfReadOnly */
    { 0xC00000E9u,   1134u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IRPerfCount */
    { 0xC0000100u,   1150u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_FS_BASE */
    { 0xC0000101u,   1162u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_GS_BASE */
    { 0xC0000102u,   1174u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_KernelGSbase */
    { 0xC0000103u,   1191u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TSC_AUX */
    { 0xC0000104u,   1203u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TscRateMsr */
    { 0xC0000108u,   1218u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PrefetchControl */
    { 0xC000010Eu,   1238u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_LastBranchStackSelect */
    { 0xC000010Fu,   1264u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_DebugExtnCtl */
    { 0xC0000300u,   1281u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PerfCntrGlobalStatus */
    { 0xC0000301u,   1306u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PerfCntrGlobalCtl */
    { 0xC0000302u,   1328u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PerfCntrGlobalStatusClr */
    { 0xC0000303u,   1356u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PerfCntrGlobalStatusSet */
    { 0xC0000410u,   1384u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_McaIntrCfg */
    { 0xC0010000u,   1399u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_LEGACY_CTL0 */
    { 0xC0010001u,   1420u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_LEGACY_CTL1 */
    { 0xC0010002u,   1441u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_LEGACY_CTL2 */
    { 0xC0010003u,   1462u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_LEGACY_CTL3 */
    { 0xC0010010u,   1483u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SYS_CFG */
    { 0xC0010015u,   1495u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_HWCR */
    { 0xC001001Au,   1504u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TOP_MEM */
    { 0xC001001Du,   1516u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_TOM2 */
    { 0xC0010020u,   1525u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PATCH_LOADER */
    { 0xC0010022u,   1542u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_McExcepRedir */
    { 0xC0010054u,   1559u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SMI_ON_IO_TRAP_CTL_STS */
    { 0xC0010055u,   1586u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IntPend */
    { 0xC0010056u,   1598u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SmiTrigIoCycle */
    { 0xC0010058u,   1617u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_MmioCfgBaseAddr */
    { 0xC0010061u,   1637u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PStateCurLim */
    { 0xC0010062u,   1654u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PStateCtl */
    { 0xC0010063u,   1668u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PStateStat */
    { 0xC0010073u,   1683u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CStateBaseAddr */
    { 0xC0010111u,   1702u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SMM_BASE */
    { 0xC0010112u,   1715u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SMMAddr */
    { 0xC0010113u,   1727u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SMMMask */
    { 0xC0010114u,   1739u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_VM_CR */
    { 0xC0010115u,   1749u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IGNNE */
    { 0xC0010117u,   1759u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_VM_HSAVE_PA */
    { 0xC0010118u,   1775u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SvmLockKey */
    { 0xC001011Bu,   1790u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_AvicDoorbell */
    { 0xC0010134u,   1807u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_GUEST_TSC_FREQ */
    { 0xC0010135u,   1826u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_VIRTUAL_TOM */
    { 0xC0010138u,   1842u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_SecureAVIC */
    { 0xC0010140u,   1857u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_OSVW_ID_Length */
    { 0xC0010141u,   1876u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_OSVW_Status */
    { 0xC0010200u,   1892u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_CTL0 */
    { 0xC0010202u,   1906u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_CTL1 */
    { 0xC0010204u,   1920u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_CTL2 */
    { 0xC0010206u,   1934u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_CTL3 */
    { 0xC0010208u,   1948u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_CTL4 */
    { 0xC001020Au,   1962u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PERF_CTL5 */
    { 0xC001029Au,   1976u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CORE_ENERGY_STAT */
    { 0xC00102B0u,   1997u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CppcCapability1 */
    { 0xC00102B1u,   2017u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CppcEnable */
    { 0xC00102B2u,   2032u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CppcCapability2 */
    { 0xC00102B3u,   2052u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CppcRequest */
    { 0xC00102B4u,   2068u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CppcStatus */
    { 0xC00102F0u,   2083u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PPIN_CTL */
    { 0xC00102F1u,   2096u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_PPIN */
    { 0xC0011003u,   2105u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_CPUID_PWR_THERM */
    { 0xC0011019u,   2125u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_DR1_ADDR_MASK */
    { 0xC001101Au,   2143u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_DR2_ADDR_MASK */
    { 0xC001101Bu,   2161u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_DR3_ADDR_MASK */
    { 0xC0011027u,   2179u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_DR0_ADDR_MASK */
    { 0xC0011030u,   2197u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_FETCH_CTL */
    { 0xC0011031u,   2215u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_FETCH_LINADDR */
    { 0xC0011032u,   2237u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_FETCH_PHYSADDR */
    { 0xC0011033u,   2260u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_OP_CTL */
    { 0xC0011034u,   2275u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_OP_RIP */
    { 0xC0011035u,   2290u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_OP_DATA */
    { 0xC0011036u,   2306u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_OP_DATA2 */
    { 0xC0011037u,   2323u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_OP_DATA3 */
    { 0xC0011038u,   2340u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_DC_LINADDR */
    { 0xC0011039u,   2359u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_DC_PHYSADDR */
    { 0xC001103Au,   2379u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IBS_CTL */
    { 0xC001103Bu,   2391u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_BP_IBSTGT_RIP */
    { 0xC001103Cu,   2409u, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,  0u,  0u, 0u             , {0u, 0u} },  /* AMD_IC_IBS_EXTD_CTL */
};

const msr_table msr_table_amd = {
    rows,
    (u32)(sizeof rows / sizeof rows[0]),
    names
};
