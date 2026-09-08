/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file probe/cpu_tables.h
 * @brief
 * \~english Which tables describe this part, chosen BY THE TABLE.
 * \~spanish Que tablas describen a esta pieza, elegidas POR LA TABLA.
 * \~
 *
 * \~english
 * WHY IT EXISTS.  The choice used to be written into the dump's code:
 *
 *     t = (vendor == AMD) ? &cpuid_table_amd : &cpuid_table_intel;
 *
 * and that has two defects.  The first is that adding a vendor forces touching
 * the dump, which should not know how many there are.  The second, worse: the
 * `else` turns "I do not recognise it" into "it is Intel", which is a silent
 * assumption about a part we know nothing about.
 *
 * Here the vendor-to-tables relation is DATA in a list.  Adding a vendor is
 * adding an entry; the dump does not change and does not find out.
 *
 * THE SIGNATURE IS WHAT DECIDES.  `CPUID.0` returns twelve characters split
 * across EBX, EDX and ECX -- in that order, which is not the one the register
 * names suggest -- and that string is the key.  There is not one vendor constant
 * on the decision path.
 *
 *      cpuid(0)
 *         |
 *         v
 *      EBX EDX ECX  ==  "GenuineIntel"  ->  Intel tables
 *                   ==  "AuthenticAMD"  ->  AMD tables
 *                   ==  anything else   ->  zero: we say we do not know
 *
 * \~spanish
 * POR QUE EXISTE.  La eleccion estaba escrita en el codigo del volcado:
 *
 *     t = (vendor == AMD) ? &cpuid_table_amd : &cpuid_table_intel;
 *
 * y eso tiene dos defectos.  El primero es que anadir un fabricante obliga a
 * tocar el volcado, que no deberia saber cuantos hay.  El segundo, peor: el
 * `else` convierte "no lo reconozco" en "es Intel", que es una suposicion
 * silenciosa sobre una pieza de la que no sabemos nada.
 *
 * Aqui la relacion fabricante-tablas es un DATO en una lista.  Anadir un
 * fabricante es anadir una entrada; el volcado no cambia y no se entera.
 *
 * LA FIRMA ES LO QUE DECIDE.  `CPUID.0` devuelve doce caracteres repartidos en
 * EBX, EDX y ECX -- en ese orden, que no es el que sugiere el nombre de los
 * registros --, y esa cadena es la clave.  No hay ninguna constante de
 * fabricante en el camino de decision.
 *
 *      cpuid(0)
 *         |
 *         v
 *      EBX EDX ECX  ==  "GenuineIntel"  ->  tablas de Intel
 *                   ==  "AuthenticAMD"  ->  tablas de AMD
 *                   ==  cualquier otra  ->  cero: se dice que no se sabe
 */

#ifndef VXP_COMMON_PROBE_CPU_TABLES_H
#define VXP_COMMON_PROBE_CPU_TABLES_H

#include "cpuid/table.h"
#include "msr/table.h"
#include "vxp_base.h"

/** @brief
 *  \~english A vendor: how it is recognised and which tables describe it.
 *  \~spanish Un fabricante: como se reconoce y que tablas lo describen. \~ */
typedef struct cpu_tables {
    u32 sig_ebx;              /**< \~english this vendor's `CPUID.0` EBX \~spanish `CPUID.0` EBX de este fabricante \~ */
    u32 sig_edx;              /**< \~english ...EDX \~spanish ...EDX \~ */
    u32 sig_ecx;              /**< \~english ...ECX \~spanish ...ECX \~ */
    const char *vendor;       /**< \~english the string, so it can be printed \~spanish la cadena, para poder imprimirla \~ */
    const cpuid_table *cpuid; /**< \~english its CPUID fields \~spanish sus campos de CPUID \~ */
    const msr_table *msr;     /**< \~english its MSRs \~spanish sus MSR \~ */
} cpu_tables;

/**
 * @brief
 * \~english The tables describing the part this runs on.
 * \~spanish Las tablas que describen a la pieza donde esto corre.
 * \~
 *
 * @return
 * \~english the matching entry, or zero if the vendor is not in the list.
 * \~spanish la entrada que coincide, o cero si el fabricante no esta en la
 *           lista.
 * \~
 *
 * \~english
 * Returning zero is part of the contract and not a rare case that can be
 * ignored: it is the difference between "I do not know it" and "I will treat it
 * as Intel and see what comes out".  A dump made with the wrong table does not
 * give an error, it gives field names that do not correspond to the bits that
 * were read.
 *
 * \~spanish
 * Devolver cero es parte del contrato y no un caso raro que se pueda ignorar: es
 * la diferencia entre "no lo conozco" y "lo trato como Intel a ver que sale".
 * Un volcado hecho con la tabla equivocada no da un error, da nombres de campo
 * que no corresponden a los bits que se leyeron.
 */
const cpu_tables *cpu_tables_detect(void);

#endif /* VXP_COMMON_PROBE_CPU_TABLES_H */
