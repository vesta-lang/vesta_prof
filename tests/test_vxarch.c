/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_vxarch.c
 * @brief
 * \~english The `.vxarch` reader, exercised without the 300 KB file.
 * \~spanish El lector de `.vxarch`, ejercitado sin el fichero de 300 KB.
 * \~
 *
 * \~english
 * WHY IT CAN BE TESTED WITHOUT THE FILE.  The parser takes bytes, not a path, so
 * four lines written inline exercise the same code that reads a real
 * microarchitecture.  That is what allows checking the cases a real file does not
 * contain: a version from the future, a class that does not fit, a line that is
 * not one.
 *
 * WHAT IS CHECKED, and the first one is the one that matters:
 *
 *   THE COLUMNS.  A `class` line has eight fields and the interesting ones are
 *   the first and the seventh.  Reading one column across gives a cost that is
 *   still a plausible number -- micro-operations read as latency is a small
 *   integer, same as a latency -- and nothing anywhere says so.  So the values
 *   checked here are all DIFFERENT from each other on purpose: a shift lands on
 *   the wrong one and the test sees it.
 *
 *   THAT AN UNKNOWN VERSION IS REFUSED, instead of being parsed line by line.
 *
 *   THAT A FORM WITHOUT COST IS NOT A FORM THAT IS FREE.
 *
 * \~spanish
 * POR QUE SE PUEDE PROBAR SIN EL FICHERO.  El analizador toma bytes, no una
 * ruta, asi que cuatro lineas escritas aqui ejercitan el mismo codigo que lee
 * una microarquitectura de verdad.  Eso es lo que permite comprobar los casos
 * que un fichero real no contiene: una version del futuro, una clase que no
 * cabe, una linea que no lo es.
 *
 * QUE SE COMPRUEBA, y lo primero es lo que importa:
 *
 *   LAS COLUMNAS.  Una linea `class` tiene ocho campos y los interesantes son el
 *   primero y el septimo.  Leer una columna corrida da un coste que sigue siendo
 *   un numero plausible -- las microoperaciones leidas como latencia son un
 *   entero pequeno, igual que una latencia -- y no lo dice nada en ninguna
 *   parte.  Asi que los valores que se comprueban aqui son todos DISTINTOS entre
 *   si a proposito: un corrimiento cae en el que no es y el test lo ve.
 *
 *   QUE UNA VERSION DESCONOCIDA SE RECHAZA, en vez de interpretarse linea a
 *   linea.
 *
 *   QUE UNA FORMA SIN COSTE NO ES UNA FORMA QUE SALGA GRATIS.
 */

#include "vxarch.h"

#include <stdio.h>

static int failures = 0;

static void check(int cond, const char *what) {
    if (!cond) {
        printf("FAIL: %s\n", what);
        failures += 1;
    }
}

#define CLASSES 8u
#define FORMS 64u

static vxarch_class g_classes[CLASSES];
static u16 g_forms[FORMS];

/* \~english Compares the whole name and not a couple of letters of it.  The
 * first version of this test checked two positions and got one wrong, which
 * failed for a reason that had nothing to do with the reader.  A check that is
 * harder to write than the thing it checks is a check that reports itself.
 * \~spanish Compara el nombre entero y no un par de letras suyas.  La primera
 * version de este test comprobaba dos posiciones y se equivoco en una, con lo
 * que fallo por un motivo que no tenia nada que ver con el lector.  Una
 * comprobacion mas dificil de escribir que lo que comprueba es una comprobacion
 * que se delata a si misma. \~ */
static int same(const char *a, const char *b) {
    usize i = 0;
    while (a[i] != 0 && a[i] == b[i]) {
        i += 1;
    }
    return a[i] == b[i];
}

static void reset(vxarch *a) {
    a->classes = g_classes;
    a->class_cap = CLASSES;
    a->form_class = g_forms;
    a->form_cap = FORMS;
}

/* \~english Every number here is different from every other on purpose: it is
 * what turns a column shift into a visible failure instead of a plausible
 * result.  Class 1 is the interesting one -- throughput 0.5, four micro
 * operations, and a longest edge of 6 that is NOT the first one listed.
 * \~spanish Cada numero de aqui es distinto de todos los demas a proposito: es
 * lo que convierte un corrimiento de columna en un fallo visible en vez de en un
 * resultado plausible.  La clase 1 es la interesante -- throughput 0,5, cuatro
 * microoperaciones, y una arista mas larga de 6 que NO es la primera de la
 * lista. \~ */
