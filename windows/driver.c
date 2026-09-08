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

/** @brief
 *  \~english Appends an unsigned integer in decimal.
 *  \~spanish Anade un entero sin signo en decimal. \~ */
static void append_u32(char *buf, usize cap, usize *len, u32 v) {
    char tmp[12];
    int n = 0;
    if (v == 0) {
        append_str(buf, cap, len, "0");
        return;
    }
    while (v > 0 && n < 12) {
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
 * \~english Unload.  Nothing is armed, so there is nothing to disarm.
 * \~spanish Descarga.  No hay nada armado, asi que no hay nada que desarmar.
 * \~
 *
 * \~english
 * It is declared all the same because without a `DriverUnload` the driver cannot
 * be unloaded without rebooting, and that turns every test into a reboot.
 *
 * \~spanish
 * Se declara igualmente porque sin `DriverUnload` el driver no se puede
 * descargar sin reiniciar, y eso convierte cada prueba en un reinicio.
 */
static void driver_unload(PDRIVER_OBJECT driver) {
    (void)driver;
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
               "vesta_prof: unloaded\n");
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
