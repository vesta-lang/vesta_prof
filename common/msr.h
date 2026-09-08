/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file msr.h
 * @brief La interfaz al sistema para leer y escribir MSR, y lo que sabemos de
 *        los registros que el perfilador decodifica.
 *
 * `common/` no incluye ni una cabecera del sistema operativo.  Lo poco que
 * necesita de el entra por aqui, y cada lado lo implementa: `windows/` con el
 * intrinseco, `linux/` con `rdmsrl_safe`, y las pruebas con una tabla falsa en
 * espacio de usuario.
 *
 * DEVUELVE ESTADO, NO `void`.  Es una condicion, no una preferencia:
 *
 *   - Linux tiene `rdmsrl_safe`/`wrmsrl_safe`, que devuelven error en vez de
 *     provocar una excepcion de proteccion general;
 *   - Windows no tiene variante segura: o se comprueba antes, o se envuelve en
 *     `__try`/`__except`.
 *
 * Con una firma `void` ninguno de los dos podria informar, y un MSR que no
 * existe en esta microarquitectura **pasaria por un valor leido**.  No un
 * fallo: un dato falso, que es peor.
 *
 * SIN TABLA DE PUNTEROS.  Una compilacion de Windows enlaza la de Windows y
 * nunca ve la otra: eso es seleccion en tiempo de enlace, no polimorfismo.  Una
 * tabla aqui pondria una llamada indirecta en el camino mas caliente e
 * impediria inlinear el intrinseco.
 *
 * EL REPARTO CON LA TABLA GENERADA.  Las DIRECCIONES de todos los MSR del
 * manual salen de `msr_index.h`, que se genera.  Aqui se queda lo que una tabla
 * no puede dar: **los campos de bits y las puertas**, con el porque de cada uno.
 * Un numero se extrae; saber que `PEBS_FMT` no son los bits bajos, no.
 */

#ifndef VXP_COMMON_MSR_H
#define VXP_COMMON_MSR_H

#include "vxp_base.h"

/* Las direcciones, todas, generadas del manual.  Ver `tools/gen_msr_index.py`. */
#include "msr_index.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Lee un MSR.
 *
 * @param index numero de MSR.
 * @param out   donde dejar el valor; no se toca si la lectura falla.
 * @return `OK`, o `ERR_FAULT` si el procesador rechazo la lectura (el MSR no
 *         existe en esta microarquitectura, o el hipervisor la intercepta), o
 *         `ERR_INVALID` si `out` es nulo.
 *
 * IMPORTANTE para quien implemente: `ERR_FAULT` tiene que distinguirse de un
 * valor leido.  Devolver ceros al fallar convierte "no se pudo" en "vale cero",
 * y esas dos cosas mandan a sitios distintos -- una a buscar otra via, la otra
 * a dar por hecho algo que no es.
 */
MUST_CHECK status msr_read(u32 index, u64 *out);

/**
 * @brief Escribe un MSR.
 *
 * @param index numero de MSR.
 * @param value valor a escribir.
 * @return `OK`, o `ERR_FAULT` si el procesador la rechazo.
 */
MUST_CHECK status msr_write(u32 index, u64 value);

