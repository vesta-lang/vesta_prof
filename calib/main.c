/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file calib/main.c
 * @brief
 * \~english `vxp_calib`: what the instructions of THIS machine really cost.
 * \~spanish `vxp_calib`: lo que cuestan de verdad las instrucciones de ESTA
 *           maquina.
 * \~
 *
 * \~english
 * WHAT IT IS FOR.  A profiler that only says where the time goes is a listing.
 * To say whether that is a lot or a little you need what the code SHOULD have
 * cost, and that comes from a table of latencies and ports per
 * microarchitecture.  This measures the same thing the table claims, on the part
 * in front of you, so the two can be compared.
 *
 * Two uses, and the second is the interesting one:
 *
 *   - a microarchitecture nobody has published yet: measure it and there is a
 *     table;
 *   - a microarchitecture that IS published: measure it and you know how far the
 *     table is from this machine.  That difference is not a curiosity -- it is
 *     the difference between "this code is slow" and "the model is lying", and
 *     nothing else can tell them apart.
 *
 * IT NEEDS THE DRIVER LOADED.  It reads the counters with `rdpmc`, which is only
 * legal in user mode once something has set `CR4.PCE`.  Without it the process
 * dies with a privileged-instruction fault, and that is the right way to fail:
 * loud, immediate, and pointing at the cause.
 *
 * THE METHOD IS THE MINIMUM, not the average.  Anything that gets in the way --
 * a preemption, an interrupt, another thread on the core -- can only ADD cycles.
 * So the minimum of many runs converges on the real cost, and the spread above it
 * is the noise, which is worth showing rather than hiding.
 *
 * \~spanish
 * PARA QUE SIRVE.  Un perfilador que solo dice donde se va el tiempo es un
 * listado.  Para decir si eso es mucho o poco hace falta lo que el codigo
 * DEBERIA haber costado, y eso sale de una tabla de latencias y puertos por
 * microarquitectura.  Esto mide lo mismo que la tabla afirma, en la pieza que
 * tienes delante, para poder comparar las dos.
 *
 * Dos usos, y el segundo es el interesante:
 *
 *   - una microarquitectura que nadie ha publicado: se mide y ya hay tabla;
 *   - una microarquitectura que SI esta publicada: se mide y se sabe cuanto se
 *     aparta la tabla de esta maquina.  Esa diferencia no es una curiosidad --
 *     es la diferencia entre "este codigo va lento" y "el modelo miente", y no
 *     hay otra cosa que las distinga.
 *
 * NECESITA EL DRIVER CARGADO.  Lee los contadores con `rdpmc`, que solo es legal
 * en modo usuario si alguien puso `CR4.PCE`.  Sin eso el proceso muere con una
 * excepcion de instruccion privilegiada, y esa es la forma correcta de fallar:
 * ruidosa, inmediata y senalando la causa.
 *
 * EL METODO ES EL MINIMO, no la media.  Cualquier cosa que se cruce -- una
 * expropiacion, una interrupcion, otro hilo en el nucleo -- solo puede SUMAR
 * ciclos.  Asi que el minimo de muchas corridas converge al coste real, y la
 * dispersion por encima es el ruido, que conviene ensenar en vez de esconder.
 */

#include "asm/x86_64/calib.h"
#include "count/rdpmc.h"
#include "vxarch.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* \~english Which counter holds what.  It is the order `counter_arm` programs
 * them in, walking the architectural events in the manual's order: 0 is core
 * cycles and 1 is instructions retired.
 * \~spanish Que lleva cada contador.  Es el orden en que los programa
 * `counter_arm`, recorriendo los eventos arquitectonicos en el orden del manual:
 * el 0 son ciclos de nucleo y el 1 instrucciones retiradas. \~ */
#define PMC_CYCLES 0u
#define PMC_INSTRS 1u

/* \~english How many times each kernel is run.  The minimum needs enough tries
 * to have hit at least one clean one; a dozen is plenty because a call is
 * milliseconds and the machine is not busy the whole time.
 * \~spanish Cuantas veces se corre cada nucleo.  El minimo necesita intentos
 * suficientes como para haber pillado al menos uno limpio; una docena sobra
 * porque una llamada son milisegundos y la maquina no esta ocupada todo el
 * rato. \~ */
