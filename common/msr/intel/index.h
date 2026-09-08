/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file index.h
 * @brief Todas las direcciones de MSR del manual, por familia.
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_msr_index.py
 *
 * a partir del volcado de texto del manual de Intel, que no viaja en el
 * repositorio.  Editarlo aqui se pierde en la siguiente regeneracion, y ademas
 * separa el fichero de su fuente, que es como una tabla empieza a mentir.
 *
 * Solo DIRECCIONES.  Los campos de bits de los registros que se decodifican
 * viven en `common/msr.h`, escritos a mano, porque llevan el porque de cada uno
 * y eso no sale de una tabla.
 *
 * Los nombres son los del manual, literales, para que se pueda buscar en el la
 * misma cadena que hay aqui.
 *
 * ALCANCE: **SOLO INTEL**, que no es lo mismo que "completo".  Los MSR de AMD
 * -- los de IBS entre ellos, que es su mecanismo de muestreo preciso -- se
 * documentan en el manual de AMD y NO estan aqui.  Que falten no significa que
 * no existan: significa que su fuente es otra, y que hay que tratarla igual que
 * esta en vez de escribirlos de memoria.
 */

#ifndef VXP_COMMON_MSR_INTEL_INDEX_H
#define VXP_COMMON_MSR_INTEL_INDEX_H

#include "msr_legacy.h"
#include "msr_mcheck.h"
#include "msr_memory.h"
#include "msr_misc.h"
#include "msr_perf.h"
#include "msr_platform.h"
#include "msr_power.h"
#include "msr_security.h"
#include "msr_virt.h"

