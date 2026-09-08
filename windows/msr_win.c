/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_win.c
 * @brief La mitad de Windows de `common/msr.h`.
 *
 * Aqui no hay logica: solo la instruccion, por su intrinseco.  Toda la decision
 * de QUE MSR leer y que significa lo leido vive en `common/`, que es lo que
 * permite probarla sin cargar nada.
 *
 * SELECCION EN TIEMPO DE ENLACE, NO TABLA DE PUNTEROS.  Una compilacion de
 * Windows enlaza este fichero y nunca ve el de Linux ni el falso de las
 * pruebas.  Ver el porque en `common/msr.h`.
 *
 * SIN ENSAMBLADOR, Y COMPROBADO.  La primera version de este fichero llevaba
 * `rdmsr` en asm en linea, dando por hecho que GCC no tenia intrinseco.  Es
 * falso: mingw-w64 lo trae en `<intrin.h>`, igual que MSVC y Clang, y el codigo
 * que sale es el mismo que se escribiria a mano.  Medido con `-O2
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
 * Asi que la regla del ensamblador de `common/README.md` se cumple tal cual
 * esta escrita, sin excepcion que justificar.  Merece quedar anotado porque la
 * excepcion ya estaba escrita y argumentada, y era simplemente que nadie habia
 * mirado si hacia falta.
 *
 * LO QUE ESTA IMPLEMENTACION TODAVIA NO SABE HACER: capturar el fallo.  Leer un
 * MSR que no existe provoca una excepcion de proteccion general, y para
 * convertirla en `ERR_FAULT` hace falta SEH (`__try`/`__except`), que MSVC y
 * Clang tienen y GCC no.  Mientras eso no este, la unica proteccion es no
 * pedirlo: `common/pmu_caps.c` solo lee MSR cuya existencia esta garantizada
 * por un bit de CPUID que ya comprobo.  Es menos ambicioso y no arriesga el
 * equipo, que es el orden correcto de hacer las cosas.
 */

#include "msr.h"

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
