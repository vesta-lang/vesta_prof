/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file driver.c
 * @brief El `.sys`: pregunta el PMU en CADA procesador logico y deja el
 *        informe en un fichero.
 *
 * ES LO PRIMERO QUE TIENE QUE HACER UN PERFILADOR, y no una herramienta
 * auxiliar: mientras no se sepa que contadores hay, si son precisos y si
 * alguien mas los esta usando, cualquier numero que se publique despues es una
 * suposicion con formato.
 *
 * Y responde a una pregunta que desde espacio de usuario NO SE PUEDE responder.
 * CPUID dice que no hay Debug Store en esta maquina, pero bajo un hipervisor
 * CPUID no es una lectura del silicio -- cada `cpuid` sale por una salida de VM
 * y la respuesta la pone el hipervisor --.  `IA32_MISC_ENABLE` bit 12 es la
 * palabra autorizada, y a un MSR solo se llega desde aqui.
 *
 * POR QUE NUCLEO A NUCLEO.  En una pieza hibrida los nucleos P y los E no
 * tienen por que responder lo mismo, y una sola lectura no dice cual de los dos
 * contesto.  Fijar la afinidad antes de cada pregunta es lo unico que convierte
 * el informe en 24 respuestas en vez de en una repetida 24 veces.
 *
 * LO QUE ESTE DRIVER NO HACE, A PROPOSITO: no crea dispositivo, no expone
 * IOCTL, no programa un solo contador y no deja nada armado.  Solo lee.  Todo
 * lo que arma algo puede dejar la maquina con la PMU programada si falla en
 * medio, y eso es un incremento posterior con su propia descarga ordenada.
 */

#include "nt.h"

#include "dump.h"
#include "msr.h"
#include "pmu_caps.h"

/**
 * @brief Lectura de MSR que sobrevive a que el registro no exista.
 *
 * Vive en `asm/x86_64/msr_guard.S`; se declara aqui porque no tiene cabecera
 * propia -- es una sola funcion y del lado de Windows.
 *
 * @return distinto de cero si leyo; cero si el procesador rechazo la lectura.
 */
int msr_read_guarded(u32 addr, u64 *out);

/* La etiqueta de pool no es opcional: es lo que permite ver desde el depurador
 * quien retiene memoria (`!poolused`).  Sin ella una fuga es una cifra anonima.
 * Se escribe como bytes y no como constante multi-caracter porque eso ultimo es
 * una extension que cada compilador ordena a su manera. */
#define REPORT_POOL_TAG                                                        \
    ((ULONG)('V' | ('X' << 8) | ('P' << 16) | ('P' << 24)))

/* 24 procesadores logicos gastan unos 32 KiB.  Se pide de sobra y de una vez:
 * la regla es reservar al arrancar y nunca mas, asi que no hay opcion de
 * crecer sobre la marcha. */
#define REPORT_BYTES (256u * 1024u)

/* Donde se deja el informe.  `\??\` es el prefijo del kernel para las rutas con
 * letra de unidad. */
#define REPORT_PATH L"\\??\\C:\\vxp_pmu_report.txt"

/* Los dos volcados, cada uno su fichero: son ESQUEMAS distintos y juntarlos
 * daria algo que ningun lector de CSV puede procesar. */
#define CPUID_CSV_PATH L"\\??\\C:\\vxp_cpuid.csv"
#define MSR_CSV_PATH L"\\??\\C:\\vxp_msr.csv"

/* Un volcado de CPUID son ~730 filas por procesador y el de MSR 1.588; con 24
 * procesadores, el de MSR ronda los 4 MiB.  Se pide de sobra y de una vez: la
 * regla es reservar al arrancar y nunca mas. */
#define CSV_BYTES (8u * 1024u * 1024u)

/*
 * ¿Se leen tambien los MSR que el manual no documenta con condicion adyacente?
 *
 * Con guarda, si: `msr_read_guarded` sobrevive a que el registro no exista, y
 * el que no exista sale como `faulted`, que es un dato -- dice que esta pieza
 * no lo tiene.  Son 1.273 de 1.588, o sea la mayor parte del mapa.
 *
 * Se deja en una macro y no cableado porque las dos cosas que se prueban aqui
 * son INDEPENDIENTES y conviene poder separarlas: que el volcado en CSV
 * funcione, y que el guarda aguante en anillo cero.  Mezcladas, un fallo no
 * dice cual de las dos fue.
 */
#ifndef VXP_MSR_GUARDED
#define VXP_MSR_GUARDED 0
#endif

/** @brief ¿Salio bien una llamada al kernel? */
#define NT_SUCCESS(st) (((NTSTATUS)(st)) >= 0)

/**
 * @brief Anade una cadena al informe, sin pasarse del bufer.
 *
 * @param buf destino.
 * @param cap cuanto cabe.
 * @param len cuanto hay escrito ya; se actualiza.
 * @param s   texto terminado en nul.
 *
 * No hay `strlen` ni `memcpy` aqui: en kernel no hay biblioteca estandar, y
 * traerse las de `ntoskrnl` para copiar cuatro rotulos seria mas dependencia
 * que codigo.
 */
static void append_str(char *buf, usize cap, usize *len, const char *s) {
    usize i = 0;
    while (s[i] != 0 && *len < cap) {
        buf[*len] = s[i];
        *len += 1;
        i += 1;
    }
}

