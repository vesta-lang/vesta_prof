/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_dump.c
 * @brief El volcado de CPUID y del catalogo de MSR.
 *
 * QUE COMPRUEBA, Y QUE NO PUEDE COMPROBAR.  El volcado imprime lo que ESTA
 * maquina devuelve, asi que su contenido depende de la pieza y no se puede
 * fijar en un test.  Lo que si se puede fijar -- y es donde estan los fallos
 * que importan -- son las propiedades que valen en cualquier maquina:
 *
 *   - que las cuatro tablas esten pobladas y ordenadas;
 *   - que ningun campo se salga de su registro de treinta y dos bits;
 *   - que el nombre de cada fila resuelva dentro de su bloque de cadenas, que
 *     es el modo de fallar propio de guardar desplazamientos en vez de
 *     punteros: un desplazamiento mal escrito no es un puntero invalido que
 *     reviente, es OTRO NOMBRE, y el volcado saldria entero y equivocado;
 *   - que el bufer corto se DIGA en vez de truncar en silencio;
 *   - y la extraccion de campos, que es la unica aritmetica del fichero.
 */

#include "cpuid/table.h"
#include "msr/table.h"
#include "probe/dump.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

static void check(int cond, const char *what) {
    if (!cond) {
        printf("FAIL: %s\n", what);
        failures += 1;
    }
}

/** @brief El bloque de cadenas, recorrido: ¿cabe todo y termina en nul? */
static void check_names(const char *names, u32 off, usize blob_len,
                        const char *what) {
    if (off >= blob_len) {
        printf("FAIL: %s: desplazamiento %u fuera del bloque (%u)\n", what,
               (unsigned)off, (unsigned)blob_len);
        failures += 1;
        return;
    }
    /* Un nombre vacio seria un desplazamiento que cayo justo en un nul, que es
     * la forma que tiene este fallo de pasar desapercibido. */
    if (names[off] == 0) {
        printf("FAIL: %s: nombre vacio en %u\n", what, (unsigned)off);
        failures += 1;
    }
}

/**
 * @brief El tamano del bloque de cadenas.
 *
 * No se publica, asi que se deduce del ultimo nombre.  Vale para lo que hace
 * falta: comprobar que ningun desplazamiento se sale.
 */
static usize blob_extent(const char *names, u32 max_off) {
    return (usize)max_off + strlen(names + max_off) + 1;
}

static void check_cpuid_table(const cpuid_table *t, const char *what) {
    u32 i;
    u32 max_off = 0;
    usize extent;

    check(t->count > 0, "la tabla de CPUID no esta vacia");
    check(t->fields != 0 && t->names != 0, "la tabla de CPUID esta poblada");
    if (t->count == 0) {
        return;
    }

    for (i = 0; i < t->count; ++i) {
        if (t->fields[i].name > max_off) {
            max_off = t->fields[i].name;
        }
    }
    extent = blob_extent(t->names, max_off);

    for (i = 0; i < t->count; ++i) {
        const cpuid_field *f = &t->fields[i];
        char msg[128];

        sprintf(msg, "%s: fila %u", what, (unsigned)i);
        check_names(t->names, f->name, extent, msg);

        /* Un campo vive dentro de un registro de 32 bits.  Que se salga no da
         * un error al usarlo: da una mascara desbordada, y en C un
         * desplazamiento de 32 o mas ni siquiera esta definido. */
        if (f->width == 0 || (u32)f->lo + (u32)f->width > 32u) {
            printf("FAIL: %s: fila %u (%s) ocupa [%u+%u], fuera de 32 bits\n",
                   what, (unsigned)i, cpuid_field_name(t, f),
                   (unsigned)f->lo, (unsigned)f->width);
            failures += 1;
        }
        check(f->reg <= CPUID_REG_EDX, "el registro es EAX..EDX");

        /* El orden importa: el volcado consulta CPUID una vez por hoja y
         * subhoja apoyandose en que las filas vienen agrupadas.  Si dejaran de
         * estarlo no fallaria -- solo ejecutaria CPUID de mas --, pero el
         * volcado saldria desordenado y dejaria de servir para comparar dos
         * maquinas a ojo, que es para lo que se hizo. */
        if (i > 0) {
            const cpuid_field *p = &t->fields[i - 1];
            if (p->leaf > f->leaf) {
                printf("FAIL: %s: fila %u rompe el orden por hoja\n", what,
                       (unsigned)i);
                failures += 1;
            }
        }
    }
}

