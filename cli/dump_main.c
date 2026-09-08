/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file dump_main.c
 * @brief
 * \~english `vxp_dump`: dumps this machine's CPUID and MSR catalogue.
 * \~spanish `vxp_dump`: vuelca CPUID y el catalogo de MSR de esta maquina.
 * \~
 *
 * \~english
 * WHAT IT IS FOR.  To compare two parts, attach it to a bug report, or find out
 * a capability exists before it occurs to anyone to ask for it.  What it prints
 * comes from the tables generated from Intel's and AMD's manuals, so if it says
 * `PDCM` it is because the manual calls it that.
 *
 * IT COMES OUT AS CSV on standard output, unaligned.  It is `cli/`'s rule taken
 * to the end: whoever measures does not interpret.  Aligning columns would pin
 * how the dump looks in here, and changing it would force rebuilding the
 * profiler; in CSV, whoever wants it aligned runs `column -t -s,` and whoever
 * wants to compare two machines runs `diff`.
 *
 * IT ASKS FOR NO PRIVILEGES, and that is why the MSRs come out as a CATALOGUE --
 * address, name and existence condition -- but with no value: reading an MSR is
 * ring 0.  The values are the driver's business.
 *
 * IT WALKS EVERY PROCESSOR, pinning itself to each in turn, the same as the
 * driver.  On a hybrid part that is not a luxury: a P core and an E core do not
 * answer the same, and a dump of only one does not describe the machine.  It is
 * also what makes the two files -- this one's and the driver's -- the same table
 * and joinable.
 *
 * IT IS THE THIN LAYER `cli/README.md` describes: there is no logic here, only
 * choosing what gets dumped and writing it.  Everything that decides lives in
 * `common/probe/dump.c`, which is the same source that runs inside the kernel.
 *
 * \~spanish
 * PARA QUE SIRVE.  Para comparar dos piezas, adjuntarlo a un informe de fallo, o
 * enterarse de que una capacidad existe antes de que a nadie se le ocurra
 * preguntar por ella.  Lo que imprime sale de las tablas generadas de los
 * manuales de Intel y AMD, asi que si dice `PDCM` es porque el manual lo llama
 * asi.
 *
 * SALE EN CSV a la salida estandar, sin alinear.  Es la regla de `cli/` llevada
 * al final: quien mide no interpreta.  Alinear columnas fijaria como se ve el
 * volcado aqui dentro, y cambiarlo obligaria a reconstruir el perfilador; en
 * CSV, quien lo quiera alineado hace `column -t -s,` y quien quiera comparar dos
 * maquinas hace `diff`.
 *
 * NO PIDE PRIVILEGIOS, y por eso los MSR salen como CATALOGO -- direccion,
 * nombre y condicion de existencia -- pero sin valor: leer un MSR es anillo
 * cero.  Los valores los da el driver.
 *
 * RECORRE TODOS LOS PROCESADORES, fijandose a cada uno por turno, igual que el
 * driver.  En una pieza hibrida eso no es un lujo: un nucleo P y uno E no
 * responden lo mismo, y un volcado de uno solo no describe la maquina.  Es
 * ademas lo que hace que los dos ficheros -- el de aqui y el del driver -- sean
 * la misma tabla y se puedan unir.
 *
 * ES LA CAPA FINA QUE `cli/README.md` describe: aqui no hay logica, solo elegir
 * que se vuelca y escribirlo.  Todo lo que decide vive en
 * `common/probe/dump.c`, que es el mismo fuente que corre dentro del kernel.
 */

