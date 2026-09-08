/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file msr.h
 * @brief La interfaz al sistema, y practicamente la unica que este directorio
 *        se permite: leer y escribir un MSR.
 *
 * `common/` no incluye ni una cabecera del sistema operativo.  Lo poco que
 * necesita de el entra por aqui, y cada lado lo implementa: `windows/` con el
 * intrinseco de MSVC/Clang o `rdmsr` donde no lo haya, `linux/` con
 * `rdmsrl_safe`, y las pruebas con una tabla falsa en espacio de usuario.
 *
 * DEVUELVE ESTADO, NO `void`.  Es una condicion, no una preferencia, y esta
 * escrita en `common/README.md`:
 *
 *   - Linux tiene `rdmsrl_safe`/`wrmsrl_safe`, que devuelven error en vez de
 *     provocar una excepcion de proteccion general;
 *   - Windows no tiene variante segura: o se comprueba CPUID antes, o se
 *     envuelve en `__try`/`__except`.
 *
 * Con una firma `void` ninguno de los dos podria informar, y un MSR que no
 * existe en esta microarquitectura **pasaria por un valor leido**.  No un
 * fallo: un dato falso, que es peor.
 *
 * SIN TABLA DE PUNTEROS.  Una compilacion de Windows enlaza la de Windows y
 * nunca ve la otra: eso es seleccion en tiempo de enlace, no polimorfismo.  Una
 * tabla aqui pondria una llamada indirecta en el camino mas caliente e
 * impediria inlinear el intrinseco -- y con las mitigaciones actuales
 * (retpoline, IBT, kCFI) las llamadas indirectas en kernel son ademas mas caras
 * y objetivo de secuestro de flujo.
 */

#ifndef VXP_COMMON_MSR_H
#define VXP_COMMON_MSR_H

#include "vxp_base.h"

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
 *
 * No la usa la deteccion -- que solo lee --, pero se declara aqui porque la
 * pareja es la interfaz, y separarlas invitaria a que la escritura apareciera
 * mas adelante con otra forma.
 */
MUST_CHECK status msr_write(u32 index, u64 value);

/* -------------------------------------------------------------------------
 *  Los MSR que usa la deteccion.
 *
 *  Con nombre y no como numeros sueltos: un numero mal tecleado en una llamada
 *  no da un error, da la lectura de OTRO registro.
 * ------------------------------------------------------------------------- */

/** Habilitacion general.  Su bit 12 es la palabra autorizada sobre PEBS. */
#define MSR_IA32_MISC_ENABLE 0x1A0u

/** Bit 12 de lo anterior: 1 = PEBS NO disponible. */
#define MSR_MISC_ENABLE_PEBS_UNAVAILABLE 12

/** Capacidades del PMU.  Legible solo si CPUID.01:ECX[15] (PDCM) esta puesto. */
#define MSR_IA32_PERF_CAPABILITIES 0x345u

/** Control de depuracion: de aqui cuelga el LBR. */
#define MSR_IA32_DEBUGCTL 0x1D9u

/** Cuantos contadores fijos hay activos y como.  Solo lectura aqui. */
#define MSR_IA32_FIXED_CTR_CTRL 0x38Du

/**
 * Compuerta global de los contadores.
 *
 * OJO A LO QUE **NO** SIGNIFICA: que sus bits esten puestos no quiere decir que
 * alguien este midiendo.  Es una compuerta -- un contador cuenta si su bit esta
 * aqui **y ademas** su `PERFEVTSEL` tiene el `EN` puesto --, y en esta maquina
 * se lee `0x70000003F` (los 6 de proposito general y los 3 fijos abiertos) con
 * `IA32_FIXED_CTR_CTRL` a cero, o sea con los fijos parados.
 *
 * La primera version de la deteccion dedujo "alguien esta contando" solo de
 * aqui y publico un hecho falso.  Queda escrito porque el error es facil de
 * repetir: el registro se llama "control" y parece que manda el.
 */
#define MSR_IA32_PERF_GLOBAL_CTRL 0x38Fu

/**
 * El primero de los selectores de evento; los demas son consecutivos.
 *
 * Existen desde `IA32_PERFEVTSEL0` hasta el numero de contadores de proposito
 * general que anuncie CPUID.0A -- por eso leerlos va acotado por ese numero y
 * no por una constante.
 */
#define MSR_IA32_PERFEVTSEL0 0x186u

/** Bit 22 de un `PERFEVTSEL`: este contador esta armado. */
#define MSR_PERFEVTSEL_EN 22

/** Tope de selectores que se leen.  El PMU arquitectonico no pasa de 8. */
#define MSR_PERFEVTSEL_MAX 8u

/*
 * DELIBERADAMENTE FUERA DE LA PRIMERA VERSION: `IA32_DS_AREA` (0x600) y
 * `IA32_PEBS_ENABLE` (0x3F1).
 *
 * Los dos existen SOLO si existe el Debug Store, que es justo lo que se esta
 * intentando averiguar.  Leer un MSR inexistente provoca una excepcion de
 * proteccion general, y en kernel eso es un pantallazo salvo que se capture --
 * y capturar exige SEH, que MSVC y Clang tienen y GCC no.
 *
 * Se leen cuando la implementacion de `msr_read` de esta compilacion sepa
 * capturar el fallo.  Mientras tanto, `IA32_MISC_ENABLE` y
 * `IA32_PERF_CAPABILITIES` son lecturas GARANTIZADAS y responden casi lo mismo,
 * que es la diferencia entre averiguarlo y arriesgar el equipo por adelantarlo.
 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_COMMON_MSR_H */
