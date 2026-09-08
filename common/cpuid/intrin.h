/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Esta cabecera vive junto a su fuente y NO en
 * `include/`: esta escrita para tres entornos y da por hechas primitivas que
 * quien la incluya tiene que proporcionar.
 */

/**
 * @file cpuid/intrin.h
 * @brief
 * \~english CPUID through each compiler's intrinsic, and the few bits the Intel
 *           manual CANNOT give.
 * \~spanish CPUID por el intrinseco de cada compilador, y los pocos bits que el
 *           manual de Intel NO puede dar.
 * \~
 *
 * \~english
 * WHAT CPUID IS.  One instruction that answers questions about the processor
 * itself.  You put a LEAF number in EAX -- and sometimes a SUBLEAF in ECX -- and
 * it comes back with four registers full of bit fields: how many counters there
 * are, whether it is a hybrid part, which extensions it implements.  It needs no
 * privileges, which is what lets a user-space program ask the same as the
 * driver.
 *
 *      eax = leaf, ecx = subleaf
 *              |
 *            cpuid
 *              |
 *      eax  ebx  ecx  edx      <- each one packed with fields
 *
 * It is the first thing needed from `common/`: before programming a counter you
 * have to know what PMU there is, and CPUID is what says so.
 *
 * WHY IT IS NOT CALLED `cpuid.h`, WHICH WAS THE OBVIOUS NAME.  Because GCC and
 * Clang bring a `<cpuid.h>` of their own, and this directory is on the include
 * path.  With the short name, the `#include <cpuid.h>` down below resolves to
 * THIS VERY FILE: the include guard cuts the recursion, nothing from the
 * compiler arrives, and the error that comes out is `implicit declaration of
 * __cpuid_count`, which does not point remotely at the cause.  It cost a build
 * to find out.
 *
 * WHY AN INTRINSIC AND NOT ASM.  For a one-instruction operation, inline asm is
 * an optimisation barrier and usually produces WORSE code, and a separate `.S`
 * turns one instruction into a call with a register spill.
 *
 * WHERE THE BIT POSITIONS COME FROM.  From `cpuid/index.h`, which is GENERATED
 * from the manual.  A bit number written from memory fails silently: you ask for
 * the wrong one, zero comes out, and you conclude the machine does not have
 * something it does have.  It does not give an error -- it gives a lost
 * capability.
 *
 * WATCH OUT FOR THE HYPERVISOR.  CPUID is not a read of the silicon: under a
 * hypervisor every `cpuid` leaves through a VM exit and the answers are put
 * there by it.  A zero bit here means "this is how it is announced", not "it
 * does not exist".  What CPUID cannot contradict is asked by MSR, which is
 * `msr/access.h`'s job.
 *
 * \~spanish
 * QUE ES CPUID.  Una instruccion que responde preguntas sobre el propio
 * procesador.  Se pone un numero de HOJA en EAX -- y a veces una SUBHOJA en ECX
 * -- y vuelve con cuatro registros llenos de campos de bits: cuantos contadores
 * hay, si es una pieza hibrida, que extensiones implementa.  No necesita
 * privilegios, que es lo que permite a un programa de usuario preguntar lo mismo
 * que el driver.
 *
 *      eax = hoja, ecx = subhoja
 *              |
 *            cpuid
 *              |
 *      eax  ebx  ecx  edx      <- cada uno lleno de campos
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
 * POR QUE INTRINSECO Y NO ASM.  Para una operacion de una instruccion, el asm en
 * linea es una barrera de optimizacion y suele salir PEOR codigo, y un `.S`
 * aparte convierte una instruccion en una llamada con volcado de registros.
 *
 * DE DONDE SALEN LAS POSICIONES DE BIT.  De `cpuid/index.h`, que se GENERA del
 * manual.  Un numero de bit escrito de memoria falla callado: se pregunta por el
 * equivocado, sale cero, y se concluye que la maquina no tiene algo que si
 * tiene.  No da un error -- da una capacidad perdida.
 *
 * OJO CON EL HIPERVISOR.  CPUID no es una lectura del silicio: bajo un
 * hipervisor cada `cpuid` sale por una salida de VM y las respuestas las pone
 * el.  Un bit a cero aqui significa "asi se anuncia", no "no existe".  Lo que
 * CPUID no puede desmentir se pregunta por MSR, que es el papel de
 * `msr/access.h`.
 */

#ifndef VXP_COMMON_CPUID_INTRIN_H
#define VXP_COMMON_CPUID_INTRIN_H

#include "vxp_base.h"

/* \~english Every bit position, generated from the manual.  See
 * `tools/gen_cpuid_index.py`.
 * \~spanish Las posiciones de bit, todas, generadas del manual.  Ver
 * `tools/gen_cpuid_index.py`. \~ */
