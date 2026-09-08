/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file msr/access.h
 * @brief
 * \~english The interface to the system for reading and writing MSRs, and what
 *           we know about the registers the profiler decodes.
 * \~spanish La interfaz al sistema para leer y escribir MSR, y lo que sabemos de
 *           los registros que el perfilador decodifica.
 * \~
 *
 * \~english
 * WHAT AN MSR IS.  A model-specific register: processor state that is not
 * reached through an ordinary instruction but by number, with `rdmsr`/`wrmsr`,
 * and only from ring 0.  Everything about the performance counters -- which
 * event each one counts, how often it fires, whether they are running -- lives
 * in one of them.
 *
 * `common/` includes not one operating system header.  The little it needs from
 * it comes in through here, and each side implements it: `windows/` with the
 * intrinsic, `linux/` with `rdmsrl_safe`, and the tests with a fake table in
 * user space.
 *
 * IT RETURNS STATUS, NOT `void`.  That is a condition, not a preference:
 *
 *   - Linux has `rdmsrl_safe`/`wrmsrl_safe`, which return an error instead of
 *     raising a general protection fault;
 *   - Windows has no safe variant: either you check beforehand, or you wrap it
 *     in a guarded region.
 *
 * With a `void` signature neither could report, and an MSR that does not exist
 * on this microarchitecture **would pass for a value that was read**.  Not a
 * failure: false data, which is worse.
 *
 * NO POINTER TABLE.  A Windows build links the Windows one and never sees the
 * other: that is link-time selection, not polymorphism.  A table here would put
 * an indirect call on the hottest path and would prevent inlining the intrinsic.
 *
 * THE SPLIT WITH THE GENERATED TABLE.  The ADDRESSES of every MSR in the manual
 * come from `msr/index.h`, which is generated.  What stays here is what a table
 * cannot give: **the bit fields and the gates**, with the why of each.  A number
 * can be extracted; knowing that `PEBS_FMT` is not the low bits cannot.
 *
 * \~spanish
 * QUE ES UN MSR.  Un registro especifico del modelo: estado del procesador al
 * que no se llega con una instruccion corriente sino por numero, con
 * `rdmsr`/`wrmsr`, y solo desde anillo cero.  Todo lo de los contadores de
 * rendimiento -- que evento cuenta cada uno, cada cuanto salta, si estan
 * corriendo -- vive en alguno de ellos.
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
 *     una region guardada.
 *
 * Con una firma `void` ninguno de los dos podria informar, y un MSR que no
 * existe en esta microarquitectura **pasaria por un valor leido**.  No un fallo:
 * un dato falso, que es peor.
 *
 * SIN TABLA DE PUNTEROS.  Una compilacion de Windows enlaza la de Windows y
 * nunca ve la otra: eso es seleccion en tiempo de enlace, no polimorfismo.  Una
 * tabla aqui pondria una llamada indirecta en el camino mas caliente e impediria
 * inlinear el intrinseco.
 *
 * EL REPARTO CON LA TABLA GENERADA.  Las DIRECCIONES de todos los MSR del manual
 * salen de `msr/index.h`, que se genera.  Aqui se queda lo que una tabla no
 * puede dar: **los campos de bits y las puertas**, con el porque de cada uno.
 * Un numero se extrae; saber que `PEBS_FMT` no son los bits bajos, no.
 */

#ifndef VXP_COMMON_MSR_ACCESS_H
#define VXP_COMMON_MSR_ACCESS_H

#include "vxp_base.h"

/* \~english Every address, generated from the manual.  See
 * `tools/gen_msr_index.py`.
 * \~spanish Las direcciones, todas, generadas del manual.  Ver
 * `tools/gen_msr_index.py`. \~ */
