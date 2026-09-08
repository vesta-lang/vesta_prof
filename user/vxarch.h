/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file user/vxarch.h
 * @brief
 * \~english Reading what a microarchitecture costs: the `.vxarch` format.
 * \~spanish Leer lo que cuesta una microarquitectura: el formato `.vxarch`.
 * \~
 *
 * \~english
 * WHAT A `.vxarch` IS.  One text file per microarchitecture with what each
 * instruction form costs there: reciprocal throughput, latency, micro-operations
 * and which ports it uses.  It comes from the compiler, which builds it from
 * measurements published by uops.info and from LLVM's scheduling models.
 *
 * WHY THE PROFILER READS IT.  To say whether the time something took is a lot or
 * a little you need what it SHOULD have taken, and that is what this holds:
 *
 * @code
 *      residual  =  measured cycles   -   predicted cost
 *                   (the PMU)             (this table)
 * @endcode
 *
 * Everything else this tree does answers "where does the time go".  This is what
 * turns that into "and is that reasonable".
 *
 * A FORMAT DEPENDENCY, NOT A CODE ONE, and the difference is what keeps
 * `user/README.md`'s rule intact.  Nothing here links against the compiler; it
 * parses a documented text format whose header carries its version (`vxarch 1`)
 * and the provenance of its numbers -- the source, the date and the hash of the
 * data they came from.  The file can be read, diffed and archived on its own.
 *
 * IT DOES NOT ALLOCATE AND IT DOES NOT OPEN ANYTHING.  The caller reads the file
 * and hands over the bytes plus the arrays to fill.  It is the same discipline as
 * the rest of the tree, and here it buys something concrete: the parser is
 * exercised by a test with a handful of lines written inline, without needing the
 * three-hundred-kilobyte file.
 *
 * \~spanish
 * QUE ES UN `.vxarch`.  Un fichero de texto por microarquitectura con lo que
 * cuesta alli cada forma de instruccion: throughput reciproco, latencia,
 * microoperaciones y que puertos usa.  Viene del compilador, que lo construye
 * con medidas publicadas por uops.info y con los modelos de planificacion de
 * LLVM.
 *
 * POR QUE LO LEE EL PERFILADOR.  Para decir si el tiempo que costo algo es mucho
 * o poco hace falta lo que DEBERIA haber costado, y eso es lo que hay aqui: ver
 * el diagrama de arriba.  Todo lo demas de este arbol responde a "donde se va el
 * tiempo".  Esto es lo que lo convierte en "y eso es razonable".
 *
 * DEPENDENCIA DE FORMATO, NO DE CODIGO, y la diferencia es lo que deja intacta
 * la regla de `user/README.md`.  Aqui no se enlaza nada del compilador; se
 * interpreta un formato de texto documentado cuya cabecera lleva su version
 * (`vxarch 1`) y la procedencia de sus numeros -- la fuente, la fecha y el
 * resumen de los datos de los que salieron.  El fichero se puede leer, comparar
 * y archivar por su cuenta.
 *
 * NO RESERVA NI ABRE NADA.  El llamante lee el fichero y entrega los bytes mas
 * los arrays que hay que rellenar.  Es la misma disciplina que el resto del
 * arbol, y aqui compra algo concreto: el analizador lo ejercita un test con
 * cuatro lineas escritas a mano, sin necesitar el fichero de trescientos
 * kilobytes.
 */

#ifndef VXP_USER_VXARCH_H
#define VXP_USER_VXARCH_H

#include "vxp_base.h"

/** @brief
 *  \~english The version of the format this reader understands.
 *  \~spanish La version del formato que entiende este lector. \~ */
#define VXARCH_VERSION 1

/** @brief
 *  \~english Longest microarchitecture name kept.
 *  \~spanish Nombre de microarquitectura mas largo que se guarda. \~ */
#define VXARCH_NAME_MAX 32

/** @brief
 *  \~english The form has no cost in this microarchitecture.
 *  \~spanish La forma no tiene coste en esta microarquitectura. \~ */
#define VXARCH_NO_CLASS ((u16)0xFFFF)

/** @brief `class` flags. */
#define VXARCH_MICROCODED 0x01u
#define VXARCH_MACRO_FUSIBLE 0x02u

/**
 * @brief
 * \~english What one scheduling class costs.
 * \~spanish Lo que cuesta una clase de planificacion.
 * \~
 *
 * \~english
 * THOUSANDTHS AND NOT DECIMALS, on purpose.  The file writes two decimals
 * (`0.33`, `3.00`), so multiplying by a thousand is exact and there is no
 * rounding anywhere.  It also means this module needs no floating-point parser,
 * which in a tree that includes no standard library would be two hundred lines
 * to read numbers that are never going to have more than two decimals.
 *
 * A CLASS AND NOT A FORM.  Forms that cost the same share a class, which is why
 * a part with twenty-two thousand forms has one thousand six hundred classes.
 * The map from one to the other is the second half of the file.
 *
 * \~spanish
 * MILESIMAS Y NO DECIMALES, a proposito.  El fichero escribe dos decimales
 * (`0,33`, `3,00`), asi que multiplicar por mil es exacto y no hay redondeo en
 * ningun sitio.  Ademas evita que este modulo necesite un analizador de coma
 * flotante, que en un arbol que no incluye biblioteca estandar serian doscientas
 * lineas para leer numeros que nunca van a tener mas de dos decimales.
 *
 * UNA CLASE Y NO UNA FORMA.  Las formas que cuestan lo mismo comparten clase,
 * que es por lo que una pieza con veintidos mil formas tiene mil seiscientas
 * clases.  El mapa de una a otra es la segunda mitad del fichero.
 */
