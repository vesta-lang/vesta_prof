/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file driver.c
 * @brief
 * \~english The `.sys`: asks the PMU on EVERY logical processor and leaves the
 *           report in a file.
 * \~spanish El `.sys`: pregunta el PMU en CADA procesador logico y deja el
 *           informe en un fichero.
 * \~
 *
 * \~english
 * WHAT A DRIVER IS, FOR WHOEVER DOES NOT WRITE THEM DAILY.  A `.sys` is not a
 * program: it has no `main`, nobody runs it, and it never has a process of its
 * own.  It is a block of code the operating system LOADS INTO ITS OWN ADDRESS
 * SPACE and calls at `DriverEntry`, running in ring 0 -- the privilege level
 * where the `rdmsr` instruction is legal and where a bad pointer does not kill a
 * process, it kills the machine.
 *
 * From that come the three rules that shape everything here: no standard
 * library, memory only at load time, and no waiting for anybody.
 *
 * WHAT IT DOES, ONCE, WHEN IT LOADS:
 *
 *      DriverEntry
 *          |
 *          +--> for each logical processor:
 *          |        pin this thread to it     <- otherwise you read whichever
 *          |        ask CPUID and the MSRs       core the scheduler chose
 *          |        release the pinning
 *          |
 *          +--> writes  C:\vxp_pmu_report.txt   the readable report
 *          +--> writes  C:\vxp_cpuid.csv        every CPUID field x 24
 *          +--> writes  C:\vxp_msr.csv          every MSR x 24
 *
 * IT IS THE FIRST THING A PROFILER HAS TO DO, and not an auxiliary tool: until
 * it is known which counters exist, whether they are precise and whether
 * somebody else is using them, any number published afterwards is a guess with
 * formatting.
 *
 * And it answers a question that from user space CANNOT be answered.  CPUID says
 * there is no Debug Store on this machine, but under a hypervisor CPUID is not a
 * read of the silicon -- every `cpuid` leaves through a VM exit and the answer is
 * put there by the hypervisor.  `IA32_MISC_ENABLE` bit 12 is the authoritative
 * word, and an MSR is only reachable from here.
 *
 * WHY CORE BY CORE.  On a hybrid part the P cores and the E cores need not
 * answer the same, and a single read does not say which of the two replied.
 * Pinning the affinity before each question is the only thing that turns the
 * report into 24 answers instead of one repeated 24 times.
 *
 * WHAT THIS DRIVER DOES NOT DO, ON PURPOSE: it creates no device, exposes no
 * IOCTL, programs not one counter and leaves nothing armed.  It only reads.
 * Anything that arms something can leave the machine with the PMU programmed if
 * it fails halfway, and that is a later increment with its own orderly teardown.
 *
 * \~spanish
 * QUE ES UN DRIVER, PARA QUIEN NO LOS ESCRIBE A DIARIO.  Un `.sys` no es un
 * programa: no tiene `main`, nadie lo ejecuta, y nunca tiene un proceso propio.
 * Es un bloque de codigo que el sistema operativo CARGA DENTRO DE SU PROPIO
 * ESPACIO DE DIRECCIONES y llama en `DriverEntry`, corriendo en anillo cero --
 * el nivel de privilegio donde la instruccion `rdmsr` es legal y donde un
 * puntero malo no mata un proceso, mata la maquina.
 *
 * De ahi salen las tres reglas que dan forma a todo esto: sin biblioteca
 * estandar, memoria solo al cargar, y sin esperar a nadie.
 *
 * QUE HACE, UNA VEZ, AL CARGARSE:
 *
 *      DriverEntry
 *          |
 *          +--> por cada procesador logico:
 *          |        fija este hilo a el       <- si no, se lee del nucleo que
 *          |        pregunta CPUID y los MSR     el planificador haya elegido
 *          |        suelta la fijacion
 *          |
 *          +--> escribe  C:\vxp_pmu_report.txt   el informe legible
 *          +--> escribe  C:\vxp_cpuid.csv        todos los campos CPUID x 24
 *          +--> escribe  C:\vxp_msr.csv          todos los MSR x 24
 *
 * ES LO PRIMERO QUE TIENE QUE HACER UN PERFILADOR, y no una herramienta
 * auxiliar: mientras no se sepa que contadores hay, si son precisos y si alguien
 * mas los esta usando, cualquier numero que se publique despues es una
 * suposicion con formato.
 *
 * Y responde a una pregunta que desde espacio de usuario NO SE PUEDE responder.
 * CPUID dice que no hay Debug Store en esta maquina, pero bajo un hipervisor
 * CPUID no es una lectura del silicio -- cada `cpuid` sale por una salida de VM
 * y la respuesta la pone el hipervisor --.  `IA32_MISC_ENABLE` bit 12 es la
 * palabra autorizada, y a un MSR solo se llega desde aqui.
 *
 * POR QUE NUCLEO A NUCLEO.  En una pieza hibrida los nucleos P y los E no tienen
 * por que responder lo mismo, y una sola lectura no dice cual de los dos
 * contesto.  Fijar la afinidad antes de cada pregunta es lo unico que convierte
 * el informe en 24 respuestas en vez de en una repetida 24 veces.
 *
 * LO QUE ESTE DRIVER NO HACE, A PROPOSITO: no crea dispositivo, no expone IOCTL,
 * no programa un solo contador y no deja nada armado.  Solo lee.  Todo lo que
 * arma algo puede dejar la maquina con la PMU programada si falla en medio, y
 * eso es un incremento posterior con su propia descarga ordenada.
 */

#include "nt.h"

#include "pce.h"

#include "count/counter.h"
#include "msr/access.h"
#include "probe/dump.h"
#include "probe/pmu_caps.h"

/**
 * @brief
 * \~english MSR read that survives the register not existing.
 * \~spanish Lectura de MSR que sobrevive a que el registro no exista.
 * \~
 *
 * \~english
 * It lives in `asm/x86_64/msr_guard.S`; it is declared here because it has no
 * header of its own -- it is a single function and on the Windows side.
 *
 * @return non-zero if it read; zero if the processor refused the read.
 *
 * \~spanish
 * Vive en `asm/x86_64/msr_guard.S`; se declara aqui porque no tiene cabecera
 * propia -- es una sola funcion y del lado de Windows.
 *
 * @return distinto de cero si leyo; cero si el procesador rechazo la lectura.
 */
int msr_read_guarded(u32 addr, u64 *out);

/* \~english A POOL TAG is four characters Windows stores next to every kernel
 * allocation.  It is not optional: it is what lets you see from the debugger who
 * is holding memory (`!poolused`).  Without it a leak is an anonymous figure.
 *
 * It is written as bytes and not as a multi-character constant because the
 * latter is an extension each compiler orders its own way.
 *
 * \~spanish Una ETIQUETA DE POOL son cuatro caracteres que Windows guarda junto
 * a cada reserva del kernel.  No es opcional: es lo que permite ver desde el
 * depurador quien retiene memoria (`!poolused`).  Sin ella una fuga es una cifra
 * anonima.
 *
 * Se escribe como bytes y no como constante multi-caracter porque eso ultimo es
 * una extension que cada compilador ordena a su manera. \~ */
#define REPORT_POOL_TAG                                                        \
    ((ULONG)('V' | ('X' << 8) | ('P' << 16) | ('P' << 24)))

