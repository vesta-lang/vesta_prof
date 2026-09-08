/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file nt.h
 * @brief
 * \~english What the driver needs from the Windows kernel, declared here.
 * \~spanish Lo que el driver necesita del kernel de Windows, declarado aqui.
 * \~
 *
 * \~english
 * THE WDK is Microsoft's Windows Driver Kit: the headers, import libraries and
 * tools normally used to build a `.sys`.  It is a separate install of several
 * gigabytes.
 *
 * WHY IT IS NOT USED.  So that nobody has to install anything extra to build the
 * driver.  It is the same stance as the rest of the project: the PE and ELF
 * emitters do not depend on anybody's toolchain either.
 *
 * Only three things come out of the WDK, and all three are brought in here: the
 * declarations -- this file --, the import library -- generated from
 * `ntoskrnl.def` -- and the link flags, which are just that, flags.
 *
 * WHAT IS DECLARED AND WHAT IS NOT.  Only what is used.  This file grows when
 * the driver needs something more, not before: a declaration nobody exercises is
 * a declaration nobody has checked.
 *
 * THE STRUCTS ARE THE DANGER.  A wrong function signature gives a link error,
 * which is loud.  A wrong OFFSET gives nothing: it writes into the neighbouring
 * field and the driver corrupts memory silently.
 *
 *      what we declare        what the kernel really has
 *      +--------+--------+    +--------+--------+
 *      | Length | Group  |    | Length | pad    |
 *      +--------+--------+    +--------+--------+
 *      | Mask            |    | Group  | Mask   |   <- everything shifted, and
 *      +-----------------+    +-----------------+      nothing says so
 *
 * That is why every struct declared here appears in `nt_layout.inc` with its
 * size and its offsets, and that is checked at compile time -- as well as
 * against a reference header, when there is one.  See tests/test_nt_layout.c.
 *
 * Hence also that fields nobody uses get declared: what matters is not their
 * name, it is that they push the following ones into place.
 *
 * \~spanish
 * EL WDK es el kit de desarrollo de drivers de Microsoft: las cabeceras,
 * bibliotecas de importacion y herramientas con las que normalmente se compila
 * un `.sys`.  Es una instalacion aparte de varios gigabytes.
 *
 * POR QUE NO SE USA.  Para que nadie tenga que instalar nada aparte para
 * construir el driver.  Es la misma postura que el resto del proyecto: los
 * emisores de PE y de ELF tampoco dependen de la cadena de nadie.
 *
 * Del WDK solo salen tres cosas, y las tres se traen aqui: las declaraciones --
 * este fichero --, la biblioteca de importacion -- generada de `ntoskrnl.def` --
 * y las banderas del enlace, que son eso, banderas.
 *
 * LO QUE SE DECLARA Y LO QUE NO.  Solo lo que se usa.  Este fichero crece cuando
 * el driver necesita algo mas, no antes: una declaracion que nadie ejercita es
 * una declaracion que nadie ha comprobado.
 *
 * LAS ESTRUCTURAS SON EL PELIGRO.  Una firma de funcion equivocada da un error
 * de enlace, que es ruidoso.  Un DESPLAZAMIENTO equivocado no da nada: escribe
 * en el campo de al lado y el driver corrompe memoria en silencio.
 *
 *      lo que declaramos      lo que el kernel tiene de verdad
 *      +--------+--------+    +--------+--------+
 *      | Length | Group  |    | Length | relleno|
 *      +--------+--------+    +--------+--------+
 *      | Mask            |    | Group  | Mask   |   <- todo corrido, y nada
 *      +-----------------+    +-----------------+      lo dice
 *
 * Por eso toda estructura declarada aqui aparece en `nt_layout.inc` con su
 * tamano y sus desplazamientos, y eso se comprueba al compilar -- ademas de
 * contra una cabecera de referencia, cuando la hay.  Ver tests/test_nt_layout.c.
 *
 * De ahi tambien que se declaren campos que no se usan: lo que importa no es su
 * nombre, es que empujan a los siguientes a su sitio.
 */

#ifndef VXP_WINDOWS_NT_H
#define VXP_WINDOWS_NT_H

#include "vxp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 *  \~english  The kernel's base types.
 *  \~spanish  Tipos base del kernel.
 *  \~
 * ------------------------------------------------------------------------- */

