/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_win.c
 * @brief
 * \~english The Windows half of `common/msr/access.h`.
 * \~spanish La mitad de Windows de `common/msr/access.h`.
 * \~
 *
 * \~english
 * There is no logic here: only the instruction, through its intrinsic.  All the
 * decision about WHICH MSR to read and what the reading means lives in
 * `common/`, which is what allows testing it without loading anything.
 *
 * LINK-TIME SELECTION, NOT A POINTER TABLE.  A Windows build links this file and
 * never sees the Linux one nor the tests' fake one.  See why in
 * `common/msr/access.h`.
 *
 * NO ASSEMBLY, AND CHECKED.  The first version of this file carried `rdmsr` in
 * inline asm, taking for granted that GCC had no intrinsic.  That is false:
 * mingw-w64 brings it in `<intrin.h>`, same as MSVC and Clang, and the code that
 * comes out is the same one would write by hand.  Measured with `-O2
 * -ffreestanding -mno-red-zone`:
 *
 * @code
 *   TDM-GCC 10.3        Clang 22.1
 *   rdmsr               rdmsr
 *   shl  $0x20,%rdx     shl  $0x20,%rdx
 *   or   %rdx,%rax      mov  %eax,%eax
 *                       or   %rdx,%rax
 * @endcode
 *
 * So `common/README.md`'s assembly rule holds exactly as written, with no
 * exception to justify.  It is worth writing down because the exception was
 * already written and argued, and it was simply that nobody had looked whether
 * it was needed.
 *
 * \~spanish
 * Aqui no hay logica: solo la instruccion, por su intrinseco.  Toda la decision
 * de QUE MSR leer y que significa lo leido vive en `common/`, que es lo que
 * permite probarla sin cargar nada.
 *
 * SELECCION EN TIEMPO DE ENLACE, NO TABLA DE PUNTEROS.  Una compilacion de
 * Windows enlaza este fichero y nunca ve el de Linux ni el falso de las pruebas.
 * Ver el porque en `common/msr/access.h`.
 *
 * SIN ENSAMBLADOR, Y COMPROBADO.  La primera version de este fichero llevaba
 * `rdmsr` en asm en linea, dando por hecho que GCC no tenia intrinseco.  Es
 * falso: mingw-w64 lo trae en `<intrin.h>`, igual que MSVC y Clang, y el codigo
 * que sale es el mismo que se escribiria a mano.  Medido con `-O2
 * -ffreestanding -mno-red-zone`: ver el listado de arriba.
 *
 * Asi que la regla del ensamblador de `common/README.md` se cumple tal cual esta
 * escrita, sin excepcion que justificar.  Merece quedar anotado porque la
 * excepcion ya estaba escrita y argumentada, y era simplemente que nadie habia
 * mirado si hacia falta.
 * \~
 *
 * \~english
 * THIS READ DOES NOT CATCH THE FAULT, and that is deliberate, not a limitation.
 * Reading an MSR that does not exist raises a general protection fault.  There
 * are two ways of not dying from it and they are for different jobs:
 *
 *   this one    ask CPUID FIRST and do not attempt what is not there.  It costs
 *               nothing and covers the 315 registers whose condition the manual
 *               documents.  It is what `common/probe/pmu_caps.c` uses
 *   the guard   `asm/x86_64/msr_guard.S`, which catches the #GP through a scope
 *               table.  It is what allows looking at the other 1,273, at the
 *               price of a guarded region
 *
 * An earlier version of this comment said catching it required
 * `__try`/`__except`, "which MSVC and Clang have and GCC does not".  That was
 * WRONG: x64 exception handling is table-driven, and GNU as emits those tables
 * with `.seh_handler`.  It is measured and working -- see the guard.
 *
 * \~spanish
 * ESTA LECTURA NO CAPTURA EL FALLO, y es deliberado, no una limitacion.  Leer un
 * MSR que no existe provoca una excepcion de proteccion general.  Hay dos formas
 * de no morir en el intento y son para trabajos distintos:
 *
 *   esta        preguntar ANTES a CPUID y no intentar lo que no esta.  No cuesta
 *               nada y cubre los 315 registros cuya condicion documenta el
 *               manual.  Es lo que usa `common/probe/pmu_caps.c`
 *   el guarda   `asm/x86_64/msr_guard.S`, que captura el #GP con una tabla de
 *               ambitos.  Es lo que permite mirar los otros 1.273, al precio de
 *               una region guardada
 *
 * Una version anterior de este comentario decia que capturarlo exigia
 * `__try`/`__except`, "que MSVC y Clang tienen y GCC no".  Era FALSO: el manejo
 * de excepciones de x64 va por tablas, y GNU as las emite con `.seh_handler`.
 * Esta medido y funcionando -- ver el guarda.
 */

#include "msr/access.h"

#include <intrin.h>

status msr_read(u32 index, u64 *out) {
    if (out == 0) {
        return ERR_INVALID;
    }
    *out = __readmsr(index);
    return OK;
}

status msr_write(u32 index, u64 value) {
    __writemsr(index, value);
    return OK;
}