#include "index.h"

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * \~english What the four registers of a CPUID return.
 * \~spanish Lo que devuelven los cuatro registros de un CPUID.
 * \~
 *
 * \~english
 * Transparent on purpose: it is output data, not an object with invariants to
 * protect.  The directory's rule is "opaque by default"; the exception is
 * exactly this, and that is why it is said.
 *
 * \~spanish
 * Transparente a proposito: es un dato de salida, no un objeto con invariantes
 * que proteger.  La regla del directorio es "opaco por defecto"; la excepcion es
 * justo esto, y por eso se dice.
 */
typedef struct cpuid_regs {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
} cpuid_regs;

/**
 * @brief
 * \~english Runs CPUID with a leaf and a subleaf.
 * \~spanish Ejecuta CPUID con hoja y subhoja.
 * \~
 *
 * @param leaf    \~english leaf number (input EAX) \~spanish numero de hoja (EAX de entrada) \~
 * @param subleaf \~english subleaf number (input ECX) \~spanish numero de subhoja (ECX de entrada) \~
 * @param out     \~english where the four registers are left \~spanish donde se dejan los cuatro registros \~
 *
 * \~english
 * It is `inline` in the header and not a function in a `.c` on purpose: it is
 * called from detection loops and a real call here would cost more than the
 * instruction itself.
 *
 * \~spanish
 * Es `inline` en la cabecera y no una funcion en un `.c` a proposito: se llama
 * desde bucles de deteccion y una llamada real aqui costaria mas que la propia
 * instruccion.
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
    /* \~english The form with an explicit subleaf.  `__get_cpuid_count` is
     * NOT used: it checks the maximum supported leaf first and therefore
     * REJECTS the hypervisor range leaves (0x40000000), considering them out of
     * range when in fact they belong to neither the basic nor the extended one.
     * That rejection already fooled us once.
     * \~spanish La forma con subhoja explicita.  NO se usa
     * `__get_cpuid_count`, que comprueba antes la hoja maxima soportada y por
     * eso RECHAZA las hojas del rango de los hipervisores (0x40000000): las
     * considera fuera de rango cuando en realidad no pertenecen ni al basico ni
     * al extendido.  Ese rechazo ya nos engano una vez. \~ */
    __cpuid_count(leaf, subleaf, out->eax, out->ebx, out->ecx, out->edx);
#endif
}

/**
 * @brief
 * \~english The highest basic leaf this processor supports.
 * \~spanish La hoja basica mas alta que soporta este procesador.
 * \~
 *
 * \~english
 * Asking for a leaf above this does not give an error: it returns the values of
 * the highest supported one, which is worse -- they get read as if they were
 * those of the leaf that was asked for.  Hence everything below checks first.
 *
 * \~spanish
 * Preguntar por una hoja mayor que esta no da un error: devuelve los valores de
 * la mayor soportada, que es peor -- se leen como si fueran los de la que se
 * pidio.  De ahi que todo lo de abajo compruebe antes.
 */
static inline u32 cpuid_max_basic_leaf(void) {
    cpuid_regs r;
    cpuid_query(0, 0, &r);
    return r.eax;
}

/** @brief
 *  \~english The highest supported extended leaf (0x80000000 range).
 *  \~spanish La hoja extendida mas alta soportada (rango 0x80000000). \~ */
static inline u32 cpuid_max_extended_leaf(void) {
    cpuid_regs r;
    cpuid_query(0x80000000u, 0, &r);
    return r.eax;
}

/* \~english The leaves detection uses, named so that no bare numbers are left
 * in the calls.
 * \~spanish Las hojas que usa la deteccion, con nombre para que no queden
 * numeros sueltos en las llamadas. \~ */
#define CPUID_LEAF_FEATURES 0x00000001u      /**< \~english family/model + DS, PDCM \~spanish familia/modelo + DS, PDCM \~ */
#define CPUID_LEAF_THERMAL_POWER 0x00000006u /**< \~english the APERF/MPERF gate \~spanish la puerta de APERF/MPERF \~ */
#define CPUID_LEAF_EXT_FEATURES 0x00000007u  /**< \~english hybrid \~spanish hibrido \~ */
#define CPUID_LEAF_PMU 0x0000000Au           /**< \~english architectural PMU \~spanish PMU arquitectonico \~ */
#define CPUID_LEAF_TSC_FREQ 0x00000015u      /**< \~english TSC / crystal \~spanish TSC / cristal \~ */
#define CPUID_LEAF_HYBRID 0x0000001Au        /**< \~english core type: P or E \~spanish tipo de nucleo: P o E \~ */
#define CPUID_LEAF_HYPERVISOR 0x40000000u    /**< \~english hypervisor signature \~spanish firma del hipervisor \~ */
#define CPUID_LEAF_EXT_POWER 0x80000007u     /**< \~english invariant TSC \~spanish TSC invariante \~ */
#define CPUID_LEAF_EXT_FEATURES_AMD 0x80000001u /**< \~english IBS on AMD \~spanish IBS en AMD \~ */