typedef s32 NTSTATUS;
typedef void *PVOID;
typedef u32 ULONG;
typedef u16 USHORT;
typedef s16 CSHORT;
typedef u16 WCHAR;
typedef u64 ULONG_PTR;
typedef u8 KIRQL;

#define STATUS_SUCCESS ((NTSTATUS)0)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
#define STATUS_INSUFFICIENT_RESOURCES ((NTSTATUS)0xC000009AL)

/** \~english How the kernel passes strings: explicit length, no terminator.
 *            The length is in BYTES, not characters, and it is a classic bug.
 *  \~spanish Como el kernel pasa cadenas: longitud explicita, sin terminador.
 *            La longitud va en BYTES, no en caracteres, y es un fallo
 *            clasico. \~ */
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    WCHAR *Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

/* -------------------------------------------------------------------------
 *  \~english
 *  The driver object.
 *
 *  It is what the system hands to `DriverEntry`: the driver's identity, where
 *  its entry points get written down.
 *
 *  It is declared WHOLE even though only `DriverUnload` is written: the fields
 *  in front are what place it at its offset.  The unused ones are `PVOID` on
 *  purpose -- their shape is not needed, only their size, and that way twenty
 *  more structs are not dragged in that would not be checked either.
 *
 *  \~spanish
 *  El objeto de driver.
 *
 *  Es lo que el sistema entrega a `DriverEntry`: la identidad del driver, donde
 *  se apuntan sus puntos de entrada.
 *
 *  Se declara ENTERO aunque solo se escriba `DriverUnload`: los campos de
 *  delante son los que lo colocan en su desplazamiento.  Los que no se usan van
 *  como `PVOID` a proposito -- no hace falta su forma, solo su tamano, y asi no
 *  se arrastran veinte estructuras mas que tampoco se comprobarian.
 *  \~
 * ------------------------------------------------------------------------- */

struct _DRIVER_OBJECT;
typedef void (*PDRIVER_UNLOAD)(struct _DRIVER_OBJECT *);

typedef struct _DRIVER_OBJECT {
    CSHORT Type;
    CSHORT Size;
    PVOID DeviceObject;
    ULONG Flags;
    PVOID DriverStart;
    ULONG DriverSize;
    PVOID DriverSection;
    PVOID DriverExtension;
    UNICODE_STRING DriverName;
    PUNICODE_STRING HardwareDatabase;
    PVOID FastIoDispatch;
    PVOID DriverInit;
    PVOID DriverStartIo;
    PDRIVER_UNLOAD DriverUnload;
    PVOID MajorFunction[28];
} DRIVER_OBJECT, *PDRIVER_OBJECT;

/* -------------------------------------------------------------------------
 *  \~english
 *  Memory.
 *
 *  The POOL is the kernel's heap.  The TAGGED form is declared and not the
 *  plain one: the tag is what lets you see from the debugger who is holding
 *  memory (`!poolused`), and without it a driver leak is an anonymous figure.
 *
 *  A reminder of the rule: in the kernel you allocate at start-up and never
 *  again.  These two are called at initialisation and at unload, not on the hot
 *  path.  See profiler/README.md, "Memoria".
 *
 *  \~spanish
 *  Memoria.
 *
 *  El POOL es el monton del kernel.  Se declara la forma CON ETIQUETA y no la
 *  simple: la etiqueta es lo que permite ver desde el depurador quien retiene
 *  memoria (`!poolused`), y sin ella una fuga del driver es una cifra anonima.
 *
 *  Recordatorio de la regla: en el kernel se reserva al arrancar y nunca mas.
 *  Estas dos se llaman en la inicializacion y en la descarga, no en el camino
 *  caliente.  Ver profiler/README.md, seccion "Memoria".
 *  \~
 * ------------------------------------------------------------------------- */

typedef enum _POOL_TYPE {
    NonPagedPool = 0,
    PagedPool = 1,
    NonPagedPoolNx = 512
} POOL_TYPE;

PVOID ExAllocatePoolWithTag(POOL_TYPE PoolType, ULONG_PTR NumberOfBytes,
                            ULONG Tag);
void ExFreePoolWithTag(PVOID P, ULONG Tag);