/* \~english 24 logical processors take about 32 KiB.  It is asked for with slack
 * and all at once: the rule is to allocate at start-up and never again, so there
 * is no option of growing along the way.
 * \~spanish 24 procesadores logicos gastan unos 32 KiB.  Se pide de sobra y de
 * una vez: la regla es reservar al arrancar y nunca mas, asi que no hay opcion
 * de crecer sobre la marcha. \~ */
#define REPORT_BYTES (256u * 1024u)

/* \~english Where the report is left.  `\??\` is the kernel's prefix for paths
 * with a drive letter: down here there is no `C:` on its own, because drive
 * letters are a user-space convention that the object manager resolves.
 * \~spanish Donde se deja el informe.  `\??\` es el prefijo del kernel para las
 * rutas con letra de unidad: aqui abajo no existe `C:` a secas, porque las
 * letras de unidad son una convencion de espacio de usuario que resuelve el
 * gestor de objetos. \~ */
#define REPORT_PATH L"\\??\\C:\\vxp_pmu_report.txt"

/* \~english The two dumps, each its own file: they are different SCHEMAS and
 * joining them would give something no CSV reader can process.
 * \~spanish Los dos volcados, cada uno su fichero: son ESQUEMAS distintos y
 * juntarlos daria algo que ningun lector de CSV puede procesar. \~ */
#define CPUID_CSV_PATH L"\\??\\C:\\vxp_cpuid.csv"
#define MSR_CSV_PATH L"\\??\\C:\\vxp_msr.csv"

/* \~english A CPUID dump is ~730 rows per processor and the MSR one 1,588; with
 * 24 processors the MSR one is around 4 MiB.  Asked for with slack and all at
 * once, same rule as above.
 * \~spanish Un volcado de CPUID son ~730 filas por procesador y el de MSR 1.588;
 * con 24 procesadores, el de MSR ronda los 4 MiB.  Se pide de sobra y de una
 * vez, misma regla que arriba. \~ */
#define CSV_BYTES (8u * 1024u * 1024u)

/*
 * \~english
 * Are the MSRs the manual does not document with an adjacent condition read too?
 *
 * With a guard, yes: `msr_read_guarded` survives the register not existing, and
 * one that does not exist comes out as `faulted`, which is data -- it says this
 * part does not have it.  They are 1,273 of 1,588, that is, most of the map.
 *
 * It is left as a macro and not hardwired because the two things being tested
 * here are INDEPENDENT and it is worth being able to separate them: that the CSV
 * dump works, and that the guard holds in ring 0.  Mixed together, a failure
 * does not say which of the two it was.
 *
 * \~spanish
 * ¿Se leen tambien los MSR que el manual no documenta con condicion adyacente?
 *
 * Con guarda, si: `msr_read_guarded` sobrevive a que el registro no exista, y el
 * que no exista sale como `faulted`, que es un dato -- dice que esta pieza no lo
 * tiene.  Son 1.273 de 1.588, o sea la mayor parte del mapa.
 *
 * Se deja en una macro y no cableado porque las dos cosas que se prueban aqui
 * son INDEPENDIENTES y conviene poder separarlas: que el volcado en CSV
 * funcione, y que el guarda aguante en anillo cero.  Mezcladas, un fallo no dice
 * cual de las dos fue.
 * \~
 */
#ifndef VXP_MSR_GUARDED
#define VXP_MSR_GUARDED 0
#endif

/** @brief
 *  \~english Did a kernel call go well?  `NTSTATUS` is negative on failure.
 *  \~spanish ¿Salio bien una llamada al kernel?  `NTSTATUS` es negativo al
 *            fallar. \~ */
#define NT_SUCCESS(st) (((NTSTATUS)(st)) >= 0)

/**
 * @brief
 * \~english Appends a string to the report, without overrunning the buffer.
 * \~spanish Anade una cadena al informe, sin pasarse del bufer.
 * \~
 *
 * @param buf \~english destination \~spanish destino \~
 * @param cap \~english how much fits \~spanish cuanto cabe \~
 * @param len \~english how much is written already; updated \~spanish cuanto hay escrito ya; se actualiza \~
 * @param s   \~english nul-terminated text \~spanish texto terminado en nul \~
 *
 * \~english
 * There is no `strlen` nor `memcpy` here: there is no standard library in the
 * kernel, and pulling in `ntoskrnl`'s to copy four labels would be more
 * dependency than code.
 *
 * \~spanish
 * No hay `strlen` ni `memcpy` aqui: en kernel no hay biblioteca estandar, y
 * traerse las de `ntoskrnl` para copiar cuatro rotulos seria mas dependencia que
 * codigo.
 */
static void append_str(char *buf, usize cap, usize *len, const char *s) {
    usize i = 0;
    while (s[i] != 0 && *len < cap) {
        buf[*len] = s[i];
        *len += 1;
        i += 1;
    }
}

/**
 * @brief
 * \~english Appends an unsigned integer in decimal.
 * \~spanish Anade un entero sin signo en decimal.
 * \~
 *
 * \~english
 * SIXTY-FOUR BITS, because a count is not a small number: a counter is 48 bits
 * wide and a core retires instructions in the thousands of millions per second.
 * Truncating to 32 does not fail -- it publishes a plausible figure, which is
 * worse.  Twenty digits is what the largest value takes.
 *
 * \~spanish
 * SESENTA Y CUATRO BITS, porque una cuenta no es un numero pequeno: un contador
 * tiene 48 bits de ancho y un nucleo retira instrucciones a miles de millones
 * por segundo.  Truncar a 32 no falla -- publica una cifra plausible, que es
 * peor.  Veinte digitos es lo que ocupa el valor mas grande.
 */
static void append_u64(char *buf, usize cap, usize *len, u64 v) {
    char tmp[20];
    int n = 0;
    if (v == 0) {
        append_str(buf, cap, len, "0");
        return;
    }
    while (v > 0 && n < 20) {
        tmp[n] = (char)('0' + (int)(v % 10u));
        v /= 10u;
        n += 1;
    }
    while (n > 0 && *len < cap) {
        n -= 1;
        buf[*len] = tmp[n];
        *len += 1;
    }
}

/** @brief
 *  \~english The same, for a value that is known to be narrow.
 *  \~spanish Lo mismo, para un valor que se sabe estrecho. \~ */
static void append_u32(char *buf, usize cap, usize *len, u32 v) {
    append_u64(buf, cap, len, v);
}

