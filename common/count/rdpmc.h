/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file count/rdpmc.h
 * @brief
 * \~english Reading a counter from user mode, in one instruction.
 * \~spanish Leer un contador desde modo usuario, en una instruccion.
 * \~
 *
 * \~english
 * WHAT IT IS.  `rdpmc` takes a counter number and gives back what it holds.  It
 * is a handful of cycles and no privilege transition -- which is the whole point:
 * a measured region can be as short as a few instructions, instead of having to
 * be long enough to be worth asking the kernel about.
 *
 * IT IS ONLY LEGAL IF SOMEBODY OPENED IT.  By default `rdpmc` is ring 0 only,
 * and executing it in user mode raises a privileged-instruction fault.  Bit 8 of
 * `CR4` makes it legal at any level, and that is a switch only a driver can
 * flip.  Whoever uses this has to be able to survive the fault, because the bit
 * can also be lost -- Windows restores `CR4` from a saved context on some
 * transitions, and nobody says so.
 *
 * WHICH NUMBER GOES IN.  Not the MSR address.  General-purpose counter `i` is
 * number `i`; a fixed counter is `1 << 30 | i`.  Passing `IA32_PMC0` here would
 * read counter 0xC1, which does not exist and faults -- and the mistake looks
 * completely reasonable while reading the code.
 *
 * IT DOES NOT SERIALISE, and this is the trap that costs a measurement.  `rdpmc`
 * is an ordinary instruction as far as the out-of-order engine is concerned, so
 * it can be reordered against the code around it and two reads can overlap.  The
 * difference then comes out SMALLER than what really happened, and there is no
 * error anywhere -- just a flattering number.
 *
 * Measured here: two back-to-back reads gave a minimum of 3 cycles unfenced and
 * 49 fenced, on the same core.  The 3 was not a fast read, it was two reads
 * overlapping.
 *
 * @code
 *   without a fence          with a fence
 *   ---------------          ------------
 *   lfence  <- absent        lfence
 *   rdpmc  --,               rdpmc
 *   rdpmc  --' overlap       lfence lfence
 *   -> 3 cycles              rdpmc
 *   (a lie)                  lfence
 *                            -> 49 cycles (P core), 71 (E core)
 * @endcode
 *
 * WHAT GIVES IT AWAY, and it is worth knowing because it generalises: with the
 * fence, the minimum and the MODE are the same value -- 99% of the samples land
 * on 49 or 50.  Without it, the minimum was a rare outlier far below the bulk of
 * the distribution.  **A minimum that is not the mode is not a minimum, it is an
 * artefact.**  Whoever measures with this should look at the shape and not only
 * at the smallest number.
 *
 * The fence is NOT put in here on purpose: it costs cycles and whoever brackets
 * a long region does not need it on the inside.  It is a decision of whoever
 * measures, so it has to be visible where they can see it.
 *
 * AND IT SETS A FLOOR ON WHAT CAN BE MEASURED.  Bracketing anything costs those
 * 49 to 71 cycles, so a region of a handful of instructions is drowned by its own
 * measurement.  Whatever is measured has to be big enough -- a thousand
 * instructions is comfortable -- or repeated inside the brackets.
 *
 * THE COUNTERS ARE PER CORE, not per thread.  Two reads from different
 * processors are two counts from two places and subtracting them means nothing;
 * and the operating system does not save or restore them across a context
 * switch, so anybody else who runs on the core in between is inside the
 * measurement.  Pinning is the caller's job, and taking the minimum of several
 * runs is how the intrusion is kept out: it can only ever add.
 *
 * \~spanish
 * QUE ES.  `rdpmc` toma un numero de contador y devuelve lo que lleva.  Son unos
 * pocos ciclos y ninguna transicion de privilegio -- que es justo el asunto: una
 * region medida puede durar unas pocas instrucciones, en vez de tener que durar
 * lo bastante como para que compense preguntarle al nucleo.
 *
 * SOLO ES LEGAL SI ALGUIEN LO ABRIO.  Por omision `rdpmc` es de anillo cero, y
 * ejecutarla en modo usuario levanta una excepcion de instruccion privilegiada.
 * El bit 8 de `CR4` la hace legal en cualquier nivel, y ese interruptor solo lo
 * acciona un driver.  Quien use esto tiene que sobrevivir a que falle, porque el
 * bit tambien se puede perder -- Windows restaura `CR4` desde un contexto
 * guardado en algunas transiciones, y nadie lo dice.
 *
 * QUE NUMERO ENTRA.  No la direccion del MSR.  El contador de proposito general
 * `i` es el numero `i`; un contador fijo es `1 << 30 | i`.  Pasar aqui
 * `IA32_PMC0` leeria el contador 0xC1, que no existe y falla -- y el error tiene
 * un aspecto perfectamente razonable al leer el codigo.
 *
 * LOS CONTADORES SON POR NUCLEO, no por hilo.  Dos lecturas desde procesadores
 * distintos son dos cuentas de dos sitios y restarlas no significa nada; y el
 * sistema operativo no los guarda ni los restaura en un cambio de contexto, asi
 * que cualquiera que corra en el nucleo entre medias esta dentro de la medida.
 * Fijarse es cosa del llamante, y quedarse con el minimo de varias corridas es
 * como se deja fuera la intrusion: solo puede sumar.
 */