#include "index.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * \~english Reads an MSR.
 * \~spanish Lee un MSR.
 * \~
 *
 * @param index \~english MSR number \~spanish numero de MSR \~
 * @param out   \~english where to leave the value; untouched if the read fails \~spanish donde dejar el valor; no se toca si la lectura falla \~
 *
 * @return
 * \~english `OK`, or `ERR_FAULT` if the processor refused the read (the MSR does
 *           not exist on this microarchitecture, or the hypervisor intercepts
 *           it), or `ERR_INVALID` if `out` is null.
 * \~spanish `OK`, o `ERR_FAULT` si el procesador rechazo la lectura (el MSR no
 *           existe en esta microarquitectura, o el hipervisor la intercepta), o
 *           `ERR_INVALID` si `out` es nulo.
 * \~
 *
 * \~english
 * IMPORTANT for whoever implements it: `ERR_FAULT` has to be distinguishable
 * from a value that was read.  Returning zeros on failure turns "could not" into
 * "is zero", and those two send you to different places -- one to look for
 * another route, the other to take for granted something that is not so.
 *
 * \~spanish
 * IMPORTANTE para quien implemente: `ERR_FAULT` tiene que distinguirse de un
 * valor leido.  Devolver ceros al fallar convierte "no se pudo" en "vale cero",
 * y esas dos cosas mandan a sitios distintos -- una a buscar otra via, la otra a
 * dar por hecho algo que no es.
 */
MUST_CHECK status msr_read(u32 index, u64 *out);

/**
 * @brief
 * \~english Writes an MSR.
 * \~spanish Escribe un MSR.
 * \~
 *
 * @param index \~english MSR number \~spanish numero de MSR \~
 * @param value \~english the value to write \~spanish valor a escribir \~
 * @return \~english `OK`, or `ERR_FAULT` if the processor refused it \~spanish `OK`, o `ERR_FAULT` si el procesador la rechazo \~
 */
MUST_CHECK status msr_write(u32 index, u64 value);

/* -------------------------------------------------------------------------
 *  \~english
 *  THE GATES.
 *
 *  Reading an MSR that does not exist raises a general protection fault, and in
 *  the kernel that is a blue screen.  The way out is NOT to catch the fault: it
 *  is **not to cause it**.  Every one that gets read has a documented existence
 *  condition, so the condition is checked and then it is read.
 *
 *  This table is what makes detection safe, and that is why it lives here
 *  written by hand and not in the generated table: it is knowledge, not a
 *  number.
 *
 *  \~spanish
 *  LAS PUERTAS.
 *
 *  Leer un MSR que no existe provoca una excepcion de proteccion general, y en
 *  kernel eso es un pantallazo.  La salida NO es capturar el fallo: es **no
 *  provocarlo**.  Cada uno de los que se leen tiene una condicion de existencia
 *  documentada, asi que se comprueba la condicion y despues se lee.
 *
 *  Esta tabla es la que hace segura la deteccion, y por eso vive aqui escrita a
 *  mano y no en la tabla generada: es conocimiento, no un numero.
 *  \~
 *
 *      IA32_MISC_ENABLE          Intel, P6 onwards.  Always
 *      IA32_PERF_CAPABILITIES    CPUID.01H:ECX[15]  (PDCM)
 *      IA32_DEBUGCTL             Intel, with an architectural PMU
 *      IA32_FIXED_CTR_CTRL       PMU >= 1 and fixed counters > 0
 *      IA32_FIXED_CTR0 + i       i below the fixed ones in CPUID.0AH
 *      IA32_PERF_GLOBAL_CTRL     PMU >= 2
 *      IA32_PERF_GLOBAL_STATUS   PMU >= 2
 *      IA32_PERFEVTSEL0 + i      PMU >= 1, i below those in CPUID.0AH
 *      IA32_PMC0 + i             likewise
 *      IA32_A_PMC0 + i           IA32_PERF_CAPABILITIES.FULL_WRITE[13]
 *      IA32_DS_AREA              CPUID.01H:EDX[21]  (Debug Store)
 *      IA32_PEBS_ENABLE          IA32_PERF_CAPABILITIES.PEBS_BASELINE[14]
 *      MSR_PEBS_DATA_CFG         likewise
 *      IA32_MPERF / IA32_APERF   CPUID.06H:ECX[0]  (HW_FEEDBACK_CAP)
 *      IA32_TSC_AUX              CPUID.80000001H:EDX[27]  (RDTSCP)
 *
 *  \~english
 *  The last two PEBS ones say it literally: the manual defines bit 14 as "this
 *  MSR exists".  And WATCH the name of the configuration one --
 *  `MSR_PEBS_DATA_CFG`, not `IA32_`: it is not architectural even though a bit
 *  that is governs it.
 *
 *  \~spanish
 *  Las dos ultimas de PEBS lo dicen literalmente: el manual define el bit 14
 *  como "este MSR existe".  Y OJO con el nombre de la de configuracion --
 *  `MSR_PEBS_DATA_CFG`, no `IA32_`: no es arquitectonica aunque la gobierne un
 *  bit que si lo es.
 *  \~
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 *  \~english  Fields of `IA32_MISC_ENABLE`.
 *  \~spanish  Campos de `IA32_MISC_ENABLE`.
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * \~english
 * The PMU is available (bit 7).
 *
 * It is read BEFORE anything else: if it comes back zero there is nothing to
 * program and everything CPUID says about counters is worthless.  It is checked
 * and stated, instead of programming counters nobody is going to increment.
 *
 * \~spanish
 * El PMU esta disponible (bit 7).
 *
 * Se lee ANTES que nada: si viene a cero, no hay nada que programar y todo lo
 * que CPUID diga de contadores es papel mojado.  Se comprueba y se dice, en vez
 * de programar contadores que nadie va a incrementar.
 */
