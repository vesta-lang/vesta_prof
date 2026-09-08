/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file cpuid_index.h
 * @brief Los campos de CPUID de los dos fabricantes.
 *
 * ESTE FICHERO SI SE EDITA A MANO.  Los de dentro no: `cpuid/intel/` sale del
 * manual de Intel y `cpuid/amd/` del de AMD, cada uno con su generador.
 *
 * POR QUE ESTAN SEPARADOS.  Porque **el rango extendido lo reparten los dos
 * fabricantes y no dicen lo mismo**.  Intel documenta hasta `80000008H` y ahi
 * acaba lo suyo; de `80000009H` en adelante manda AMD, y ahi estan justo las
 * hojas que a un perfilador le hacen falta:
 *
 *     Fn8000_001B   IBS, el muestreo preciso de AMD
 *     Fn8000_001D   topologia de cache
 *     Fn8000_001E   hermanos SMT y nodo
 *     Fn8000_0022   extensiones de monitorizacion de rendimiento
 *
 * Que una hoja no este en el lado de Intel NO significa que no exista.
 * Significa que su fuente es otra -- y durante un rato de esta sesion se
 * confundieron las dos cosas, que es de donde sale este aviso.
 *
 * Los nombres de AMD llevan el prefijo `AMD_` para que no haya duda de que
 * manual salio cada uno.
 *
 * Y no incluye nada mas: solo macros.
 */

#ifndef VXP_COMMON_CPUID_INDEX_H
#define VXP_COMMON_CPUID_INDEX_H

#include "cpuid/intel/index.h"
#include "cpuid/amd/index.h"

#endif /* VXP_COMMON_CPUID_INDEX_H */