#define RUNS 15u

/**
 * @brief
 * \~english One measured kernel and the band its result should fall in.
 * \~spanish Un nucleo medido y la banda en la que deberia caer su resultado.
 * \~
 *
 * \~english
 * THE BAND IS A SANITY CHECK, NOT A TRUTH.  It is not what the table says the
 * cost is -- comparing against the table is the next step and needs the table.
 * It is what is true of ANY x86-64 part: an addition takes about one cycle, a
 * multiplication several, a first-level cache hit a handful.  If a result falls
 * outside, what is broken is the bench, and knowing that before believing
 * anything is the whole point of having a band.
 *
 * \~spanish
 * LA BANDA ES UNA COMPROBACION DE CORDURA, NO UNA VERDAD.  No es lo que la tabla
 * dice que cuesta -- comparar contra la tabla es el paso siguiente y necesita la
 * tabla.  Es lo que es cierto de CUALQUIER pieza x86-64: una suma cuesta
 * alrededor de un ciclo, una multiplicacion varios, un acierto en la primera
 * cache un punado.  Si un resultado se sale, lo que esta roto es el banco, y
 * saberlo antes de creerse nada es justo para lo que sirve una banda.
 */
typedef struct kernel {
    const char *name;
    void (*run)(void);
    const char *what;
    double lo;
    double hi;
    /**
     * \~english Which form of the cost table this kernel measures, and which
     * half of its cost the measurement corresponds to.  `form` is 0 when the
     * kernel measures nothing comparable -- the empty loop.
     * \~spanish Que forma de la tabla de coste mide este nucleo, y a que mitad
     * de su coste corresponde la medida.  `form` es 0 cuando el nucleo no mide
     * nada comparable -- el bucle vacio. \~
     */
    u32 form;
    const char *uid; /**< \~english the form's stable name \~spanish el nombre estable de la forma \~ */
    int is_latency;
} kernel;

/**
 * @brief
 * \~english The kernels, and which form of the table each one measures.
 * \~spanish Los nucleos, y que forma de la tabla mide cada uno.
 * \~
 *
 * \~english
 * THE FORM NUMBER IS WRITTEN DOWN HERE AND IT IS NOT STABLE, which is why the
 * `uid` beside it is not decoration.  The number is a dense index the database
 * build assigns; regenerate the database and it can come to mean another
 * instruction, with no error anywhere -- the comparison would simply be against
 * the wrong row.
 *
 * What makes it safe is `CALIB_TABLE_HASH`: the file says which data it was
 * built from, and if that does not match what these numbers were taken from,
 * `--check` refuses instead of comparing.
 *
 * \~spanish
 * EL NUMERO DE FORMA SE APUNTA AQUI Y NO ES ESTABLE, que es por lo que el `uid`
 * de al lado no es adorno.  El numero es un indice denso que asigna la
 * construccion de la base; se regenera la base y puede pasar a significar otra
 * instruccion, sin ningun error en ningun sitio -- la comparacion seria
 * simplemente contra la fila equivocada.
 *
 * Lo que lo hace seguro es `CALIB_TABLE_HASH`: el fichero dice de que datos
 * salio, y si eso no cuadra con aquello de lo que se sacaron estos numeros,
 * `--check` se niega en vez de comparar.
 */
static const kernel kernels[] = {
    {"empty", calib_empty, "el bucle vacio (por vuelta)", 0.0, 4.0, 0, 0, 0},
    {"add.lat", calib_add_lat, "add r64,r64  latencia", 0.7, 2.0, 340,
     "ADD_GPRv_GPRv_01/64x64", 1},
    {"add.tp", calib_add_tp, "add r64,r64  throughput", 0.15, 1.2, 340,
     "ADD_GPRv_GPRv_01/64x64", 0},
    {"imul.lat", calib_imul_lat, "imul r64,r64  latencia", 2.5, 6.0, 2704,
     "IMUL_GPRv_GPRv/64x64", 1},
    {"imul.tp", calib_imul_tp, "imul r64,r64  throughput", 0.4, 2.0, 2704,
     "IMUL_GPRv_GPRv/64x64", 0},
    {"load.lat", calib_load_lat, "mov r64,[r64]  latencia L1", 3.0, 8.0, 1033,
     "MOV_GPRv_MEMv/64x64", 1}};

