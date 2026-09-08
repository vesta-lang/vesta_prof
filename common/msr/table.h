/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file msr/table.h
 * @brief
 * \~english The manual's MSRs as DATA, so they can be walked.
 * \~spanish Los MSR del manual como DATOS, para recorrerlos.
 * \~
 *
 * \~english
 * Same reason as `cpuid/table.h`: a macro serves to ask about a register you
 * already know, not to walk the ones there are.
 *
 * WHAT THIS TABLE HAS AND WHAT IT DOES NOT.  It brings the address, the name and
 * the GATE -- the CPUID condition documenting that the register exists.  It does
 * not bring the bit fields inside: the manual gives them, but as prose
 * (`1: Freeze while SMM is supported.`) and not as identifiers, so they are
 * another artefact with another purpose and have not been extracted yet.  A dump
 * using this table prints the whole sixty-four bit value and says which register
 * it belongs to; it does not break it down.
 *
 * THE GATE IS WHAT ALLOWS READING WITHOUT CATCHING FAULTS.  Reading an MSR that
 * does not exist is a general protection fault, and in ring 0 that kills the
 * machine.  With the gate, CPUID is asked BEFORE and the read that was going to
 * fail is not attempted.
 *
 * AND ALMOST NONE HAS ONE.  Of Intel's one thousand five hundred and eighty
 * eight, only three hundred and fifteen bring the condition adjacent in the
 * manual.  The rest do not fail to exist: it is that the manual does not say
 * next to them how to know -- sometimes the condition IS the table's caption,
 * which lists the models.  A dump has to say that instead of hiding it or
 * risking the read.
 *
 * \~spanish
 * Misma razon que `cpuid/table.h`: una macro sirve para preguntar por un
 * registro que ya se conoce, no para recorrer los que hay.
 *
 * LO QUE ESTA TABLA TIENE Y LO QUE NO.  Trae la direccion, el nombre y la PUERTA
 * -- la condicion de CPUID que documenta que el registro existe --.  No trae los
 * campos de bits de dentro: el manual los da, pero como prosa (`1: Freeze while
 * SMM is supported.`) y no como identificadores, asi que son otro artefacto con
 * otro proposito y todavia no se han extraido.  Un volcado que use esta tabla
 * imprime el valor de sesenta y cuatro bits entero y dice a que registro
 * pertenece; no lo desglosa.
 *
 * LA PUERTA ES LA QUE PERMITE LEER SIN CAPTURAR FALLOS.  Leer un MSR que no
 * existe es una excepcion de proteccion general, y en anillo cero eso mata la
 * maquina.  Con la puerta se pregunta a CPUID ANTES y no se intenta la lectura
 * que iba a fallar.
 *
 * Y CASI NINGUNO LA TIENE.  De los mil quinientos ochenta y ocho de Intel, solo
 * trescientos quince traen la condicion adyacente en el manual.  Los demas no es
 * que no existan: es que el manual no dice al lado como saberlo -- a veces la
 * condicion ES el titulo de la tabla, que enumera los modelos --.  Un volcado
 * tiene que decir eso en vez de callarlo o de arriesgar la lectura.
 */

#ifndef VXP_COMMON_MSR_TABLE_H
#define VXP_COMMON_MSR_TABLE_H

#include "cpuid/table.h"
#include "vxp_base.h"

/**
 * @brief
 * \~english The manual documents no adjacent condition for this register.
 * \~spanish El manual no documenta condicion adyacente para este registro.
 * \~
 *
 * \~english
 * It does NOT mean "it has no condition".  It means we do not know it by this
 * route, which is different and has to be said differently.
 *
 * \~spanish
 * NO significa "no tiene condicion".  Significa que no la sabemos por esta via,
 * que es distinto y hay que decirlo distinto.
 */
#define MSR_NO_GATE 0xFFu

/**
 * @brief
 * \~english The address is a BASE, and the index is added to it.
 * \~spanish La direccion es una BASE, y el indice se suma.
 * \~
 *
 * \~english
 * The manual writes `C90H+n` for a whole family of registers.  Whoever walks the
 * table has to know: reading only the base would be reading the first and taking
 * for granted there are no more, which is worse than reading none because the
 * result looks complete.
 *
 * \~spanish
 * El manual escribe `C90H+n` para una familia entera de registros.  Quien
 * recorra la tabla tiene que saberlo: leer solo la base seria leer el primero y
 * dar por hecho que no hay mas, que es peor que no leer ninguno porque el
 * resultado parece completo.
 */
#define MSR_REG_INDEXED 0x01u

/**
 * @brief
 * \~english An MSR: where it lives, what it is called and when it exists.
 * \~spanish Un MSR: donde vive, como se llama y cuando existe.
 * \~
 *
 * \~english Twenty bytes.  \~spanish Veinte bytes. \~
 */
typedef struct msr_reg {
    u32 addr;         /**< \~english the address: 0x345, 0xC0000080... \~spanish la direccion: 0x345, 0xC0000080... \~ */
    u32 name;         /**< \~english offset of the name in the blob \~spanish desplazamiento del nombre en el bloque \~ */
    u32 gate_leaf;    /**< \~english the gate's CPUID leaf \~spanish hoja de CPUID de la puerta \~ */
    u16 gate_subleaf; /**< \~english subleaf, or `CPUID_NO_SUBLEAF` \~spanish subhoja, o `CPUID_NO_SUBLEAF` \~ */
    u8 gate_reg;      /**< \~english `CPUID_REG_*`, or `MSR_NO_GATE` if none \~spanish `CPUID_REG_*`, o `MSR_NO_GATE` si no hay \~ */
    u8 gate_lo;       /**< \~english the gate's low bit \~spanish bit bajo de la puerta \~ */
    u8 gate_width;    /**< \~english width; 1 = a single bit \~spanish anchura; 1 = un bit suelto \~ */
    u8 flags;         /**< `MSR_REG_*` */
    u8 _pad[2];
} msr_reg;

/** @brief
 *  \~english A whole table: one vendor's MSRs.
 *  \~spanish Una tabla entera: los MSR de un fabricante. \~ */
typedef struct msr_table {
    const msr_reg *regs; /**< \~english ordered by address \~spanish ordenados por direccion \~ */
    u32 count;           /**< \~english how many \~spanish cuantos \~ */
    const char *names;   /**< \~english the string blob `name` indexes \~spanish el bloque de cadenas que indexa `name` \~ */
} msr_table;

/** @brief
 *  \~english A register's name, already resolved against its blob.
 *  \~spanish El nombre de un registro, ya resuelto contra su bloque. \~ */
static inline const char *msr_reg_name(const msr_table *t, const msr_reg *r) {
    return t->names + r->name;
}

/** @brief
 *  \~english Does this register bring a documented CPUID condition?
 *  \~spanish ¿Trae este registro una condicion de CPUID documentada? \~ */
static inline int msr_reg_has_gate(const msr_reg *r) {
    return r->gate_reg != MSR_NO_GATE;
}

/** @brief
 *  \~english The MSRs Intel's manual documents.
 *  \~spanish Los MSR que documenta el manual de Intel. \~ */
extern const msr_table msr_table_intel;

/** @brief
 *  \~english The ones AMD's manual documents.
 *  \~spanish Los que documenta el manual de AMD. \~ */
extern const msr_table msr_table_amd;

#endif /* VXP_COMMON_MSR_TABLE_H */
