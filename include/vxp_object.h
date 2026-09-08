/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: MIT (ver LICENSE.MIT).  Esta en `include/`, que es el contrato
 * publico: una herramienta de terceros puede incluirlo sin que la GPLv2 del
 * resto del proyecto le alcance.
 */

/**
 * @file vxp_object.h
 * @brief
 * \~english The idiom for writing modules with an owner, in C.
 * \~spanish El idioma para escribir modulos con dueno en C.
 * \~
 *
 * \~english
 * Object orientation **without polymorphism and without function pointers**:
 * everything here costs zero at run time.  It is not a framework simulating
 * classes -- that would fight readability and blind debuggers -- but the two
 * pieces C does not give and that are needed to encapsulate without allocating.
 *
 * THE SHAPE OF A MODULE, so it is predictable without reading it:
 *
 * @code
 *   OPAQUE_STORAGE(session, 64, 8);            // in the header: the SIZE
 *
 *   MUST_CHECK status session_init(session *s, ...);
 *   MUST_CHECK status session_do(session *s, ...);
 *   MUST_CHECK status session_read(const session *s, ...);  // observer: const
 *   void session_fini(session *s);
 * @endcode
 *
 *   - the prefix is the namespace and the first parameter is the `this`;
 *   - `init`/`fini` always with that shape and that name;
 *   - `const` on the observers, which is documentation the compiler checks;
 *   - `static` in the `.c` for everything not in the header: the file is the
 *     module's boundary;
 *   - return `status`, never a pointer that could be null.
 *
 * \~spanish
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
 *   - `static` en el `.c` para todo lo que no este en la cabecera: el fichero es
 *     la frontera del modulo;
 *   - devolver `status`, nunca un puntero que pueda ser nulo.
 * \~
 *
 * @see profiler/common/README.md
 */

#ifndef VXP_OBJECT_H
#define VXP_OBJECT_H

#include "vxp_base.h"

/**
 * @brief
 * \~english Declares the STORAGE of an opaque type (form A: fixed size).
 * \~spanish Declara el ALMACENAMIENTO de un tipo opaco (forma A: tamano fijo).
 * \~
 *
 * \~english
 * The problem it solves: a genuinely opaque type -- declared and not defined in
 * the header -- protects its invariants, but then whoever uses it does not know
 * how big it is and cannot place it.  The usual way out is for the module to
 * allocate it; here it cannot, because allocation only happens at start-up and
 * `common/` never allocates.
 *
 * The way out is to publish the SIZE and not the shape: a block of bytes with
 * the right alignment, which the caller places wherever they want -- the stack,
 * inside another struct, the block reserved when the driver loads -- and whose
 * fields they cannot touch because they do not know them.
 *
 * For objects whose size depends on something known at run time, see form B
 * below.
 *
 * \~spanish
 * El problema que resuelve: un tipo opaco de verdad -- declarado y no definido
 * en la cabecera -- protege sus invariantes, pero entonces quien lo usa no sabe
 * cuanto ocupa y no puede colocarlo.  Lo normal seria que el modulo lo
 * reservase; aqui no puede, porque solo se reserva al arrancar y `common/` no
 * reserva nunca.
 *
 * La salida es publicar el TAMANO y no la forma: un bloque de bytes con la
 * alineacion correcta, que el llamante coloca donde quiera -- la pila, dentro de
 * otra estructura, el bloque reservado al cargar el driver -- y cuyos campos no
 * puede tocar porque no los conoce.
 *
 * Para objetos cuyo tamano depende de algo que se sabe en ejecucion, ver la
 * forma B mas abajo.
 * \~
 *
 * @param name  \~english the type's name \~spanish nombre del tipo \~
 * @param bytes \~english how big the real struct is, with slack if wanted \~spanish cuanto ocupa la estructura real, con holgura si se quiere \~
 * @param align \~english the alignment it needs \~spanish alineacion que necesita \~
 */
#define OPAQUE_STORAGE(name, bytes, align)                                     \
    typedef struct name {                                                      \
        ALIGNAS(align) u8 opaque[bytes];                                       \
    } name

/**
 * @brief
 * \~english Checks, in the implementing `.c`, that the real struct fits.
 * \~spanish Comprueba, en el `.c` que implementa, que la estructura real cabe.
 * \~
 *
 * \~english
 * It goes next to the struct's definition and not in the header: whoever
 * defines it is the one who must find out when adding a field.  Without this
 * check, adding one does not give an error -- it gives writes past the storage
 * the caller reserved, and the symptom shows up far away.
 *
 * \~spanish
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
 * \~english
 * FORM B: objects whose size depends on something known at run time.
 *
 * `OPAQUE_STORAGE` publishes a constant size, and that does not work for a ring
 * of N slots.  Those objects split in two: a fixed-size control block --
 * opaque, form A -- and a data region handed to it.
 *
 *     MUST_CHECK status ring_init(ring *r, void *data, usize bytes);
 *     usize ring_bytes_needed(u32 slots);
 *
 * No machinery is needed for this: it is a function returning a size.  What has
 * to be honoured is that the module does NOT allocate -- it says how much, and
 * the caller supplies it.
 *
 * It is documented here, and not only in the README, because it is the part of
 * the idiom that gets forgotten: without it the temptation is for the module to
 * allocate "just this once".
 *
 * \~spanish
 * FORMA B: objetos cuyo tamano depende de algo de ejecucion.
 *
 * `OPAQUE_STORAGE` publica un tamano constante, y eso no vale para un anillo de
 * N ranuras.  Esos objetos se parten en dos: un bloque de control de tamano fijo
 * -- opaco, forma A -- y una region de datos que se le entrega.
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
 * \~
 */

#endif /* VXP_OBJECT_H */