/**
 * @brief
 * \~english Asks the PMU on one specific logical processor.
 * \~spanish Pregunta el PMU en un procesador logico concreto.
 * \~
 *
 * \~english
 * WHY GROUP AND BIT AND NOT A NUMBER.  Windows arranges processors into GROUPS
 * of at most 64, because the affinity mask is a 64-bit word.  A machine with 80
 * processors has two groups, and the processor's identity is the pair
 * `(group, bit within the group)`.  Assuming a single group works on every
 * machine anybody normally tests on, and silently reads the wrong core on the
 * one that has two.
 *
 * @param group \~english the processor's group
 * @param bit   its position within the group
 * @param index global number, only to label the result
 * @param out   where to leave what was detected
 * @return whatever the detection returned
 *
 * The affinity is set and ALWAYS released, even if the detection failed:
 * leaving a system thread pinned to a core is the kind of leftover that does
 * not give an error, it gives a machine that behaves oddly.
 *
 * \~spanish
 * POR QUE GRUPO Y BIT Y NO UN NUMERO.  Windows reparte los procesadores en
 * GRUPOS de como mucho 64, porque la mascara de afinidad es una palabra de 64
 * bits.  Una maquina con 80 procesadores tiene dos grupos, y la identidad del
 * procesador es la pareja `(grupo, bit dentro del grupo)`.  Suponer un solo
 * grupo funciona en cualquier maquina en la que uno prueba normalmente, y lee el
 * nucleo equivocado en silencio justo en la que tiene dos.
 *
 * @param group  grupo del procesador.
 * @param bit    su posicion dentro del grupo.
 * @param index  numero global, solo para etiquetar el resultado.
 * @param out    donde dejar lo detectado.
 * @return lo que devolviera la deteccion.
 *
 * La afinidad se pone y se QUITA siempre, incluso si la deteccion fallara: dejar
 * un hilo del sistema fijado a un nucleo es la clase de resto que no da un
 * error, da una maquina que se comporta raro.
 */
static status detect_on_cpu(USHORT group, ULONG bit, u32 index, pmu_caps *out) {
    GROUP_AFFINITY want;
    GROUP_AFFINITY previous;
    status rc;

    want.Mask = ((KAFFINITY)1) << bit;
    want.Group = group;
    want.Reserved[0] = 0;
    want.Reserved[1] = 0;
    want.Reserved[2] = 0;

    previous.Mask = 0;
    previous.Group = 0;
    previous.Reserved[0] = 0;
    previous.Reserved[1] = 0;
    previous.Reserved[2] = 0;

    KeSetSystemGroupAffinityThread(&want, &previous);
    /* A partir de aqui, y solo hasta la linea de abajo, este hilo corre en el
     * nucleo que se quiere describir. */
    rc = pmu_caps_detect(index, out);
    KeRevertToUserGroupAffinityThread(&previous);
    return rc;
}

/**
 * @brief
 * \~english Reads an MSR for real, for the dump.
 * \~spanish Lee un MSR de verdad, para el volcado.
 * \~
 *
 * \~english
 * It is the only thing the dump cannot do on its own: `common/probe/dump.c`
 * knows no system, and an MSR is only reachable from ring 0.  It comes in as a
 * function pointer so the same source serves here and in user mode.
 *
 * \~spanish
 * Es lo unico que el volcado no puede hacer por su cuenta: `common/probe/dump.c`
 * no conoce ningun sistema, y a un MSR solo se llega desde anillo cero.  Entra
 * por puntero a funcion para que el mismo fuente sirva aqui y en modo usuario.
 */
static msr_value dump_read_msr(u32 addr, void *ctx) {
    msr_value v;
    (void)ctx;
    v.value = 0;
    /* \~english WITH A GUARD: `msr_read_guarded` is in `asm/x86_64/msr_guard.S`
     * and survives reading a register that does not exist.  It is what allows
     * looking at the 1,273 the manual does not document with an adjacent
     * condition instead of skipping them -- without it, each of those would be a
     * blue screen.
     * \~spanish Con GUARDA: `msr_read_guarded` esta en `asm/x86_64/msr_guard.S`
     * y sobrevive a leer un registro que no existe.  Es lo que permite mirar los
     * 1.273 que el manual no documenta con condicion adyacente en vez de
     * saltarselos -- sin el, cada uno de esos seria una pantalla azul. \~ */
    v.rc = msr_read_guarded(addr, &v.value) ? OK : ERR_FAULT;
    return v;
}

/* ===========================================================================
 * \~english COUNTING.  Everything from here to `build_csv` is the counter, and
 * it is a different job from the dump above: the dump ASKS what the part has,
 * this one USES it.
 * \~spanish CONTAR.  Todo de aqui a `build_csv` es el contador, y es un trabajo
 * distinto del volcado de arriba: el volcado PREGUNTA que tiene la pieza, este
 * la USA.
 * ========================================================================= */

/**
 * @brief
 * \~english At which privilege level the events are counted.
 * \~spanish En que nivel de privilegio se cuentan los eventos.
 * \~
 *
 * \~english
 * USER MODE ONLY, and the choice is the measurement.  With `PERFEVTSEL_OS` added
 * the count also picks up every timer interrupt, every page fault and every
 * driver on the machine; to see what a `hello world` does, that is not noise
 * around the signal, it is several times the signal.  Ring 0 work is not the
 * program's, so it is not counted.
 *
 * It is a macro and not a hardwired constant because the opposite measurement --
 * what the kernel does on the program's behalf -- is just as legitimate and
 * should not need editing code to take.
 *
 * \~spanish
 * SOLO MODO USUARIO, y la eleccion es la medida.  Anadiendo `PERFEVTSEL_OS` la
 * cuenta recoge ademas cada interrupcion de reloj, cada fallo de pagina y cada
 * driver de la maquina; para ver lo que hace un `hola mundo`, eso no es ruido
 * alrededor de la senal, es varias veces la senal.  El trabajo en anillo cero no
 * es del programa, asi que no se cuenta.
 *
 * Es una macro y no una constante cableada porque la medida contraria -- lo que
 * el nucleo hace por cuenta del programa -- es igual de legitima y no deberia
 * exigir editar codigo para tomarla.
 */
#ifndef VXP_COUNT_QUALIFIERS
#define VXP_COUNT_QUALIFIERS PERFEVTSEL_USR
#endif

/** @brief
 *  \~english Its own pool tag, so `!poolused` tells the two allocations apart.
 *  \~spanish Etiqueta de pool propia, para que `!poolused` distinga las dos
 *            reservas. \~ */
#define COUNT_POOL_TAG                                                         \
    ((ULONG)('V' | ('X' << 8) | ('P' << 16) | ('C' << 24)))

/** @brief
 *  \~english Where the counts are left when the driver unloads.
 *  \~spanish Donde quedan las cuentas al descargar el driver. \~ */
#define COUNT_CSV_PATH L"\\??\\C:\\vxp_count.csv"

/* \~english 24 processors by 8 counters is under 200 rows; 64 KiB is ample.
 * \~spanish 24 procesadores por 8 contadores no llegan a 200 filas; 64 KiB va
 * de sobra. \~ */
#define COUNT_BYTES (64u * 1024u)

/**
 * @brief
 * \~english One processor's counters, and how arming them went.
 * \~spanish Los contadores de un procesador, y como fue armarlos.
 * \~
 *
 * \~english
 * The result of arming is KEPT, rather than being inferred later from
 * `armed == 0`.  Both a part with no usable PMU and one where somebody else was
 * already counting end up with zero armed counters, and those are different
 * facts: the first says this machine cannot, the second says it could and we
 * gave way.  Collapsing them into one empty cell is how a report starts lying.
 *
 * \~spanish
 * El resultado de armar se GUARDA, en vez de deducirlo despues de `armed == 0`.
 * Tanto una pieza sin PMU utilizable como una donde ya estaba contando otro
 * acaban con cero contadores armados, y son hechos distintos: el primero dice
 * que esta maquina no puede, el segundo que si podia y cedimos.  Juntarlos en
 * una celda vacia es como empieza a mentir un informe.
 */