/*
 * NOMBRES CON MAS DE UNA DIRECCION, y por eso NO se definen aqui.
 *
 * No es un descuido del manual: los bancos de comprobacion de
 * maquina y algunos contadores se rebasan por familia de
 * procesador, asi que el mismo nombre vale una cosa en una pieza y
 * otra en otra.  Elegir una direccion seria publicar la equivocada
 * para la mitad de las maquinas.
 *
 * Quien los necesite tiene que resolverlos POR FAMILIA, sabiendo
 * en cual esta.  Se listan para que conste que se vieron.
 *
 *   IA32_MC3_ADDR                      0x40E, 0x412
 *   IA32_MC3_CTL                       0x40C, 0x410
 *   IA32_MC3_MISC                      0x40F, 0x413
 *   IA32_MC3_STATUS                    0x40D, 0x411
 *   IA32_MC4_ADDR                      0x40E, 0x412
 *   IA32_MC4_CTL                       0x40C, 0x410
 *   IA32_MC4_STATUS                    0x40D, 0x411
 *   MSR_C0_PMON_BOX_CTL                0xD04, 0xE00
 *   MSR_C0_PMON_BOX_FILTER1            0xD1A, 0xE06
 *   MSR_C0_PMON_BOX_STATUS             0xD01, 0xE07
 *   MSR_C0_PMON_CTR0                   0xD11, 0xD16, 0xE08
 *   MSR_C0_PMON_CTR1                   0xD13, 0xD17, 0xE09
 *   MSR_C0_PMON_CTR2                   0xD15, 0xD18, 0xE0A
 *   MSR_C0_PMON_CTR3                   0xD17, 0xD19, 0xE0B
 *   MSR_C0_PMON_EVNTSEL0               0xD10, 0xE01
 *   MSR_C0_PMON_EVNTSEL1               0xD11, 0xE02
 *   MSR_C0_PMON_EVNTSEL2               0xD12, 0xE03
 *   MSR_C0_PMON_EVNTSEL3               0xD13, 0xE04
 *   MSR_C10_PMON_BOX_CTL               0xE44, 0xEA0
 *   MSR_C10_PMON_BOX_FILTER1           0xE5A, 0xEA6
 *   MSR_C10_PMON_CTR0                  0xE56, 0xEA8
 *   MSR_C10_PMON_CTR1                  0xE57, 0xEA9
 *   MSR_C10_PMON_CTR2                  0xE58, 0xEAA
 *   MSR_C10_PMON_CTR3                  0xE59, 0xEAB
 *   MSR_C10_PMON_EVNTSEL0              0xE50, 0xEA1
 *   MSR_C10_PMON_EVNTSEL1              0xE51, 0xEA2
 *   MSR_C10_PMON_EVNTSEL2              0xE52, 0xEA3
 *   MSR_C10_PMON_EVNTSEL3              0xE53, 0xEA4
 *   MSR_C11_PMON_BOX_CTL               0xE64, 0xEB0
 *   MSR_C11_PMON_BOX_FILTER1           0xE7A, 0xEB6
 *   MSR_C11_PMON_CTR0                  0xE76, 0xEB8
 *   MSR_C11_PMON_CTR1                  0xE77, 0xEB9
 *   MSR_C11_PMON_CTR2                  0xE78, 0xEBA
 *   MSR_C11_PMON_CTR3                  0xE79, 0xEBB
 *   MSR_C11_PMON_EVNTSEL0              0xE70, 0xEB1
 *   MSR_C11_PMON_EVNTSEL1              0xE71, 0xEB2
 *   MSR_C11_PMON_EVNTSEL2              0xE72, 0xEB3
 *   MSR_C11_PMON_EVNTSEL3              0xE73, 0xEB4
 *   MSR_C12_PMON_BOX_CTL               0xE84, 0xEC0
 *   MSR_C12_PMON_BOX_FILTER1           0xE9A, 0xEC6
 *   MSR_C12_PMON_CTR0                  0xE96, 0xEC8
 *   MSR_C12_PMON_CTR1                  0xE97, 0xEC9
 *   MSR_C12_PMON_CTR2                  0xE98, 0xECA
 *   MSR_C12_PMON_CTR3                  0xE99, 0xECB
 *   MSR_C12_PMON_EVNTSEL0              0xE90, 0xEC1
 *   MSR_C12_PMON_EVNTSEL1              0xE91, 0xEC2
 *   MSR_C12_PMON_EVNTSEL2              0xE92, 0xEC3
 *   MSR_C12_PMON_EVNTSEL3              0xE93, 0xEC4
 *   MSR_C13_PMON_BOX_CTL               0xEA4, 0xED0
 *   MSR_C13_PMON_BOX_FILTER1           0xEBA, 0xED6
 *   MSR_C13_PMON_CTR0                  0xEB6, 0xED8
 *   MSR_C13_PMON_CTR1                  0xEB7, 0xED9
 *   MSR_C13_PMON_CTR2                  0xEB8, 0xEDA
 *   MSR_C13_PMON_CTR3                  0xEB9, 0xEDB
 *   MSR_C13_PMON_EVNTSEL0              0xEB0, 0xED1
 *   MSR_C13_PMON_EVNTSEL1              0xEB1, 0xED2
 *   MSR_C13_PMON_EVNTSEL2              0xEB2, 0xED3
 *   MSR_C13_PMON_EVNTSEL3              0xEB3, 0xED4
 *   MSR_C14_PMON_BOX_CTL               0xEC4, 0xEE0
 *   MSR_C14_PMON_BOX_FILTER            0xED4, 0xEE5
 *   MSR_C14_PMON_BOX_FILTER1           0xEDA, 0xEE6
 *   MSR_C14_PMON_CTR0                  0xED6, 0xEE8
 *   MSR_C14_PMON_CTR1                  0xED7, 0xEE9
 *   MSR_C14_PMON_CTR2                  0xED8, 0xEEA
 *   MSR_C14_PMON_CTR3                  0xED9, 0xEEB
 *   MSR_C14_PMON_EVNTSEL0              0xED0, 0xEE1
 *   MSR_C14_PMON_EVNTSEL1              0xED1, 0xEE2
 *   MSR_C14_PMON_EVNTSEL2              0xED2, 0xEE3
 *   MSR_C14_PMON_EVNTSEL3              0xED3, 0xEE4
 *   MSR_C1_PMON_BOX_CTL                0xD24, 0xE10
 *   MSR_C1_PMON_BOX_FILTER1            0xD3A, 0xE16
 *   MSR_C1_PMON_BOX_STATUS             0xD81, 0xE17
 *   MSR_C1_PMON_CTR0                   0xD36, 0xD91, 0xE18
 *   MSR_C1_PMON_CTR1                   0xD37, 0xD93, 0xE19
 *   MSR_C1_PMON_CTR2                   0xD38, 0xD95, 0xE1A
 *   MSR_C1_PMON_CTR3                   0xD39, 0xD97, 0xE1B
 *   MSR_C1_PMON_EVNTSEL0               0xD30, 0xE11
 *   MSR_C1_PMON_EVNTSEL1               0xD31, 0xE12
 *   MSR_C1_PMON_EVNTSEL2               0xD32, 0xE13
 *   MSR_C1_PMON_EVNTSEL3               0xD33, 0xE14
 *   MSR_C2_PMON_BOX_CTL                0xD44, 0xE20
 *   MSR_C2_PMON_BOX_FILTER1            0xD5A, 0xE26
 *   MSR_C2_PMON_BOX_STATUS             0xD41, 0xE27
 *   MSR_C2_PMON_CTR0                   0xD51, 0xD56, 0xE28
 *   MSR_C2_PMON_CTR1                   0xD53, 0xD57, 0xE29
 *   MSR_C2_PMON_CTR2                   0xD55, 0xD58, 0xE2A
 *   MSR_C2_PMON_CTR3                   0xD57, 0xD59, 0xE2B
 *   MSR_C2_PMON_EVNTSEL0               0xD50, 0xE21
 *   MSR_C2_PMON_EVNTSEL1               0xD51, 0xE22
 *   MSR_C2_PMON_EVNTSEL2               0xD52, 0xE23
 *   MSR_C2_PMON_EVNTSEL3               0xD53, 0xE24
 *   MSR_C3_PMON_BOX_CTL                0xD64, 0xE30
 *   MSR_C3_PMON_BOX_FILTER1            0xD7A, 0xE36
 *   MSR_C3_PMON_BOX_STATUS             0xDC1, 0xE37
 *   MSR_C3_PMON_CTR0                   0xD76, 0xDD1, 0xE38
 *   MSR_C3_PMON_CTR1                   0xD77, 0xDD3, 0xE39
 *   MSR_C3_PMON_CTR2                   0xD78, 0xDD5, 0xE3A
 *   MSR_C3_PMON_CTR3                   0xD79, 0xDD7, 0xE3B
 *   MSR_C3_PMON_EVNTSEL0               0xD70, 0xE31
 *   MSR_C3_PMON_EVNTSEL1               0xD71, 0xE32
 *   MSR_C3_PMON_EVNTSEL2               0xD72, 0xE33
 *   MSR_C3_PMON_EVNTSEL3               0xD73, 0xE34
 *   MSR_C4_PMON_BOX_CTL                0xD84, 0xE40
 *   MSR_C4_PMON_BOX_FILTER1            0xD9A, 0xE46
 *   MSR_C4_PMON_BOX_STATUS             0xD21, 0xE47
 *   MSR_C4_PMON_CTR0                   0xD31, 0xD96, 0xE48
 *   MSR_C4_PMON_CTR1                   0xD33, 0xD97, 0xE49
 *   MSR_C4_PMON_CTR2                   0xD35, 0xD98, 0xE4A
 *   MSR_C4_PMON_CTR3                   0xD37, 0xD99, 0xE4B
 *   MSR_C4_PMON_EVNTSEL0               0xD90, 0xE41
 *   MSR_C4_PMON_EVNTSEL1               0xD91, 0xE42
 *   MSR_C4_PMON_EVNTSEL2               0xD92, 0xE43
 *   MSR_C4_PMON_EVNTSEL3               0xD93, 0xE44
 *   MSR_C5_PMON_BOX_CTL                0xDA4, 0xE50
 *   MSR_C5_PMON_BOX_FILTER1            0xDBA, 0xE56
 *   MSR_C5_PMON_BOX_STATUS             0xDA1, 0xE57
 *   MSR_C5_PMON_CTR0                   0xDB1, 0xDB6, 0xE58
 *   MSR_C5_PMON_CTR1                   0xDB3, 0xDB7, 0xE59
 *   MSR_C5_PMON_CTR2                   0xDB5, 0xDB8, 0xE5A
 *   MSR_C5_PMON_CTR3                   0xDB7, 0xDB9, 0xE5B
 *   MSR_C5_PMON_EVNTSEL0               0xDB0, 0xE51
 *   MSR_C5_PMON_EVNTSEL1               0xDB1, 0xE52
 *   MSR_C5_PMON_EVNTSEL2               0xDB2, 0xE53
 *   MSR_C5_PMON_EVNTSEL3               0xDB3, 0xE54
 *   MSR_C6_PMON_BOX_CTL                0xDC4, 0xE60
 *   MSR_C6_PMON_BOX_FILTER1            0xDDA, 0xE66
 *   MSR_C6_PMON_BOX_STATUS             0xD61, 0xE67
 *   MSR_C6_PMON_CTR0                   0xD71, 0xDD6, 0xE68
 *   MSR_C6_PMON_CTR1                   0xD73, 0xDD7, 0xE69
 *   MSR_C6_PMON_CTR2                   0xD75, 0xDD8, 0xE6A
 *   MSR_C6_PMON_CTR3                   0xD77, 0xDD9, 0xE6B
 *   MSR_C6_PMON_EVNTSEL0               0xDD0, 0xE61
 *   MSR_C6_PMON_EVNTSEL1               0xDD1, 0xE62
 *   MSR_C6_PMON_EVNTSEL2               0xDD2, 0xE63
 *   MSR_C6_PMON_EVNTSEL3               0xDD3, 0xE64
 *   MSR_C7_PMON_BOX_CTL                0xDE4, 0xE70
 *   MSR_C7_PMON_BOX_FILTER1            0xDFA, 0xE76
 *   MSR_C7_PMON_BOX_STATUS             0xDE1, 0xE77
 *   MSR_C7_PMON_CTR0                   0xDF1, 0xDF6, 0xE78
 *   MSR_C7_PMON_CTR1                   0xDF3, 0xDF7, 0xE79
 *   MSR_C7_PMON_CTR2                   0xDF5, 0xDF8, 0xE7A
 *   MSR_C7_PMON_CTR3                   0xDF7, 0xDF9, 0xE7B
 *   MSR_C7_PMON_EVNTSEL0               0xDF0, 0xE71
 *   MSR_C7_PMON_EVNTSEL1               0xDF1, 0xE72
 *   MSR_C7_PMON_EVNTSEL2               0xDF2, 0xE73
 *   MSR_C7_PMON_EVNTSEL3               0xDF3, 0xE74
 *   MSR_C8_PMON_BOX_CTL                0xE04, 0xE80
 *   MSR_C8_PMON_BOX_FILTER1            0xE1A, 0xE86
 *   MSR_C8_PMON_BOX_STATUS             0xE87, 0xF41
 *   MSR_C8_PMON_CTR0                   0xE16, 0xE88, 0xF51
 *   MSR_C8_PMON_CTR1                   0xE17, 0xE89, 0xF53
 *   MSR_C8_PMON_CTR2                   0xE18, 0xE8A, 0xF55
 *   MSR_C8_PMON_CTR3                   0xE19, 0xE8B, 0xF57
 *   MSR_C8_PMON_EVNTSEL0               0xE10, 0xE81
 *   MSR_C8_PMON_EVNTSEL1               0xE11, 0xE82
 *   MSR_C8_PMON_EVNTSEL2               0xE12, 0xE83
 *   MSR_C8_PMON_EVNTSEL3               0xE13, 0xE84
 *   MSR_C9_PMON_BOX_CTL                0xE24, 0xE90
 *   MSR_C9_PMON_BOX_FILTER1            0xE3A, 0xE96
 *   MSR_C9_PMON_BOX_STATUS             0xE97, 0xFC1
 *   MSR_C9_PMON_CTR0                   0xE36, 0xE98, 0xFD1
 *   MSR_C9_PMON_CTR1                   0xE37, 0xE99, 0xFD3
 *   MSR_C9_PMON_CTR2                   0xE38, 0xE9A, 0xFD5
 *   MSR_C9_PMON_CTR3                   0xE39, 0xE9B, 0xFD7
 *   MSR_C9_PMON_EVNTSEL0               0xE30, 0xE91
 *   MSR_C9_PMON_EVNTSEL1               0xE31, 0xE92
 *   MSR_C9_PMON_EVNTSEL2               0xE32, 0xE93
 *   MSR_C9_PMON_EVNTSEL3               0xE33, 0xE94
 *   MSR_CORE_C3_RESIDENCY              0x3FC, 0x662
 *   MSR_CORE_C6_RESIDENCY              0x3FD, 0x3FF
 *   MSR_CORE_PERF_LIMIT_REASONS        0x64F, 0x690
 *   MSR_CORE_UARCH_CTL                 0x540, 0x541
 *   MSR_LASTBRANCH_0                   0x40, 0x1DB
 *   MSR_LASTBRANCH_0_FROM_IP           0x40, 0x680
 *   MSR_LASTBRANCH_0_TO_IP             0x60, 0x6C0
 *   MSR_LASTBRANCH_1                   0x41, 0x1DC
 *   MSR_LASTBRANCH_1_FROM_IP           0x41, 0x681
 *   MSR_LASTBRANCH_1_TO_IP             0x61, 0x6C1
 *   MSR_LASTBRANCH_2                   0x42, 0x1DD
 *   MSR_LASTBRANCH_2_FROM_IP           0x42, 0x682
 *   MSR_LASTBRANCH_2_TO_IP             0x62, 0x6C2
 *   MSR_LASTBRANCH_3                   0x43, 0x1DE
 *   MSR_LASTBRANCH_3_FROM_IP           0x43, 0x683
 *   MSR_LASTBRANCH_3_TO_IP             0x63, 0x6C3
 *   MSR_LASTBRANCH_4_FROM_IP           0x44, 0x684
 *   MSR_LASTBRANCH_4_TO_IP             0x64, 0x6C4
 *   MSR_LASTBRANCH_5_FROM_IP           0x45, 0x685
 *   MSR_LASTBRANCH_5_TO_IP             0x65, 0x6C5
 *   MSR_LASTBRANCH_6_FROM_IP           0x46, 0x686
 *   MSR_LASTBRANCH_6_TO_IP             0x66, 0x6C6
 *   MSR_LASTBRANCH_7_FROM_IP           0x47, 0x687
 *   MSR_LASTBRANCH_7_TO_IP             0x67, 0x6C7
 *   MSR_LASTBRANCH_TOS                 0x1C9, 0x1DA
 *   MSR_LER_FROM_LIP                   0x1D7, 0x1DD, 0x1DE
 *   MSR_LER_TO_LIP                     0x1D8, 0x1DD, 0x1DE
 *   MSR_MC6_RESIDENCY                  0x3FD, 0x664
 *   MSR_PCU_PMON_BOX_CTL               0x710, 0xC24
 *   MSR_PCU_PMON_BOX_FILTER            0x715, 0xC34
 *   MSR_PCU_PMON_BOX_STATUS            0x716, 0xC35
 *   MSR_PCU_PMON_CTR0                  0x717, 0xC36
 *   MSR_PCU_PMON_CTR1                  0x718, 0xC37
 *   MSR_PCU_PMON_CTR2                  0x719, 0xC38
 *   MSR_PCU_PMON_CTR3                  0x71A, 0xC39
 *   MSR_PCU_PMON_EVNTSEL0              0x711, 0xC30
 *   MSR_PCU_PMON_EVNTSEL1              0x712, 0xC31
 *   MSR_PCU_PMON_EVNTSEL2              0x713, 0xC32
 *   MSR_PCU_PMON_EVNTSEL3              0x714, 0xC33
 *   MSR_PKG_C2_RESIDENCY               0x3F8, 0x60D
 *   MSR_PKG_C6_RESIDENCY               0x3F9, 0x3FA
 *   MSR_PKG_POWER_INFO                 0x614, 0x66E
 *   MSR_PMON_GLOBAL_CONFIG             0x702, 0xC06
 *   MSR_PMON_GLOBAL_CTL                0x700, 0xC00
 *   MSR_PMON_GLOBAL_STATUS             0x701, 0xC01
 *   MSR_S0_PMON_CTR0                   0x726, 0xC51
 *   MSR_S0_PMON_CTR1                   0x727, 0xC53
 *   MSR_S0_PMON_CTR2                   0x728, 0xC55
 *   MSR_S0_PMON_CTR3                   0x729, 0xC57
 *   MSR_S1_PMON_CTR0                   0x730, 0xCD1
 *   MSR_S1_PMON_CTR1                   0x731, 0xCD3
 *   MSR_S1_PMON_CTR2                   0x732, 0xCD5
 *   MSR_S1_PMON_CTR3                   0x733, 0xCD7
 *   MSR_UNC_CBO_0_PERFCTR0             0x702, 0x706, 0x2002
 *   MSR_UNC_CBO_0_PERFCTR1             0x703, 0x707, 0x2003
 *   MSR_UNC_CBO_0_PERFEVTSEL0          0x700, 0x2000
 *   MSR_UNC_CBO_0_PERFEVTSEL1          0x701, 0x2001
 *   MSR_UNC_CBO_1_PERFCTR0             0x70A, 0x716, 0x200A
 *   MSR_UNC_CBO_1_PERFCTR1             0x70B, 0x717, 0x200B
 *   MSR_UNC_CBO_1_PERFEVTSEL0          0x708, 0x710, 0x2008
 *   MSR_UNC_CBO_1_PERFEVTSEL1          0x709, 0x711, 0x2009
 *   MSR_UNC_CBO_2_PERFCTR0             0x712, 0x726, 0x2012
 *   MSR_UNC_CBO_2_PERFCTR1             0x713, 0x727, 0x2013
 *   MSR_UNC_CBO_2_PERFEVTSEL0          0x710, 0x720, 0x2010
 *   MSR_UNC_CBO_2_PERFEVTSEL1          0x711, 0x721, 0x2011
 *   MSR_UNC_CBO_3_PERFCTR0             0x71A, 0x736, 0x201A
 *   MSR_UNC_CBO_3_PERFCTR1             0x71B, 0x737, 0x201B
 *   MSR_UNC_CBO_3_PERFCTR2             0x728, 0x738
 *   MSR_UNC_CBO_3_PERFCTR3             0x729, 0x739
 *   MSR_UNC_CBO_3_PERFEVTSEL0          0x718, 0x730, 0x2018
 *   MSR_UNC_CBO_3_PERFEVTSEL1          0x719, 0x731, 0x2019
 *   MSR_UNC_CBO_4_PERFCTR0             0x722, 0x746, 0x2022
 *   MSR_UNC_CBO_4_PERFCTR1             0x723, 0x747, 0x2023
 *   MSR_UNC_CBO_4_PERFEVTSEL0          0x720, 0x740, 0x2020
 *   MSR_UNC_CBO_4_PERFEVTSEL1          0x721, 0x741, 0x2021
 *   MSR_UNC_CBO_5_PERFCTR0             0x72A, 0x202A
 *   MSR_UNC_CBO_5_PERFCTR1             0x72B, 0x202B
 *   MSR_UNC_CBO_5_PERFEVTSEL0          0x728, 0x2028
 *   MSR_UNC_CBO_5_PERFEVTSEL1          0x729, 0x2029
 *   MSR_UNC_CBO_6_PERFCTR0             0x732, 0x2032
 *   MSR_UNC_CBO_6_PERFCTR1             0x733, 0x2033
 *   MSR_UNC_CBO_6_PERFEVTSEL0          0x730, 0x2030
 *   MSR_UNC_CBO_6_PERFEVTSEL1          0x731, 0x2031
 *   MSR_UNC_CBO_7_PERFCTR0             0x73A, 0x203A
 *   MSR_UNC_CBO_7_PERFCTR1             0x73B, 0x203B
 *   MSR_UNC_CBO_7_PERFEVTSEL0          0x738, 0x2038
 *   MSR_UNC_CBO_7_PERFEVTSEL1          0x739, 0x2039
 *   MSR_UNC_PERF_FIXED_CTR             0x395, 0x2FDF
 *   MSR_UNC_PERF_FIXED_CTRL            0x394, 0x2FDE
 *   MSR_UNC_PERF_GLOBAL_CTRL           0x391, 0xE01, 0x2FF0
 *   MSR_UNC_PERF_GLOBAL_STATUS         0x392, 0xE02, 0x2FF2
 *   MSR_U_PMON_BOX_STATUS              0x708, 0xC15
 *   MSR_U_PMON_CTR0                    0x709, 0xC16
 *   MSR_U_PMON_CTR1                    0x70A, 0xC17
 *   MSR_U_PMON_EVNTSEL0                0x705, 0xC10
 *   MSR_U_PMON_EVNTSEL1                0x706, 0xC11
 *   MSR_U_PMON_UCLK_FIXED_CTL          0x703, 0xC08
 *   MSR_U_PMON_UCLK_FIXED_CTR          0x704, 0xC09
 */

/*
 * NOMBRADOS EN EL MANUAL, pero con la direccion definida por
 * FORMULA en otro sitio -- de la forma `base + n`, donde la base
 * depende de lo que enumere CPUID.
 *
 * No se definen porque no hay un numero que definir.  Se listan
 * para que conste que se vieron: omitirlos sin decirlo es como se
 * pierde un registro sin que nadie se entere.
 *
 *   IA32_MC0_CTL2
 *   IA32_MC1_CTL2
 *   IA32_MC2_CTL2
 *   IA32_MTRR_PHYSMASK9
 *   IA32_PMC_FXm_CFG_C
 *   IA32_PMC_GPn_CFG_C
 */

#endif /* VXP_COMMON_MSR_INTEL_INDEX_H */
