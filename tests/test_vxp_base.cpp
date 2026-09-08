/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_vxp_base.cpp
 * @brief El cimiento del perfilador: que dice lo mismo en C y en C++, y que el
 *        idioma de modulo opaco funciona de verdad.
 *
 * POR QUE ESTE TEST SE COMPILA EN DOS LENGUAJES.  El cimiento tiene que valer
 * en kernel de Windows, kernel de Linux y espacio de usuario, como C y como
 * C++.  Comprobarlo desde un solo lenguaje no prueba nada: la manera de que la
 * cabecera diga cosas distintas a cada lado es justo que nadie mire los dos.
 *
 * Asi que la prueba se reparte: `opaque_probe.c` se compila como **C** y
 * publica lo que ve; este fichero se compila como **C++**, y compara. Si los
 * tipos midieran distinto, o si `extern "C"` faltara, esto no enlazaria -- que
 * ya seria el fallo cazado.
 *
 * Es el mismo modo de fallo que dio problemas con `CreateELF.h` en este arbol:
 * una cabecera de C que nadie habia intentado compilar como C++, y el codigo
 * mas facil de equivocar era el unico sin pruebas por ese motivo.
 */

#include "opaque_probe.h"
#include "vxp_base.h"

#include <cstdio>

static int g_checks = 0, g_fails = 0;
#define CHECK(c)                                                               \
    do {                                                                       \
        ++g_checks;                                                            \
        if (!(c)) {                                                            \
            ++g_fails;                                                         \
            std::printf("  FAIL L%d: %s\n", __LINE__, #c);                     \
        }                                                                      \
    } while (0)

/* Las mismas aserciones que hace la cabecera, pero del lado de C++: si alguna
 * dependiera del lenguaje, aqui se caeria la compilacion. */
STATIC_ASSERT(sizeof(u32) == 4, "u32 is not 4 bytes in C++");
STATIC_ASSERT(sizeof(u64) == 8, "u64 is not 8 bytes in C++");
STATIC_ASSERT(OK == 0, "OK must be zero");

int main() {
    std::printf("== profiler foundation (vxp_base) ==\n");

    // --- Los tipos dicen lo mismo en los dos lenguajes --------------------
    {
        /* Esto es el corazon del fichero.  El lado de C devuelve lo que EL ve;
         * aqui se compara con lo que ve C++.  Un desacuerdo significa que la
         * misma cabecera define cosas distintas segun quien la lea, y con una
         * estructura compartida por un anillo eso no da error: da campos
         * corridos. */
        CHECK(probe_sizeof_u64() == sizeof(u64));
        CHECK(probe_sizeof_size() == sizeof(usize));
        CHECK(probe_sizeof_storage() == sizeof(probe));
        CHECK(probe_alignof_storage() == ALIGNOF(probe));
        CHECK(probe_ok_value() == static_cast<int>(OK));
    }

    // --- Anchos y alineacion ---------------------------------------------
    {
        CHECK(sizeof(u8) == 1);
        CHECK(sizeof(u16) == 2);
        CHECK(sizeof(u32) == 4);
        CHECK(sizeof(u64) == 8);
        CHECK(sizeof(s32) == 4);
        CHECK(sizeof(s64) == 8);
        CHECK(sizeof(usize) == sizeof(void *));
        // El almacenamiento opaco respeta la alineacion que se pidio.
        CHECK(ALIGNOF(probe) >= 8);
        CHECK(sizeof(probe) == 32);
    }

    // --- El estado ---------------------------------------------------------
    {
        /* Cero para el exito, para que `if (rc)` se lea como "paso algo". */
        CHECK(OK == 0);
        /* Y los fallos negativos, para que una funcion pueda devolver "cuantos"
         * y un error con el mismo tipo sin ambiguedad. */
        CHECK(ERR_INVALID < 0);
        CHECK(ERR_NOSPACE < 0);
        CHECK(ERR_UNSUPPORTED < 0);
        CHECK(ERR_STATE < 0);
        CHECK(ERR_FAULT < 0);
        // Distintos entre si: si dos coincidieran, un fallo se contaria por otro.
        CHECK(ERR_INVALID != ERR_NOSPACE);
        CHECK(ERR_NOSPACE != ERR_UNSUPPORTED);
        CHECK(ERR_UNSUPPORTED != ERR_STATE);
        CHECK(ERR_STATE != ERR_FAULT);
    }

    // --- El idioma completo, ejercitado ----------------------------------
    {
        /* Almacenamiento del LLAMANTE: aqui, en la pila.  Es lo que permite que
         * el modulo no reserve nunca. */
        probe p;
        CHECK(probe_init(&p, 100) == OK);

        u32 v = 0xDEAD;
        CHECK(probe_value(&p, &v) == OK);
        CHECK(v == 0);

        CHECK(probe_add(&p, 40) == OK);
        CHECK(probe_add(&p, 60) == OK);
        CHECK(probe_value(&p, &v) == OK);
        CHECK(v == 100);

        /* En el tope: se rechaza Y no modifica.  Que no modifique es la mitad
         * que se olvida, y la que convierte un error en corrupcion. */
        CHECK(probe_add(&p, 1) == ERR_NOSPACE);
        CHECK(probe_value(&p, &v) == OK);
        CHECK(v == 100);

        /* Un valor enorme no debe desbordar al comprobar el tope. */
        CHECK(probe_add(&p, 0xFFFFFFFFu) == ERR_NOSPACE);
        CHECK(probe_value(&p, &v) == OK);
        CHECK(v == 100);
    }

    // --- La frontera: lo que entra de fuera se comprueba ------------------
    {
        CHECK(probe_init(0, 10) == ERR_INVALID);
        CHECK(probe_init(0, 0) == ERR_INVALID);

        probe p;
        CHECK(probe_init(&p, 0) == ERR_INVALID); // tope cero no tiene sentido
        CHECK(probe_add(0, 1) == ERR_INVALID);
        CHECK(probe_value(0, 0) == ERR_INVALID);

        CHECK(probe_init(&p, 10) == OK);
        u32 v = 0;
        CHECK(probe_value(&p, 0) == ERR_INVALID); // destino nulo
        CHECK(probe_value(&p, &v) == OK);
    }

    // --- Usarlo despues de terminarlo se detecta --------------------------
    {
        /* No es paranoia: el almacenamiento lo pone el llamante y puede
         * reutilizarlo.  Si `fini` dejara los campos como estaban, seguir
         * usandolo devolveria valores con pinta razonable en vez de un error. */
        probe p;
        CHECK(probe_init(&p, 10) == OK);
        CHECK(probe_add(&p, 5) == OK);
        probe_fini(&p);

        u32 v = 0;
        CHECK(probe_add(&p, 1) == ERR_STATE);
        CHECK(probe_value(&p, &v) == ERR_STATE);

        probe_fini(&p); // dos veces no debe romper nada
        probe_fini(0);  // ni con nulo
    }

    // --- Forma B: el tamano se sabe en ejecucion --------------------------
    {
        /* La otra mitad del idioma, y la que usara el anillo.  El modulo dice
         * cuanto hace falta; el llamante aporta. */
        CHECK(probe_log_bytes_needed(4) == 4 * sizeof(u32));
        CHECK(probe_log_bytes_needed(0) == 0);

        u32 storage[4];
        probe_log g;
        CHECK(probe_log_init(&g, storage, sizeof(storage), 4) == OK);

        u32 n = 99;
        CHECK(probe_log_count(&g, &n) == OK);
        CHECK(n == 0);

        for (u32 i = 0; i < 4; ++i)
            CHECK(probe_log_push(&g, 100 + i) == OK);
        CHECK(probe_log_count(&g, &n) == OK);
        CHECK(n == 4);

        // Lleno: se rechaza y no escribe fuera.
        CHECK(probe_log_push(&g, 999) == ERR_NOSPACE);
        CHECK(probe_log_count(&g, &n) == OK);
        CHECK(n == 4);

        u32 v = 0;
        CHECK(probe_log_at(&g, 0, &v) == OK && v == 100);
        CHECK(probe_log_at(&g, 3, &v) == OK && v == 103);
        // Fuera de lo ESCRITO, no de lo reservado.
        CHECK(probe_log_at(&g, 4, &v) == ERR_INVALID);

        probe_log_fini(&g);
        CHECK(probe_log_push(&g, 1) == ERR_STATE);
    }

    // --- Y lo que hace que la forma B sea segura: comprobar lo que dan ----
    {
        /* Si `init` se fiara del tamano que le pasan, un bufer corto seria una
         * escritura fuera de sitio.  Y equivocarse al calcularlo es el caso
         * normal, no el raro. */
        u32 corto[2];
        probe_log g;
        CHECK(probe_log_init(&g, corto, sizeof(corto), 4) == ERR_NOSPACE);
        CHECK(probe_log_init(&g, corto, sizeof(corto), 2) == OK);

        // Y la frontera, como siempre.
        CHECK(probe_log_init(0, corto, sizeof(corto), 2) == ERR_INVALID);
        CHECK(probe_log_init(&g, 0, 16, 2) == ERR_INVALID);
        CHECK(probe_log_init(&g, corto, sizeof(corto), 0) == ERR_INVALID);
    }

    // --- Las utilidades ---------------------------------------------------
    {
        int a[7];
        CHECK(COUNT_OF(a) == 7);
        CHECK(INDEX_INVALID == 0xFFFFFFFFu);
        /* El centinela tiene que quedar FUERA de cualquier indice valido: es
         * lo que permite comprobar con `i < n` en vez de comparar punteros. */
        CHECK(INDEX_INVALID > 1000000u);
    }

    std::printf("--- %d checks, %d failures ---\n", g_checks, g_fails);
    return g_fails ? 1 : 0;
}