typedef struct cpu_counters {
    counter_state st;
    status armed_rc;
} cpu_counters;

static cpu_counters *g_counters;
static ULONG g_counter_cpus;

/**
 * @brief
 * \~english The MSR pair the counter uses, in ring 0.
 * \~spanish La pareja de MSR que usa el contador, en anillo cero.
 * \~
 *
 * \~english
 * NO GUARD HERE, deliberately, and it is the opposite decision from the dump's.
 * The dump walks 1,588 addresses of which most may not exist, so it needs to
 * survive a fault.  These four exist by construction: nothing is touched unless
 * `CPUID.0AH` said there is an architectural PMU with that many counters.  A
 * fault here would not be a register that is missing, it would be a bug in the
 * decision that led us to write it -- and swallowing it would hide exactly the
 * thing worth knowing.
 *
 * \~spanish
 * AQUI NO HAY GUARDA, a proposito, y es la decision contraria a la del volcado.
 * El volcado recorre 1.588 direcciones de las que la mayoria puede no existir,
 * asi que necesita sobrevivir a una excepcion.  Estos cuatro existen por
 * construccion: no se toca nada salvo que `CPUID.0AH` haya dicho que hay un PMU
 * arquitectonico con esos contadores.  Una excepcion aqui no seria un registro
 * que falta, seria un fallo en la decision que nos llevo a escribirlo -- y
 * tragarsela escoderia justo lo que interesa saber.
 */
static status kernel_msr_read(u32 addr, u64 *out, void *ctx) {
    (void)ctx;
    return msr_read(addr, out);
}

static status kernel_msr_write(u32 addr, u64 value, void *ctx) {
    (void)ctx;
    return msr_write(addr, value);
}

static const msr_ops g_msr_ops = {kernel_msr_read, kernel_msr_write, 0};

/**
 * @brief
 * \~english Runs something on every logical processor, pinned to each in turn.
 * \~spanish Ejecuta algo en cada procesador logico, fijado a cada uno por turno.
 * \~
 *
 * @param visit \~english what to do, once per processor \~spanish que hacer, una vez por procesador \~
 * @param ctx   \~english passed through untouched \~spanish se pasa tal cual \~
 * @return \~english how many were visited \~spanish cuantos se visitaron \~
 *
 * \~english
 * WHY IT IS A FUNCTION AND NOT COPIED TWICE.  Arming and reading both need the
 * same twelve lines of affinity, and the interesting part of those twelve lines
 * is the one that is easy to leave out: the affinity is released ALWAYS, also
 * when the work failed.  Leaving a system thread pinned to a core does not give
 * an error, it gives a machine that behaves oddly, and a bug like that is much
 * easier to introduce into the second copy than into the only one.
 *
 * \~spanish
 * POR QUE ES UNA FUNCION Y NO SE COPIA DOS VECES.  Armar y leer necesitan las
 * mismas doce lineas de afinidad, y lo interesante de esas doce lineas es
 * justamente lo facil de olvidar: la afinidad se QUITA siempre, tambien cuando
 * el trabajo fallo.  Dejar un hilo del sistema fijado a un nucleo no da un
 * error, da una maquina que se comporta raro, y ese fallo es mucho mas facil de
 * meter en la segunda copia que en la unica.
 */
typedef void (*cpu_visit_fn)(u32 index, void *ctx);

static ULONG for_each_cpu(cpu_visit_fn visit, void *ctx) {
    ULONG total = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    ULONG done = 0;
    USHORT group;

    for (group = 0; group < 64u && done < total; ++group) {
        ULONG in_group = KeQueryActiveProcessorCountEx(group);
        ULONG bit;
        for (bit = 0; bit < in_group && done < total; ++bit) {
            GROUP_AFFINITY want;
            GROUP_AFFINITY previous;

            want.Mask = ((KAFFINITY)1) << bit;
            want.Group = group;
            want.Reserved[0] = 0;
            want.Reserved[1] = 0;
            want.Reserved[2] = 0;
            previous.Mask = 0;
            previous.Group = 0;
            previous.Reserved[0] = 0;
            previous.Reserved[1] = 0;
            previous.Reserved[2] = 0;

            KeSetSystemGroupAffinityThread(&want, &previous);
            visit(done, ctx);
            KeRevertToUserGroupAffinityThread(&previous);
            done += 1;
        }
    }
    return done;
}

/** @brief
 *  \~english Detects and arms, on the processor already pinned to.
 *  \~spanish Detecta y arma, en el procesador al que ya se esta fijado. \~ */
static void arm_one(u32 index, void *ctx) {
    pmu_caps caps;
    (void)ctx;

    if (index >= g_counter_cpus) {
        return;
    }
    /* \~english The capabilities are asked for AGAIN, on this processor, and not
     * reused from the report.  On a hybrid part a P core and an E core do not
     * answer the same -- they do not even have the same number of counters --
     * so one detection for the whole machine would arm several cores with
     * somebody else's answer.
     * \~spanish Las capacidades se preguntan OTRA VEZ, en este procesador, y no
     * se reusan del informe.  En una pieza hibrida un nucleo P y uno E no
     * responden lo mismo -- ni siquiera tienen el mismo numero de contadores --,
     * asi que una sola deteccion para toda la maquina armaria varios nucleos con
     * la respuesta de otro. \~ */
    g_counters[index].armed_rc = pmu_caps_detect(index, &caps);
    if (g_counters[index].armed_rc != OK) {
        return;
    }
    g_counters[index].armed_rc = counter_arm(&caps, VXP_COUNT_QUALIFIERS,
                                             &g_msr_ops, &g_counters[index].st);
}

/**
 * @brief
 * \~english Arms every processor.  From here on, the machine is counting.
 * \~spanish Arma todos los procesadores.  Desde aqui, la maquina esta contando.
 * \~
 *
 * @return `OK`, \~english or `ERR_NOSPACE` if there was no pool \~spanish o `ERR_NOSPACE` si no hubo pool \~
 *
 * \~english
 * THE WINDOW BEING MEASURED starts here and closes at `DriverUnload`.  Whatever
 * runs on the machine in between is inside it -- this counts SYSTEM-WIDE per
 * processor, the same as `perf stat -a`, not one process.  Telling a program
 * apart from the rest is what sampling is for, and it is the next step.
 *
 * IF THE DRIVER DIES WITHOUT UNLOADING, the counters stay armed until somebody
 * reprograms them.  Nothing breaks -- they count into a register nobody reads --
 * but the next tool to look will find the PMU busy.  It is the price of not
 * having an arbiter, and it is the reason `DriverUnload` exists from the first
 * version of this and not from the one where it seemed necessary.
 *
 * \~spanish
 * LA VENTANA QUE SE MIDE empieza aqui y se cierra en `DriverUnload`.  Lo que
 * corra en la maquina entre medias esta dentro -- esto cuenta la MAQUINA ENTERA
 * por procesador, igual que `perf stat -a`, no un proceso --.  Distinguir un
 * programa del resto es para lo que sirve muestrear, y es el paso siguiente.
 *
 * SI EL DRIVER MUERE SIN DESCARGARSE, los contadores se quedan armados hasta que
 * alguien los reprograme.  No se rompe nada -- cuentan hacia un registro que
 * nadie lee -- pero la siguiente herramienta que mire encontrara el PMU ocupado.
 * Es el precio de no tener arbitro, y es la razon de que `DriverUnload` exista
 * desde la primera version de esto y no desde aquella en que hiciera falta.
 */
