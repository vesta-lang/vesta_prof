/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 + excepcion de runtime (ver LICENSE).
 */

/**
 * @file test_nt_layout.c
 * @brief La disposicion de las estructuras del kernel que declaramos.
 *
 * POR QUE ESTE TEST EXISTE.  El driver no depende del WDK: las declaraciones
 * del kernel las escribimos nosotros, en `windows/nt.h`.  Eso tiene un riesgo
 * concreto y asimetrico:
 *
 *   - una FIRMA de funcion equivocada da un error de enlace -- ruidoso;
 *   - un DESPLAZAMIENTO de campo equivocado no da nada.  Escribe en el campo de
 *     al lado, y el driver corrompe memoria del kernel en silencio.
 *
 * Asi que se comprueba lo segundo, y se comprueba AL COMPILAR: las
 * comprobaciones son aserciones estaticas, no cosas que pasen en ejecucion.  Un
 * test que hay que acordarse de correr no protege un desplazamiento.
 *
 * SE COMPILA COMO C, no como C++: es el lenguaje en el que se va a usar `nt.h`,
 * y la disposicion de una estructura no tiene por que coincidir entre los dos
 * si alguien mete algo que no deberia.
 *
 * @see test_nt_layout_ref.c -- la otra mitad, que comprueba que los numeros de
 *      la tabla no son un error de tecleo.
 */

#include "nt.h"

#include <stddef.h>
#include <stdio.h>

/* La tabla, comprobada contra NUESTRAS estructuras. */
#define NT_LAYOUT_SIZE(type, bytes)                                            \
    STATIC_ASSERT(sizeof(type) == (bytes), #type " has the wrong size");
#define NT_LAYOUT_OFF(type, field, off)                                        \
    STATIC_ASSERT(offsetof(type, field) == (off),                              \
                  #type "." #field " is at the wrong offset");
#include "nt_layout.inc"
#undef NT_LAYOUT_SIZE
#undef NT_LAYOUT_OFF

int main(void) {
    printf("== kernel struct layout ==\n");

/* Y se imprimen, para que al anadir una estructura nueva los numeros esten a
 * mano en vez de haber que sacarlos de un depurador. */
#define NT_LAYOUT_SIZE(type, bytes)                                            \
    printf("  sizeof(%-16s) = %3u\n", #type, (unsigned)sizeof(type));
#define NT_LAYOUT_OFF(type, field, off)                                        \
    printf("  offset(%-16s.%-16s) = %3u\n", #type, #field,                     \
           (unsigned)offsetof(type, field));
#include "nt_layout.inc"
#undef NT_LAYOUT_SIZE
#undef NT_LAYOUT_OFF

    /* Si esto compilo, las aserciones de arriba se cumplieron todas: no hay
     * nada que pueda fallar en ejecucion. */
    printf("--- checked at compile time ---\n");
    return 0;
}