/** @brief
 *  \~english The data the form numbers above were taken from.
 *  \~spanish Los datos de los que se sacaron los numeros de forma de arriba. \~ */
#define CALIB_TABLE_HASH                                                       \
    "e5e702caaf04c4fc1f75192a9bbe4d8554b3e174f7933a14d124cc33ca677ab3"

#define KERNEL_COUNT (sizeof(kernels) / sizeof(kernels[0]))

/** @brief
 *  \~english Are two texts the same?
 *  \~spanish ¿Son iguales dos textos? \~ */
static int same_text(const char *a, const char *b) {
    usize i = 0;
    while (a[i] != 0 && a[i] == b[i]) {
        i += 1;
    }
    return a[i] == b[i];
}

/** @brief
 *  \~english What one run of a kernel cost.
 *  \~spanish Lo que costo una corrida de un nucleo. \~ */
typedef struct sample {
    u64 cycles;
    u64 instrs;
} sample;

/**
 * @brief
 * \~english Runs a kernel once with the counters bracketed.
 * \~spanish Corre un nucleo una vez con los contadores acotados.
 * \~
 *
 * \~english
 * THE FENCES ARE NOT DECORATION.  `rdpmc` does not serialise, so without them
 * the processor can start the kernel before the first read has happened, or
 * finish it after the second: the difference comes out smaller than what
 * actually ran, and nothing anywhere says so.  Measured on this tree: two
 * back-to-back reads gave 3 cycles unfenced and 49 fenced.
 *
 * \~spanish
 * LAS VALLAS NO SON ADORNO.  `rdpmc` no serializa, asi que sin ellas el
 * procesador puede empezar el nucleo antes de que la primera lectura haya
 * ocurrido, o acabarlo despues de la segunda: la diferencia sale menor que lo que
 * de verdad corrio, y no lo dice nada en ninguna parte.  Medido en este arbol:
 * dos lecturas seguidas dieron 3 ciclos sin valla y 49 con ella.
 */
static sample run_once(void (*run)(void)) {
    sample s;
    u64 c0, i0, c1, i1;

    __asm__ __volatile__("lfence" ::: "memory");
    c0 = rdpmc(PMC_CYCLES);
    i0 = rdpmc(PMC_INSTRS);
    __asm__ __volatile__("lfence" ::: "memory");

    run();

    __asm__ __volatile__("lfence" ::: "memory");
    c1 = rdpmc(PMC_CYCLES);
    i1 = rdpmc(PMC_INSTRS);
    __asm__ __volatile__("lfence" ::: "memory");

    s.cycles = c1 - c0;
    s.instrs = i1 - i0;
    return s;
}

/** @brief
 *  \~english Pins to one logical processor, so every read is from the same core.
 *  \~spanish Se fija a un procesador logico, para que todas las lecturas sean del
 *            mismo nucleo. \~ */
static int pin(unsigned cpu) {
#ifdef _WIN32
    if (cpu >= 64u ||
        SetThreadAffinityMask(GetCurrentThread(), ((DWORD_PTR)1) << cpu) == 0) {
        return 0;
    }
    Sleep(0); /* \~english yield so the scheduler applies it \~spanish cede para que el planificador la aplique \~ */
    return 1;
#else
    (void)cpu;
    return 1;
#endif
}