static status count_arm_all(void) {
    ULONG total = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    usize bytes;
    ULONG i;

    if (total == 0) {
        return ERR_STATE;
    }
    bytes = (usize)total * sizeof(cpu_counters);
    g_counters = (cpu_counters *)ExAllocatePoolWithTag(NonPagedPoolNx, bytes,
                                                       COUNT_POOL_TAG);
    if (g_counters == 0) {
        return ERR_NOSPACE;
    }
    /* \~english Zeroed by hand: there is no `memset` down here, and a state that
     * arrives with rubbish in `saved` would make the disarm write rubbish back
     * into somebody else's registers.
     * \~spanish A cero a mano: aqui abajo no hay `memset`, y un estado que
     * llegara con basura en `saved` haria que el desarmado devolviera basura a
     * los registros de otro. \~ */
    for (i = 0; i < bytes; ++i) {
        ((char *)g_counters)[i] = 0;
    }
    g_counter_cpus = total;

    for_each_cpu(arm_one, 0);
    return OK;
}

/** @brief
 *  \~english Where a reading pass leaves its CSV.
 *  \~spanish Donde deja su CSV una pasada de lectura. \~ */
typedef struct count_sink {
    char *buf;
    usize cap;
    usize len;
} count_sink;

/** @brief
 *  \~english Appends two hex digits, for an event or a umask.
 *  \~spanish Anade dos digitos hexadecimales, para un evento o un umask. \~ */
static void append_hex2(char *buf, usize cap, usize *len, u8 v) {
    static const char digits[] = "0123456789ABCDEF";
    append_str(buf, cap, len, "0x");
    if (*len < cap) {
        buf[*len] = digits[(v >> 4) & 0xFu];
        *len += 1;
    }
    if (*len < cap) {
        buf[*len] = digits[v & 0xFu];
        *len += 1;
    }
}

/** @brief
 *  \~english What arming this processor came to.
 *  \~spanish En que quedo armar este procesador. \~ */
static const char *arm_state_name(status rc) {
    switch (rc) {
    case OK:
        return "ok";
    case ERR_UNSUPPORTED:
        /* \~english No architectural PMU, or not Intel, or no event to arm.
         * \~spanish Sin PMU arquitectonico, o no es Intel, o ningun evento que
         * armar. \~ */
        return "unsupported";
    case ERR_STATE:
        /* \~english Somebody else was already counting and we gave way.
         * \~spanish Ya estaba contando otro y cedimos. \~ */
        return "busy";
    default:
        return "error";
    }
}

/**
 * @brief
 * \~english Reads, disarms, and writes the rows, on the pinned processor.
 * \~spanish Lee, desarma y escribe las filas, en el procesador fijado.
 * \~
 *
 * \~english
 * READING AND DISARMING GO TOGETHER, in that order and without letting go of the
 * processor in between.  Reading first closes the window at a known point;
 * disarming right after gives the machine back what it lent us.  Splitting them
 * into two passes would leave the counters running through the whole second
 * pass, so the first processor would have measured a shorter window than the
 * last -- and nothing would say so.
 *
 * \~spanish
 * LEER Y DESARMAR VAN JUNTOS, en ese orden y sin soltar el procesador entre
 * medias.  Leer primero cierra la ventana en un punto conocido; desarmar justo
 * despues devuelve a la maquina lo que nos presto.  Partirlo en dos pasadas
 * dejaria los contadores corriendo durante toda la segunda, asi que el primer
 * procesador habria medido una ventana mas corta que el ultimo -- y nada lo
 * diria.
 */
static void read_one(u32 index, void *ctx) {
    count_sink *sink = (count_sink *)ctx;
    u64 values[COUNTER_MAX];
    counter_state *st;
    status rc;
    u32 i;

    if (index >= g_counter_cpus) {
        return;
    }
    st = &g_counters[index].st;

    for (i = 0; i < COUNTER_MAX; ++i) {
        values[i] = 0;
    }
    rc = counter_read(st, &g_msr_ops, values);
    counter_disarm(st, &g_msr_ops);

    if (g_counters[index].armed_rc != OK || st->armed == 0) {
        /* \~english One row all the same, saying why there is nothing.  A
         * processor that simply does not appear reads as if it had not been
         * looked at.
         * \~spanish Una fila igualmente, diciendo por que no hay nada.  Un
         * procesador que simplemente no aparece se lee como si no se hubiera
         * mirado. \~ */
        append_u32(sink->buf, sink->cap, &sink->len, index);
        append_str(sink->buf, sink->cap, &sink->len, ",,,,,,,");
        append_str(sink->buf, sink->cap, &sink->len,
                   arm_state_name(g_counters[index].armed_rc));
        append_str(sink->buf, sink->cap, &sink->len, ",\n");
        return;
    }

    for (i = 0; i < st->armed; ++i) {
        const arch_event *e = &arch_events[st->event[i]];

        append_u32(sink->buf, sink->cap, &sink->len, index);
        append_str(sink->buf, sink->cap, &sink->len, ",");
        append_u32(sink->buf, sink->cap, &sink->len, i);
        append_str(sink->buf, sink->cap, &sink->len, ",");
        append_str(sink->buf, sink->cap, &sink->len, e->name);
        append_str(sink->buf, sink->cap, &sink->len, ",");
        append_hex2(sink->buf, sink->cap, &sink->len, e->event);
        append_str(sink->buf, sink->cap, &sink->len, ",");
        append_hex2(sink->buf, sink->cap, &sink->len, e->umask);
        append_str(sink->buf, sink->cap, &sink->len, ",");
        append_u32(sink->buf, sink->cap, &sink->len,
                   (st->qualifiers & PERFEVTSEL_USR) ? 1u : 0u);
        append_str(sink->buf, sink->cap, &sink->len, ",");
        append_u32(sink->buf, sink->cap, &sink->len,
                   (st->qualifiers & PERFEVTSEL_OS) ? 1u : 0u);
        append_str(sink->buf, sink->cap, &sink->len, ",");
        append_str(sink->buf, sink->cap, &sink->len,
                   (rc == OK) ? "ok" : "read_failed");
        append_str(sink->buf, sink->cap, &sink->len, ",");
        if (rc == OK) {
            append_u64(sink->buf, sink->cap, &sink->len, values[i]);
        }
        append_str(sink->buf, sink->cap, &sink->len, "\n");
    }
}

/**
 * @brief
 * \~english Closes the window: reads everything, disarms everything, formats.
 * \~spanish Cierra la ventana: lee todo, desarma todo, formatea.
 * \~
 *
 * @return \~english how many bytes of CSV \~spanish cuantos bytes de CSV \~
 *
 * \~english
 * THE SCHEMA carries what is needed to read a row without knowing this code:
 * which processor, which counter, what it was counting -- by name AND by
 * encoding, so it can be checked against the manual -- at which privilege
 * levels, and whether the figure is worth anything.
 *
 *      cpu,counter,name,event,umask,usr,os,state,value
 *      0,0,CORE_CYC,0x3C,0x00,1,0,ok,4812339
 *      0,1,INTR_RET,0xC0,0x00,1,0,ok,3901882
 *      ...
 *      7,,,,,,,busy,          <- this core was already somebody else's
 *
 * \~spanish
 * EL ESQUEMA lleva lo que hace falta para leer una fila sin conocer este codigo:
 * que procesador, que contador, que estaba contando -- por nombre Y por
 * codificacion, para poder contrastarlo con el manual --, en que niveles de
 * privilegio, y si la cifra vale algo.
 */
