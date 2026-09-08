/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_nt_layout_ref.c
 * @brief La misma tabla, comprobada contra una cabecera de REFERENCIA.
 *
 * ESTA ES LA MITAD QUE HACE QUE EL OTRO TEST VALGA ALGO.
 *
 * `test_nt_layout.c` comprueba que nuestras estructuras coinciden con la tabla
 * de `nt_layout.inc`.  Eso solo, no prueba nada: los numeros de la tabla los
 * escribio una persona, y si se equivoco al teclear uno, la estructura estaria
 * mal, la tabla estaria mal IGUAL, y las dos coincidirian tan tranquilas.
 *
 * Es exactamente el fallo que ya ocurrio en este arbol con la traduccion de
 * registros de DWARF: la tabla y dos de las comprobaciones estaban mal del
 * mismo modo, y solo se cazo porque OTRAS dos no lo estaban.
 *
 * Aqui la tabla se contrasta contra una cabecera que no escribimos nosotros.
 * Si un numero esta mal, esto no compila.
 *
 * NO SIEMPRE SE PUEDE.  La referencia es opcional -- las cabeceras `ddk/` que
 * trae MinGW-w64 --, y en una maquina sin ellas este objetivo sencillamente no
 * se construye.  Que la referencia no este NO es un fallo del producto: el
 * driver no la necesita para nada.  Pero cuando este, se usa.
 */

#include <ddk/ntddk.h>

#include <stddef.h>
#include <stdio.h>

/* La MISMA tabla, contra las estructuras de la referencia. */
#define NT_LAYOUT_SIZE(type, bytes)                                            \
    _Static_assert(sizeof(type) == (bytes),                                    \
                   #type ": the table disagrees with the reference header");
#define NT_LAYOUT_OFF(type, field, off)                                        \
    _Static_assert(offsetof(type, field) == (off),                             \
                   #type "." #field                                            \
                   ": the table disagrees with the reference header");
#include "nt_layout.inc"
#undef NT_LAYOUT_SIZE
#undef NT_LAYOUT_OFF

int main(void) {
    printf("== layout table vs. reference header ==\n");
    printf("--- checked at compile time ---\n");
    return 0;
}