/**
 * @brief
 * \~english Compares what was measured against what the table says.
 * \~spanish Compara lo medido con lo que dice la tabla.
 * \~
 *
 * @param path   \~english a `.vxarch` \~spanish un `.vxarch` \~
 * @param medido \~english cycles per instruction, per kernel \~spanish ciclos por instruccion, por nucleo \~
 * @return \~english 0 if everything agreed \~spanish 0 si todo cuadro \~
 *
 * \~english
 * THIS IS THE POINT OF THE WHOLE TOOL.  Up to here it says what the machine
 * costs; from here it says how far the model is from the machine, which is what
 * separates "this code is slow" from "the model is lying".
 *
 * IT REFUSES BEFORE COMPARING IF THE HASHES DIFFER.  The form numbers are dense
 * indices of one particular build of the database; against a table built from
 * other data they would point at other instructions, and the comparison would
 * come out full of enormous, meaningless deviations -- or worse, small ones.
 *
 * \~spanish
 * ESTO ES PARA LO QUE SIRVE LA HERRAMIENTA ENTERA.  Hasta aqui dice lo que
 * cuesta la maquina; a partir de aqui dice cuanto se aparta el modelo de la
 * maquina, que es lo que separa "este codigo va lento" de "el modelo miente".
 *
 * SE NIEGA ANTES DE COMPARAR SI LOS RESUMENES NO CUADRAN.  Los numeros de forma
 * son indices densos de UNA construccion concreta de la base; contra una tabla
 * construida con otros datos apuntarian a otras instrucciones, y la comparacion
 * saldria llena de desvios enormes y sin sentido -- o peor, pequenos.
 */
static int compare(const char *path, const double *medido) {
    static char buf[2u * 1024u * 1024u];
    static vxarch_class classes[4096];
    static u16 form_class[65536];
    vxarch a;
    FILE *f;
    usize n;
    usize k;
    int malos = 0;

    f = fopen(path, "rb");
    if (f == 0) {
        fprintf(stderr, "vxp_calib: no se puede abrir %s\n", path);
        return 2;
    }
    n = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    if (n == 0 || n == sizeof(buf)) {
        fprintf(stderr, "vxp_calib: %s no cabe o esta vacio\n", path);
        return 2;
    }

    a.classes = classes;
    a.class_cap = (u32)(sizeof(classes) / sizeof(classes[0]));
    a.form_class = form_class;
    a.form_cap = (u32)(sizeof(form_class) / sizeof(form_class[0]));
    if (vxarch_parse(buf, n, &a) != OK) {
        fprintf(stderr, "vxp_calib: %s no es un .vxarch que se entienda\n",
                path);
        return 2;
    }

    printf("\n  contra la tabla: %s  (%u clases, %u formas con coste)\n",
           a.name, (unsigned)a.class_count, (unsigned)a.form_count);

    /* \~english THREE CASES AND NOT TWO, and the first version had two.  A table
     * that does not SAY where it came from is not a table that says something
     * else, and treating them the same refuses exactly the tables most worth
     * comparing: a fused one -- uops.info completed with LLVM -- carries no
     * single source hash, because it does not have one source.
     *
     * So: matching, compare; different, refuse; absent, compare AND SAY that it
     * could not be checked.  Refusing there would leave the tool unable to look
     * at the tables it is most often handed; comparing in silence would hide
     * that the form numbers were not verified against anything.
     *
     * \~spanish TRES CASOS Y NO DOS, y la primera version tenia dos.  Una tabla
     * que no DICE de donde salio no es una tabla que diga otra cosa, y tratarlas
     * igual rechaza justamente las tablas que mas interesa comparar: una
     * fusionada -- uops.info completada con LLVM -- no lleva un resumen de
     * origen unico, porque no tiene un origen unico.
     *
     * Asi que: si cuadra, se compara; si difiere, se rechaza; si no lo dice, se
     * compara Y SE DICE que no se pudo comprobar.  Rechazar ahi dejaria a la
     * herramienta sin poder mirar las tablas que mas le van a dar; comparar en
     * silencio esconderia que los numeros de forma no se contrastaron con
     * nada. \~ */
    if (a.xml_sha256[0] == 0 || same_text(a.xml_sha256, "-")) {
        printf("  AVISO: la tabla no dice de que datos salio (suele ser una\n"
               "  fusionada de varias fuentes).  Se compara, pero los numeros\n"
               "  de forma no se han podido contrastar con nada.\n");
    } else if (!same_text(a.xml_sha256, CALIB_TABLE_HASH)) {
        fprintf(stderr,
                "\n  la tabla salio de otros datos que los numeros de forma de\n"
                "  este banco.  No se compara: los numeros de forma son indices\n"
                "  de UNA construccion y contra otra apuntan a otra cosa.\n"
                "    la tabla:  %s\n"
                "    el banco:  %s\n",
                a.xml_sha256, CALIB_TABLE_HASH);
        return 2;
    }

    printf("  %-10s %-24s %14s %14s %9s\n", "nucleo", "forma", "tabla",
           "medido", "desvio");
    printf("  %-10s %-24s %14s %14s %9s\n", "----------",
           "------------------------", "--------------", "--------------",
           "---------");
    for (k = 0; k < KERNEL_COUNT; ++k) {
        const vxarch_class *c;
        double dice;
        double desvio;

        if (kernels[k].form == 0) {
            continue;
        }
        c = vxarch_cost(&a, kernels[k].form);
        if (c == 0) {
            /* \~english The table has no cost for this form here.  It is said,
             * not skipped: "nobody measured it in this part" is information, and
             * it is precisely what this bench is for.
             * \~spanish La tabla no tiene coste para esta forma aqui.  Se dice,
             * no se salta: "aqui no lo midio nadie" es informacion, y es
             * exactamente para lo que sirve este banco. \~ */
            printf("  %-10s %-24s %14s %14.3f %9s\n", kernels[k].name,
                   kernels[k].uid, "sin coste", medido[k], "-");
            continue;
        }
        dice = (double)(kernels[k].is_latency ? c->latency : c->recip_tp) /
               1000.0;
        desvio = (dice > 0.0) ? (medido[k] - dice) / dice * 100.0 : 0.0;
        if (desvio < -5.0 || desvio > 5.0) {
            malos += 1;
        }
        printf("  %-10s %-24s %14.3f %14.3f %8.1f%%\n", kernels[k].name,
               kernels[k].uid, dice, medido[k], desvio);
    }

    if (malos) {
        printf("\n  %d formas se apartan mas de un 5%%.  Con el banco dentro de\n"
               "  rango, lo que se aparta de la maquina es la TABLA.\n",
               malos);
        return 1;
    }
    printf("\n  la tabla y la maquina coinciden.\n");
    return 0;
}

