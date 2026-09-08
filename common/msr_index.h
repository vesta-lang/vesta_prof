/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file msr_index.h
 * @brief Las direcciones de MSR de los dos fabricantes.
 *
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
 * `common/msr.h`, escritos a mano: un numero se extrae de una tabla, pero saber
 * que `PEBS_FMT` no son los bits bajos, no.
 *
 * Y no incluye nada mas: solo macros, para que se pueda incluir desde
 * cualquier sitio.
 */

#ifndef VXP_COMMON_MSR_INDEX_H
#define VXP_COMMON_MSR_INDEX_H

#include "msr/intel/index.h"
#include "msr/amd/index.h"

#endif /* VXP_COMMON_MSR_INDEX_H */