static const char g_file[] =
    "vxarch 1 name=prueba-de-mentira family=intel isa=x86 classes=3 mapped=2\n"
    "ports: 0=p0 1=p1\n"
    "# un comentario, que se salta\n"
    "class 0|0.33|1|0|0|-1.00|-|0*1.00\n"
    "class 1|0.50|4|1|1|-1.00|0:0:0:2.00,1:0:3:6.00:ub,2:0:1:3.00|0*1.00,1*2.00\n"
    "class 2|11.00|9|0|0|42.00|0:0:0:14.00|1*3.00\n"
    "seccion-que-no-conoce: nada\n"
    "5|1\n"
    "9|2\n"
    "63|0\n";

int main(void) {
    vxarch a;

    printf("--- las columnas, que es donde se cuela un error plausible ---\n");
    {
        const vxarch_class *c;
        reset(&a);
        check(vxarch_parse(g_file, sizeof(g_file) - 1, &a) == OK,
              "un fichero bien formado se lee");
        check(same(a.name, "prueba-de-mentira"),
              "el nombre de la microarquitectura se guarda entero");
        check(a.class_count == 3, "tres clases");
        check(a.form_count == 3, "tres formas con coste");

        c = vxarch_cost(&a, 5);
        check(c != 0, "la forma 5 tiene coste");
        if (c) {
            check(c->recip_tp == 500, "throughput 0,50 -> 500 milesimas");
            check(c->uops == 4, "cuatro microoperaciones");
            check(c->latency == 6000,
                  "la latencia es la arista MAS LARGA, no la primera");
            check((c->flags & VXARCH_MICROCODED) != 0, "microcodificada");
            check((c->flags & VXARCH_MACRO_FUSIBLE) != 0, "macro-fusionable");
        }

        c = vxarch_cost(&a, 9);
        check(c != 0 && c->recip_tp == 11000 && c->uops == 9 &&
                      c->latency == 14000,
              "la clase 2 tambien, con numeros de dos cifras");
        check(c != 0 && c->flags == 0, "y sin ninguna bandera");

        c = vxarch_cost(&a, 63);
        check(c != 0 && c->recip_tp == 330 && c->latency == 0,
              "una clase sin aristas cronometradas tiene latencia cero");
    }

    printf("--- lo que no esta, no sale gratis ---\n");
    {
        reset(&a);
        check(vxarch_parse(g_file, sizeof(g_file) - 1, &a) == OK, "se lee");
        check(vxarch_cost(&a, 6) == 0,
              "una forma sin clase devuelve NULO, no un coste de cero");
        check(vxarch_cost(&a, FORMS) == 0, "y fuera de rango, tambien");
        check(vxarch_cost(&a, FORMS + 1000u) == 0, "muy fuera, igual");
    }

    printf("--- una version que no se conoce se rechaza ---\n");
    {
        static const char futuro[] =
            "vxarch 2 name=del-futuro classes=1 mapped=1\n"
            "class 0|0.33|1|0|0|-1.00|-|0*1.00\n"
            "5|0\n";
        reset(&a);
        check(vxarch_parse(futuro, sizeof(futuro) - 1, &a) == ERR_INVALID,
              "version 2: se rechaza en vez de leer las columnas que cuadren");

        static const char otro[] = "no es un vxarch en absoluto\n";
        reset(&a);
        check(vxarch_parse(otro, sizeof(otro) - 1, &a) == ERR_INVALID,
              "y un fichero que no lo es, tambien");
    }

    printf("--- lo que no cabe se DICE ---\n");
    {
        static const char grande[] =
            "vxarch 1 name=x classes=99 mapped=1\n"
            "class 99|0.33|1|0|0|-1.00|-|0*1.00\n";
        static const char lejos[] = "vxarch 1 name=x\n"
                                    "class 0|0.33|1|0|0|-1.00|-|0*1.00\n"
                                    "9999|0\n";
        reset(&a);
        check(vxarch_parse(grande, sizeof(grande) - 1, &a) == ERR_NOSPACE,
              "una clase que no cabe no se descarta callando");
        reset(&a);
        check(vxarch_parse(lejos, sizeof(lejos) - 1, &a) == ERR_NOSPACE,
              "una forma fuera del array, tampoco");
    }

    printf("--- errores de uso ---\n");
    {
        reset(&a);
        check(vxarch_parse(0, 0, &a) == ERR_INVALID, "sin texto");
        check(vxarch_parse(g_file, 10, 0) == ERR_INVALID, "sin destino");
        a.classes = 0;
        check(vxarch_parse(g_file, 10, &a) == ERR_INVALID, "sin sitio");
    }

    if (failures == 0) {
        printf("OK\n");
        return 0;
    }
    printf("%d fallos\n", failures);
    return 1;
}