int main(int argc, char **argv) {
    unsigned cpu = 0;
    const char *table = 0;
    int arg;
    u64 best[KERNEL_COUNT];
    u64 worst[KERNEL_COUNT];
    u64 instrs[KERNEL_COUNT];
    usize k;
    unsigned r;
    int suspect = 0;

    for (arg = 1; arg < argc; ++arg) {
        if (strcmp(argv[arg], "--check") == 0 && arg + 1 < argc) {
            table = argv[++arg];
        } else if (argv[arg][0] != '-') {
            cpu = (unsigned)strtoul(argv[arg], 0, 10);
        } else {
            fprintf(stderr,
                    "usage: vxp_calib [cpu] [--check <fichero.vxarch>]\n"
                    "\n"
                    "Measures instruction latency and throughput on one\n"
                    "logical processor.  Needs the driver loaded: reading\n"
                    "a counter from user mode requires CR4.PCE.\n"
                    "\n"
                    "With --check it also compares what it measured against\n"
                    "what a microarchitecture's cost table claims.\n");
            return 2;
        }
    }
    if (!pin(cpu)) {
        fprintf(stderr, "vxp_calib: cannot pin to cpu %u\n", cpu);
        return 1;
    }

    /* \~english Warm up before measuring anything: the first pass pays
     * instruction-cache misses and a cold branch predictor, and those are not
     * the steady-state cost of anything.
     * \~spanish Calentar antes de medir nada: la primera pasada paga fallos de
     * cache de instrucciones y un predictor de saltos frio, y eso no es el coste
     * en regimen de nada. \~ */
    for (k = 0; k < KERNEL_COUNT; ++k) {
        kernels[k].run();
    }

    for (k = 0; k < KERNEL_COUNT; ++k) {
        best[k] = ~(u64)0;
        worst[k] = 0;
        instrs[k] = 0;
        for (r = 0; r < RUNS; ++r) {
            sample s = run_once(kernels[k].run);
            if (s.cycles < best[k]) {
                best[k] = s.cycles;
                instrs[k] = s.instrs;
            }
            if (s.cycles > worst[k]) {
                worst[k] = s.cycles;
            }
        }
    }

    printf("cpu %u, %u instrucciones bajo prueba por corrida, %u corridas, "
           "se toma el MINIMO\n\n",
           cpu, (unsigned)CALIB_INSTRS, RUNS);
    printf("  %-10s %-28s %9s %8s %9s\n", "nucleo", "que mide",
           "ciclos/ins", "ruido", "cordura");
    printf("  %-10s %-28s %9s %8s %9s\n", "----------",
           "----------------------------", "---------", "--------",
           "---------");

    for (k = 0; k < KERNEL_COUNT; ++k) {
        /* \~english The empty loop is reported PER ITERATION, because it has no
         * instruction under test to divide by.  The rest, per instruction.
         * \~spanish El bucle vacio se reporta POR VUELTA, porque no tiene
         * instruccion bajo prueba entre la que dividir.  El resto, por
         * instruccion. \~ */
        double div = (k == 0) ? (double)CALIB_ITERS : (double)CALIB_INSTRS;
        double per = (double)best[k] / div;
        /* \~english The noise is how much the worst run exceeded the best.  It
         * is shown because a measurement without it is a number that cannot be
         * argued with.
         * \~spanish El ruido es cuanto se paso la peor corrida sobre la mejor.
         * Se ensena porque una medida sin el es un numero con el que no se puede
         * discutir. \~ */
        double noise = (double)(worst[k] - best[k]) / (double)best[k] * 100.0;
        const char *verdict = "ok";

        if (per < kernels[k].lo || per > kernels[k].hi) {
            verdict = "REVISAR";
            suspect += 1;
        }
        printf("  %-10s %-28s %9.3f %7.1f%% %9s\n", kernels[k].name,
               kernels[k].what, per, noise, verdict);
    }

    /* \~english And the check that does not depend on any expectation: the
     * counter of retired instructions has to see what the kernel really
     * executed.  Sixty per iteration plus the loop's decrement and branch.  If
     * this does not add up, the bench is dividing by the wrong number and every
     * figure above is wrong by a constant -- which is exactly the kind of error
     * that looks plausible.
     * \~spanish Y la comprobacion que no depende de ninguna expectativa: el
     * contador de instrucciones retiradas tiene que ver lo que el nucleo
     * ejecuto de verdad.  Sesenta por vuelta mas el decremento y el salto del
     * bucle.  Si esto no cuadra, el banco esta dividiendo por el numero
     * equivocado y todas las cifras de arriba estan mal por un factor constante
     * -- que es justo la clase de error que parece plausible. \~ */
    printf("\n  cuenta de instrucciones, contra lo que el nucleo ejecuta:\n");
    for (k = 0; k < KERNEL_COUNT; ++k) {
        u64 want = ((k == 0) ? 0u : CALIB_INSTRS) + 2u * (u64)CALIB_ITERS;
        double off = ((double)instrs[k] - (double)want) / (double)want * 100.0;
        printf("    %-10s medido %10llu   esperado %10llu   %+6.2f%%\n",
               kernels[k].name, (unsigned long long)instrs[k],
               (unsigned long long)want, off);
        if (off < -2.0 || off > 2.0) {
            suspect += 1;
        }
    }

    if (suspect) {
        printf("\n  %d comprobaciones fuera de rango: el BANCO es sospechoso, "
               "no la maquina.\n",
               suspect);
        return 1;
    }
    printf("\n  todo dentro de rango.\n");

    /* --- \~english and, if asked, against the table
     *     \~spanish y, si se pide, contra la tabla \~ ------------------- */
    if (table != 0) {
        double medido[KERNEL_COUNT];
        for (k = 0; k < KERNEL_COUNT; ++k) {
            medido[k] = (double)best[k] /
                        ((k == 0) ? (double)CALIB_ITERS : (double)CALIB_INSTRS);
        }
        return compare(table, medido);
    }
    return 0;
}
