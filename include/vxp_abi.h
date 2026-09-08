/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: MIT (ver LICENSE.MIT).  Esta en `include/`, que es el contrato
 * publico: una herramienta de terceros puede incluirlo sin que la GPLv2 del
 * resto del proyecto le alcance.  Para ESTE fichero es el motivo de ser --
 * describe el formato de las muestras, y un formato que nadie puede consumir
 * sin cambiar su licencia no esta abierto.
 */

/**
 * @file vxp_abi.h
 * @brief
 * \~english How the contract between the driver and user space describes
 *           itself, so that a new version does not break the other one.
 * \~spanish Como el contrato entre el driver y el lado de usuario se describe a
 *           si mismo, para que una version nueva no rompa a la otra.
 * \~
 *
 * \~english
 * THE PROBLEM.  The driver and the consumer are separate binaries versioned
 * separately: a machine can end up with an old driver and a new consumer, or the
 * other way round.  If the shape of a sample changes and nobody checks, the
 * consumer **does not fail**: it reads shifted fields and publishes numbers that
 * look reasonable.
 *
 * THE ANSWER, in three layers, each where it can be afforded:
 *
 *   1. every ring record carries `{kind, length}` -- four bytes, and they allow
 *      SKIPPING what is not understood;
 *   2. the control structures carry their own size -- they are called once per
 *      session, the cost does not matter;
 *   3. and the layout can be ASKED FOR: size and offsets of what is published.
 *
 * THE RULE THAT HOLDS IT ALL UP:
 *
 *   > Fields are APPENDED AT THE END.  They are not reordered, not removed, and
 *   > above all not reused for something else.
 *
 * A descriptor saves you from a moved field.  It does not save you from a field
 * that changed MEANING while keeping its name, place and size: there the two
 * ends understand each other perfectly and say different things.
 *
 * \~spanish
 * EL PROBLEMA.  El driver y el consumidor son binarios distintos que se
 * versionan por separado: un equipo puede acabar con un driver viejo y un
 * consumidor nuevo, o al reves.  Si el formato de una muestra cambia y nadie lo
 * comprueba, el consumidor **no falla**: lee campos corridos y publica numeros
 * que parecen razonables.
 *
 * LA SOLUCION, en tres capas, cada una donde se puede pagar:
 *
 *   1. cada registro del anillo lleva `{tipo, longitud}` -- cuatro bytes, y
 *      permiten SALTARSE lo que no se entiende;
 *   2. las estructuras de control llevan su propio tamano -- se llaman una vez
 *      por sesion, el coste da igual;
 *   3. y se puede PREGUNTAR por la disposicion: tamano y desplazamientos de lo
 *      publicado.
 *
 * LA REGLA QUE LO SOSTIENE TODO:
 *
 *   > Los campos se ANADEN AL FINAL.  No se reordenan, no se quitan, y sobre
 *   > todo no se reutilizan para otra cosa.
 *
 * Un descriptor salva de un campo movido.  No salva de un campo que cambio de
 * SIGNIFICADO conservando nombre, sitio y tamano: ahi los dos extremos se
 * entienden perfectamente y dicen cosas distintas.
 */

#ifndef VXP_ABI_H
#define VXP_ABI_H

#include "vxp_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 *  \~english  Layer 1: the header on every ring record.
 *  \~spanish  Capa 1: la cabecera de cada registro del anillo.
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * @brief
 * \~english What precedes EVERY record written into the ring.
 * \~spanish Lo que precede a CADA registro escrito en el anillo.
 * \~
 *
 * \~english
 * Four bytes, and they buy the two properties that make a ring survive a
 * version change:
 *
 *   - **skip the unknown**: with `length` you advance to the next record
 *     without understanding this one.  Without that, a new sample kind does not
 *     degrade the reading, it stops it;
 *   - **tolerate the longer**: if a known record grew at the end, the old
 *     consumer reads what it knows and skips the rest.
 *
 * It is the most that can be afforded at this level: this is written by the
 * interrupt handler, millions of times.
 *
 * `length` counts the header, so advancing is one addition and not an addition
 * plus a correction somebody will forget.
 *
 * \~spanish
 * Cuatro bytes, y compran las dos propiedades que hacen que un anillo sobreviva
 * a un cambio de version:
 *
 *   - **saltar lo desconocido**: con `length` se avanza al siguiente registro
 *     sin entender este.  Sin eso, un tipo de muestra nuevo no degrada la
 *     lectura, la detiene;
 *   - **tolerar lo mas largo**: si un registro conocido creciera por el final,
 *     el consumidor antiguo lee lo que conoce y salta el resto.
 *
 * Es lo mas caro que se puede permitir a este nivel: esto lo escribe el
 * manejador de interrupcion, millones de veces.
 *
 * `length` cuenta la cabecera incluida, para que avanzar sea una suma y no una
 * suma con una correccion que alguien olvidara.
 */
typedef struct abi_record {
    /**< \~english which kind of record; 0 is unused, to catch zeroed memory
     *   \~spanish que clase de registro; 0 no se usa, para cazar memoria a
     *            cero \~ */
    u16 kind;
    /**< \~english TOTAL bytes, header included
     *   \~spanish bytes TOTALES, cabecera incluida \~ */
    u16 length;
} abi_record;

