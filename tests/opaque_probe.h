/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 + excepcion de runtime (ver LICENSE).
 */

/**
 * @file opaque_probe.h
 * @brief Un modulo de juguete escrito con el idioma completo, para probarlo.
 *
 * No forma parte del perfilador: existe para que la prueba ejercite el patron
 * de verdad -- tipo opaco, almacenamiento del llamante, `init`/`fini`, estado
 * que no se puede ignorar -- en vez de comprobar solo que las macros expanden.
 *
 * Es ademas el ejemplo al que mirar cuando haya que escribir un modulo de
 * `common/`: la forma es esta.
 */

#ifndef VXP_TESTS_OPAQUE_PROBE_H
#define VXP_TESTS_OPAQUE_PROBE_H

#include "vxp_object.h"

#ifdef __cplusplus
extern "C" {
#endif

/* El almacenamiento, no la forma.  Quien use esto puede colocarlo donde quiera
 * -- pila, dentro de otra estructura, el bloque que se reservo al arrancar --
 * y no puede tocar sus campos porque no los conoce.
 *
 * El tamano se publica con holgura a proposito: subirlo mas adelante no rompe
 * a nadie, bajarlo si.  Y que la estructura real quepa lo comprueba el `.c`
 * con OPAQUE_FITS, al compilar. */
OPAQUE_STORAGE(probe, 32, 8);

/**
 * @brief Prepara un contador sobre el almacenamiento que se le da.
 *
 * @param p     el almacenamiento, del llamante.  No puede ser nulo.
 * @param limit tope; sumar por encima devuelve ERR_NOSPACE.
 * @return ERR_INVALID si @p p es nulo o @p limit es cero.
 */
MUST_CHECK status probe_init(probe *p, u32 limit);

/** Suma @p n.  ERR_NOSPACE si pasaria del tope, y entonces no suma. */
MUST_CHECK status probe_add(probe *p, u32 n);

/** Lee el valor.  Observador: por eso `const`. */
MUST_CHECK status probe_value(const probe *p, u32 *out);

/** Deja el almacenamiento en un estado del que no se puede seguir usando. */
void probe_fini(probe *p);

/* -------------------------------------------------------------------------
 *  Forma B: el tamano se sabe en ejecucion.
 *
 *  `probe` de arriba tiene tamano fijo, y eso no vale para algo como un anillo
 *  de N ranuras.  Esos objetos se parten en dos: un bloque de control de tamano
 *  fijo -- opaco, forma A -- y una region de datos que se le entrega.
 *
 *  Lo que hay que respetar es que el modulo NO reserva: dice cuanto hace falta
 *  y el llamante aporta.  Y que init COMPRUEBE lo que le dan -- si se fiara del
 *  tamano que le pasan, un bufer corto seria una escritura fuera de sitio, que
 *  es justo lo que esta forma podria introducir si se hace mal.
 * ------------------------------------------------------------------------- */

/* El bloque de control es forma A: la forma B no necesita macro propia, y esa
 * es precisamente la idea. */
OPAQUE_STORAGE(probe_log, 32, 8);

/** Cuantos bytes hay que aportar para @p slots ranuras. */
usize probe_log_bytes_needed(u32 slots);

/**
 * @brief Prepara el registro sobre el almacenamiento que se le da.
 *
 * @param g       el bloque de control, del llamante.
 * @param storage la region de datos, tambien del llamante.
 * @param bytes   cuanto mide esa region.
 * @param slots   cuantas ranuras se quieren.
 * @return ERR_NOSPACE si @p bytes no llega para @p slots -- se comprueba, no se
 *         confia.
 */
MUST_CHECK status probe_log_init(probe_log *g, void *storage, usize bytes,
                                 u32 slots);

/** Anota un valor.  ERR_NOSPACE cuando ya no caben mas. */
MUST_CHECK status probe_log_push(probe_log *g, u32 value);

/** Cuantos hay anotados. */
MUST_CHECK status probe_log_count(const probe_log *g, u32 *out);

/** Lee el de la posicion @p index.  ERR_INVALID si esta fuera. */
MUST_CHECK status probe_log_at(const probe_log *g, u32 index, u32 *out);

void probe_log_fini(probe_log *g);

/* Solo para la prueba: lo que ve el lado de C, para contrastarlo con el de
 * C++.  Un desacuerdo aqui significa que la cabecera no dice lo mismo en los
 * dos lenguajes, que es el fallo que se quiere cazar. */
usize probe_sizeof_storage(void);
usize probe_alignof_storage(void);
usize probe_sizeof_u64(void);
usize probe_sizeof_size(void);
int probe_ok_value(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_TESTS_OPAQUE_PROBE_H */
