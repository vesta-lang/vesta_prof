/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Esta cabecera vive junto a su fuente y NO en
 * `include/`: esta escrita para tres entornos y da por hechas primitivas que
 * quien la incluya tiene que proporcionar.
 */

/**
 * @file cpuid_intrin.h
 * @brief CPUID, por el intrinseco de cada familia de compilador.
 *
 * Es lo primero que hace falta de `common/`: antes de programar un contador hay
 * que saber que PMU hay, y eso lo dice CPUID.
 *
 * POR QUE NO SE LLAMA `cpuid.h`, QUE ERA EL NOMBRE OBVIO.  Porque GCC y Clang
 * traen un `<cpuid.h>` propio, y este directorio esta en la ruta de inclusion.
 * Con el nombre corto, el `#include <cpuid.h>` de aqui abajo se resuelve a ESTE
 * MISMO FICHERO: la guarda de inclusion corta la recursion, no llega nada del
 * compilador, y el error que sale es `implicit declaration of __cpuid_count`,
 * que no apunta ni de lejos a la causa.  Costo una compilacion averiguarlo.
 *
 * POR QUE INTRINSECO Y NO ASM.  La regla esta en `common/README.md` y aqui se
 * cumple sin excepciones: para una operacion de una instruccion, el asm en
 * linea es una barrera de optimizacion y suele salir PEOR codigo -- el
 * compilador tiene que sanear resultados cuya forma no conoce --, y un `.S`
 * aparte convierte una instruccion en una llamada con volcado de registros.
 *
 * OJO CON EL HIPERVISOR.  CPUID no es una lectura del silicio: bajo un
 * hipervisor cada `cpuid` sale por una salida de VM y las respuestas las pone
 * el.  Un bit a cero aqui significa "asi se anuncia", no "no existe".  Lo que
 * CPUID no puede desmentir se pregunta por MSR, que es el papel de `msr.h`.
 */

#ifndef VXP_COMMON_CPUID_INTRIN_H
#define VXP_COMMON_CPUID_INTRIN_H

#include "vxp_base.h"

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Lo que devuelven los cuatro registros de un CPUID.
 *
 * Transparente a proposito: es un dato de salida, no un objeto con invariantes
 * que proteger.  La regla del directorio es "opaco por defecto"; la excepcion
 * es justo esto, y por eso se dice.
 */
typedef struct cpuid_regs {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
} cpuid_regs;

/**
 * @brief Ejecuta CPUID con hoja y subhoja.
 *
 * @param leaf    numero de hoja (EAX de entrada).
 * @param subleaf numero de subhoja (ECX de entrada).
 * @param out     donde se dejan los cuatro registros.
 *
 * Es `inline` en la cabecera y no una funcion en un `.c` a proposito: se llama
 * desde bucles de deteccion y una llamada real aqui costaria mas que la propia
 * instruccion.  Ver la nota sobre LTO en `README.md`.
 */
static inline void cpuid_query(u32 leaf, u32 subleaf, cpuid_regs *out) {
#if defined(_MSC_VER) && !defined(__clang__)
    int regs[4];
    __cpuidex(regs, (int)leaf, (int)subleaf);
    out->eax = (u32)regs[0];
    out->ebx = (u32)regs[1];
    out->ecx = (u32)regs[2];
    out->edx = (u32)regs[3];
#else
    /* La forma con subhoja explicita.  NO se usa `__get_cpuid_count`, que
     * comprueba antes la hoja maxima soportada y por eso RECHAZA las hojas del
     * rango de los hipervisores (0x40000000): las considera fuera de rango
     * cuando en realidad no pertenecen ni al basico ni al extendido.  Ese
     * rechazo ya nos engano una vez. */
    __cpuid_count(leaf, subleaf, out->eax, out->ebx, out->ecx, out->edx);
#endif
}

/**
 * @brief La hoja basica mas alta que soporta este procesador.
 *
 * Preguntar por una hoja mayor que esta no da un error: devuelve los valores de
 * la mayor soportada, que es peor -- se leen como si fueran los de la que se
 * pidio.  De ahi que todo lo de abajo compruebe antes.
 */
static inline u32 cpuid_max_basic_leaf(void) {
    cpuid_regs r;
    cpuid_query(0, 0, &r);
    return r.eax;
}

/** @brief La hoja extendida mas alta soportada (rango 0x80000000). */
static inline u32 cpuid_max_extended_leaf(void) {
    cpuid_regs r;
    cpuid_query(0x80000000u, 0, &r);
    return r.eax;
}

/* Hojas que usa la deteccion, con nombre para que no queden numeros sueltos. */
#define CPUID_LEAF_FEATURES 0x00000001u      /**< familia/modelo + DS, PDCM   */
#define CPUID_LEAF_EXT_FEATURES 0x00000007u  /**< hibrido                     */
#define CPUID_LEAF_PMU 0x0000000Au           /**< PMU arquitectonico          */
#define CPUID_LEAF_TSC_FREQ 0x00000015u      /**< TSC / cristal               */
#define CPUID_LEAF_HYBRID 0x0000001Au        /**< tipo de nucleo: P o E       */
#define CPUID_LEAF_HYPERVISOR 0x40000000u    /**< firma del hipervisor        */
#define CPUID_LEAF_EXT_POWER 0x80000007u     /**< TSC invariante              */
#define CPUID_LEAF_EXT_FEATURES_AMD 0x80000001u /**< IBS en AMD               */

/* Bits sueltos, con el registro en el nombre para no equivocar de sitio. */
#define CPUID_1_EDX_DS 21     /**< Debug Store: sin el no hay PEBS            */
#define CPUID_1_ECX_PDCM 15   /**< IA32_PERF_CAPABILITIES es legible          */
#define CPUID_1_ECX_HYPERVISOR 31 /**< hay un hipervisor debajo               */
#define CPUID_7_EDX_HYBRID 15 /**< la pieza mezcla nucleos P y E              */
#define CPUID_80000007_EDX_INVARIANT_TSC 8 /**< el TSC no cambia de ritmo     */
#define CPUID_80000001_ECX_IBS 10 /**< AMD: Instruction Based Sampling        */

/** @brief ¿Esta puesto el bit `n` de `value`? */
static inline int cpuid_bit(u32 value, u32 n) {
    return (int)((value >> n) & 1u);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_COMMON_CPUID_INTRIN_H */
