/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file cpu_tables.h
 * @brief Que tablas describen a esta pieza, elegidas POR LA TABLA.
 *
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
 */

#ifndef VXP_COMMON_CPU_TABLES_H
#define VXP_COMMON_CPU_TABLES_H

#include "cpuid_table.h"
#include "msr_table.h"
#include "vxp_base.h"

/** @brief Un fabricante: como se reconoce y que tablas lo describen. */
typedef struct cpu_tables {
    u32 sig_ebx;              /**< `CPUID.0` EBX de este fabricante       */
    u32 sig_edx;              /**< ...EDX                                 */
    u32 sig_ecx;              /**< ...ECX                                 */
    const char *vendor;       /**< la cadena, para poder imprimirla       */
    const cpuid_table *cpuid; /**< sus campos de CPUID                    */
    const msr_table *msr;     /**< sus MSR                                */
} cpu_tables;

/**
 * @brief Las tablas que describen a la pieza donde esto corre.
 *
 * @return la entrada que coincide, o cero si el fabricante no esta en la lista.
 *
 * Devolver cero es parte del contrato y no un caso raro que se pueda ignorar:
 * es la diferencia entre "no lo conozco" y "lo trato como Intel a ver que
 * sale".  Un volcado hecho con la tabla equivocada no da un error, da nombres
 * de campo que no corresponden a los bits que se leyeron.
 */
const cpu_tables *cpu_tables_detect(void);

#endif /* VXP_COMMON_CPU_TABLES_H */
