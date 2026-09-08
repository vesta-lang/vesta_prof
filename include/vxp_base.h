/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: MIT (ver LICENSE.MIT).  Esta en `include/`, que es el contrato
 * publico: una herramienta de terceros puede incluirlo sin que la GPLv2 del
 * resto del proyecto le alcance.
 */

/**
 * @file vxp_base.h
 * @brief
 * \~english The profiler's common floor: fixed-width types, status, and the
 *           four things that cannot be taken for granted across three
 *           environments.
 * \~spanish El cimiento comun del perfilador: tipos de ancho fijo, estado y las
 *           cuatro cosas que no se pueden dar por sentadas en tres entornos.
 * \~
 *
 * \~english
 * The module idiom is NOT here -- that lives in `vxp_object.h` -- nor the
 * contract's description -- that is `vxp_abi.h`.  This is only the floor: what
 * everyone else needs before being able to say anything.
 *
 * THE THREE ENVIRONMENTS.  This header is compiled in the Windows kernel, the
 * Linux kernel and user space, as C and as C++, with TDM/MinGW, Clang and MSVC.
 * Its two strangest constraints come from that:
 *
 *   - **It includes NOTHING.**  Not `<stdint.h>` -- which the Linux kernel does
 *     not have --, not `<stddef.h>`, no system header.  The types come from what
 *     the compiler already defines, and failing that, from a last resort checked
 *     at compile time.  A header that includes `<stdint.h>` compiles in two of
 *     the three environments and fails in the one that gets tested least.
 *   - **It compiles as C and as C++.**  Guards on the macros, `extern "C"` where
 *     it belongs, and no construct ISO C++ rejects.  In this very tree,
 *     `CreateELF.h` did not honour this, and the effect was that the code
 *     easiest to get wrong was the only one that could not be tested.
 *
 * ABOUT THE NAMES.  They carry no prefix -- `u32`, `OK`, `ALIGNOF` -- because
 * this is read and written constantly and a `VXP_` on every line is noise.  The
 * risk of clashing with another name is accepted knowingly, because a collision
 * **fails at compile time**: it is loud and immediate, not the kind of silent
 * failure this project hunts.  If it ever clashes, it gets fixed where it does.
 *
 * \~spanish
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
 * se lee y se escribe constantemente y `VXP_` en cada linea es ruido.  Se acepta
 * el riesgo de coincidir con otro nombre a sabiendas, porque una colision
 * **falla al compilar**: es ruidosa e inmediata, no de la clase de fallo
 * silencioso que este proyecto persigue.  Si alguna vez choca, se arregla donde
 * choca.
 */

#ifndef VXP_BASE_H
#define VXP_BASE_H

/* -------------------------------------------------------------------------
 *  \~english
 *  Fixed-width types, without including anything.
 *
 *  The Linux kernel spelling is used -- u8, u32, s64 -- and not one of our own,
 *  and it is not a matter of taste: in a kernel build those types ALREADY
 *  exist, so they are not defined here and theirs are used.  Zero chance of two
 *  definitions of the same width drifting apart.
 *
 *  \~spanish
 *  Tipos de ancho fijo, sin incluir nada.
 *
 *  Se usa la grafia del kernel de Linux -- u8, u32, s64 -- y no una propia, y no
 *  es cuestion de gusto: en una compilacion de kernel esos tipos YA existen, asi
 *  que aqui no se definen y se usan los suyos.  Cero posibilidad de que dos
 *  definiciones del mismo ancho se separen.
 *  \~
 * ------------------------------------------------------------------------- */

#if !defined(__KERNEL__)

/* \~english THE ORDER MATTERS, and it is not obvious: on Windows, Clang defines
 * `_MSC_VER` to pass itself off as MSVC.  Asking for `_MSC_VER` first makes it
 * take the `__intN` branch, which it rejects itself as a language extension
 * under `-pedantic-errors`.
 *
 * So first what GCC and Clang announce -- `clang-cl` included -- and the MSVC
 * branch is left for the real MSVC, which is the only one without them.
 *
 * \~spanish EL ORDEN IMPORTA, y no es evidente: en Windows, Clang define
 * `_MSC_VER` para hacerse pasar por MSVC.  Preguntar por `_MSC_VER` primero le
 * hace tomar la rama de los `__intN`, que el mismo rechaza como extension del
 * lenguaje bajo `-pedantic-errors`.
 *
 * Asi que primero lo que anuncian GCC y Clang -- incluido `clang-cl` --, y la
 * rama de MSVC queda para el MSVC de verdad, que es el unico que no los
 * tiene. \~ */
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

/** \~english A size in bytes, without `<stddef.h>`.
 *  \~spanish El tamano en bytes, sin `<stddef.h>`. \~ */
#if defined(__SIZE_TYPE__)
typedef __SIZE_TYPE__ usize;
#elif defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__)
typedef u64 usize;
#else
typedef u32 usize;
#endif