/* -------------------------------------------------------------------------
 *  LAS PUERTAS.
 *
 *  Leer un MSR que no existe provoca una excepcion de proteccion general, y en
 *  kernel eso es un pantallazo.  La salida NO es capturar el fallo: es **no
 *  provocarlo**.  Cada uno de los que se leen tiene una condicion de existencia
 *  documentada, asi que se comprueba la condicion y despues se lee.
 *
 *  Esta tabla es la que hace segura la deteccion, y por eso vive aqui escrita a
 *  mano y no en la tabla generada: es conocimiento, no un numero.
 *
 *      IA32_MISC_ENABLE          Intel, desde el P6.  Siempre
 *      IA32_PERF_CAPABILITIES    CPUID.01H:ECX[15]  (PDCM)
 *      IA32_DEBUGCTL             Intel, con PMU arquitectonico
 *      IA32_FIXED_CTR_CTRL       PMU >= 1 y contadores fijos > 0
 *      IA32_FIXED_CTR0 + i       i por debajo de los fijos de CPUID.0AH
 *      IA32_PERF_GLOBAL_CTRL     PMU >= 2
 *      IA32_PERF_GLOBAL_STATUS   PMU >= 2
 *      IA32_PERFEVTSEL0 + i      PMU >= 1, i por debajo de los de CPUID.0AH
 *      IA32_PMC0 + i             idem
 *      IA32_A_PMC0 + i           IA32_PERF_CAPABILITIES.FULL_WRITE[13]
 *      IA32_DS_AREA              CPUID.01H:EDX[21]  (Debug Store)
 *      IA32_PEBS_ENABLE          IA32_PERF_CAPABILITIES.PEBS_BASELINE[14]
 *      MSR_PEBS_DATA_CFG         idem
 *      IA32_MPERF / IA32_APERF   CPUID.06H:ECX[0]  (HW_FEEDBACK_CAP)
 *      IA32_TSC_AUX              CPUID.80000001H:EDX[27]  (RDTSCP)
 *
 *  Las dos ultimas de PEBS lo dicen literalmente: el manual define el bit 14
 *  como "este MSR existe".  Y OJO con el nombre de la de configuracion --
 *  `MSR_PEBS_DATA_CFG`, no `IA32_`: no es arquitectonica aunque la gobierne un
 *  bit que si lo es.
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 *  Campos de `IA32_MISC_ENABLE`.
 * ------------------------------------------------------------------------- */

/**
 * El PMU esta disponible (bit 7).
 *
 * Se lee ANTES que nada: si viene a cero, no hay nada que programar y todo lo
 * que CPUID diga de contadores es papel mojado.  Se comprueba y se dice, en vez
 * de programar contadores que nadie va a incrementar.
 */
#define MSR_MISC_ENABLE_PERFMON_AVAILABLE 7

/**
 * Branch Trace Store **NO** disponible (bit 11).  Ojo al sentido invertido.
 *
 * No se usa BTS, pero se lee porque **cuelga del mismo Debug Store que PEBS**:
 * que los dos caigan a la vez explica la causa, mientras que ver solo el de
 * PEBS deja un dato sin explicacion.
 */
#define MSR_MISC_ENABLE_BTS_UNAVAILABLE 11

/**
 * PEBS **NO** disponible (bit 12).  Sentido invertido, igual que el anterior.
 *
 * Es la palabra autorizada sobre PEBS por encima de lo que anuncie CPUID: bajo
 * un hipervisor, CPUID es lo que el hipervisor decide contar, mientras que este
 * bit se lee del registro.
 *
 * Con un matiz que el manual pone y conviene respetar: estos dos indicadores
 * describen la disponibilidad **cuando `CPUID.01H:EDX[21]` esta puesto**.  Sin
 * Debug Store, no son autorizados por si solos.
 */
#define MSR_MISC_ENABLE_PEBS_UNAVAILABLE 12

/* -------------------------------------------------------------------------
 *  Campos de `IA32_PERF_CAPABILITIES`.
 *
 *  LA TRAMPA, Y COSTO UN FALLO: **el formato de registro PEBS NO son los bits
 *  bajos**.  Los bits bajos son el formato de LBR, y leer `value & 0xF` como
 *  formato de PEBS da un numero que parece razonable y no lo es.  En una maquina
 *  sin ninguno de los dos, los dos valen cero y el error es invisible.
 * ------------------------------------------------------------------------- */

/**
 * Formato de registro de LBR (bits 5:0).
 *
 * Dice QUE VARIANTE de Last Branch Record implementa esta pieza -- cuantas
 * entradas tiene la pila y que campos lleva cada una --, que es lo que hace
 * falta para decodificarla.  **Cero significa que no hay LBR**, y ese es el
 * caso que hay que distinguir: sin esto, la pila de llamadas por hardware no
 * existe y hay que caer a copiar pila y desenrollar en usuario.
 */
#define MSR_PERF_CAP_LBR_FORMAT_SHIFT 0
#define MSR_PERF_CAP_LBR_FORMAT_MASK 0x3Fu