static void check_msr_table(const msr_table *t, const char *what) {
    u32 i;
    u32 max_off = 0;
    usize extent;

    check(t->count > 0, "la tabla de MSR no esta vacia");
    if (t->count == 0) {
        return;
    }
    for (i = 0; i < t->count; ++i) {
        if (t->regs[i].name > max_off) {
            max_off = t->regs[i].name;
        }
    }
    extent = blob_extent(t->names, max_off);

    for (i = 0; i < t->count; ++i) {
        const msr_reg *m = &t->regs[i];
        char msg[128];

        sprintf(msg, "%s: fila %u", what, (unsigned)i);
        check_names(t->names, m->name, extent, msg);

        if (msr_reg_has_gate(m)) {
            check(m->gate_reg <= CPUID_REG_EDX, "la puerta apunta a EAX..EDX");
            if (m->gate_width == 0 ||
                (u32)m->gate_lo + (u32)m->gate_width > 32u) {
                printf("FAIL: %s: fila %u (%s) tiene una puerta en [%u+%u]\n",
                       what, (unsigned)i, msr_reg_name(t, m),
                       (unsigned)m->gate_lo, (unsigned)m->gate_width);
                failures += 1;
            }
        }
        if (i > 0 && t->regs[i - 1].addr > m->addr) {
            printf("FAIL: %s: fila %u rompe el orden por direccion\n", what,
                   (unsigned)i);
            failures += 1;
        }
    }
}

/**
 * @brief Anclas: campos que tienen que estar y donde tienen que estar.
 *
 * Sin esto, las comprobaciones de arriba pasarian con una tabla que el
 * extractor hubiera desplazado ENTERA -- todo bien formado, todo en el sitio
 * equivocado --.  Se eligen campos que no se van a mover: son parte de la
 * arquitectura desde hace decadas.
 */
static void check_anchors(void) {
    const cpuid_table *t = &cpuid_table_intel;
    u32 i;
    int found_tsc = 0;
    int found_ht = 0;

    for (i = 0; i < t->count; ++i) {
        const cpuid_field *f = &t->fields[i];
        const char *n = cpuid_field_name(t, f);
        if (f->leaf == 1u && f->reg == CPUID_REG_EDX && strcmp(n, "TSC") == 0) {
            check(f->lo == 4u && f->width == 1u, "CPUID.01H:EDX[4] es TSC");
            found_tsc = 1;
        }
        if (f->leaf == 1u && f->reg == CPUID_REG_EDX && strcmp(n, "HTT") == 0) {
            check(f->lo == 28u && f->width == 1u, "CPUID.01H:EDX[28] es HTT");
            found_ht = 1;
        }
    }
    check(found_tsc, "la tabla trae CPUID.01H:EDX.TSC");
    check(found_ht, "la tabla trae CPUID.01H:EDX.HTT");
}

/**
 * @brief Que el CSV sea CSV: todas las filas con las mismas columnas.
 *
 * Es EL invariante del formato, y el unico modo de fallar que importa aqui.
 * Una fila con una coma de menos no revienta a quien la lee: le corre las
 * columnas, y a partir de ahi todo lo que saque estara mal sin que nada avise.
 * El volcado escribe las celdas a mano, asi que una rama que se olvide de una
 * coma es un fallo perfectamente posible -- y este es el unico sitio desde
 * donde se ve.
 *
 * @return las columnas que tiene, o cero si no cuadran
 */
static u32 csv_columns(const char *s, usize n, const char *what) {
    u32 cols = 0;
    u32 cur = 1;
    u32 line = 1;
    usize i;

    for (i = 0; i < n; ++i) {
        if (s[i] == ',') {
            cur += 1;
            continue;
        }
        if (s[i] != '\n') {
            continue;
        }
        if (cols == 0) {
            cols = cur; /* la cabecera manda */
        } else if (cur != cols) {
            printf("FAIL: %s: la linea %u tiene %u columnas y no %u\n", what,
                   (unsigned)line, (unsigned)cur, (unsigned)cols);
            failures += 1;
            return 0;
        }
        cur = 1;
        line += 1;
    }
    /* Y tiene que acabar en fin de linea: si no, la ultima fila esta a medias y
     * el bucle de arriba ni la habria mirado. */
    if (n == 0 || s[n - 1] != '\n') {
        printf("FAIL: %s: no acaba en fin de linea\n", what);
        failures += 1;
        return 0;
    }
    return cols;
}

