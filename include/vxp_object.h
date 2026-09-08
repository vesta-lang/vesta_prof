/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 + excepcion de runtime (ver LICENSE).
 */

/**
 * @file vxp_object.h
 * @brief El idioma para escribir modulos con dueno en C.
 *
 * Orientacion a objetos **sin polimorfismo y sin punteros a funcion**: todo lo
 * que hay aqui cuesta cero en ejecucion.  No es un armazon que simule clases --
 * eso pelearia con la legibilidad y ciega a los depuradores --; son las dos
 * piezas que C no da y que hacen falta para encapsular sin reservar memoria.
 *
 * LA FORMA DE UN MODULO, para que sea predecible sin leerlo:
 *
 * @code
 *   OPAQUE_STORAGE(sesion, 64, 8);            // en la cabecera: el TAMANO
 *
 *   MUST_CHECK status sesion_init(sesion *s, ...);
 *   MUST_CHECK status sesion_algo(sesion *s, ...);
 *   MUST_CHECK status sesion_leer(const sesion *s, ...);  // observador: const
 *   void sesion_fini(sesion *s);
 * @endcode
 *
 *   - el prefijo es el espacio de nombres y el primer parametro es el `this`;
 *   - `init`/`fini` siempre con esa forma y ese nombre;
 *   - `const` en los observadores, que es documentacion que el compilador
 *     comprueba;
 *   - `static` en el `.c` para todo lo que no este en la cabecera: el fichero
 *     es la frontera del modulo;
 *   - devolver `status`, nunca un puntero que pueda ser nulo.
 *
 * @see profiler/common/README.md para el porque de cada punto.
 */

#ifndef VXP_OBJECT_H
#define VXP_OBJECT_H

#include "vxp_base.h"

/**
 * @brief Declara el ALMACENAMIENTO de un tipo opaco (forma A: tamano fijo).
 *
 * El problema que resuelve: un tipo opaco de verdad -- declarado y no definido
 * en la cabecera -- protege sus invariantes, pero entonces quien lo usa no sabe
 * cuanto ocupa y no puede colocarlo.  Lo normal seria que el modulo lo
 * reservase; aqui no puede, porque solo se reserva al arrancar y `common/` no
 * reserva nunca.
 *
 * La salida es publicar el TAMANO y no la forma: un bloque de bytes con la
 * alineacion correcta, que el llamante coloca donde quiera -- la pila, dentro
 * de otra estructura, el bloque reservado al cargar el driver -- y cuyos campos
 * no puede tocar porque no los conoce.
 *
 * Para objetos cuyo tamano depende de algo que se sabe en ejecucion, ver la
 * forma B mas abajo.
 *
 * @param name  nombre del tipo.
 * @param bytes cuanto ocupa la estructura real, con holgura si se quiere.
 * @param align alineacion que necesita.
 */
#define OPAQUE_STORAGE(name, bytes, align)                                     \
    typedef struct name {                                                      \
        ALIGNAS(align) u8 opaque[bytes];                                       \
    } name

/**
 * @brief Comprueba, en el `.c` que implementa, que la estructura real cabe.
 *
 * Va junto a la definicion de la estructura y no en la cabecera: es quien la
 * define el que tiene que enterarse al anadir un campo.  Sin esta comprobacion,
 * anadir uno no da un error -- da escrituras fuera del almacenamiento que
 * reservo el llamante, y el sintoma aparece lejos.
 */
#define OPAQUE_FITS(impl_type, storage_type)                                   \
    STATIC_ASSERT(sizeof(impl_type) <= sizeof(storage_type),                   \
                  "the real struct does not fit its opaque storage: "          \
                  "raise the published size");                                 \
    STATIC_ASSERT(ALIGNOF(impl_type) <= ALIGNOF(storage_type),                 \
                  "the real struct needs more alignment than its opaque "      \
                  "storage")

/*
 * FORMA B: objetos cuyo tamano depende de algo de ejecucion.
 *
 * `OPAQUE_STORAGE` publica un tamano constante, y eso no vale para un anillo de
 * N ranuras.  Esos objetos se parten en dos: un bloque de control de tamano
 * fijo -- opaco, forma A -- y una region de datos que se le entrega.
 *
 *     MUST_CHECK status ring_init(ring *r, void *datos, usize bytes);
 *     usize ring_bytes_needed(u32 ranuras);
 *
 * No hace falta maquinaria para esto: es una funcion que devuelve un tamano.  Lo
 * que hay que respetar es que el modulo NO reserva -- dice cuanto, y el llamante
 * aporta.
 *
 * Se documenta aqui, y no solo en el README, porque es la parte del idioma que
 * se olvida: sin ella la tentacion es que el modulo reserve "solo esta vez".
 */

#endif /* VXP_OBJECT_H */
