/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file cpuid/table.h
 * @brief
 * \~english The CPUID fields as DATA, so they can be walked.
 * \~spanish Los campos de CPUID como DATOS, para recorrerlos.
 * \~
 *
 * \~english
 * WHY IT IS NEEDED BESIDES THE MACROS.  `cpuid/index.h` defines one macro per
 * field, and that serves to ASK about a specific one knowing its name while
 * writing the code.  It does not serve the opposite -- walking them all without
 * knowing which there are -- which is exactly what a dump does.  A macro does
 * not exist at run time: you cannot iterate over it, nor print its name.
 *
 * They are the two faces of the same data and come out of the same generator, so
 * they cannot disagree.
 *
 * THE NAME GOES AS AN OFFSET, NOT AS A POINTER.  A `const char *` per field is
 * eight bytes AND one relocation per field, which the loader has to resolve one
 * by one at start-up.  With fifteen hundred fields that is a lot of work just to
 * print text.  An offset within a single string blob costs four bytes, carries
 * no relocation at all and stays position-independent -- which is what would be
 * needed if this ever crossed into the kernel.
 *
 * EACH TABLE IS ITS OWN TRANSLATION UNIT.  Whoever does not reference it does
 * not link it, so the driver does not pay the sixty kilobytes of names just for
 * having them in the tree.
 *
 * \~spanish
 * POR QUE HACE FALTA ADEMAS DE LAS MACROS.  `cpuid/index.h` define una macro por
 * campo, y eso sirve para PREGUNTAR por uno concreto sabiendo su nombre al
 * escribir el codigo.  No sirve para lo contrario -- recorrerlos todos sin saber
 * cuales hay --, que es justo lo que hace un volcado.  Una macro no existe en
 * tiempo de ejecucion: no se puede iterar sobre ella, ni imprimir su nombre.
 *
 * Son las dos caras del mismo dato y salen del mismo generador, asi que no
 * pueden discrepar.
 *
 * EL NOMBRE VA COMO DESPLAZAMIENTO, NO COMO PUNTERO.  Un `const char *` por
 * campo son ocho bytes Y una reubicacion por campo, que el cargador tiene que
 * resolver una a una al arrancar.  Con mil quinientos campos eso es mucho
 * trabajo solo para imprimir texto.  Un desplazamiento dentro de un unico bloque
 * de cadenas cuesta cuatro bytes, no lleva reubicacion ninguna y queda
 * independiente de la posicion -- que es lo que haria falta si algun dia esto
 * cruzara al nucleo.
 *
 * CADA TABLA ES SU PROPIA UNIDAD DE TRADUCCION.  Quien no la referencie no la
 * enlaza, asi que el driver no paga los sesenta kilobytes de nombres por
 * tenerlos en el arbol.
 */

#ifndef VXP_COMMON_CPUID_TABLE_H
#define VXP_COMMON_CPUID_TABLE_H

#include "vxp_base.h"

/** @brief
 *  \~english A CPUID output register, by index.
 *  \~spanish Un registro de salida de CPUID, por indice. \~ */
enum {
    CPUID_REG_EAX = 0,
    CPUID_REG_EBX = 1,
    CPUID_REG_ECX = 2,
    CPUID_REG_EDX = 3
};

/**
 * @brief
 * \~english The leaf carries no subleaf, or carries a parametric one.
 * \~spanish La hoja no lleva subhoja, o la lleva parametrica.
 * \~
 *
 * \~english
 * It is not the same as subleaf zero: `Fn0000_000D_x0` is one specific subleaf
 * and `Fn8000_001D_x[N:0]` is "one per cache level".  Inventing a number for the
 * second would be asserting something the manual does not say.
 *
 * \~spanish
 * No es lo mismo que la subhoja cero: `Fn0000_000D_x0` es una subhoja concreta y
 * `Fn8000_001D_x[N:0]` es "una por cada nivel de cache".  Inventarle un numero a
 * la segunda seria afirmar algo que el manual no dice.
 */
#define CPUID_NO_SUBLEAF 0xFFFFu

/** @brief
 *  \~english The field is a reserved one: it says which bits are not touched.
 *  \~spanish El campo es un reservado: dice que bits no se tocan. \~ */
#define CPUID_FIELD_RESERVED 0x01u

/**
 * @brief
 * \~english A CPUID field: where it lives and what it is called.
 * \~spanish Un campo de CPUID: donde vive y como se llama.
 * \~
 *
 * \~english
 * Sixteen bytes, and the padding is named on purpose so it can be seen it is
 * padding and not a hole waiting to be filled.
 *
 * \~spanish
 * Dieciseis bytes, y el relleno va nombrado a proposito para que se vea que es
 * relleno y no un hueco por rellenar.
 */
typedef struct cpuid_field {
    u32 leaf;    /**< \~english the leaf: 0x0000000B, 0x80000008... \~spanish la hoja: 0x0000000B, 0x80000008... \~ */
    u32 name;    /**< \~english offset of the name in the blob \~spanish desplazamiento del nombre en el bloque \~ */
    u16 subleaf; /**< \~english the subleaf, or `CPUID_NO_SUBLEAF` \~spanish la subhoja, o `CPUID_NO_SUBLEAF` \~ */
    u8 reg;      /**< `CPUID_REG_*` */
    u8 lo;       /**< \~english the field's low bit \~spanish bit bajo del campo \~ */
    u8 width;    /**< \~english width in bits; 1 = a single bit \~spanish anchura en bits; 1 = un bit suelto \~ */
    u8 flags;    /**< `CPUID_FIELD_*` */
    u16 _pad;
} cpuid_field;

/**
 * @brief
 * \~english A whole table: one vendor's fields.
 * \~spanish Una tabla entera: los campos de un fabricante.
 * \~
 *
 * \~english
 * The vendors are kept APART for the same reason as the MSRs: the same leaf can
 * mean different things.  `Fn8000_001B` is IBS on AMD and does not exist on
 * Intel.
 *
 * \~spanish
 * Los fabricantes van SEPARADOS por la misma razon que los MSR: la misma hoja
 * puede querer decir cosas distintas.  `Fn8000_001B` es IBS en AMD y no existe
 * en Intel.
 */
typedef struct cpuid_table {
    const cpuid_field *fields; /**< \~english ordered by leaf, subleaf, register and bit \~spanish ordenados por hoja, subhoja, registro y bit \~ */
    u32 count;                 /**< \~english how many \~spanish cuantos \~ */
    const char *names;         /**< \~english the string blob `name` indexes \~spanish el bloque de cadenas que indexa `name` \~ */
} cpuid_table;

/** @brief
 *  \~english A field's name, already resolved against its blob.
 *  \~spanish El nombre de un campo, ya resuelto contra su bloque. \~ */
static inline const char *cpuid_field_name(const cpuid_table *t,
                                           const cpuid_field *f) {
    return t->names + f->name;
}

/** @brief
 *  \~english The CPUID fields Intel's manual documents.
 *  \~spanish Los campos de CPUID que documenta el manual de Intel. \~ */
extern const cpuid_table cpuid_table_intel;

/** @brief
 *  \~english The ones AMD's manual documents.
 *  \~spanish Los que documenta el manual de AMD. \~ */
extern const cpuid_table cpuid_table_amd;

#endif /* VXP_COMMON_CPUID_TABLE_H */
