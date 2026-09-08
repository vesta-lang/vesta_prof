/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file dump_main.c
 * @brief `vxp_dump`: vuelca CPUID y el catalogo de MSR de esta maquina.
 *
 * PARA QUE SIRVE.  Para comparar dos piezas, adjuntarlo a un informe de fallo,
 * o enterarse de que una capacidad existe antes de que a nadie se le ocurra
 * preguntar por ella.  Lo que imprime sale de las tablas generadas de los
 * manuales de Intel y AMD, asi que si dice `PDCM` es porque el manual lo llama
 * asi.
 *
 * SALE EN CSV a la salida estandar, sin alinear.  Es la regla de `cli/` llevada
 * al final: quien mide no interpreta.  Alinear columnas fijaria como se ve el
 * volcado aqui dentro, y cambiarlo obligaria a reconstruir el perfilador; en
 * CSV, quien lo quiera alineado hace `column -t -s,` y quien quiera comparar
 * dos maquinas hace `diff`.
 *
 * NO PIDE PRIVILEGIOS, y por eso los MSR salen como CATALOGO -- direccion,
 * nombre y condicion de existencia -- pero sin valor: leer un MSR es anillo
 * cero.  Los valores los da el driver.
 *
 * ES LA CAPA FINA QUE `cli/README.md` describe: aqui no hay logica, solo elegir
 * que se vuelca y escribirlo.  Todo lo que decide vive en `common/dump.c`, que
 * es el mismo fuente que corre dentro del kernel.
 */

#include "dump.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Medio megabyte: el volcado de CPUID de una pieza moderna ronda los cincuenta
 * kilobytes y el catalogo de MSR los doscientos.  Se reserva de una vez en vez
 * de crecer sobre la marcha porque el volcado no vuelve atras: se escribe
 * seguido, y si no cupiera lo dice en vez de truncar. */
#define DUMP_CAP (512u * 1024u)

static int emit(const char *what, status rc, const char *buf, usize n) {
    if (rc == ERR_NOSPACE) {
        fprintf(stderr, "vxp_dump: %s does not fit in %u bytes\n", what,
                (unsigned)DUMP_CAP);
        return 1;
    }
    if (rc != OK) {
        fprintf(stderr, "vxp_dump: %s failed (%d)\n", what, (int)rc);
        return 1;
    }
    fwrite(buf, 1, n, stdout);
    return 0;
}

static void usage(void) {
    /* No hay modo "los dos": son dos ESQUEMAS distintos, y concatenarlos
     * produce algo que ningun lector de CSV puede leer.  Se piden por
     * separado, que es una tuberia mas y ningun formato roto. */
    fprintf(stderr,
            "usage: vxp_dump [cpuid|msr]\n"
            "  cpuid  every CPUID field the manuals document (default)\n"
            "  msr    the MSR catalogue: address, name and gate\n"
            "\n"
            "Writes CSV to stdout.  Pipe through 'column -t -s,' to read it,\n"
            "or diff two of them to compare machines.\n");
}

/**
 * @brief Cuantos procesadores logicos hay, y como fijarse a uno.
 *
 * Se recorren TODOS, igual que hace el driver, para que los dos ficheros sean
 * la misma tabla y se puedan unir o comparar.  En una pieza hibrida no es un
 * lujo: un nucleo P y uno E no responden lo mismo, y un volcado de uno solo no
 * describe la maquina.
 *
 * Fuera de Windows todavia no hay forma de fijarse, asi que se vuelca el
 * procesador que toque y se DICE que es uno solo -- en vez de recorrer sin
 * fijar, que daria filas etiquetadas con un procesador desde el que quiza no se
 * leyeron.
 */
#ifdef _WIN32
static u32 cpu_count(void) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (u32)si.dwNumberOfProcessors;
}

static int cpu_pin(u32 i) {
    DWORD_PTR want = (DWORD_PTR)1 << i;
    if (i >= 64u || SetThreadAffinityMask(GetCurrentThread(), want) == 0) {
        return 0;
    }
    Sleep(0); /* cede, para que el planificador aplique la afinidad */
    return 1;
}
#else
static u32 cpu_count(void) {
    return 1u;
}

static int cpu_pin(u32 i) {
    (void)i;
    return 1;
}
#endif

int main(int argc, char **argv) {
    const char *what = (argc > 1) ? argv[1] : "cpuid";
    char *buf;
    int rc = 0;
    u32 total;
    u32 i;

    if (argc > 2 ||
        (strcmp(what, "cpuid") != 0 && strcmp(what, "msr") != 0)) {
        usage();
        return 2;
    }

    buf = (char *)malloc(DUMP_CAP);
    if (buf == 0) {
        fprintf(stderr, "vxp_dump: out of memory\n");
        return 1;
    }

    total = cpu_count();
    for (i = 0; i < total && rc == 0; ++i) {
        usize n = 0;
        status s;

        if (!cpu_pin(i)) {
            fprintf(stderr, "vxp_dump: cannot pin to cpu %u; skipped\n",
                    (unsigned)i);
            continue;
        }
        /* El volcado y la escritura van en DOS sentencias, no anidados en la
         * llamada.  El orden en que C evalua los argumentos no esta
         * especificado, asi que `emit(..., cpuid_dump(&n), buf, n)` puede leer
         * `n` ANTES de que el volcado la fije -- y entonces escribe cero bytes
         * y sale con exito. */
        if (strcmp(what, "cpuid") == 0) {
            /* La cabecera solo en el primero: repetirla en medio convierte la
             * tabla en algo que ningun lector de CSV procesa de una pasada. */
            s = cpuid_dump(i, i == 0, buf, DUMP_CAP, &n);
            rc = emit("the CPUID dump", s, buf, n);
        } else {
            /* Sin lector de MSR: en modo usuario no lo hay, y la columna
             * `state` lo dice fila por fila en vez de dejar el valor vacio sin
             * motivo. */
            s = msr_dump(i, i == 0, 0, 0, 0, buf, DUMP_CAP, &n);
            rc = emit("the MSR catalogue", s, buf, n);
        }
    }

    free(buf);
    return rc;
}