#define MSR_MISC_ENABLE_PERFMON_AVAILABLE 7

/**
 * \~english
 * Branch Trace Store **NOT** available (bit 11).  Mind the inverted sense.
 *
 * BTS is not used, but it is read because **it hangs off the same Debug Store as
 * PEBS**: both falling at once explains the cause, whereas seeing only the PEBS
 * one leaves a fact with no explanation.
 *
 * \~spanish
 * Branch Trace Store **NO** disponible (bit 11).  Ojo al sentido invertido.
 *
 * No se usa BTS, pero se lee porque **cuelga del mismo Debug Store que PEBS**:
 * que los dos caigan a la vez explica la causa, mientras que ver solo el de PEBS
 * deja un dato sin explicacion.
 */
#define MSR_MISC_ENABLE_BTS_UNAVAILABLE 11

/**
 * \~english
 * PEBS **NOT** available (bit 12).  Inverted sense, like the previous one.
 *
 * It is the authoritative word on PEBS above whatever CPUID announces: under a
 * hypervisor, CPUID is what the hypervisor decides to tell, while this bit is
 * read from the register.
 *
 * With one caveat the manual states and which is worth honouring: these two
 * indicators describe availability **when `CPUID.01H:EDX[21]` is set**.  Without
 * a Debug Store they are not authoritative on their own.
 *
 * \~spanish
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
 *  \~english
 *  Fields of `IA32_PERF_CAPABILITIES`.
 *
 *  THE TRAP, AND IT COST A BUG: **the PEBS record format is NOT the low bits**.
 *  The low bits are the LBR format, and reading `value & 0xF` as the PEBS format
 *  gives a number that looks reasonable and is not.  On a machine with neither of
 *  the two, both are zero and the mistake is invisible.
 *
 *  \~spanish
 *  Campos de `IA32_PERF_CAPABILITIES`.
 *
 *  LA TRAMPA, Y COSTO UN FALLO: **el formato de registro PEBS NO son los bits
 *  bajos**.  Los bits bajos son el formato de LBR, y leer `value & 0xF` como
 *  formato de PEBS da un numero que parece razonable y no lo es.  En una maquina
 *  sin ninguno de los dos, los dos valen cero y el error es invisible.
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * \~english
 * LBR record format (bits 5:0).
 *
 * LBR -- Last Branch Record -- is a small hardware stack of the most recent
 * branches taken, which is what allows reconstructing a call chain without
 * walking the stack.  This field says WHICH VARIANT this part implements -- how
 * many entries the stack has and what fields each one carries -- which is what
 * is needed to decode it.  **Zero means there is no LBR**, and that is the case
 * that must be told apart: without it the hardware call stack does not exist and
 * one has to fall back to copying the stack and unwinding in user space.
 *
 * \~spanish
 * Formato de registro de LBR (bits 5:0).
 *
 * LBR -- Last Branch Record -- es una pequena pila en hardware de los ultimos
 * saltos tomados, que es lo que permite reconstruir una cadena de llamadas sin
 * recorrer la pila.  Este campo dice QUE VARIANTE implementa esta pieza --
 * cuantas entradas tiene la pila y que campos lleva cada una --, que es lo que
 * hace falta para decodificarla.  **Cero significa que no hay LBR**, y ese es el
 * caso que hay que distinguir: sin esto, la pila de llamadas por hardware no
 * existe y hay que caer a copiar pila y desenrollar en usuario.
 */
