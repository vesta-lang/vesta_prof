/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file nt.h
 * @brief Lo que el driver necesita del kernel de Windows, declarado aqui.
 *
 * POR QUE NO SE USA EL WDK.  Para que nadie tenga que instalar nada aparte
 * para construir el driver.  Es la misma postura que el resto del proyecto:
 * los emisores de PE y de ELF tampoco dependen de la cadena de nadie.
 *
 * Del WDK solo salen tres cosas, y las tres se traen: las declaraciones -- este
 * fichero --, la biblioteca de importacion -- generada de `ntoskrnl.def` -- y
 * las banderas del enlace, que son eso, banderas.
 *
 * LO QUE SE DECLARA Y LO QUE NO.  Solo lo que se usa.  Este fichero crece
 * cuando el driver necesita algo mas, no antes: una declaracion que nadie
 * ejercita es una declaracion que nadie ha comprobado.
 *
 * LAS ESTRUCTURAS SON EL PELIGRO.  Una firma de funcion equivocada da un error
 * de enlace, que es ruidoso.  Un DESPLAZAMIENTO equivocado no da nada: escribe
 * en el campo de al lado y el driver corrompe memoria en silencio.  Por eso
 * toda estructura declarada aqui aparece en `nt_layout.inc` con su tamano y
 * sus desplazamientos, y eso se comprueba al compilar -- ademas de contra una
 * cabecera de referencia, cuando la hay.  Ver tests/test_nt_layout.c.
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
 *  Tipos base del kernel.
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

/** Como el kernel pasa cadenas: longitud explicita, sin terminador. */
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    WCHAR *Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

/* -------------------------------------------------------------------------
 *  El objeto de driver.
 *
 *  Se declara ENTERO aunque solo se escriba `DriverUnload`: los campos de
 *  delante son los que lo colocan en su desplazamiento.  Los que no se usan van
 *  como `PVOID` a proposito -- no hace falta su forma, solo su tamano, y asi no
 *  se arrastran veinte estructuras mas que tampoco se comprobarian.
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
 *  Memoria.
 *
 *  Se declara la forma CON ETIQUETA y no la simple: la etiqueta es lo que
 *  permite ver desde el depurador quien retiene memoria (`!poolused`), y sin
 *  ella una fuga del driver es una cifra anonima.
 *
 *  Recordatorio de la regla: en el kernel se reserva al arrancar y nunca mas.
 *  Estas dos se llaman en la inicializacion y en la descarga, no en el camino
 *  caliente.  Ver profiler/README.md, seccion "Memoria".
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
 *  IRQL.
 *
 *  Hace falta para AFIRMAR en que contexto corre algo, no para cambiarlo: buena
 *  parte de las reglas del driver -- no reservar, no tomar cerrojos -- dependen
 *  del IRQL, y poder comprobarlo en depuracion convierte una regla escrita en
 *  una regla verificada.
 * ------------------------------------------------------------------------- */

KIRQL KeGetCurrentIrql(void);

#define PASSIVE_LEVEL 0
#define APC_LEVEL 1
#define DISPATCH_LEVEL 2

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_WINDOWS_NT_H */