#ifndef VXP_COMMON_COUNT_RDPMC_H
#define VXP_COMMON_COUNT_RDPMC_H

#include "vxp_base.h"

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#endif

/** @brief
 *  \~english Turns a fixed counter's index into its `rdpmc` number.
 *  \~spanish Convierte el indice de un contador fijo en su numero para
 *            `rdpmc`. \~ */
#define RDPMC_FIXED(i) (0x40000000u | (u32)(i))

/**
 * @brief
 * \~english Reads counter number `which`.
 * \~spanish Lee el contador numero `which`.
 * \~
 *
 * @param which \~english the counter's NUMBER, not its MSR address \~spanish el NUMERO del contador, no su direccion de MSR \~
 * @return \~english what it holds \~spanish lo que lleva \~
 *
 * \~english
 * THREE COMPILERS AND TWO SPELLINGS, and which is which was measured, not
 * guessed.  MSVC has `__readpmc` in `<intrin.h>`.  GCC and Clang have
 * `__builtin_ia32_rdpmc`, and both emit a single `rdpmc`.
 *
 * `__readpmc` is NOT the portable spelling even though it is declared
 * everywhere: mingw-w64 declares it in `<intrin.h>` and does not implement it,
 * so the code compiles and fails at LINK time with an undefined reference --
 * which is at least a loud failure, but it is a failure, and it is why there is
 * no assembly here and no `__readpmc` either.
 *
 * \~spanish
 * TRES COMPILADORES Y DOS GRAFIAS, y cual es cual esta medido, no supuesto.
 * MSVC tiene `__readpmc` en `<intrin.h>`.  GCC y Clang tienen
 * `__builtin_ia32_rdpmc`, y los dos emiten un solo `rdpmc`.
 *
 * `__readpmc` NO es la grafia portable aunque este declarada en todas partes:
 * mingw-w64 la declara en `<intrin.h>` y no la implementa, asi que el codigo
 * compila y falla al ENLAZAR con una referencia sin resolver -- que al menos es
 * un fallo ruidoso, pero es un fallo, y es la razon de que aqui no haya
 * ensamblador y tampoco `__readpmc`.
 */
static inline u64 rdpmc(u32 which) {
#if defined(_MSC_VER) && !defined(__clang__)
    return (u64)__readpmc((unsigned long)which);
#else
    return (u64)__builtin_ia32_rdpmc((int)which);
#endif
}

#endif /* VXP_COMMON_COUNT_RDPMC_H */
