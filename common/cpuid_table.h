/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file cpuid_table.h
 * @brief Los campos de CPUID como DATOS, para recorrerlos.
 *
 * POR QUE HACE FALTA ADEMAS DE LAS MACROS.  `cpuid_index.h` define una macro
 * por campo, y eso sirve para PREGUNTAR por uno concreto sabiendo su nombre al
 * escribir el codigo.  No sirve para lo contrario -- recorrerlos todos sin
 * saber cuales hay --, que es justo lo que hace un volcado.  Una macro no
 * existe en tiempo de ejecucion: no se puede iterar sobre ella, ni imprimir su
 * nombre.
 *
 * Son las dos caras del mismo dato y salen del mismo generador, asi que no
 * pueden discrepar.
 *
 * EL NOMBRE VA COMO DESPLAZAMIENTO, NO COMO PUNTERO.  Un `const char *` por
 * campo son ocho bytes Y una reubicacion por campo, que el cargador tiene que
 * resolver una a una al arrancar.  Con mil quinientos campos eso es medio
 * megabyte de trabajo para imprimir texto.  Un desplazamiento dentro de un
 * unico bloque de cadenas cuesta cuatro bytes, no lleva reubicacion ninguna y
 * queda independiente de la posicion -- que es lo que haria falta si algun dia
 * esto cruzara al nucleo.
 *
 * CADA TABLA ES SU PROPIA UNIDAD DE TRADUCCION.  Quien no la referencie no la
 * enlaza, asi que el driver no paga los sesenta kilobytes de nombres por
 * tenerlos en el arbol.
 */

#ifndef VXP_COMMON_CPUID_TABLE_H
#define VXP_COMMON_CPUID_TABLE_H

#include "vxp_base.h"

/** @brief Un registro de salida de CPUID, por indice. */
enum {
    CPUID_REG_EAX = 0,
    CPUID_REG_EBX = 1,
    CPUID_REG_ECX = 2,
    CPUID_REG_EDX = 3
};

/**
 * @brief La hoja no lleva subhoja, o la lleva parametrica.
 *
 * No es lo mismo que la subhoja cero: `Fn0000_000D_x0` es una subhoja concreta
 * y `Fn8000_001D_x[N:0]` es "una por cada nivel de cache".  Inventarle un
 * numero a la segunda seria afirmar algo que el manual no dice.
 */
#define CPUID_NO_SUBLEAF 0xFFFFu

/** @brief El campo es un reservado: dice que bits no se tocan. */
#define CPUID_FIELD_RESERVED 0x01u

/**
 * @brief Un campo de CPUID: donde vive y como se llama.
 *
 * Dieciseis bytes, y el relleno va nombrado a proposito para que se vea que es
 * relleno y no un hueco por rellenar.
 */
typedef struct cpuid_field {
    u32 leaf;    /**< la hoja: 0x0000000B, 0x80000008...            */
    u32 name;    /**< desplazamiento del nombre en el bloque        */
    u16 subleaf; /**< la subhoja, o `CPUID_NO_SUBLEAF`              */
    u8 reg;      /**< `CPUID_REG_*`                                 */
    u8 lo;       /**< bit bajo del campo                            */
    u8 width;    /**< anchura en bits; 1 = un bit suelto            */
    u8 flags;    /**< `CPUID_FIELD_*`                               */
    u16 _pad;
} cpuid_field;

/**
 * @brief Una tabla entera: los campos de un fabricante.
 *
 * Los fabricantes van SEPARADOS por la misma razon que los MSR: la misma hoja
 * puede querer decir cosas distintas.  `Fn8000_001B` es IBS en AMD y no existe
 * en Intel.
 */
typedef struct cpuid_table {
    const cpuid_field *fields; /**< ordenados por hoja, subhoja, registro y bit */
    u32 count;                 /**< cuantos                                     */
    const char *names;         /**< el bloque de cadenas que indexa `name`      */
} cpuid_table;

/** @brief El nombre de un campo, ya resuelto contra su bloque. */
static inline const char *cpuid_field_name(const cpuid_table *t,
                                           const cpuid_field *f) {
    return t->names + f->name;
}

/** @brief Los campos de CPUID que documenta el manual de Intel. */
extern const cpuid_table cpuid_table_intel;

/** @brief Los que documenta el manual de AMD. */
extern const cpuid_table cpuid_table_amd;

#endif /* VXP_COMMON_CPUID_TABLE_H */
