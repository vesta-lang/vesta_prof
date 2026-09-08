/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 + excepcion de runtime (ver LICENSE).
 */

/**
 * @file vxp_base.h
 * @brief El cimiento comun del perfilador: tipos de ancho fijo, estado y las
 *        cuatro cosas que no se pueden dar por sentadas en tres entornos.
 *
 * Aqui NO esta el idioma de los modulos -- eso vive en `vxp_object.h` -- ni la
 * descripcion del contrato -- en `vxp_abi.h`.  Esto es solo el suelo: lo que
 * todos los demas necesitan antes de poder decir nada.
 *
 * LOS TRES ENTORNOS.  Esta cabecera se compila en kernel de Windows, kernel de
 * Linux y espacio de usuario, como C y como C++, con TDM/MinGW, Clang y MSVC.
 * De ahi salen sus dos restricciones mas raras:
 *
 *   - **No incluye NADA.**  Ni `<stdint.h>` -- que el kernel de Linux no tiene
 *     --, ni `<stddef.h>`, ni cabeceras del sistema.  Los tipos salen de lo que
 *     el compilador ya define, y si eso falla, de un recurso final que se
 *     comprueba al compilar.  Una cabecera que incluya `<stdint.h>` compila en
 *     dos de los tres entornos y falla en el que menos veces se prueba.
 *   - **Compila como C y como C++.**  Guardas en las macros, `extern "C"` donde
 *     toca, y ningun constructo que ISO C++ rechace.  En este mismo arbol,
 *     `CreateELF.h` no cumplia esto y el efecto fue que el codigo mas facil de
 *     equivocar era el unico que no se podia probar.
 *
 * SOBRE LOS NOMBRES.  Van sin prefijo -- `u32`, `OK`, `ALIGNOF` -- porque esto
 * se lee y se escribe constantemente y `VXP_` en cada linea es ruido.  Se
 * acepta el riesgo de coincidir con otro nombre a sabiendas, porque una
 * colision **falla al compilar**: es ruidosa e inmediata, no de la clase de
 * fallo silencioso que este proyecto persigue.  Si alguna vez choca, se arregla
 * donde choca.
 */

#ifndef VXP_BASE_H
#define VXP_BASE_H

/* -------------------------------------------------------------------------
 *  Tipos de ancho fijo, sin incluir nada.
 *
 *  Se usa la grafia del kernel de Linux -- u8, u32, s64 -- y no una propia, y
 *  no es cuestion de gusto: en una compilacion de kernel esos tipos YA existen,
 *  asi que aqui no se definen y se usan los suyos.  Cero posibilidad de que dos
 *  definiciones del mismo ancho se separen.
 * ------------------------------------------------------------------------- */

#if !defined(__KERNEL__)

/* EL ORDEN IMPORTA, y no es evidente: en Windows, Clang define `_MSC_VER` para
 * hacerse pasar por MSVC.  Preguntar por `_MSC_VER` primero le hace tomar la
 * rama de los `__intN`, que el mismo rechaza como extension del lenguaje bajo
 * `-pedantic-errors`.
 *
 * Asi que primero lo que anuncian GCC y Clang -- incluido `clang-cl` --, y la
 * rama de MSVC queda para el MSVC de verdad, que es el unico que no los tiene. */
#if defined(__UINT32_TYPE__)
typedef __UINT8_TYPE__ u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;
typedef __INT8_TYPE__ s8;
typedef __INT16_TYPE__ s16;
typedef __INT32_TYPE__ s32;
typedef __INT64_TYPE__ s64;
#elif defined(_MSC_VER)
typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
typedef signed __int8 s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;
#else
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
#endif

#endif /* !__KERNEL__ */

/** El tamano en bytes, sin `<stddef.h>`. */
#if defined(__SIZE_TYPE__)
typedef __SIZE_TYPE__ usize;
#elif defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__)
typedef u64 usize;
#else
typedef u32 usize;
#endif

/* -------------------------------------------------------------------------
 *  Asercion de compilacion.
 *
 *  Tiene que funcionar en C++ y en C, incluido C sin C11: el kernel se compila
 *  con C11 o posterior, pero un consumidor puede no hacerlo.  El recurso final
 *  es el array de tamano negativo -- feo, pero universal.
 * ------------------------------------------------------------------------- */