#define MSR_PERF_CAP_LBR_FORMAT_SHIFT 0
#define MSR_PERF_CAP_LBR_FORMAT_MASK 0x3Fu

/**
 * \~english
 * PEBS behaves as a TRAP and not as a FAULT (bit 6).
 *
 * It changes which instruction the record points at: with a trap, the one AFTER
 * the guilty one.  That is the skid `EventingIP` came to fix, and it has to be
 * known in order to declare which of the two forms was used instead of assuming
 * the good one.
 *
 * \~spanish
 * PEBS se comporta como TRAMPA y no como FALLO (bit 6).
 *
 * Cambia a que instruccion apunta el registro: con trampa, a la SIGUIENTE a la
 * culpable.  Es la desviacion que `EventingIP` vino a arreglar, y hay que saberlo
 * para declarar cual de las dos formas se uso en vez de dar por hecha la buena.
 */
#define MSR_PERF_CAP_PEBS_TRAP 6

/**
 * \~english
 * The PEBS record includes the architectural registers (bit 7).
 *
 * Without this, only the address comes out of the event; with it, the state of
 * the general registers at the moment it was recorded.
 *
 * \~spanish
 * El registro PEBS incluye los registros arquitectonicos (bit 7).
 *
 * Sin esto, del evento solo se saca la direccion; con ello, el estado de los
 * registros generales en el momento de grabarlo.
 */
#define MSR_PERF_CAP_PEBS_ARCH_REGS 7

/**
 * \~english
 * Version of the PEBS record layout (bits 11:8).
 *
 * It is the field that decides HOW what the hardware wrote is decoded, and that
 * is why it travels in the run's log: a reader ignoring it will read shifted
 * fields without failing.  **Zero means there is no PEBS.**
 *
 * And it is the field that was read wrong in the first version: it is NOT the
 * low bits.
 *
 * \~spanish
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
 * \~english
 * The counters can be frozen while the processor is in SMM (bit 12).
 *
 * It matters more than it looks: the time inside a system management interrupt
 * is INVISIBLE to the operating system, so without freezing it gets counted and
 * attributed to code that did not spend it.
 *
 * \~spanish
 * Los contadores se pueden congelar mientras el procesador esta en SMM (bit 12).
 *
 * Importa mas de lo que parece: el tiempo dentro de una interrupcion de gestion
 * del sistema es INVISIBLE para el sistema operativo, asi que sin congelar se
 * cuenta y se atribuye a codigo que no lo gasto.
 */
#define MSR_PERF_CAP_SMM_FREEZE 12

/**
 * \~english
 * The `IA32_A_PMCx` aliases exist, which write the whole counter (bit 13).
 *
 * Without them, preloading a counter goes through `IA32_PMCx`, which only takes
 * a signed 32-bit value.  That is: **without this you cannot ask for a period
 * larger than 2^31**, which is a limit worth finding out about when opening the
 * session and not when seeing twice as many interrupts come out.
 *
 * \~spanish
 * Existen los alias `IA32_A_PMCx`, que escriben el contador entero (bit 13).
 *
 * Sin ellos, prefijar un contador pasa por `IA32_PMCx`, que solo admite un valor
 * de 32 bits con signo.  Es decir: **sin esto no se puede pedir un periodo mayor
 * que 2^31**, que es un limite del que conviene enterarse al abrir la sesion y
 * no al ver salir el doble de interrupciones.
 */
