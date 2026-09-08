/*
 * VestaVM - Maquina Virtual Distribuida
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
 * @brief Como el contrato entre el driver y el lado de usuario se describe a si
 *        mismo, para que una version nueva no rompa a la otra.
 *
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
 *  Capa 1: la cabecera de cada registro del anillo.
 * ------------------------------------------------------------------------- */

/**
 * @brief Lo que precede a CADA registro escrito en el anillo.
 *
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
 * manejador de la PMI, millones de veces.
 *
 * `length` cuenta la cabecera incluida, para que avanzar sea una suma y no una
 * suma con una correccion que alguien olvidara.
 */
typedef struct abi_record {
    u16 kind;   ///< que clase de registro; 0 no se usa, para cazar memoria a cero
    u16 length; ///< bytes TOTALES, cabecera incluida
} abi_record;

STATIC_ASSERT(sizeof(abi_record) == 4, "the record header must be 4 bytes");

/** Tipo reservado: un registro a cero es memoria sin escribir, no un registro. */
#define ABI_KIND_NONE 0

/* -------------------------------------------------------------------------
 *  Capa 2: las estructuras de control dicen su tamano.
 * ------------------------------------------------------------------------- */

/**
 * @brief Primer campo de toda estructura que cruce por un IOCTL.
 *
 * Quien la envia pone el tamano de SU version; quien la recibe compara con la
 * suya y decide.  Es lo que permite anadir campos al final sin romper al otro
 * extremo.
 *
 * Va como estructura y no como un `u32` suelto para que se declare siempre
 * igual y no acabe llamandose `size` en un sitio y `cb` en otro.
 */
typedef struct abi_sized {
    u32 size; ///< sizeof de la estructura que lo contiene, en QUIEN lo escribio
} abi_sized;

/**
 * @brief ¿Trae al menos los campos que esperabamos?
 *
 * @param got      lo que dijo el otro extremo.
 * @param expected `sizeof` de la parte que necesitamos leer.
 *
 * Mas grande de lo esperado **es correcto**: significa que el otro extremo es
 * mas nuevo y trae campos que aqui no se conocen.  Se leen los conocidos y se
 * ignora el resto.  Mas pequeno no: faltarian campos que se iban a leer.
 */
static inline int abi_sized_ok(u32 got, u32 expected) {
    return got >= expected;
}

/* -------------------------------------------------------------------------
 *  Capa 3: preguntar por la disposicion.
 * ------------------------------------------------------------------------- */

/**
 * @brief Un campo publicado: donde esta y cuanto ocupa.
 *
 * PARA VALIDAR, NO PARA ACCEDER.  El consumidor pide esto al conectar y lo
 * compara con lo que trae compilado; despues usa sus desplazamientos de
 * siempre.  Un analizador que resolviera cada campo por aqui seria correcto y
 * lentisimo, y estariamos midiendo el perfilador en vez del programa.
 */
typedef struct abi_field {
    u32 id;     ///< identidad estable del campo; NO cambia aunque cambie el nombre
    u32 offset; ///< desde el principio de su estructura
    u32 size;   ///< bytes que ocupa
} abi_field;

/** Una estructura publicada. */
typedef struct abi_struct {
    u32 id;                  ///< identidad estable de la estructura
    u32 size;                ///< sizeof total
    u32 field_count;         ///< cuantas entradas siguen
    const abi_field *fields; ///< tabla, en el orden de la declaracion
} abi_struct;

/*
 * ESTE FORMATO NO PUEDE CAMBIAR NUNCA.  Es el problema del huevo y la gallina:
 * si cambiara la forma de describir la disposicion, no quedaria manera de
 * describir ese cambio.  De ahi que sea lo mas simple posible -- enteros de
 * ancho fijo y nada mas -- y que no lleve version: no la necesita porque no se
 * toca.
 */
STATIC_ASSERT(sizeof(abi_field) == 12, "abi_field size must never change");

/* -------------------------------------------------------------------------
 *  De una sola tabla salen las tres cosas.
 *
 *  Una estructura publicada se declara UNA vez en un `.inc`, y ese fichero se
 *  incluye varias veces con las macros definidas de forma distinta: para
 *  afirmar la disposicion al compilar, y para construir el descriptor de
 *  ejecucion.  Es el mismo patron que `windows/nt_layout.inc`.
 *
 *  Escribir el descriptor a mano seria una segunda copia de la disposicion, y
 *  la copia que se separa siempre es la que nadie mira.
 *
 *  Uso:
 *
 *      // mi_abi.inc
 *      ABI_SIZE(muestra, 24)
 *      ABI_FIELD(muestra, ip,  8, 1)
 *      ABI_FIELD(muestra, tid, 16, 2)
 *
 *      // al compilar, para afirmar:
 *      #define ABI_SIZE(t, b)         ABI_ASSERT_SIZE(t, b)
 *      #define ABI_FIELD(t, f, o, id) ABI_ASSERT_FIELD(t, f, o)
 *      #include "mi_abi.inc"
 *      #undef ABI_SIZE
 *      #undef ABI_FIELD
 *
 *      // y para construir la tabla:
 *      static const abi_field muestra_fields[] = {
 *      #define ABI_SIZE(t, b)
 *      #define ABI_FIELD(t, f, o, id) ABI_DESC_FIELD(t, f, id),
 *      #include "mi_abi.inc"
 *      #undef ABI_SIZE
 *      #undef ABI_FIELD
 *      };
 * ------------------------------------------------------------------------- */

/* `offsetof` sin incluir `<stddef.h>`, que aqui no se puede.
 *
 * `__builtin_offsetof` es de GCC y Clang -- MSVC NO lo tiene, aunque a veces se
 * de por hecho porque acepta casi todo lo demas.  Para el resto queda la forma
 * clasica, que es exactamente la que `<stddef.h>` define en esos compiladores.
 *
 * Se prefiere el builtin donde lo hay porque la forma clasica desreferencia un
 * puntero nulo -- en contexto no evaluado, pero los analizadores lo senalan --
 * y ademas no siempre cuenta como expresion constante. */
#if defined(__GNUC__) || defined(__clang__)
#define ABI_OFFSET_OF(type, field) __builtin_offsetof(type, field)
#else
#define ABI_OFFSET_OF(type, field) ((usize) & (((type *)0)->field))
#endif

/** Afirma el tamano total de una estructura publicada. */
#define ABI_ASSERT_SIZE(type, bytes)                                           \
    STATIC_ASSERT(sizeof(type) == (bytes),                                     \
                  #type ": published size does not match the real one")

/** Afirma donde cae un campo publicado. */
#define ABI_ASSERT_FIELD(type, field, off)                                     \
    STATIC_ASSERT(ABI_OFFSET_OF(type, field) == (off),                         \
                  #type "." #field ": published offset does not match the "    \
                                    "real one")

/** Una entrada de la tabla de descripcion. */
#define ABI_DESC_FIELD(type, field, ident)                                     \
    { (u32)(ident), (u32)ABI_OFFSET_OF(type, field),                           \
      (u32)sizeof(((type *)0)->field) }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VXP_ABI_H */
