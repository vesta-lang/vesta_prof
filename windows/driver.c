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

#include "pmu_caps.h"

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
 * @brief Escribe el informe en disco.
 * @return el estado de la llamada que fallara, o `STATUS_SUCCESS`.
 */
static NTSTATUS write_report(const char *buf, usize len) {
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES attributes;
    IO_STATUS_BLOCK iosb;
    HANDLE file = 0;
    NTSTATUS st;

    RtlInitUnicodeString(&name, (const WCHAR *)REPORT_PATH);

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
    st = write_report(report, len);

    ExFreePoolWithTag(report, REPORT_POOL_TAG);

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
