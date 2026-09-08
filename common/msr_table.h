/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file msr_table.h
 * @brief Los MSR del manual como DATOS, para recorrerlos.
 *
 * Misma razon que `cpuid_table.h`: una macro sirve para preguntar por un
 * registro que ya se conoce, no para recorrer los que hay.
 *
 * LO QUE ESTA TABLA TIENE Y LO QUE NO.  Trae la direccion, el nombre y la
 * PUERTA -- la condicion de CPUID que documenta que el registro existe --.  No
 * trae los campos de bits de dentro: el manual los da, pero como prosa
 * (`1: Freeze while SMM is supported.`) y no como identificadores, asi que son
 * otro artefacto con otro proposito y todavia no se han extraido.  Un volcado
 * que use esta tabla imprime el valor de sesenta y cuatro bits entero y dice a
 * que registro pertenece; no lo desglosa.
 *
 * LA PUERTA ES LA QUE PERMITE LEER SIN CAPTURAR FALLOS.  Leer un MSR que no
 * existe es una excepcion de proteccion general, y en anillo cero eso mata la
 * maquina.  Con la puerta se pregunta a CPUID ANTES y no se intenta la lectura
 * que iba a fallar.
 *
 * Y CASI NINGUNO LA TIENE.  De los mil quinientos ochenta y ocho de Intel, solo
 * trescientos quince traen la condicion adyacente en el manual.  Los demas no
 * es que no existan: es que el manual no dice al lado como saberlo -- a veces
 * la condicion ES el titulo de la tabla, que enumera los modelos --.  Un
 * volcado tiene que decir eso en vez de callarlo o de arriesgar la lectura.
 */

#ifndef VXP_COMMON_MSR_TABLE_H
#define VXP_COMMON_MSR_TABLE_H

#include "cpuid_table.h"
#include "vxp_base.h"

/**
 * @brief El manual no documenta condicion adyacente para este registro.
 *
 * NO significa "no tiene condicion".  Significa que no la sabemos por esta via,
 * que es distinto y hay que decirlo distinto.
 */
#define MSR_NO_GATE 0xFFu

/**
 * @brief La direccion es una BASE, y el indice se suma.
 *
 * El manual escribe `C90H+n` para una familia entera de registros.  Quien
 * recorra la tabla tiene que saberlo: leer solo la base seria leer el primero y
 * dar por hecho que no hay mas, que es peor que no leer ninguno porque el
 * resultado parece completo.
 */
#define MSR_REG_INDEXED 0x01u

/**
 * @brief Un MSR: donde vive, como se llama y cuando existe.
 *
 * Veinte bytes.
 */
typedef struct msr_reg {
    u32 addr;         /**< la direccion: 0x345, 0xC0000080...             */
    u32 name;         /**< desplazamiento del nombre en el bloque         */
    u32 gate_leaf;    /**< hoja de CPUID de la puerta                     */
    u16 gate_subleaf; /**< subhoja, o `CPUID_NO_SUBLEAF`                  */
    u8 gate_reg;      /**< `CPUID_REG_*`, o `MSR_NO_GATE` si no hay       */
    u8 gate_lo;       /**< bit bajo de la puerta                          */
    u8 gate_width;    /**< anchura; 1 = un bit suelto                     */
    u8 flags;         /**< `MSR_REG_*`                                    */
    u8 _pad[2];
} msr_reg;

/** @brief Una tabla entera: los MSR de un fabricante. */
typedef struct msr_table {
    const msr_reg *regs; /**< ordenados por direccion            */
    u32 count;           /**< cuantos                            */
    const char *names;   /**< el bloque de cadenas que indexa `name` */
} msr_table;

/** @brief El nombre de un registro, ya resuelto contra su bloque. */
static inline const char *msr_reg_name(const msr_table *t, const msr_reg *r) {
    return t->names + r->name;
}

/** @brief ¿Trae este registro una condicion de CPUID documentada? */
static inline int msr_reg_has_gate(const msr_reg *r) {
    return r->gate_reg != MSR_NO_GATE;
}

/** @brief Los MSR que documenta el manual de Intel. */
extern const msr_table msr_table_intel;

/** @brief Los que documenta el manual de AMD. */
extern const msr_table msr_table_amd;

#endif /* VXP_COMMON_MSR_TABLE_H */