/* -------------------------------------------------------------------------
 *  \~english
 *  IRQL -- the interrupt request level.
 *
 *  A per-processor priority that says what is legal to do right now.  At
 *  PASSIVE_LEVEL, the lowest, everything is allowed; the higher you go the more
 *  is forbidden, because anything that could block would deadlock the machine.
 *
 *  It is needed to ASSERT what context something runs in, not to change it: a
 *  good part of the driver's rules -- do not allocate, do not take locks --
 *  depend on the IRQL, and being able to check it in a debug build turns a
 *  written rule into a verified one.
 *
 *  \~spanish
 *  IRQL -- el nivel de peticion de interrupcion.
 *
 *  Una prioridad por procesador que dice que es legal hacer en este momento.  A
 *  PASSIVE_LEVEL, el mas bajo, todo esta permitido; cuanto mas se sube, mas
 *  cosas quedan prohibidas, porque cualquiera que pudiera bloquear colgaria la
 *  maquina.
 *
 *  Hace falta para AFIRMAR en que contexto corre algo, no para cambiarlo: buena
 *  parte de las reglas del driver -- no reservar, no tomar cerrojos -- dependen
 *  del IRQL, y poder comprobarlo en depuracion convierte una regla escrita en
 *  una regla verificada.
 *  \~
 * ------------------------------------------------------------------------- */

KIRQL KeGetCurrentIrql(void);

#define PASSIVE_LEVEL 0
#define APC_LEVEL 1
#define DISPATCH_LEVEL 2

/* -------------------------------------------------------------------------
 *  \~english
 *  Affinity: running something ON one specific logical processor.
 *
 *  It is what is needed to ask each core about its PMU separately, and on a
 *  hybrid part that is no luxury: the P cores and the E cores need not answer
 *  the same, and a single read does not say which of the two replied.
 *
 *  The GROUP form is used and not the old `KeSetSystemAffinityThread`: above 64
 *  logical processors Windows splits them into groups, and the old form only
 *  reaches the current group.  Today it would be redundant; the day it is not,
 *  the failure would be silent -- the same core would be measured twice.
 *
 *  \~spanish
 *  Afinidad: correr algo EN un procesador logico concreto.
 *
 *  Es lo que hace falta para preguntarle el PMU a cada nucleo por separado, y en
 *  una pieza hibrida eso no es un lujo: los nucleos P y los E no tienen por que
 *  responder lo mismo, y una sola lectura no dice cual de los dos contesto.
 *
 *  Se usa la forma con GRUPO y no la vieja `KeSetSystemAffinityThread`: por
 *  encima de 64 procesadores logicos Windows los reparte en grupos, y la forma
 *  antigua solo alcanza al grupo actual.  Hoy sobraria; el dia que no, el fallo
 *  seria silencioso -- se mediria dos veces el mismo nucleo.
 *  \~
 * ------------------------------------------------------------------------- */

typedef u64 KAFFINITY;

/** \~english A logical processor within its group.
 *  \~spanish Un procesador logico dentro de su grupo. \~ */
typedef struct _GROUP_AFFINITY {
    KAFFINITY Mask;
    USHORT Group;
    USHORT Reserved[3];
} GROUP_AFFINITY, *PGROUP_AFFINITY;

/** \~english For `KeQueryActiveProcessorCountEx`: every group, not only the
 *            current one.
 *  \~spanish Para `KeQueryActiveProcessorCountEx`: todos los grupos, no solo el
 *            actual. \~ */
#define ALL_PROCESSOR_GROUPS ((USHORT)0xFFFF)

ULONG KeQueryActiveProcessorCountEx(USHORT GroupNumber);
void KeSetSystemGroupAffinityThread(PGROUP_AFFINITY Affinity,
                                    PGROUP_AFFINITY PreviousAffinity);
void KeRevertToUserGroupAffinityThread(PGROUP_AFFINITY PreviousAffinity);

