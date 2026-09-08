/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file cpu_tables.c
 * @brief La lista de fabricantes y sus tablas.
 *
 * ESTE ES EL UNICO FICHERO QUE SABE CUANTOS FABRICANTES HAY.  Anadir uno es
 * anadir una linea aqui y sus tablas generadas; ni el volcado ni la deteccion
 * de capacidades se enteran.
 *
 * Y es tambien el unico sitio donde vive la relacion "esta firma -> estas
 * tablas".  Tenerla repartida seria la forma habitual de que un dia una parte
 * elija la tabla de Intel y otra la de AMD para la misma pieza.
 */

#include "cpu_tables.h"

#include "cpuid_intrin.h"

/**
 * Los fabricantes conocidos.
 *
 * La firma son los doce caracteres que devuelve `CPUID.0` repartidos en EBX,
 * EDX y ECX -- en ESE orden, que no es el que sugiere el nombre de los
 * registros y es un error clasico --.  Se escriben como los enteros que la
 * instruccion devuelve y no como cadena, porque asi la comparacion son tres
 * enteros y no un recorrido de caracteres, y porque es la forma en que aparecen
 * al depurar.
 *
 *   "GenuineIntel" -> EBX 0x756E6547 "Genu", EDX 0x49656E69 "ineI", ECX 0x6C65746E "ntel"
 *   "AuthenticAMD" -> EBX 0x68747541 "Auth", EDX 0x69746E65 "enti", ECX 0x444D4163 "cAMD"
 */
static const cpu_tables known[] = {
    {0x756E6547u, 0x49656E69u, 0x6C65746Eu, "GenuineIntel", &cpuid_table_intel,
     &msr_table_intel},
    {0x68747541u, 0x69746E65u, 0x444D4163u, "AuthenticAMD", &cpuid_table_amd,
     &msr_table_amd},
};

const cpu_tables *cpu_tables_detect(void) {
    cpuid_regs r;
    usize i;

    cpuid_query(0, 0, &r);
    for (i = 0; i < sizeof known / sizeof known[0]; ++i) {
        if (known[i].sig_ebx == r.ebx && known[i].sig_edx == r.edx &&
            known[i].sig_ecx == r.ecx) {
            return &known[i];
        }
    }
    /* Nadie coincide.  Se dice, no se supone: elegir una tabla "por si acaso"
     * produciria un volcado con nombres de campo que no corresponden a los bits
     * leidos, y eso no falla -- miente. */
    return 0;
}