/* -------------------------------------------------------------------------
 *  \~english
 *  The TWO bits the Intel manual cannot give.
 *
 *  Everything else comes from `cpuid/index.h`, generated.  These two are still
 *  written by hand, and it is worth saying why -- the migration to the generated
 *  table is exactly what brought it into view:
 *
 *  \~spanish
 *  Los DOS bits que el manual de Intel no puede dar.
 *
 *  Todo lo demas sale de `cpuid/index.h`, generado.  Estos dos siguen escritos a
 *  mano, y merece la pena decir por que -- que la migracion a la tabla generada
 *  es justo lo que lo puso a la vista:
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * \~english
 * There is a hypervisor underneath (leaf 1, ECX bit 31).
 *
 * **Intel documents that bit as "not used, always returns 0".**  That a
 * hypervisor sets it to one is an industry convention, honoured by all of them,
 * but it is not Intel documentation -- and that is why it cannot come out of the
 * generated table.
 *
 * It is used all the same, because what it says matters: under a hypervisor
 * CPUID stops being a read of the silicon.  But we know where it comes from.
 *
 * \~spanish
 * Hay un hipervisor debajo (hoja 1, ECX bit 31).
 *
 * **Intel documenta ese bit como "no usado, siempre devuelve 0".**  Que un
 * hipervisor lo ponga a uno es una convencion de la industria, respetada por
 * todos, pero no es documentacion de Intel -- y por eso no puede salir de la
 * tabla generada.
 *
 * Se usa igualmente, porque lo que dice importa: bajo un hipervisor CPUID deja
 * de ser una lectura del silicio.  Pero se sabe de donde viene.
 */
#define CPUID_1_ECX_HYPERVISOR 31

/**
 * \~english
 * AMD: Instruction Based Sampling (leaf 80000001H, ECX bit 10).
 *
 * IBS is AMD's equivalent of PEBS: the hardware records the sample itself,
 * without the skid of reading the state after the interrupt.
 *
 * **Intel marks that whole range as reserved** in its manual, because it
 * describes the use Intel makes of that leaf.  The bit is AMD's and its source
 * is AMD's documentation, which is not here.
 *
 * It stays written by hand and flagged: the day it is really needed -- IBS is
 * not optional in the medium term -- it has to be checked against AMD's manual,
 * not against this one.
 *
 * \~spanish
 * AMD: Instruction Based Sampling (hoja 80000001H, ECX bit 10).
 *
 * IBS es el equivalente de PEBS en AMD: el hardware graba la muestra el mismo,
 * sin la desviacion de leer el estado despues de la interrupcion.
 *
 * **Intel marca ese rango entero como reservado** en su manual, porque describe
 * el uso que Intel hace de esa hoja.  El bit es de AMD y su fuente es la
 * documentacion de AMD, que aqui no esta.
 *
 * Queda escrito a mano y senalado: el dia que haga falta de verdad -- IBS no es
 * opcional a medio plazo -- hay que contrastarlo contra el manual de AMD, no
 * contra este.
 */
#define CPUID_80000001_ECX_IBS 10

/** @brief
 *  \~english Is bit `n` of `value` set?
 *  \~spanish ¿Esta puesto el bit `n` de `value`? \~ */
static inline int cpuid_bit(u32 value, u32 n) {
    return (int)((value >> n) & 1u);
}

/* \~english Who made the part, in `cpu_vendor`'s encoding.  The values are
 * repeated here instead of including `pmu_caps.h` because the dependency would
 * go backwards: the PMU capabilities lean on CPUID, not the other way round.
 * \~spanish Quien fabrico la pieza, en la codificacion de `cpu_vendor`.  Los
 * valores se repiten aqui en vez de incluir `pmu_caps.h` porque la dependencia
 * iria al reves: las capacidades del PMU se apoyan en CPUID, no al
 * contrario. \~ */
#define CPUID_VENDOR_UNKNOWN 0u
#define CPUID_VENDOR_INTEL 1u
#define CPUID_VENDOR_AMD 2u

/**
 * @brief
 * \~english Who made the part.
 * \~spanish Quien fabrico la pieza.
 * \~
 *
 * \~english
 * It is needed before anything else: the same leaf means different things
 * depending on the vendor, and the manuals' tables are kept apart for that very
 * reason.
 *
 * \~spanish
 * Hace falta antes que nada: la misma hoja quiere decir cosas distintas segun el
 * fabricante, y las tablas de los manuales van separadas por eso mismo.
 */
static inline u32 cpuid_vendor(void) {
    cpuid_regs r;
    cpuid_query(0, 0, &r);
    /* \~english "GenuineIntel" split across EBX, EDX, ECX -- in THAT order,
     * which is not the one the register names suggest.
     * \~spanish "GenuineIntel" repartido en EBX, EDX, ECX -- en ESE orden, que
     * no es el que sugiere el nombre de los registros. \~ */
    if (r.ebx == 0x756E6547u && r.edx == 0x49656E69u && r.ecx == 0x6C65746Eu) {
        return CPUID_VENDOR_INTEL;
    }
    if (r.ebx == 0x68747541u && r.edx == 0x69746E65u && r.ecx == 0x444D4163u) {
        return CPUID_VENDOR_AMD;
    }
    return CPUID_VENDOR_UNKNOWN;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_COMMON_CPUID_INTRIN_H */