STATIC_ASSERT(sizeof(abi_record) == 4, "the record header must be 4 bytes");

/** \~english Reserved kind: a zeroed record is unwritten memory, not a record.
 *  \~spanish Tipo reservado: un registro a cero es memoria sin escribir, no un
 *            registro. \~ */
#define ABI_KIND_NONE 0

/* -------------------------------------------------------------------------
 *  \~english  Layer 2: control structures state their size.
 *  \~spanish  Capa 2: las estructuras de control dicen su tamano.
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * @brief
 * \~english First field of every structure that crosses an IOCTL.
 * \~spanish Primer campo de toda estructura que cruce por un IOCTL.
 * \~
 *
 * \~english
 * Whoever sends it puts the size of THEIR version; whoever receives it compares
 * against their own and decides.  That is what allows appending fields at the
 * end without breaking the other end.
 *
 * It is a struct and not a bare `u32` so that it is always declared the same way
 * and does not end up called `size` in one place and `cb` in another.
 *
 * \~spanish
 * Quien la envia pone el tamano de SU version; quien la recibe compara con la
 * suya y decide.  Es lo que permite anadir campos al final sin romper al otro
 * extremo.
 *
 * Va como estructura y no como un `u32` suelto para que se declare siempre igual
 * y no acabe llamandose `size` en un sitio y `cb` en otro.
 */
typedef struct abi_sized {
    /**< \~english sizeof of the containing struct, in WHOEVER wrote it
     *   \~spanish sizeof de la estructura que lo contiene, en QUIEN lo
     *            escribio \~ */
    u32 size;
} abi_sized;

/**
 * @brief
 * \~english Does it bring at least the fields we expected?
 * \~spanish ¿Trae al menos los campos que esperabamos?
 * \~
 *
 * @param got      \~english what the other end said \~spanish lo que dijo el otro extremo \~
 * @param expected \~english `sizeof` of the part we need to read \~spanish `sizeof` de la parte que necesitamos leer \~
 *
 * \~english
 * Bigger than expected **is correct**: it means the other end is newer and
 * brings fields not known here.  The known ones are read and the rest ignored.
 * Smaller is not: fields that were going to be read would be missing.
 *
 * \~spanish
 * Mas grande de lo esperado **es correcto**: significa que el otro extremo es
 * mas nuevo y trae campos que aqui no se conocen.  Se leen los conocidos y se
 * ignora el resto.  Mas pequeno no: faltarian campos que se iban a leer.
 */
static inline bool abi_sized_ok(u32 got, u32 expected) {
    return got >= expected;
}

/* -------------------------------------------------------------------------
 *  \~english  Layer 3: asking for the layout.
 *  \~spanish  Capa 3: preguntar por la disposicion.
 *  \~
 * ------------------------------------------------------------------------- */

/**
 * @brief
 * \~english A published field: where it is and how much it takes.
 * \~spanish Un campo publicado: donde esta y cuanto ocupa.
 * \~
 *
 * \~english
 * TO VALIDATE, NOT TO ACCESS.  The consumer asks for this when it connects and
 * compares it against what it was compiled with; afterwards it uses its usual
 * offsets.  An analyser resolving every field through here would be correct and
 * dreadfully slow, and we would be measuring the profiler instead of the
 * program.
 *
 * \~spanish
 * PARA VALIDAR, NO PARA ACCEDER.  El consumidor pide esto al conectar y lo
 * compara con lo que trae compilado; despues usa sus desplazamientos de siempre.
 * Un analizador que resolviera cada campo por aqui seria correcto y lentisimo, y
 * estariamos midiendo el perfilador en vez del programa.
 */
typedef struct abi_field {
    /**< \~english stable identity; does NOT change even if the name does
     *   \~spanish identidad estable; NO cambia aunque cambie el nombre \~ */
    u32 id;
    /**< \~english from the start of its structure
     *   \~spanish desde el principio de su estructura \~ */
    u32 offset;
    /**< \~english bytes it takes \~spanish bytes que ocupa \~ */
    u32 size;
} abi_field;

/** \~english A published structure.  \~spanish Una estructura publicada. \~ */
typedef struct abi_struct {
    /**< \~english the structure's stable identity
     *   \~spanish identidad estable de la estructura \~ */
    u32 id;
    /**< \~english total sizeof \~spanish sizeof total \~ */
    u32 size;
    /**< \~english how many entries follow \~spanish cuantas entradas siguen \~ */
    u32 field_count;
    /**< \~english the table, in declaration order
     *   \~spanish la tabla, en el orden de la declaracion \~ */
    const abi_field *fields;
} abi_struct;