#define MSR_PERF_CAP_FULL_WIDTH_WRITE 13

/**
 * \~english
 * Adaptive PEBS (bit 14).
 *
 * It is the generation in which what gets recorded is chosen BY GROUPS
 * (`MSR_PEBS_DATA_CFG`: basic, general registers, XMM, LBR, memory).  Being able
 * to ask for small records instead of always paying the maximum depends on it.
 *
 * And it is also a GATE: the manual defines this bit as "the `IA32_PEBS_ENABLE`
 * and `MSR_PEBS_DATA_CFG` MSRs exist".
 *
 * \~spanish
 * PEBS adaptativo (bit 14).
 *
 * Es la generacion en la que se elige POR GRUPOS que se graba
 * (`MSR_PEBS_DATA_CFG`: basico, registros generales, XMM, LBR, memoria).  De ella
 * depende poder pedir registros pequenos en vez de pagar el maximo siempre.
 *
 * Y es ademas una PUERTA: el manual define este bit como "los MSR
 * `IA32_PEBS_ENABLE` y `MSR_PEBS_DATA_CFG` existen".
 */
#define MSR_PERF_CAP_PEBS_BASELINE 14

/**
 * \~english
 * The topdown metrics register exists (bit 15).
 *
 * It gives the slot breakdown without spending general-purpose counters.
 *
 * \~spanish
 * Existe el registro de metricas de topdown (bit 15).
 *
 * Da el reparto de ranuras sin gastar contadores de proposito general.
 */
#define MSR_PERF_CAP_PERF_METRICS 15

/**
 * \~english
 * PEBS records can go out through the Intel PT stream (bit 16).
 *
 * Another transport route, not another source: the same data with another
 * destination.
 *
 * \~spanish
 * Los registros PEBS pueden salir por el flujo de Intel PT (bit 16).
 *
 * Otra ruta de transporte, no otra fuente: el mismo dato con otro destino.
 */
#define MSR_PERF_CAP_PEBS_OUTPUT_PT 16

/**
 * \~english
 * Timed PEBS (bit 17).
 *
 * The record includes how long the operation took, not only where it happened.
 *
 * \~spanish
 * Timed PEBS (bit 17).
 *
 * El registro incluye cuanto tardo la operacion, no solo donde ocurrio.
 */
#define MSR_PERF_CAP_PEBS_TIMING_INFO 17

/**
 * \~english
 * "RDPMC Metrics Clear" mode (bit 19).
 *
 * It lets reading the metrics zero them in the same operation, instead of
 * reading and clearing separately.
 *
 * \~spanish
 * Modo "RDPMC Metrics Clear" (bit 19).
 *
 * Permite que leer las metricas las ponga a cero en la misma operacion, en vez
 * de leer y limpiar por separado.
 */
#define MSR_PERF_CAP_RDPMC_METRICS_CLEAR 19

/* -------------------------------------------------------------------------
 *  \~english  Fields of `IA32_PERFEVTSELx`.
 *  \~spanish  Campos de `IA32_PERFEVTSELx`.
 *  \~
 * ------------------------------------------------------------------------- */

/** \~english Bit 22 of a `PERFEVTSEL`: this counter is armed.
 *  \~spanish Bit 22 de un `PERFEVTSEL`: este contador esta armado. \~ */
#define MSR_PERFEVTSEL_EN 22

/** \~english Cap on the selectors that get read.  The architectural PMU does
 *            not go past 8.
 *  \~spanish Tope de selectores que se leen.  El PMU arquitectonico no pasa de
 *            8. \~ */
#define MSR_PERFEVTSEL_MAX 8u

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_COMMON_MSR_ACCESS_H */