/* -------------------------------------------------------------------------
 *  \~english
 *  Files, to get the report out.
 *
 *  WHY A FILE AND NOT `DbgPrint`.  The latter is only visible with a kernel
 *  debugger attached, or with a trace viewer and the print filter tweaked in the
 *  registry.  A report you have to set something up to read is a report nobody
 *  reads.  A file you just open.
 *
 *  This runs in `DriverEntry`, that is at PASSIVE_LEVEL, which is where file I/O
 *  is legal.  From the PMI handler it would not be remotely possible -- and
 *  hence the sample path is a ring and not this.
 *
 *  \~spanish
 *  Ficheros, para sacar el informe.
 *
 *  POR QUE UN FICHERO Y NO `DbgPrint`.  Lo segundo solo se ve con un depurador
 *  de kernel enganchado o con un visor de trazas y el filtro de impresion tocado
 *  en el registro.  Un informe que hay que montar algo para leer es un informe
 *  que no se lee.  Un fichero se abre y ya.
 *
 *  Esto corre en `DriverEntry`, o sea a PASSIVE_LEVEL, que es donde la E/S de
 *  ficheros es legal.  Desde el manejador de la PMI no se podria ni de lejos --
 *  y de ahi que el camino de las muestras sea un anillo y no esto.
 *  \~
 * ------------------------------------------------------------------------- */

typedef void *HANDLE;
typedef u32 ACCESS_MASK;

/** \~english The outcome of an I/O operation: how it went, and how much it
 *            moved.
 *  \~spanish El resultado de una operacion de E/S: como fue, y cuanto
 *            movio. \~ */
typedef struct _IO_STATUS_BLOCK {
    /* La union va CON NOMBRE, no anonima: esta cabecera tiene que compilar
     * tambien como C++, y una union anonima sin marcar no es portable entre
     * los dos. */
    union {
        NTSTATUS Status;
        PVOID Pointer;
    } u;
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

/** \~english How a kernel object is named: here, the file's path.
 *  \~spanish Como se nombra un objeto del kernel: aqui, la ruta del
 *            fichero. \~ */
typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

/* Atributos del objeto. */
#define OBJ_CASE_INSENSITIVE 0x00000040u
/* Sin esto el descriptor seria del proceso que casualmente estuviera activo al
 * cargar el driver, y se cerraria con el.  Un descriptor de kernel es del
 * kernel. */
#define OBJ_KERNEL_HANDLE 0x00000200u

/* Accesos y banderas de creacion.  Solo los que se usan. */
#define GENERIC_WRITE 0x40000000u
#define SYNCHRONIZE 0x00100000u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define FILE_SHARE_READ 0x00000001u
#define FILE_OVERWRITE_IF 0x00000005u
#define FILE_SYNCHRONOUS_IO_NONALERT 0x00000020u
#define FILE_NON_DIRECTORY_FILE 0x00000040u

NTSTATUS ZwCreateFile(HANDLE *FileHandle, ACCESS_MASK DesiredAccess,
                      POBJECT_ATTRIBUTES ObjectAttributes,
                      PIO_STATUS_BLOCK IoStatusBlock, s64 *AllocationSize,
                      ULONG FileAttributes, ULONG ShareAccess,
                      ULONG CreateDisposition, ULONG CreateOptions,
                      PVOID EaBuffer, ULONG EaLength);
NTSTATUS ZwWriteFile(HANDLE FileHandle, HANDLE Event, PVOID ApcRoutine,
                     PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,
                     PVOID Buffer, ULONG Length, s64 *ByteOffset, ULONG *Key);
NTSTATUS ZwClose(HANDLE Handle);

void RtlInitUnicodeString(PUNICODE_STRING DestinationString,
                          const WCHAR *SourceString);

/* -------------------------------------------------------------------------
 *  \~english
 *  Traces.
 *
 *  It complements the file, it does not replace it: if the driver fails BEFORE
 *  being able to open it, this is all that is left.  Hence it is used only for
 *  the news and the start-up errors.
 *
 *  \~spanish
 *  Trazas.
 *
 *  Complementa al fichero, no lo sustituye: si el driver falla ANTES de poder
 *  abrirlo, esto es lo unico que queda.  De ahi que se use solo para las
 *  novedades y los errores del arranque.
 *  \~
 * ------------------------------------------------------------------------- */

#define DPFLTR_IHVDRIVER_ID 77
#define DPFLTR_ERROR_LEVEL 0

ULONG DbgPrintEx(ULONG ComponentId, ULONG Level, const char *Format, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_WINDOWS_NT_H */
