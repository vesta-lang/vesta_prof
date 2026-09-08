/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_msr_index.c
 * @brief La tabla de direcciones de MSR, que se GENERA del manual de Intel.
 *
 * POR QUE ESTE TEST EXISTE.  `common/msr_index.h` no lo escribe nadie: sale de
 * `tools/gen_msr_index.py` leyendo el volcado del manual.  Eso quita los errores
 * de tecleo y mete otro riesgo, que es peor porque es silencioso: **una
 * regeneracion puede cambiar una direccion sin que nadie mire**.  Un cambio de
 * version del manual, un ajuste del extractor, un nombre que Intel renombra.
 *
 * Nada de eso da un error de compilacion por si solo.  Da un perfilador que
 * programa OTRO registro, que es de las averias mas caras de encontrar: no
 * falla, mide otra cosa.
 *
 * De modo que las direcciones de las que depende el codigo se fijan aqui, con
 * su numero escrito a mano, y se comprueban AL COMPILAR.  Si una regeneracion
 * mueve alguna, esto no compila y dice cual.
 *
 * NO ES DUPLICAR LA TABLA.  Se fijan las que se USAN -- unas veinte de mil
 * quinientas ochenta y ocho --, mas un punado de anclas archiconocidas que
 * sirven de control: si el extractor se descolocara una fila, el contador de
 * marca de tiempo dejaria de estar en 10H y se veria aqui.
 *
 * Se compila como C, que es el lenguaje en el que se usa la tabla.
 */

/* La tabla generada no incluye NADA, a proposito: solo define macros, asi que
 * se puede incluir desde cualquier sitio sin arrastrar dependencias.  Quien
 * necesite `STATIC_ASSERT` se lo trae el, como aqui. */
#include "vxp_base.h"

#include "msr_index.h"

#include <stdio.h>

/* -------------------------------------------------------------------------
 *  Las anclas de control.
 *
 *  Registros cuya direccion es de dominio publico desde hace decadas.  No los
 *  usa el perfilador: estan para cazar un desplazamiento del extractor, que es
 *  el modo de fallar que tendria un lector de tablas por columnas.
 * ------------------------------------------------------------------------- */

STATIC_ASSERT(IA32_TIME_STAMP_COUNTER == 0x10u,
              "TSC moved: the extractor is probably off by a row");
STATIC_ASSERT(IA32_APIC_BASE == 0x1Bu, "APIC base moved");
STATIC_ASSERT(IA32_EFER == 0xC0000080u, "EFER moved");
STATIC_ASSERT(IA32_FS_BASE == 0xC0000100u, "FS base moved");
STATIC_ASSERT(IA32_GS_BASE == 0xC0000101u, "GS base moved");

/* -------------------------------------------------------------------------
 *  Las que el perfilador usa de verdad.
 *
 *  Cada una lleva al lado para que se lee, porque una lista de numeros sin
 *  motivo es lo primero que alguien borra al no entenderla.
 * ------------------------------------------------------------------------- */

/* Habilitacion general: de aqui sale la palabra autorizada sobre PEBS y BTS. */
STATIC_ASSERT(IA32_MISC_ENABLE == 0x1A0u, "MISC_ENABLE moved");

/* Capacidades del PMU: formato de LBR y de PEBS, y la escritura de ancho
 * completo, que decide si se puede pedir un periodo por encima de 2^31. */
STATIC_ASSERT(IA32_PERF_CAPABILITIES == 0x345u, "PERF_CAPABILITIES moved");

/* Control de depuracion: de aqui cuelga el LBR. */
STATIC_ASSERT(IA32_DEBUGCTL == 0x1D9u, "DEBUGCTL moved");

/* Los contadores y su gobierno. */
STATIC_ASSERT(IA32_PERFEVTSEL0 == 0x186u, "PERFEVTSEL0 moved");
STATIC_ASSERT(IA32_PMC0 == 0xC1u, "PMC0 moved");
STATIC_ASSERT(IA32_A_PMC0 == 0x4C1u, "A_PMC0 moved");
STATIC_ASSERT(IA32_FIXED_CTR0 == 0x309u, "FIXED_CTR0 moved");
STATIC_ASSERT(IA32_FIXED_CTR_CTRL == 0x38Du, "FIXED_CTR_CTRL moved");
STATIC_ASSERT(IA32_PERF_GLOBAL_STATUS == 0x38Eu, "PERF_GLOBAL_STATUS moved");
STATIC_ASSERT(IA32_PERF_GLOBAL_CTRL == 0x38Fu, "PERF_GLOBAL_CTRL moved");

/* Debug Store, y lo que cuelga de el. */
STATIC_ASSERT(IA32_DS_AREA == 0x600u, "DS_AREA moved");
STATIC_ASSERT(IA32_PEBS_ENABLE == 0x3F1u, "PEBS_ENABLE moved");

/* La configuracion del PEBS adaptativo.  OJO al nombre: es `MSR_`, no `IA32_`,
 * porque no es arquitectonica aunque la gobierne un bit que si lo es.  Escribir
 * `IA32_PEBS_DATA_CFG` fue un error real de la primera version. */
STATIC_ASSERT(MSR_PEBS_DATA_CFG == 0x3F2u, "PEBS_DATA_CFG moved");

/* Frecuencia entregada frente a la de referencia: lo que hace comparables dos
 * medidas tomadas en nucleos que no corren a la misma velocidad. */
STATIC_ASSERT(IA32_MPERF == 0xE7u, "MPERF moved");
STATIC_ASSERT(IA32_APERF == 0xE8u, "APERF moved");

/* El identificador de nucleo que devuelve `rdtscp`, del que depende detectar
 * una migracion entre dos lecturas. */
STATIC_ASSERT(IA32_TSC_AUX == 0xC0000103u, "TSC_AUX moved");

int main(void) {
    printf("== generated MSR address table ==\n");
    printf("--- checked at compile time ---\n");

    /* Se imprimen las que mas se consultan, para tenerlas a mano sin abrir el
     * fichero generado, que tiene mil quinientas ochenta y ocho entradas. */
    printf("  IA32_MISC_ENABLE        = 0x%03X\n", (unsigned)IA32_MISC_ENABLE);
    printf("  IA32_PERF_CAPABILITIES  = 0x%03X\n",
           (unsigned)IA32_PERF_CAPABILITIES);
    printf("  IA32_PERFEVTSEL0        = 0x%03X\n", (unsigned)IA32_PERFEVTSEL0);
    printf("  IA32_PERF_GLOBAL_CTRL   = 0x%03X\n",
           (unsigned)IA32_PERF_GLOBAL_CTRL);
    printf("  IA32_DS_AREA            = 0x%03X\n", (unsigned)IA32_DS_AREA);
    printf("  MSR_PEBS_DATA_CFG       = 0x%03X\n", (unsigned)MSR_PEBS_DATA_CFG);
    return 0;
}