static usize count_report(char *buf, usize cap) {
    count_sink sink;
    sink.buf = buf;
    sink.cap = cap;
    sink.len = 0;

    if (g_counters == 0) {
        return 0;
    }
    append_str(buf, cap, &sink.len,
               "cpu,counter,name,event,umask,usr,os,state,value\n");
    for_each_cpu(read_one, &sink);
    return sink.len;
}

/**
 * @brief
 * \~english Walks the logical processors dumping CPUID or MSRs into a CSV.
 * \~spanish Recorre los procesadores logicos volcando CPUID o MSR a un CSV.
 * \~
 *
 * @param msr \~english non-zero to dump the MSRs; zero for CPUID \~spanish distinto de cero para volcar los MSR; cero para CPUID \~
 *
 * \~english
 * ONE SINGLE FILE WITH EVERY PROCESSOR, and the header only on the first:
 * repeating it in the middle turns the table into something no CSV reader
 * processes in one pass.  The `cpu` column is what lets the rows of all
 * twenty-four coexist without stepping on each other.
 *
 *      cpu,leaf,...      <- header, once
 *      0,0x00000000,...  <- pinned to cpu 0
 *      0,0x00000001,...
 *      1,0x00000000,...  <- pinned to cpu 1
 *      ...
 *
 * \~spanish
 * UN SOLO FICHERO CON TODOS LOS PROCESADORES, y la cabecera solo en el primero:
 * repetirla en medio convierte la tabla en algo que ningun lector de CSV procesa
 * de una pasada.  La columna `cpu` es la que hace que las filas de los
 * veinticuatro convivan sin pisarse.
 *
 *      cpu,leaf,...      <- cabecera, una vez
 *      0,0x00000000,...  <- fijado al cpu 0
 *      0,0x00000001,...
 *      1,0x00000000,...  <- fijado al cpu 1
 *      ...
 */
static usize build_csv(char *buf, usize cap, int msr) {
    usize len = 0;
    ULONG total = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    ULONG emitted = 0;
    USHORT group;

    for (group = 0; group < 64u && emitted < total; ++group) {
        ULONG in_group = KeQueryActiveProcessorCountEx(group);
        ULONG bit;
        for (bit = 0; bit < in_group && emitted < total; ++bit) {
            GROUP_AFFINITY want;
            GROUP_AFFINITY previous;
            usize written = 0;
            status rc;

            want.Mask = ((KAFFINITY)1) << bit;
            want.Group = group;
            want.Reserved[0] = 0;
            want.Reserved[1] = 0;
            want.Reserved[2] = 0;
            previous.Mask = 0;
            previous.Group = 0;
            previous.Reserved[0] = 0;
            previous.Reserved[1] = 0;
            previous.Reserved[2] = 0;

            /* \~english The affinity is set and ALWAYS released, also if the
             * dump runs out of room: leaving a system thread pinned to a core
             * does not give an error, it gives a machine that behaves oddly.
             * \~spanish La afinidad se pone y se QUITA siempre, tambien si el
             * volcado se queda sin sitio: dejar un hilo del sistema fijado a un
             * nucleo no da un error, da una maquina que se comporta raro. \~ */
            KeSetSystemGroupAffinityThread(&want, &previous);
            if (msr) {
                rc = msr_dump(emitted, emitted == 0, VXP_MSR_GUARDED,
                              dump_read_msr, 0, buf + len, cap - len,
                              &written);
            } else {
                rc = cpuid_dump(emitted, emitted == 0, buf + len, cap - len,
                                &written);
            }
            KeRevertToUserGroupAffinityThread(&previous);

            len += written;
            if (rc != OK) {
                /* \~english It ran short, or the vendor is not in the tables.
                 * It is said IN the file: a truncated one that does not say so
                 * looks complete, and that is the only way of failing that
                 * cannot be seen.
                 * \~spanish Se queda corto o el fabricante no esta en las
                 * tablas.  Se dice EN el fichero: uno cortado que no lo diga
                 * parece completo, y esa es la unica forma de fallar que no se
                 * ve. \~ */
                append_str(buf, cap, &len, "# truncated at cpu ");
                append_u32(buf, cap, &len, emitted);
                append_str(buf, cap, &len, "\n");
                return len;
            }
            emitted += 1;
        }
    }
    return len;
}

/**
 * @brief
 * \~english Walks every logical processor and composes the report.
 * \~spanish Recorre todos los procesadores logicos y compone el informe.
 * \~
 *
 * @return \~english how many bytes of `buf` were used \~spanish cuantos bytes de `buf` se usaron \~
 *
 * \~english
 * It walks GROUP BY GROUP asking how many there are in each, rather than
 * assuming the groups are full: a half-filled group would make the global count
 * and the masks drift apart, and the symptom would be measuring the same core
 * twice -- which does not fail, it just lies.
 *
 * \~spanish
 * Se recorre GRUPO A GRUPO preguntando cuantos hay en cada uno, y no suponiendo
 * que los grupos estan llenos: un grupo a medias haria que la cuenta global y
 * las mascaras se separasen, y el sintoma seria medir dos veces el mismo nucleo
 * -- que no falla, solo miente.
 */
static usize build_report(char *buf, usize cap) {
    usize len = 0;
    ULONG total;
    ULONG emitted = 0;
    USHORT group;

    total = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

    append_str(buf, cap, &len, "vesta_prof -- PMU capability report\n");
    append_str(buf, cap, &len, "logical processors: ");
    append_u32(buf, cap, &len, (u32)total);
    append_str(buf, cap, &len, "\n\n");

    for (group = 0; group < 64u && emitted < total; ++group) {
        ULONG in_group = KeQueryActiveProcessorCountEx(group);
        ULONG bit;
        for (bit = 0; bit < in_group && emitted < total; ++bit) {
            pmu_caps caps;
            usize written = 0;
            status rc;

            rc = detect_on_cpu(group, bit, emitted, &caps);
            if (rc != OK) {
                /* \~english It should not happen -- it only fails with a null
                 * destination -- but a report that silently skips a core is
                 * worse than one that says which it skipped.
                 * \~spanish No deberia pasar -- solo falla con destino nulo --,
                 * pero un informe que se salta un nucleo en silencio es peor que
                 * uno que dice cual se salto. \~ */
                append_str(buf, cap, &len, "cpu ");
                append_u32(buf, cap, &len, emitted);
                append_str(buf, cap, &len, ": detection refused\n\n");
                emitted += 1;
                continue;
            }

            rc = pmu_caps_format(&caps, buf + len, cap - len, &written);
            len += written;
            if (rc != OK) {
                /* \~english It ran out of room.  It is said, instead of handing
                 * over a truncated report that looks complete.
                 * \~spanish Se quedo sin sitio.  Se dice, en vez de entregar un
                 * informe cortado que parezca completo. \~ */
                append_str(buf, cap, &len, "\n*** report truncated: buffer full ***\n");
                return len;
            }
            append_str(buf, cap, &len, "\n");
            emitted += 1;
        }
    }
    return len;
}