/* -------------------------------------------------------------------------
 *  \~english
 *  Truth value, without `<stdbool.h>`.
 *
 *  WHY IT IS NOT JUST `int`.  A predicate that returns `int` says nothing about
 *  what it returns: nothing stops it from giving 2, and nothing stops the caller
 *  from comparing it against 1.  Comparing a truth value against `== 1` is a bug
 *  that works right up until somebody returns the result of an `&` instead of a
 *  `!=`, and then it fails silently -- which is the class of failure this
 *  project exists to avoid.
 *
 *  WHY IT NEEDS FIVE BRANCHES.  Because there is no way to include
 *  `<stdbool.h>`, and each environment leaves it in a different state:
 *
 *    C++            `bool` is a keyword.  Touching it would be an error
 *    Linux kernel   already provides `bool`, `true` and `false`
 *    <stdbool.h>    the consumer may have included it BEFORE this header, and
 *                   then `bool` is a macro; typedef'ing over it does not compile
 *    C99 onwards    `_Bool` exists; the spelling `bool` does not
 *
 *  There is no branch for C older than C99, and that is not an oversight:
 *  **this header cannot be compiled as C90 at all**, because `u64` needs
 *  `long long` and C90 does not have it.  A fallback for a case that cannot
 *  arise would be dead code pretending to be a guarantee.
 *
 *  `true` and `false` are defined as `1` and `0`, which is exactly what
 *  `<stdbool.h>` defines: if a consumer includes it afterwards, the redefinition
 *  is identical and therefore legal.
 *
 *  \~spanish
 *  Valor de verdad, sin `<stdbool.h>`.
 *
 *  POR QUE NO VALE `int` A SECAS.  Un predicado que devuelve `int` no dice nada
 *  de lo que devuelve: nada le impide dar 2, y nada le impide a quien llama
 *  compararlo con 1.  Comparar un valor de verdad con `== 1` es un fallo que
 *  funciona hasta que alguien devuelve el resultado de un `&` en vez de un `!=`,
 *  y entonces falla en silencio -- que es la clase de fallo para la que existe
 *  este proyecto.
 *
 *  POR QUE NECESITA CINCO RAMAS.  Porque no hay forma de incluir `<stdbool.h>`,
 *  y cada entorno lo deja en un estado distinto:
 *
 *    C++              `bool` es palabra clave.  Tocarlo seria un error
 *    kernel de Linux  ya trae `bool`, `true` y `false`
 *    <stdbool.h>      el consumidor puede haberlo incluido ANTES que esta
 *                     cabecera, y entonces `bool` es una macro; hacerle un
 *                     typedef encima no compila
 *    C99 en adelante  existe `_Bool`; la grafia `bool` no
 *
 *  No hay rama para C anterior a C99, y no es un olvido: **esta cabecera no se
 *  puede compilar como C90 en absoluto**, porque `u64` necesita `long long` y
 *  C90 no lo tiene.  Un recurso final para un caso que no puede darse seria
 *  codigo muerto haciendose pasar por una garantia.
 *
 *  `true` y `false` se definen como `1` y `0`, que es exactamente lo que define
 *  `<stdbool.h>`: si un consumidor lo incluye despues, la redefinicion es
 *  identica y por tanto legal.
 *  \~
 * ------------------------------------------------------------------------- */

#if defined(__cplusplus) || defined(__KERNEL__) || defined(bool)
/* \~english Already there.  Nothing to declare.
 * \~spanish Ya esta.  No hay nada que declarar. \~ */
#else
typedef _Bool bool;
#define true 1
#define false 0
#endif

/* \~english A truth value is compared against `false`, or used bare, never
 * against `true`.  `_Bool` normalises -- assigning 2 to it yields 1 -- so here
 * it would work; the habit is kept because the day somebody widens a predicate
 * to `int` by mistake, `== true` starts lying and nothing says so.
 * \~spanish Un valor de verdad se compara contra `false`, o se usa a pelo, nunca
 * contra `true`.  `_Bool` normaliza -- asignarle 2 da 1 -- asi que aqui
 * funcionaria; la costumbre se mantiene porque el dia que alguien ensanche un
 * predicado a `int` por error, `== true` empieza a mentir y nada lo dice. \~ */

/* -------------------------------------------------------------------------
 *  \~english
 *  Compile-time assertion.
 *
 *  It has to work in C++ and in C, including C without C11: the kernel is built
 *  with C11 or later, but a consumer may not be.  The last resort is the
 *  negative-sized array -- ugly, but universal.
 *
 *  \~spanish
 *  Asercion de compilacion.
 *
 *  Tiene que funcionar en C++ y en C, incluido C sin C11: el kernel se compila
 *  con C11 o posterior, pero un consumidor puede no hacerlo.  El recurso final
 *  es el array de tamano negativo -- feo, pero universal.
 *  \~
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

/* \~english The widths, checked right here.  If the last resort were to miss on
 * some platform, it is known AT COMPILE TIME and not while reading a ring whose
 * fields are shifted.
 * \~spanish Los anchos, comprobados aqui mismo.  Si el recurso final no acertara
 * en alguna plataforma, se sabe AL COMPILAR y no al leer un anillo cuyos campos
 * estan corridos. \~ */