typedef struct vxarch_class {
    u32 recip_tp; /**< \~english reciprocal throughput, in thousandths \~spanish throughput reciproco, en milesimas \~ */
    u32 latency;  /**< \~english longest edge, in thousandths \~spanish arista mas larga, en milesimas \~ */
    u16 uops;
    u8 flags; /**< `VXARCH_*` */
    u8 _pad;
} vxarch_class;

/**
 * @brief
 * \~english A whole microarchitecture, already parsed.
 * \~spanish Una microarquitectura entera, ya interpretada.
 * \~
 *
 * \~english
 * The arrays are the CALLER's.  `form_class` is indexed by form number, so it
 * has to be as long as the highest one plus one, and the holes are
 * `VXARCH_NO_CLASS` -- most forms do not have a cost in most parts.
 *
 * \~spanish
 * Los arrays son del LLAMANTE.  `form_class` se indexa por numero de forma, asi
 * que tiene que ser tan largo como el mayor mas uno, y los huecos son
 * `VXARCH_NO_CLASS` -- la mayoria de las formas no tienen coste en la mayoria de
 * las piezas.
 */
/**
 * @brief
 * \~english How many characters the source hash takes, plus its terminator.
 * \~spanish Cuantos caracteres ocupa el resumen del origen, mas su terminador.
 * \~
 *
 * \~english
 * IT IS WHAT MAKES A FORM NUMBER SAFE TO WRITE DOWN.  The number that identifies
 * a form is a dense index the build assigns; it is not stable across
 * regenerations of the database, so anybody who records one -- a bench that knows
 * which form its kernel measures, for instance -- is recording something that can
 * come to mean another instruction.
 *
 * The header carries the hash of the data it was all built from.  Recording it
 * alongside turns a silent drift into a refusal: same hash, the numbers still
 * mean what they meant; different hash, say so and stop.
 *
 * \~spanish
 * ES LO QUE HACE SEGURO APUNTAR UN NUMERO DE FORMA.  El numero que identifica
 * una forma es un indice denso que asigna la construccion; no es estable entre
 * regeneraciones de la base, asi que quien apunte uno -- un banco que sabe que
 * forma mide su nucleo, por ejemplo -- esta apuntando algo que puede pasar a
 * significar otra instruccion.
 *
 * La cabecera lleva el resumen de los datos de los que salio todo.  Apuntarlo al
 * lado convierte una deriva silenciosa en una negativa: mismo resumen, los
 * numeros siguen queriendo decir lo que querian; resumen distinto, se dice y se
 * para.
 */
#define VXARCH_HASH_MAX 65

typedef struct vxarch {
    char name[VXARCH_NAME_MAX];
    char xml_sha256[VXARCH_HASH_MAX]; /**< \~english source hash from the header \~spanish resumen del origen, de la cabecera \~ */
    vxarch_class *classes;
    u32 class_cap;   /**< \~english how many fit \~spanish cuantas caben \~ */
    u32 class_count; /**< \~english how many the file had \~spanish cuantas traia el fichero \~ */
    u16 *form_class; /**< \~english form -> class, or `VXARCH_NO_CLASS` \~spanish forma -> clase, o `VXARCH_NO_CLASS` \~ */
    u32 form_cap;
    u32 form_count; /**< \~english how many forms carried a cost \~spanish cuantas formas llevaban coste \~ */
} vxarch;

/**
 * @brief
 * \~english Parses a `.vxarch` already in memory.
 * \~spanish Interpreta un `.vxarch` que ya esta en memoria.
 * \~
 *
 * @param text \~english the file's bytes \~spanish los bytes del fichero \~
 * @param len  \~english how many \~spanish cuantos \~
 * @param out  \~english with `classes`, `class_cap`, `form_class` and `form_cap` already set \~spanish con `classes`, `class_cap`, `form_class` y `form_cap` ya puestos \~
 * @return `OK`; `ERR_INVALID` \~english if it is not a `.vxarch` of a version this understands; `ERR_NOSPACE` if the arrays are short \~spanish si no es un `.vxarch` de una version que entienda; `ERR_NOSPACE` si los arrays se quedan cortos \~
 *
 * \~english
 * IT REFUSES A VERSION IT DOES NOT KNOW instead of reading what it recognises.
 * A format that grows a field would still parse line by line and would give
 * costs that are quietly wrong, which is the failure this whole tree is written
 * to not have.
 *
 * \~spanish
 * SE NIEGA ANTE UNA VERSION QUE NO CONOCE en vez de leer lo que reconozca.  Un
 * formato que anada un campo se seguiria interpretando linea a linea y daria
 * costes callada y equivocadamente, que es el fallo que este arbol entero esta
 * escrito para no tener.
 */
status vxarch_parse(const char *text, usize len, vxarch *out);

/**
 * @brief
 * \~english What form number `form` costs, or null if it costs nothing here.
 * \~spanish Lo que cuesta la forma numero `form`, o nulo si aqui no cuesta nada.
 * \~
 *
 * \~english
 * NULL IS AN ANSWER, and a different one from zero.  A form with no class in
 * this microarchitecture is one nobody measured or modelled there; returning a
 * cost of zero would say it is free.
 *
 * \~spanish
 * NULO ES UNA RESPUESTA, y distinta de cero.  Una forma sin clase en esta
 * microarquitectura es una que alli nadie midio ni modelo; devolver un coste de
 * cero diria que sale gratis.
 */
const vxarch_class *vxarch_cost(const vxarch *a, u32 form);

#endif /* VXP_USER_VXARCH_H */