#include "probe/dump.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* \~english Half a megabyte: a modern part's CPUID dump is around fifty
 * kilobytes and the MSR catalogue around two hundred.  It is allocated in one go
 * rather than growing along the way because the dump does not go back: it is
 * written straight through, and if it did not fit it says so instead of
 * truncating.
 * \~spanish Medio megabyte: el volcado de CPUID de una pieza moderna ronda los
 * cincuenta kilobytes y el catalogo de MSR los doscientos.  Se reserva de una
 * vez en vez de crecer sobre la marcha porque el volcado no vuelve atras: se
 * escribe seguido, y si no cupiera lo dice en vez de truncar. \~ */
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
    /* \~english There is no "both" mode: they are two different SCHEMAS, and
     * concatenating them produces something no CSV reader can read.  They are
     * asked for separately, which is one more pipe and no broken format.
     * \~spanish No hay modo "los dos": son dos ESQUEMAS distintos, y
     * concatenarlos produce algo que ningun lector de CSV puede leer.  Se piden
     * por separado, que es una tuberia mas y ningun formato roto. \~ */
    fprintf(stderr,
            "usage: vxp_dump [cpuid|msr]\n"
            "  cpuid  every CPUID field the manuals document (default)\n"
            "  msr    the MSR catalogue: address, name and gate\n"
            "\n"
            "Writes CSV to stdout.  Pipe through 'column -t -s,' to read it,\n"
            "or diff two of them to compare machines.\n");
}

/**
 * @brief
 * \~english How many logical processors there are, and how to pin to one.
 * \~spanish Cuantos procesadores logicos hay, y como fijarse a uno.
 * \~
 *
 * \~english
 * PINNING means telling the scheduler this thread may only run on one specific
 * processor.  Without it the thread migrates whenever it feels like it, and a
 * row would be labelled with a processor it was perhaps not read from.
 *
 * ALL of them are walked, the same as the driver does, so that the two files are
 * the same table and can be joined or compared.  On a hybrid part that is no
 * luxury: a P core and an E core do not answer the same, and a dump of just one
 * does not describe the machine.
 *
 * Outside Windows there is no way to pin yet, so whichever processor comes up is
 * dumped and it is SAID that it is only one -- rather than walking without
 * pinning, which would give rows labelled with a processor they were perhaps not
 * read from.
 *
 * \~spanish
 * FIJARSE quiere decir decirle al planificador que este hilo solo puede correr
 * en un procesador concreto.  Sin eso el hilo migra cuando le parece, y una fila
 * quedaria etiquetada con un procesador desde el que quiza no se leyo.
 *
 * Se recorren TODOS, igual que hace el driver, para que los dos ficheros sean la
 * misma tabla y se puedan unir o comparar.  En una pieza hibrida no es un lujo:
 * un nucleo P y uno E no responden lo mismo, y un volcado de uno solo no
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
    /* \~english yield, so the scheduler applies the affinity
     * \~spanish cede, para que el planificador aplique la afinidad \~ */
    Sleep(0);
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
        /* \~english The dump and the write go in TWO statements, not nested
         * in the call.  The order in which C evaluates arguments is
         * unspecified, so `emit(..., cpuid_dump(&n), buf, n)` may read `n`
         * BEFORE the dump sets it -- and then it writes zero bytes and exits
         * successfully.
         * \~spanish El volcado y la escritura van en DOS sentencias, no
         * anidados en la llamada.  El orden en que C evalua los argumentos no
         * esta especificado, asi que `emit(..., cpuid_dump(&n), buf, n)` puede
         * leer `n` ANTES de que el volcado la fije -- y entonces escribe cero
         * bytes y sale con exito. \~ */
        if (strcmp(what, "cpuid") == 0) {
            /* \~english The header only on the first: repeating it in the
             * middle turns the table into something no CSV reader processes in
             * one pass.
             * \~spanish La cabecera solo en el primero: repetirla en medio
             * convierte la tabla en algo que ningun lector de CSV procesa de
             * una pasada. \~ */
            s = cpuid_dump(i, i == 0, buf, DUMP_CAP, &n);
            rc = emit("the CPUID dump", s, buf, n);
        } else {
            /* \~english No MSR reader: in user mode there is none, and the
             * `state` column says so row by row instead of leaving the value
             * empty with no reason given.
             * \~spanish Sin lector de MSR: en modo usuario no lo hay, y la
             * columna `state` lo dice fila por fila en vez de dejar el valor
             * vacio sin motivo. \~ */
            s = msr_dump(i, i == 0, 0, 0, 0, buf, DUMP_CAP, &n);
            rc = emit("the MSR catalogue", s, buf, n);
        }
    }

    free(buf);
    return rc;
}
