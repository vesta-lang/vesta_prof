/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file msr/index.h
 * @brief
 * \~english The MSR addresses of both vendors.
 * \~spanish Las direcciones de MSR de los dos fabricantes.
 * \~
 *
 * \~english
 * THIS FILE IS EDITED BY HAND.  The ones inside are not: `msr/intel/` comes from
 * Intel's manual and `msr/amd/` from AMD's PPR, each with its own generator.
 * This is the seam, and the seam is where it has to be explained why there are
 * two sides.
 *
 * WHY THEY ARE KEPT APART, AND IT IS NOT TIDINESS.  **The two address spaces
 * overlap without coinciding.**  There are three cases and they are worth not
 * confusing:
 *
 *   - the SAME register with two names: `0xC0000080` is `IA32_EFER` for Intel
 *     and `AMD_EFER` for AMD.  Both definitions are correct and worth the same,
 *     because they describe the same silicon from two manuals;
 *   - registers that only exist in ONE: `0xC0011030` (`AMD_IBS_FETCH_CTL`) has
 *     no Intel equivalent, and `IA32_PEBS_ENABLE` has none on AMD;
 *   - and the trap: the SAME address with ANOTHER meaning on each vendor.
 *
 * Mixing them into one file would lose whose each thing is, which is exactly
 * what has to be known before writing an MSR.  Hence the `AMD_` prefix on the
 * AMD side: it is not decoration, it is the answer to "which manual did this
 * come out of?".
 *
 * WHAT IS HERE AND WHAT IS NOT.  Only ADDRESSES.  The bit fields of the
 * registers the profiler decodes, and their existence conditions, live in
 * `common/msr/access.h`, written by hand: a number can be extracted from a
 * table, but knowing that `PEBS_FMT` is not the low bits cannot.
 *
 * And it includes nothing else: only macros, so it can be included from
 * anywhere.
 *
 * \~spanish
 * ESTE FICHERO SI SE EDITA A MANO.  Los de dentro no: `msr/intel/` sale del
 * manual de Intel y `msr/amd/` del PPR de AMD, cada uno con su generador.  Este
 * es la costura, y la costura es donde hay que explicar por que hay dos lados.
 *
 * POR QUE ESTAN SEPARADOS, Y NO ES ORDEN.  **Los dos espacios de direcciones se
 * solapan sin coincidir.**  Hay tres casos y conviene no confundirlos:
 *
 *   - el MISMO registro con dos nombres: `0xC0000080` es `IA32_EFER` para Intel
 *     y `AMD_EFER` para AMD.  Las dos definiciones son correctas y valen lo
 *     mismo, porque describen el mismo silicio desde dos manuales;
 *   - registros que solo existen en UNO: `0xC0011030` (`AMD_IBS_FETCH_CTL`) no
 *     tiene equivalente Intel, e `IA32_PEBS_ENABLE` no lo tiene en AMD;
 *   - y la trampa: la MISMA direccion con OTRO significado en cada fabricante.
 *
 * Mezclarlos en un fichero perderia de quien es cada cosa, que es justo lo que
 * hace falta saber antes de escribir un MSR.  De ahi el prefijo `AMD_` en el
 * lado de AMD: no es decoracion, es la respuesta a "¿de que manual salio esto?".
 *
 * QUE HAY Y QUE NO.  Solo DIRECCIONES.  Los campos de bits de los registros que
 * el perfilador decodifica, y sus condiciones de existencia, viven en
 * `common/msr/access.h`, escritos a mano: un numero se extrae de una tabla, pero
 * saber que `PEBS_FMT` no son los bits bajos, no.
 *
 * Y no incluye nada mas: solo macros, para que se pueda incluir desde cualquier
 * sitio.
 */

#ifndef VXP_COMMON_MSR_INDEX_H
#define VXP_COMMON_MSR_INDEX_H

#include "intel/index.h"
#include "amd/index.h"

#endif /* VXP_COMMON_MSR_INDEX_H */