/**
 * PEBS se comporta como TRAMPA y no como FALLO (bit 6).
 *
 * Cambia a que instruccion apunta el registro: con trampa, a la SIGUIENTE a la
 * culpable.  Es la desviacion que `EventingIP` vino a arreglar, y hay que
 * saberlo para declarar cual de las dos formas se uso en vez de dar por hecha
 * la buena.
 */
#define MSR_PERF_CAP_PEBS_TRAP 6

/**
 * El registro PEBS incluye los registros arquitectonicos (bit 7).
 *
 * Sin esto, del evento solo se saca la direccion; con ello, el estado de los
 * registros generales en el momento de grabarlo.
 */
#define MSR_PERF_CAP_PEBS_ARCH_REGS 7

/**
 * Version de la disposicion del registro PEBS (bits 11:8).
 *
 * Es el campo que decide COMO se decodifica lo que el hardware escribio, y por
 * eso viaja en el registro de la corrida: un lector que lo ignore leera campos
 * corridos sin fallar.  **Cero significa que no hay PEBS.**
 *
 * Y es el campo que se leyo mal en la primera version: NO son los bits bajos.
 */
#define MSR_PERF_CAP_PEBS_FORMAT_SHIFT 8
#define MSR_PERF_CAP_PEBS_FORMAT_MASK 0xFu

/**
 * Los contadores se pueden congelar mientras el procesador esta en SMM
 * (bit 12).
 *
 * Importa mas de lo que parece: el tiempo dentro de una interrupcion de gestion
 * del sistema es INVISIBLE para el sistema operativo, asi que sin congelar se
 * cuenta y se atribuye a codigo que no lo gasto.
 */
#define MSR_PERF_CAP_SMM_FREEZE 12

/**
 * Existen los alias `IA32_A_PMCx`, que escriben el contador entero (bit 13).
 *
 * Sin ellos, prefijar un contador pasa por `IA32_PMCx`, que solo admite un
 * valor de 32 bits con signo.  Es decir: **sin esto no se puede pedir un
 * periodo mayor que 2^31**, que es un limite del que conviene enterarse al
 * abrir la sesion y no al ver salir el doble de interrupciones.
 */
#define MSR_PERF_CAP_FULL_WIDTH_WRITE 13

/**
 * PEBS adaptativo (bit 14).
 *
 * Es la generacion en la que se elige POR GRUPOS que se graba
 * (`MSR_PEBS_DATA_CFG`: basico, registros generales, XMM, LBR, memoria).  De
 * ella depende poder pedir registros pequenos en vez de pagar el maximo
 * siempre.
 *
 * Y es ademas una PUERTA: el manual define este bit como "los MSR
 * `IA32_PEBS_ENABLE` y `MSR_PEBS_DATA_CFG` existen".
 */
#define MSR_PERF_CAP_PEBS_BASELINE 14

/**
 * Existe el registro de metricas de topdown (bit 15).
 *
 * Da el reparto de ranuras sin gastar contadores de proposito general.
 */
#define MSR_PERF_CAP_PERF_METRICS 15

/**
 * Los registros PEBS pueden salir por el flujo de Intel PT (bit 16).
 *
 * Otra ruta de transporte, no otra fuente: el mismo dato con otro destino.
 */
#define MSR_PERF_CAP_PEBS_OUTPUT_PT 16

/**
 * Timed PEBS (bit 17).
 *
 * El registro incluye cuanto tardo la operacion, no solo donde ocurrio.
 */
#define MSR_PERF_CAP_PEBS_TIMING_INFO 17

/**
 * Modo "RDPMC Metrics Clear" (bit 19).
 *
 * Permite que leer las metricas las ponga a cero en la misma operacion, en vez
 * de leer y limpiar por separado.
 */
#define MSR_PERF_CAP_RDPMC_METRICS_CLEAR 19

/* -------------------------------------------------------------------------
 *  Campos de `IA32_PERFEVTSELx`.
 * ------------------------------------------------------------------------- */

/** Bit 22 de un `PERFEVTSEL`: este contador esta armado. */
#define MSR_PERFEVTSEL_EN 22

/** Tope de selectores que se leen.  El PMU arquitectonico no pasa de 8. */
#define MSR_PERFEVTSEL_MAX 8u

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_COMMON_MSR_H */