#if defined(__cplusplus)
#define STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define STATIC_ASSERT_JOIN2(a, b) a##b
#define STATIC_ASSERT_JOIN(a, b) STATIC_ASSERT_JOIN2(a, b)
#define STATIC_ASSERT(cond, msg)                                               \
    typedef char STATIC_ASSERT_JOIN(static_assert_, __LINE__)[(cond) ? 1 : -1]
#endif

/* Los anchos, comprobados aqui mismo.  Si el recurso final no acertara en
 * alguna plataforma, se sabe AL COMPILAR y no al leer un anillo cuyos campos
 * estan corridos. */
STATIC_ASSERT(sizeof(u8) == 1, "u8 is not 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "u16 is not 2 bytes");
STATIC_ASSERT(sizeof(u32) == 4, "u32 is not 4 bytes");
STATIC_ASSERT(sizeof(u64) == 8, "u64 is not 8 bytes");
STATIC_ASSERT(sizeof(s32) == 4, "s32 is not 4 bytes");
STATIC_ASSERT(sizeof(s64) == 8, "s64 is not 8 bytes");
STATIC_ASSERT(sizeof(usize) == sizeof(void *),
              "usize does not match pointer width");

/* -------------------------------------------------------------------------
 *  Alineacion.
 * ------------------------------------------------------------------------- */

#if defined(__cplusplus)
#define ALIGNAS(n) alignas(n)
#define ALIGNOF(t) alignof(t)
#elif defined(_MSC_VER) && !defined(__clang__)
#define ALIGNAS(n) __declspec(align(n))
#define ALIGNOF(t) __alignof(t)
#else
#define ALIGNAS(n) __attribute__((aligned(n)))
#define ALIGNOF(t) __alignof__(t)
#endif

/* -------------------------------------------------------------------------
 *  Estado.
 * ------------------------------------------------------------------------- */

/* Marca el retorno para que IGNORARLO sea un aviso del compilador.  Es la mitad
 * que falta de "devolver estado, no puntero": de nada sirve devolver un codigo
 * si quien llama puede olvidarse de mirarlo, y olvidarse es lo normal cuando la
 * funcion casi nunca falla -- que es exactamente cuando importa.
 *
 * Dos cosas de MSVC, las dos aprendidas rompiendolo:
 *
 *  - `_MSVC_LANG` y no `__cplusplus`.  MSVC anuncia 199711L en `__cplusplus`
 *    salvo que se le pase `/Zc:__cplusplus`, asi que preguntar de la forma
 *    habitual da que no hay C++17 aunque lo haya.
 *  - NO se usa `_Check_return_`: es una macro de SAL, vive en `sal.h`, y esta
 *    cabecera no incluye nada.  En MSVC sin C++17 se queda sin marca -- peor
 *    aviso, pero no rompe. */
#if defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#define MUST_CHECK [[nodiscard]]
#elif defined(__cplusplus) && __cplusplus >= 201703L
#define MUST_CHECK [[nodiscard]]
#elif defined(__GNUC__) || defined(__clang__)
#define MUST_CHECK __attribute__((warn_unused_result))
#else
#define MUST_CHECK
#endif

/**
 * @brief Como termino una operacion.
 *
 * `OK` es CERO a proposito, para que `if (rc)` se lea como "si paso algo".  Los
 * demas son negativos para que no se confundan con una cuenta: una funcion que
 * devuelva "cuantos" y pueda fallar usa el mismo tipo sin ambiguedad.
 */
typedef enum status {
    OK = 0,               ///< sin novedad
    ERR_INVALID = -1,     ///< argumento fuera de contrato
    ERR_NOSPACE = -2,     ///< no cabe: bufer corto, anillo lleno
    ERR_UNSUPPORTED = -3, ///< esta maquina no lo puede hacer
    ERR_STATE = -4,       ///< se pidio en el momento equivocado
    ERR_FAULT = -5        ///< el sistema fallo al hacerlo: MSR inexistente,
                          ///< memoria de usuario ilegible
} status;

/* -------------------------------------------------------------------------
 *  Utilidades minimas.
 * ------------------------------------------------------------------------- */

/** Cuantos elementos tiene un array.  Solo sobre arrays de verdad. */
#define COUNT_OF(a) (sizeof(a) / sizeof((a)[0]))

/** Indice invalido.  Ver la regla: indices con centinela, nunca punteros. */
#define INDEX_INVALID ((u32)0xFFFFFFFFu)

#endif /* VXP_BASE_H */