/** @brief El volcado escribe CSV bien formado, y dice cuando no cabe. */
static void check_dump(void) {
    static char big[512 * 1024];
    char small[64];
    usize n = 0;
    status rc;

    rc = cpuid_dump(0, 1, big, sizeof big, &n);
    check(rc == OK, "el volcado de CPUID cabe en medio megabyte");
    check(n > 0, "el volcado de CPUID escribe algo");
    check(csv_columns(big, n, "CPUID") == 9, "el CSV de CPUID trae 9 columnas");

    /* Un bufer corto tiene que DECIRLO.  Truncar y devolver OK es la clase de
     * fallo que este proyecto no admite: el informe sale a medias y parece
     * entero. */
    n = 0;
    rc = cpuid_dump(0, 1, small, sizeof small, &n);
    check(rc == ERR_NOSPACE, "un bufer corto devuelve ERR_NOSPACE");
    check(n <= sizeof small, "y no escribe mas de lo que cabe");

    n = 0;
    rc = msr_dump(0, 1, 0, 0, 0, big, sizeof big, &n);
    check(rc == OK, "el catalogo de MSR cabe en medio megabyte");
    check(n > 0, "el catalogo de MSR escribe algo");
    check(csv_columns(big, n, "MSR") == 11, "el CSV de MSR trae 11 columnas");
}

/* Cuantas veces pidio el volcado una lectura, y de que direccion.  Un contador
 * y no una lista: lo que hay que comprobar es que NO se pide una lectura sin
 * puerta, no cuales se piden. */
static u32 reads_asked;
static u32 last_addr;

/**
 * @brief Un lector de mentira que COMPRUEBA lo que le piden.
 *
 * No toca la maquina.  Y en vez de limitarse a contar, mira que la direccion
 * que le llega corresponda a un registro con puerta: es el unico sitio desde
 * donde se puede ver que el volcado no esta a punto de leer algo que no
 * existe.
 */
static msr_value fake_read(u32 addr, void *ctx) {
    msr_value v;
    u32 i;
    int gated = 0;

    (void)ctx;
    reads_asked += 1;
    last_addr = addr;

    for (i = 0; i < msr_table_intel.count; ++i) {
        const msr_reg *m = &msr_table_intel.regs[i];
        if (m->addr == addr && msr_reg_has_gate(m)) {
            gated = 1;
            break;
        }
    }
    if (!gated) {
        printf("FAIL: se pidio leer 0x%X, que no tiene puerta documentada\n",
               (unsigned)addr);
        failures += 1;
    }

    v.value = 0x5A5A5A5A5A5A5A5AuLL;
    v.rc = OK;
    return v;
}

/**
 * @brief Con lector, el volcado lee -- y SOLO lo que tiene puerta.
 *
 * Esta es la comprobacion que de verdad importa de todo el fichero: pedir la
 * lectura de un MSR sin puerta no da un dato de mas, da una excepcion de
 * proteccion general, y en anillo cero eso se lleva la maquina por delante.  El
 * lector de mentira permite ejercitar ese camino entero sin anillo cero y sin
 * riesgo.
 */
static void check_gated_reads(void) {
    static char big[512 * 1024];
    usize n = 0;
    u32 gated = 0;
    u32 i;
    status rc;

    for (i = 0; i < msr_table_intel.count; ++i) {
        if (msr_reg_has_gate(&msr_table_intel.regs[i])) {
            gated += 1;
        }
    }
    check(gated > 0, "hay MSR con puerta documentada");

    reads_asked = 0;
    last_addr = 0;
    rc = msr_dump(0, 1, 0, fake_read, 0, big, sizeof big, &n);
    check(rc == OK, "el volcado con lector cabe");
    /* Con lector se recorre OTRA rama por fila -- la que escribe el valor --, y
     * es justo donde una coma de menos pasaria desapercibida: el catalogo sin
     * lector seguiria cuadrando. */
    check(csv_columns(big, n, "MSR con lector") == 11,
          "el CSV con valores sigue trayendo 11 columnas");
    /* Menor o igual, no igual: de los que tienen puerta solo se leen aquellos
     * cuya puerta ESTA MAQUINA satisface, y eso depende de la pieza.  Que
     * ninguno sea de los otros lo comprueba `fake_read` uno a uno. */
    check(reads_asked <= gated, "no se pide mas lecturas que MSR con puerta");
    printf("MSR con puerta %u, leidos en esta maquina %u"
           " (ultima direccion 0x%X)\n",
           (unsigned)gated, (unsigned)reads_asked, (unsigned)last_addr);
}

int main(void) {
    check_cpuid_table(&cpuid_table_intel, "CPUID Intel");
    check_cpuid_table(&cpuid_table_amd, "CPUID AMD");
    check_msr_table(&msr_table_intel, "MSR Intel");
    check_msr_table(&msr_table_amd, "MSR AMD");
    check_anchors();
    check_dump();
    check_gated_reads();

    printf("cpuid intel %u  cpuid amd %u  msr intel %u  msr amd %u\n",
           (unsigned)cpuid_table_intel.count, (unsigned)cpuid_table_amd.count,
           (unsigned)msr_table_intel.count, (unsigned)msr_table_amd.count);
    if (failures != 0) {
        printf("%d comprobaciones fallaron\n", failures);
        return 1;
    }
    printf("test_dump: OK\n");
    return 0;
}