/** @brief Anade un entero sin signo en decimal. */
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
 * @brief Pregunta el PMU en un procesador logico concreto.
 *
 * @param group  grupo del procesador.
 * @param bit    su posicion dentro del grupo.
 * @param index  numero global, solo para etiquetar el resultado.
 * @param out    donde dejar lo detectado.
 *
 * @return lo que devolviera la deteccion.
 *
 * La afinidad se pone y se QUITA siempre, incluso si la deteccion fallara:
 * dejar un hilo del sistema fijado a un nucleo es la clase de resto que no da
 * un error, da una maquina que se comporta raro.
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
 * @brief Lee un MSR de verdad, para el volcado.
 *
 * Es lo unico que el volcado no puede hacer por su cuenta: `common/dump.c` no
 * conoce ningun sistema, y a un MSR solo se llega desde anillo cero.  Entra por
 * puntero a funcion para que el mismo fuente sirva aqui y en modo usuario.
 */
static msr_value dump_read_msr(u32 addr, void *ctx) {
    msr_value v;
    (void)ctx;
    v.value = 0;
    /* Con GUARDA: `msr_read_guarded` esta en `asm/x86_64/msr_guard.S` y
     * sobrevive a leer un registro que no existe.  Es lo que permite mirar los
     * 1.273 que el manual no documenta con condicion adyacente en vez de
     * saltarselos -- sin el, cada uno de esos seria una pantalla azul. */
    v.rc = msr_read_guarded(addr, &v.value) ? OK : ERR_FAULT;
    return v;
}

/**
 * @brief Recorre los procesadores logicos volcando CPUID o MSR a un CSV.
 *
 * @param msr distinto de cero para volcar los MSR; cero para CPUID.
 *
 * UN SOLO FICHERO CON TODOS LOS PROCESADORES, y la cabecera solo en el primero:
 * repetirla en medio convierte la tabla en algo que ningun lector de CSV
 * procesa de una pasada.  La columna `cpu` es la que hace que las filas de los
 * veinticuatro convivan sin pisarse.
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

            /* La afinidad se pone y se QUITA siempre, tambien si el volcado se
             * queda sin sitio: dejar un hilo del sistema fijado a un nucleo no
             * da un error, da una maquina que se comporta raro. */
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
                /* Se queda corto o el fabricante no esta en las tablas.  Se
                 * dice EN el fichero: uno cortado que no lo diga parece
                 * completo, y esa es la unica forma de fallar que no se ve. */
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
 * @brief Recorre todos los procesadores logicos y compone el informe.
 * @return cuantos bytes de `buf` se usaron.
 *
 * Se recorre GRUPO A GRUPO preguntando cuantos hay en cada uno, y no
 * suponiendo que los grupos estan llenos: un grupo a medias haria que la
 * cuenta global y las mascaras se separasen, y el sintoma seria medir dos veces
 * el mismo nucleo -- que no falla, solo miente.
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
                /* No deberia pasar -- solo falla con destino nulo --, pero un
                 * informe que se salta un nucleo en silencio es peor que uno
                 * que dice cual se salto. */
                append_str(buf, cap, &len, "cpu ");
                append_u32(buf, cap, &len, emitted);
                append_str(buf, cap, &len, ": detection refused\n\n");
                emitted += 1;
                continue;
            }

            rc = pmu_caps_format(&caps, buf + len, cap - len, &written);
            len += written;
            if (rc != OK) {
                /* Se quedo sin sitio.  Se dice, en vez de entregar un informe
                 * cortado que parezca completo. */
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
 * @brief Escribe un bufer en un fichero, sobrescribiendo.
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
 * @brief Descarga.  No hay nada armado, asi que no hay nada que desarmar.
 *
 * Se declara igualmente porque sin `DriverUnload` el driver no se puede
 * descargar sin reiniciar, y eso convierte cada prueba en un reinicio.
 */
static void driver_unload(PDRIVER_OBJECT driver) {
    (void)driver;
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
               "vesta_prof: unloaded\n");
}

/**
 * @brief Punto de entrada.
 *
 * Todo el trabajo ocurre aqui, a PASSIVE_LEVEL, que es donde reservar memoria y
 * abrir un fichero son operaciones legales.  Nada de esto seria posible desde
 * el manejador de la PMI -- y por eso el camino de las muestras sera un anillo
 * preasignado y no esto.
 */
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registry_path) {
    char *report;
    usize len;
    NTSTATUS st;

    (void)registry_path;
    driver->DriverUnload = driver_unload;

    /* La unica reserva, y en la inicializacion, que es lo que la regla permite.
     * Ver profiler/README.md, seccion "Memoria". */
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

    /* Y los dos volcados completos, cada uno en su fichero.  Van con su propia
     * reserva y no reusando la del informe porque son de otro orden de tamano:
     * 1.588 MSR por 24 procesadores no caben en 256 KiB. */
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
        /* El informe se construyo y no se pudo guardar.  Se dice por la traza,
         * que es lo unico que queda, y se carga igualmente: un fallo al
         * escribir un fichero no es motivo para no estar. */
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                   "vesta_prof: could not write the report, status 0x%08X\n",
                   (unsigned)st);
        return STATUS_SUCCESS;
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
               "vesta_prof: report written to C:\\vxp_pmu_report.txt\n");
    return STATUS_SUCCESS;
}