/**
 * @brief
 * \~english Writes a buffer to a file, overwriting.
 * \~spanish Escribe un bufer en un fichero, sobrescribiendo.
 * \~
 *
 * \~english
 * THE `Zw` PREFIX is not decoration: `ZwCreateFile` and `NtCreateFile` are the
 * same call, but the `Zw` form tells the kernel the caller is the kernel itself,
 * so the parameters are not validated as if they came from user space and the
 * previous access mode is `KernelMode`.  Calling the `Nt` form from here would
 * make the system treat our own pointers as untrusted.
 *
 * @return the status of whichever call failed, or `STATUS_SUCCESS`.
 *
 * \~spanish
 * EL PREFIJO `Zw` no es decoracion: `ZwCreateFile` y `NtCreateFile` son la misma
 * llamada, pero la forma `Zw` le dice al kernel que quien llama es el propio
 * kernel, con lo que los parametros no se validan como si vinieran de espacio de
 * usuario y el modo de acceso previo es `KernelMode`.  Llamar a la forma `Nt`
 * desde aqui haria que el sistema tratara nuestros propios punteros como no
 * fiables.
 *
 * @return el estado de la llamada que fallara, o `STATUS_SUCCESS`.
 */
static NTSTATUS write_file(const WCHAR *path, const char *buf, usize len) {
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES attributes;
    IO_STATUS_BLOCK iosb;
    HANDLE file = 0;
    NTSTATUS st;

    RtlInitUnicodeString(&name, path);

    attributes.Length = (ULONG)sizeof(attributes);
    attributes.RootDirectory = 0;
    attributes.ObjectName = &name;
    /* `OBJ_KERNEL_HANDLE` importa: sin el, el descriptor seria del proceso que
     * casualmente estuviera activo al cargar el driver. */
    attributes.Attributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
    attributes.SecurityDescriptor = 0;
    attributes.SecurityQualityOfService = 0;

    iosb.u.Status = 0;
    iosb.Information = 0;

    st = ZwCreateFile(&file, GENERIC_WRITE | SYNCHRONIZE, &attributes, &iosb, 0,
                      FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OVERWRITE_IF,
                      FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE, 0,
                      0);
    if (!NT_SUCCESS(st)) {
        return st;
    }

    st = ZwWriteFile(file, 0, 0, 0, &iosb, (PVOID)(ULONG_PTR)buf, (ULONG)len, 0,
                     0);
    ZwClose(file);
    return st;
}

/**
 * @brief
 * \~english Unload: it closes the measuring window and gives the PMU back.
 * \~spanish Descarga: cierra la ventana de medida y devuelve el PMU.
 * \~
 *
 * \~english
 * THIS IS WHERE THE MEASUREMENT ENDS.  The counters were armed in `DriverEntry`
 * and have been running ever since; here they are read, and what they were on
 * top of is put back.
 *
 * THE ORDER MATTERS AND IT IS NOT THE OBVIOUS ONE.  Reading and disarming happen
 * FIRST, together, processor by processor; only afterwards is the file written.
 * Writing a file is slow -- it opens the object manager, goes down to the
 * filesystem, waits on the disk -- and doing it in the middle of the walk would
 * leave the last processors counting for milliseconds longer than the first
 * ones, giving them a longer window with nothing in the output saying so.
 *
 * @code
 *      DriverEntry     read+disarm       write the file
 *          |               |                   |
 *      ----+===============+-------------------+---->
 *          |<- the window ->|
 *              measured
 * @endcode
 *
 * DISARMING IS ATTEMPTED EVEN IF THE READING FAILED, and the file is written
 * even if there is nothing to say: leaving the machine's counters programmed is
 * worse than an empty report, because the next tool to look finds a PMU that is
 * busy for no reason anybody can see.
 *
 * \~spanish
 * AQUI ES DONDE ACABA LA MEDIDA.  Los contadores se armaron en `DriverEntry` y
 * llevan corriendo desde entonces; aqui se leen, y se devuelve a su sitio lo que
 * habia debajo.
 *
 * EL ORDEN IMPORTA Y NO ES EL OBVIO.  Leer y desarmar van PRIMERO, juntos y
 * procesador a procesador; solo despues se escribe el fichero.  Escribir un
 * fichero es lento -- abre el gestor de objetos, baja al sistema de ficheros,
 * espera al disco -- y hacerlo en mitad del recorrido dejaria a los ultimos
 * procesadores contando milisegundos mas que a los primeros, dandoles una
 * ventana mas larga sin que nada en la salida lo dijera.
 *
 * SE INTENTA DESARMAR AUNQUE LA LECTURA FALLARA, y se escribe el fichero aunque
 * no haya nada que decir: dejar los contadores de la maquina programados es peor
 * que un informe vacio, porque la siguiente herramienta que mire encuentra un
 * PMU ocupado sin que nadie pueda ver por que.
 */
static void driver_unload(PDRIVER_OBJECT driver) {
    char *csv;
    (void)driver;

    /* \~english The switch goes off FIRST, and unconditionally.  It is the only
     * thing here that is the whole machine's rather than ours: leaving `rdpmc`
     * legal in ring 3 after the driver is gone would let any program read the
     * counters, with nobody left who knows why.
     * \~spanish El interruptor se apaga LO PRIMERO, y pase lo que pase.  Es lo
     * unico de aqui que es de la maquina entera y no nuestro: dejar `rdpmc`
     * legal en anillo tres despues de que el driver se vaya permitiria a
     * cualquier programa leer los contadores, sin que quede nadie que sepa por
     * que. \~ */
    pce_restore();

    if (g_counters == 0) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                   "vesta_prof: unloaded, nothing was armed\n");
        return;
    }

    csv = (char *)ExAllocatePoolWithTag(NonPagedPoolNx, COUNT_BYTES,
                                        COUNT_POOL_TAG);
    if (csv == 0) {
        /* \~english No room for the report, but the counters still have to be
         * given back.  A pass with a sink of zero capacity reads and disarms
         * exactly the same; the rows simply do not fit, which is the part we can
         * afford to lose.
         * \~spanish Sin sitio para el informe, pero los contadores hay que
         * devolverlos igual.  Una pasada con un destino de capacidad cero lee y
         * desarma exactamente igual; lo que no cabe son las filas, que es la
         * parte que nos podemos permitir perder. \~ */
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                   "vesta_prof: out of pool for the counts; disarming anyway\n");
        count_report(0, 0);
    } else {
        usize n = count_report(csv, COUNT_BYTES);
        NTSTATUS s = write_file(COUNT_CSV_PATH, csv, n);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                   "vesta_prof: count csv %u bytes, status 0x%08X\n",
                   (unsigned)n, (unsigned)s);
        ExFreePoolWithTag(csv, COUNT_POOL_TAG);
    }

    ExFreePoolWithTag(g_counters, COUNT_POOL_TAG);
    g_counters = 0;
    g_counter_cpus = 0;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
               "vesta_prof: unloaded, counters disarmed\n");
}