STATIC_ASSERT(sizeof(u8) == 1, "u8 is not 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "u16 is not 2 bytes");
STATIC_ASSERT(sizeof(u32) == 4, "u32 is not 4 bytes");
STATIC_ASSERT(sizeof(u64) == 8, "u64 is not 8 bytes");
STATIC_ASSERT(sizeof(s32) == 4, "s32 is not 4 bytes");
STATIC_ASSERT(sizeof(s64) == 8, "s64 is not 8 bytes");
STATIC_ASSERT(sizeof(usize) == sizeof(void *),
              "usize does not match pointer width");

/* -------------------------------------------------------------------------
 *  \~english  Alignment.
 *  \~spanish  Alineacion.
 *  \~
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
 *  \~english  Status.
 *  \~spanish  Estado.
 *  \~
 * ------------------------------------------------------------------------- */

/* \~english Marks the return so that IGNORING it is a compiler warning.  It is
 * the missing half of "return status, not a pointer": returning a code is no
 * use if the caller can forget to look at it, and forgetting is the norm when
 * the function almost never fails -- which is exactly when it matters.
 *
 * Two things about MSVC, both learned by breaking it:
 *
 *  - `_MSVC_LANG` and not `__cplusplus`.  MSVC announces 199711L in
 *    `__cplusplus` unless given `/Zc:__cplusplus`, so asking the usual way says
 *    there is no C++17 even when there is.
 *  - `_Check_return_` is NOT used: it is a SAL macro, it lives in `sal.h`, and
 *    this header includes nothing.  On MSVC without C++17 it is left unmarked --
 *    a worse warning, but it does not break.
 *
 * \~spanish Marca el retorno para que IGNORARLO sea un aviso del compilador.  Es
 * la mitad que falta de "devolver estado, no puntero": de nada sirve devolver un
 * codigo si quien llama puede olvidarse de mirarlo, y olvidarse es lo normal
 * cuando la funcion casi nunca falla -- que es exactamente cuando importa.
 *
 * Dos cosas de MSVC, las dos aprendidas rompiendolo:
 *
 *  - `_MSVC_LANG` y no `__cplusplus`.  MSVC anuncia 199711L en `__cplusplus`
 *    salvo que se le pase `/Zc:__cplusplus`, asi que preguntar de la forma
 *    habitual da que no hay C++17 aunque lo haya.
 *  - NO se usa `_Check_return_`: es una macro de SAL, vive en `sal.h`, y esta
 *    cabecera no incluye nada.  En MSVC sin C++17 se queda sin marca -- peor
 *    aviso, pero no rompe. \~ */
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
 * @brief
 * \~english How an operation ended.
 * \~spanish Como termino una operacion.
 * \~
 *
 * \~english
 * `OK` is ZERO on purpose, so that `if (rc)` reads as "if something happened".
 * The rest are negative so they cannot be mistaken for a count: a function that
 * returns "how many" and can fail uses the same type with no ambiguity.
 *
 * \~spanish
 * `OK` es CERO a proposito, para que `if (rc)` se lea como "si paso algo".  Los
 * demas son negativos para que no se confundan con una cuenta: una funcion que
 * devuelva "cuantos" y pueda fallar usa el mismo tipo sin ambiguedad.
 */
typedef enum status {
    /**< \~english nothing to report \~spanish sin novedad \~ */
    OK = 0,
    /**< \~english argument outside the contract
     *   \~spanish argumento fuera de contrato \~ */
    ERR_INVALID = -1,
    /**< \~english does not fit: short buffer, full ring
     *   \~spanish no cabe: bufer corto, anillo lleno \~ */
    ERR_NOSPACE = -2,
    /**< \~english this machine cannot do it
     *   \~spanish esta maquina no lo puede hacer \~ */
    ERR_UNSUPPORTED = -3,
    /**< \~english asked for at the wrong moment
     *   \~spanish se pidio en el momento equivocado \~ */
    ERR_STATE = -4,
    /**< \~english the system failed doing it: missing MSR, unreadable user
     *            memory
     *   \~spanish el sistema fallo al hacerlo: MSR inexistente, memoria de
     *            usuario ilegible \~ */
    ERR_FAULT = -5
} status;

/* -------------------------------------------------------------------------
 *  \~english  Minimal utilities.
 *  \~spanish  Utilidades minimas.
 *  \~
 * ------------------------------------------------------------------------- */

/** \~english How many elements an array has.  Only over real arrays.
 *  \~spanish Cuantos elementos tiene un array.  Solo sobre arrays de verdad. \~ */
#define COUNT_OF(a) (sizeof(a) / sizeof((a)[0]))

/** \~english Invalid index.  See the rule: indices with a sentinel, never
 *            pointers.
 *  \~spanish Indice invalido.  Ver la regla: indices con centinela, nunca
 *            punteros. \~ */
#define INDEX_INVALID ((u32)0xFFFFFFFFu)

#endif /* VXP_BASE_H */