/*
 * \~english
 * THIS FORMAT CAN NEVER CHANGE.  It is the chicken-and-egg problem: if the way
 * of describing the layout changed, there would be no way left to describe that
 * change.  Hence it is as simple as possible -- fixed-width integers and
 * nothing else -- and carries no version: it does not need one because it is
 * not touched.
 *
 * \~spanish
 * ESTE FORMATO NO PUEDE CAMBIAR NUNCA.  Es el problema del huevo y la gallina:
 * si cambiara la forma de describir la disposicion, no quedaria manera de
 * describir ese cambio.  De ahi que sea lo mas simple posible -- enteros de
 * ancho fijo y nada mas -- y que no lleve version: no la necesita porque no se
 * toca.
 * \~
 */
STATIC_ASSERT(sizeof(abi_field) == 12, "abi_field size must never change");

/* -------------------------------------------------------------------------
 *  \~english
 *  All three things come out of a single table.
 *
 *  A published structure is declared ONCE in an `.inc`, and that file is
 *  included several times with the macros defined differently: to assert the
 *  layout at compile time, and to build the run-time descriptor.  It is the same
 *  pattern as `windows/nt_layout.inc`.
 *
 *  Writing the descriptor by hand would be a second copy of the layout, and the
 *  copy that drifts is always the one nobody looks at.
 *
 *  \~spanish
 *  De una sola tabla salen las tres cosas.
 *
 *  Una estructura publicada se declara UNA vez en un `.inc`, y ese fichero se
 *  incluye varias veces con las macros definidas de forma distinta: para afirmar
 *  la disposicion al compilar, y para construir el descriptor de ejecucion.  Es
 *  el mismo patron que `windows/nt_layout.inc`.
 *
 *  Escribir el descriptor a mano seria una segunda copia de la disposicion, y la
 *  copia que se separa siempre es la que nadie mira.
 *  \~
 *
 *      // my_abi.inc
 *      ABI_SIZE(sample, 24)
 *      ABI_FIELD(sample, ip,  8, 1)
 *      ABI_FIELD(sample, tid, 16, 2)
 *
 *      \~english  // at compile time, to assert:
 *      \~spanish  // al compilar, para afirmar:
 *      \~
 *      #define ABI_SIZE(t, b)         ABI_ASSERT_SIZE(t, b)
 *      #define ABI_FIELD(t, f, o, id) ABI_ASSERT_FIELD(t, f, o)
 *      #include "my_abi.inc"
 *      #undef ABI_SIZE
 *      #undef ABI_FIELD
 *
 *      \~english  // and to build the table:
 *      \~spanish  // y para construir la tabla:
 *      \~
 *      static const abi_field sample_fields[] = {
 *      #define ABI_SIZE(t, b)
 *      #define ABI_FIELD(t, f, o, id) ABI_DESC_FIELD(t, f, id),
 *      #include "my_abi.inc"
 *      #undef ABI_SIZE
 *      #undef ABI_FIELD
 *      };
 * ------------------------------------------------------------------------- */

/* \~english `offsetof` without including `<stddef.h>`, which cannot be done
 * here.
 *
 * `__builtin_offsetof` belongs to GCC and Clang -- MSVC does NOT have it, even
 * though it is sometimes assumed because it accepts nearly everything else.  For
 * the rest there is the classic form, which is exactly what `<stddef.h>` defines
 * on those compilers.
 *
 * The builtin is preferred where it exists because the classic form
 * dereferences a null pointer -- in an unevaluated context, but analysers flag
 * it -- and besides does not always count as a constant expression.
 *
 * \~spanish `offsetof` sin incluir `<stddef.h>`, que aqui no se puede.
 *
 * `__builtin_offsetof` es de GCC y Clang -- MSVC NO lo tiene, aunque a veces se
 * de por hecho porque acepta casi todo lo demas.  Para el resto queda la forma
 * clasica, que es exactamente la que `<stddef.h>` define en esos compiladores.
 *
 * Se prefiere el builtin donde lo hay porque la forma clasica desreferencia un
 * puntero nulo -- en contexto no evaluado, pero los analizadores lo senalan -- y
 * ademas no siempre cuenta como expresion constante. \~ */
#if defined(__GNUC__) || defined(__clang__)
#define ABI_OFFSET_OF(type, field) __builtin_offsetof(type, field)
#else
#define ABI_OFFSET_OF(type, field) ((usize) & (((type *)0)->field))
#endif

/** \~english Asserts the total size of a published structure.
 *  \~spanish Afirma el tamano total de una estructura publicada. \~ */
#define ABI_ASSERT_SIZE(type, bytes)                                           \
    STATIC_ASSERT(sizeof(type) == (bytes),                                     \
                  #type ": published size does not match the real one")

/** \~english Asserts where a published field lands.
 *  \~spanish Afirma donde cae un campo publicado. \~ */
#define ABI_ASSERT_FIELD(type, field, off)                                     \
    STATIC_ASSERT(ABI_OFFSET_OF(type, field) == (off),                         \
                  #type "." #field ": published offset does not match the "    \
                                    "real one")

/** \~english One entry of the description table.
 *  \~spanish Una entrada de la tabla de descripcion. \~ */
#define ABI_DESC_FIELD(type, field, ident)                                     \
    { (u32)(ident), (u32)ABI_OFFSET_OF(type, field),                           \
      (u32)sizeof(((type *)0)->field) }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_ABI_H */