/**
 * @brief
 * \~english Entry point.
 * \~spanish Punto de entrada.
 * \~
 *
 * \~english
 * IRQL is Windows' interrupt request level: a per-processor priority that says
 * what is legal to do right now.  All the work happens here at PASSIVE_LEVEL,
 * the lowest one, which is where allocating memory and opening a file are legal
 * operations -- both can block, and blocking above PASSIVE deadlocks the
 * machine.
 *
 * None of this would be possible from the PMI handler, which runs at a high
 * IRQL -- and that is why the sample path will be a pre-allocated ring and not
 * this.
 *
 * \~spanish
 * IRQL es el nivel de peticion de interrupcion de Windows: una prioridad por
 * procesador que dice que es legal hacer en este momento.  Todo el trabajo
 * ocurre aqui a PASSIVE_LEVEL, el mas bajo, que es donde reservar memoria y
 * abrir un fichero son operaciones legales -- las dos pueden bloquear, y
 * bloquear por encima de PASSIVE cuelga la maquina.
 *
 * Nada de esto seria posible desde el manejador de la PMI, que corre a IRQL alta
 * -- y por eso el camino de las muestras sera un anillo preasignado y no esto.
 */
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registry_path) {
    char *report;
    usize len;
    NTSTATUS st;

    (void)registry_path;
    driver->DriverUnload = driver_unload;

    /* \~english The only allocation, and at initialisation, which is what the
     * rule allows.  `NonPagedPoolNx` is kernel memory that is never paged out
     * and is not executable: paged memory cannot be touched above PASSIVE_LEVEL,
     * so anything the sample path may reach has to live here.  See
     * profiler/README.md, "Memoria".
     * \~spanish La unica reserva, y en la inicializacion, que es lo que la regla
     * permite.  `NonPagedPoolNx` es memoria del kernel que nunca se pagina a
     * disco y no es ejecutable: la memoria paginada no se puede tocar por encima
     * de PASSIVE_LEVEL, asi que todo lo que el camino de las muestras pueda
     * alcanzar tiene que vivir aqui.  Ver profiler/README.md, seccion
     * "Memoria". \~ */
    report = (char *)ExAllocatePoolWithTag(NonPagedPoolNx, REPORT_BYTES,
                                           REPORT_POOL_TAG);
    if (report == 0) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                   "vesta_prof: out of pool for the report\n");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    len = build_report(report, REPORT_BYTES);
    st = write_file(REPORT_PATH, report, len);

    ExFreePoolWithTag(report, REPORT_POOL_TAG);

    /* \~english And the two complete dumps, each in its own file.  They get
     * their own allocation rather than reusing the report's because they are of
     * another order of size: 1,588 MSRs by 24 processors do not fit in 256 KiB.
     * \~spanish Y los dos volcados completos, cada uno en su fichero.  Van con
     * su propia reserva y no reusando la del informe porque son de otro orden de
     * tamano: 1.588 MSR por 24 procesadores no caben en 256 KiB. \~ */
    {
        char *csv = (char *)ExAllocatePoolWithTag(NonPagedPoolNx, CSV_BYTES,
                                                  REPORT_POOL_TAG);
        if (csv == 0) {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                       "vesta_prof: out of pool for the CSV dumps\n");
        } else {
            usize n;
            NTSTATUS s;

            n = build_csv(csv, CSV_BYTES, 0);
            s = write_file(CPUID_CSV_PATH, csv, n);
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                       "vesta_prof: cpuid csv %u bytes, status 0x%08X\n",
                       (unsigned)n, (unsigned)s);

            n = build_csv(csv, CSV_BYTES, 1);
            s = write_file(MSR_CSV_PATH, csv, n);
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                       "vesta_prof: msr csv %u bytes, status 0x%08X\n",
                       (unsigned)n, (unsigned)s);

            ExFreePoolWithTag(csv, REPORT_POOL_TAG);
        }
    }

    /* \~english AND THE LAST THING, on purpose: from this line the machine is
     * counting, and the window closes at `DriverUnload`.  Arming before the
     * dumps would put our own work inside the measurement -- four megabytes of
     * CSV and twenty-four trips through the object manager -- which is far more
     * than whatever is being measured afterwards.
     * \~spanish Y LO ULTIMO, a proposito: desde esta linea la maquina esta
     * contando, y la ventana se cierra en `DriverUnload`.  Armar antes de los
     * volcados meteria nuestro propio trabajo dentro de la medida -- cuatro
     * megabytes de CSV y veinticuatro viajes al gestor de objetos --, que es
     * muchisimo mas que lo que se vaya a medir despues. \~ */
    /* \~english And `rdpmc` legal in ring 3, so a program can read its own
     * counters without asking anybody.  It goes BEFORE arming because a counter
     * that is running while the switch is flipped would be read by user mode
     * mid-change; this way, when the first counter starts, the way to read it is
     * already open.
     * \~spanish Y `rdpmc` legal en anillo tres, para que un programa lea sus
     * propios contadores sin pedirselo a nadie.  Va ANTES de armar porque un
     * contador en marcha mientras se acciona el interruptor lo leeria modo
     * usuario a medio cambiar; asi, cuando arranca el primer contador, la via
     * para leerlo ya esta abierta. \~ */
    {
        status pce = pce_enable();
        if (pce != OK) {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                       "vesta_prof: could not enable rdpmc (%d)\n", (int)pce);
        } else {
            /* \~english The two numbers, not one.  Equal means it went through;
             * different means somebody underneath intercepted the write and said
             * nothing, which is the case worth seeing.
             * \~spanish Los dos numeros, no uno.  Iguales quiere decir que
             * entro; distintos, que alguien de debajo intercepto la escritura y
             * no dijo nada, que es el caso que interesa ver. \~ */
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                       "vesta_prof: rdpmc enabled on %u of %u processors\n",
                       (unsigned)pce_enabled_count(),
                       (unsigned)pce_asked_count());
        }
    }

    {
        status arm = count_arm_all();
        if (arm != OK) {
            /* \~english It loads all the same and says so.  A profiler that
             * refuses to load because it could not count leaves the machine
             * without even the dumps it did manage to write.
             * \~spanish Se carga igualmente y lo dice.  Un perfilador que se
             * niegue a cargar porque no pudo contar deja la maquina sin
             * siquiera los volcados que si consiguio escribir. \~ */
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                       "vesta_prof: could not arm the counters (%d)\n",
                       (int)arm);
        } else {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                       "vesta_prof: counting on %u processors\n",
                       (unsigned)g_counter_cpus);
        }
    }

    if (!NT_SUCCESS(st)) {
        /* \~english The report was built and could not be saved.  It is said
         * through the debug trace, which is all that is left, and it loads all
         * the same: failing to write a file is no reason not to be there.
         * \~spanish El informe se construyo y no se pudo guardar.  Se dice por
         * la traza, que es lo unico que queda, y se carga igualmente: un fallo
         * al escribir un fichero no es motivo para no estar. \~ */
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                   "vesta_prof: could not write the report, status 0x%08X\n",
                   (unsigned)st);
        return STATUS_SUCCESS;
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
               "vesta_prof: report written to C:\\vxp_pmu_report.txt\n");
    return STATUS_SUCCESS;
}
