/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_memory.h
 * @brief Memoria: MTRR, PAT, cifrado y reparto de cache
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

#ifndef VXP_COMMON_MSR_MEMORY_H
#define VXP_COMMON_MSR_MEMORY_H

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MISC_PACKAGE_CTLS 0xBCu

/** antes `MTRRcap`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRRCAP 0xFEu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MTRRcap 0xFEu

/** antes `MTRRphysBase0`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE0 0x200u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase0 0x200u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK0 0x201u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask0 0x201u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE1 0x202u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase1 0x202u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK1 0x203u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask1 0x203u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE2 0x204u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase2 0x204u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK2 0x205u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask2 0x205u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE3 0x206u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase3 0x206u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK3 0x207u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask3 0x207u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE4 0x208u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase4 0x208u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK4 0x209u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask4 0x209u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE5 0x20Au

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase5 0x20Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK5 0x20Bu

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask5 0x20Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE6 0x20Cu

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase6 0x20Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK6 0x20Du

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask6 0x20Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE7 0x20Eu

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysBase7 0x20Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK7 0x20Fu

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRphysMask7 0x20Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE8 0x210u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK8 0x211u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSBASE9 0x212u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_PHYSMASK9 0x213u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX64K_00000 0x250u
#define IA32_MTRR_FIX64K_00000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX64K_00000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX64K_00000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRfix64K_00000 0x250u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MTRR_FIX16K_80000 0x258u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRfix16K_80000 0x258u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX16K_A0000 0x259u
#define IA32_MTRR_FIX16K_A0000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX16K_A0000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX16K_A0000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM  Solo, Intel(R) CoreTM Duo  Processors, and Dual-Core  Intel(R) Xeon(R) Processor LV */
#define MTRRfix16K_A0000 0x259u

/** antes `MTRRfix4K_C0000`.  SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_C0000 0x268u
#define IA32_MTRR_FIX4K_C0000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_C0000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_C0000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_C0000 0x268u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_C8000 0x269u
#define IA32_MTRR_FIX4K_C8000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_C8000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_C8000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_C8000 0x269u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_D0000 0x26Au
#define IA32_MTRR_FIX4K_D0000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_D0000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_D0000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_D0000 0x26Au

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_D8000 0x26Bu
#define IA32_MTRR_FIX4K_D8000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_D8000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_D8000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_D8000 0x26Bu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_E0000 0x26Cu
#define IA32_MTRR_FIX4K_E0000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_E0000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_E0000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_E0000 0x26Cu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_E8000 0x26Du
#define IA32_MTRR_FIX4K_E8000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_E8000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_E8000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_E8000 0x26Du

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_F0000 0x26Eu
#define IA32_MTRR_FIX4K_F0000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_F0000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_F0000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_F0000 0x26Eu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_FIX4K_F8000 0x26Fu
#define IA32_MTRR_FIX4K_F8000_GATE_LEAF 0x1u
#define IA32_MTRR_FIX4K_F8000_GATE_REG 3u /* EDX */
#define IA32_MTRR_FIX4K_F8000_GATE_BIT 12u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MTRRfix4K_F8000 0x26Fu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[16] */
#define IA32_PAT 0x277u
#define IA32_PAT_GATE_LEAF 0x1u
#define IA32_PAT_GATE_REG 3u /* EDX */
#define IA32_PAT_GATE_BIT 16u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[12] */
#define IA32_MTRR_DEF_TYPE 0x2FFu
#define IA32_MTRR_DEF_TYPE_GATE_LEAF 0x1u
#define IA32_MTRR_DEF_TYPE_GATE_REG 3u /* EDX */
#define IA32_MTRR_DEF_TYPE_GATE_BIT 12u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MTRRdefType 0x2FFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.01H:ECX[2] */
#define IA32_L3_QOS_CFG 0xC81u
#define IA32_L3_QOS_CFG_GATE_LEAF 0x10u
#define IA32_L3_QOS_CFG_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_CFG_GATE_REG 2u /* ECX */
#define IA32_L3_QOS_CFG_GATE_BIT 2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.02H:ECX[2] */
#define IA32_L2_QOS_CFG 0xC82u
#define IA32_L2_QOS_CFG_GATE_LEAF 0x10u
#define IA32_L2_QOS_CFG_GATE_SUBLEAF 0x2u
#define IA32_L2_QOS_CFG_GATE_REG 2u /* ECX */
#define IA32_L2_QOS_CFG_GATE_BIT 2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0FH.01H:EAX[10:9] */
#define IA32_L3_IO_QOS_CFG 0xC83u
#define IA32_L3_IO_QOS_CFG_GATE_LEAF 0xFu
#define IA32_L3_IO_QOS_CFG_GATE_SUBLEAF 0x1u
#define IA32_L3_IO_QOS_CFG_GATE_REG 0u /* EAX */
#define IA32_L3_IO_QOS_CFG_GATE_SHIFT 9u
#define IA32_L3_IO_QOS_CFG_GATE_MASK 0x3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[12] */
#define IA32_PQR_ASSOC 0xC8Fu
#define IA32_PQR_ASSOC_GATE_LEAF 0x7u
#define IA32_PQR_ASSOC_GATE_SUBLEAF 0x0u
#define IA32_PQR_ASSOC_GATE_REG 1u /* EBX */
#define IA32_PQR_ASSOC_GATE_BIT 12u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[1] */
#define IA32_L3_MASK_0 0xC90u
#define IA32_L3_MASK_0_GATE_LEAF 0x10u
#define IA32_L3_MASK_0_GATE_SUBLEAF 0x0u
#define IA32_L3_MASK_0_GATE_REG 1u /* EBX */
#define IA32_L3_MASK_0_GATE_BIT 1u

/** SDM Table 2-2. IA-32 Architectural MSRs.  direccion INDEXADA: la base, y el indice se suma.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_MASK_n 0xC90u
#define IA32_L3_MASK_n_GATE_LEAF 0x10u
#define IA32_L3_MASK_n_GATE_SUBLEAF 0x1u
#define IA32_L3_MASK_n_GATE_REG 3u /* EDX */
#define IA32_L3_MASK_n_GATE_SHIFT 0u
#define IA32_L3_MASK_n_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_0 0xC90u
#define IA32_L3_QOS_MASK_0_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_0_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_0_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_0_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_0_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_1 0xC91u
#define IA32_L3_QOS_MASK_1_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_1_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_1_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_1_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_1_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_2 0xC92u
#define IA32_L3_QOS_MASK_2_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_2_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_2_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_2_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_2_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_3 0xC93u
#define IA32_L3_QOS_MASK_3_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_3_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_3_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_3_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_3_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_4 0xC94u
#define IA32_L3_QOS_MASK_4_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_4_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_4_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_4_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_4_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_5 0xC95u
#define IA32_L3_QOS_MASK_5_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_5_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_5_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_5_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_5_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor        E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_6 0xC96u
#define IA32_L3_QOS_MASK_6_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_6_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_6_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_6_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_6_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor        E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_7 0xC97u
#define IA32_L3_QOS_MASK_7_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_7_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_7_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_7_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_7_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor        E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_8 0xC98u
#define IA32_L3_QOS_MASK_8_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_8_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_8_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_8_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_8_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor        E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_9 0xC99u
#define IA32_L3_QOS_MASK_9_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_9_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_9_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_9_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_9_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor        E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_10 0xC9Au
#define IA32_L3_QOS_MASK_10_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_10_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_10_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_10_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_10_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor        E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_11 0xC9Bu
#define IA32_L3_QOS_MASK_11_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_11_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_11_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_11_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_11_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor        E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_12 0xC9Cu
#define IA32_L3_QOS_MASK_12_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_12_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_12_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_12_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_12_GATE_MASK 0xFFFFu

/** SDM Table 2-50. MSRs Supported by the Intel(R) Xeon(R) Scalable Processor Family with a CPUID            Signature.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_13 0xC9Du
#define IA32_L3_QOS_MASK_13_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_13_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_13_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_13_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_13_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_14 0xC9Eu
#define IA32_L3_QOS_MASK_14_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_14_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_14_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_14_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_14_GATE_MASK 0xFFFFu

/** SDM Table 2-36. Additional MSRs Common to the Intel(R) Xeon(R) Processor D and the Intel(R) Xeon(R) Processor E5 v4 Family.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L3_QOS_MASK_15 0xC9Fu
#define IA32_L3_QOS_MASK_15_GATE_LEAF 0x10u
#define IA32_L3_QOS_MASK_15_GATE_SUBLEAF 0x1u
#define IA32_L3_QOS_MASK_15_GATE_REG 3u /* EDX */
#define IA32_L3_QOS_MASK_15_GATE_SHIFT 0u
#define IA32_L3_QOS_MASK_15_GATE_MASK 0xFFFFu

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_RMID_SNC_CONFIG 0xCA0u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[2] */
#define IA32_L2_MASK_0 0xD10u
#define IA32_L2_MASK_0_GATE_LEAF 0x10u
#define IA32_L2_MASK_0_GATE_SUBLEAF 0x0u
#define IA32_L2_MASK_0_GATE_REG 1u /* EBX */
#define IA32_L2_MASK_0_GATE_BIT 2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  direccion INDEXADA: la base, y el indice se suma.  PUERTA: CPUID.10H.02H:EDX[15:0] */
#define IA32_L2_MASK_n 0xD10u
#define IA32_L2_MASK_n_GATE_LEAF 0x10u
#define IA32_L2_MASK_n_GATE_SUBLEAF 0x2u
#define IA32_L2_MASK_n_GATE_REG 3u /* EDX */
#define IA32_L2_MASK_n_GATE_SHIFT 0u
#define IA32_L2_MASK_n_GATE_MASK 0xFFFFu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L2_QOS_MASK_0 0xD10u
#define IA32_L2_QOS_MASK_0_GATE_LEAF 0x10u
#define IA32_L2_QOS_MASK_0_GATE_SUBLEAF 0x1u
#define IA32_L2_QOS_MASK_0_GATE_REG 3u /* EDX */
#define IA32_L2_QOS_MASK_0_GATE_SHIFT 0u
#define IA32_L2_QOS_MASK_0_GATE_MASK 0xFFFFu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L2_QOS_MASK_1 0xD11u
#define IA32_L2_QOS_MASK_1_GATE_LEAF 0x10u
#define IA32_L2_QOS_MASK_1_GATE_SUBLEAF 0x1u
#define IA32_L2_QOS_MASK_1_GATE_REG 3u /* EDX */
#define IA32_L2_QOS_MASK_1_GATE_SHIFT 0u
#define IA32_L2_QOS_MASK_1_GATE_MASK 0xFFFFu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L2_QOS_MASK_2 0xD12u
#define IA32_L2_QOS_MASK_2_GATE_LEAF 0x10u
#define IA32_L2_QOS_MASK_2_GATE_SUBLEAF 0x1u
#define IA32_L2_QOS_MASK_2_GATE_REG 3u /* EDX */
#define IA32_L2_QOS_MASK_2_GATE_SHIFT 0u
#define IA32_L2_QOS_MASK_2_GATE_MASK 0xFFFFu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture.  PUERTA: CPUID.10H.01H:EDX[15:0] */
#define IA32_L2_QOS_MASK_3 0xD13u
#define IA32_L2_QOS_MASK_3_GATE_LEAF 0x10u
#define IA32_L2_QOS_MASK_3_GATE_SUBLEAF 0x1u
#define IA32_L2_QOS_MASK_3_GATE_REG 3u /* EDX */
#define IA32_L2_QOS_MASK_3_GATE_SHIFT 0u
#define IA32_L2_QOS_MASK_3_GATE_MASK 0xFFFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_8 0xD18u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_9 0xD19u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_10 0xD1Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_11 0xD1Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_12 0xD1Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_13 0xD1Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_14 0xD1Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_MASK_15 0xD1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_0 0xD50u
#define IA32_L2_QOS_EXT_BW_THRTL_0_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_0_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_0_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_0_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_1 0xD51u
#define IA32_L2_QOS_EXT_BW_THRTL_1_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_1_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_1_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_1_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_2 0xD52u
#define IA32_L2_QOS_EXT_BW_THRTL_2_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_2_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_2_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_2_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_3 0xD53u
#define IA32_L2_QOS_EXT_BW_THRTL_3_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_3_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_3_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_3_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_4 0xD54u
#define IA32_L2_QOS_EXT_BW_THRTL_4_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_4_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_4_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_4_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_L2_QOS_EXT_BW_THRTL_5 0xD55u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_6 0xD56u
#define IA32_L2_QOS_EXT_BW_THRTL_6_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_6_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_6_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_6_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_7 0xD57u
#define IA32_L2_QOS_EXT_BW_THRTL_7_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_7_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_7_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_7_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_8 0xD58u
#define IA32_L2_QOS_EXT_BW_THRTL_8_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_8_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_8_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_8_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_9 0xD59u
#define IA32_L2_QOS_EXT_BW_THRTL_9_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_9_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_9_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_9_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_10 0xD5Au
#define IA32_L2_QOS_EXT_BW_THRTL_10_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_10_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_10_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_10_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_11 0xD5Bu
#define IA32_L2_QOS_EXT_BW_THRTL_11_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_11_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_11_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_11_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_12 0xD5Cu
#define IA32_L2_QOS_EXT_BW_THRTL_12_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_12_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_12_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_12_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_13 0xD5Du
#define IA32_L2_QOS_EXT_BW_THRTL_13_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_13_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_13_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_13_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[3] */
#define IA32_L2_QOS_EXT_BW_THRTL_14 0xD5Eu
#define IA32_L2_QOS_EXT_BW_THRTL_14_GATE_LEAF 0x10u
#define IA32_L2_QOS_EXT_BW_THRTL_14_GATE_SUBLEAF 0x0u
#define IA32_L2_QOS_EXT_BW_THRTL_14_GATE_REG 1u /* EBX */
#define IA32_L2_QOS_EXT_BW_THRTL_14_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_QOS_CORE_BW_THRTL_0 0xE00u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.00H:EBX[5] */
#define IA32_QOS_CORE_BW_THRTL_1 0xE01u
#define IA32_QOS_CORE_BW_THRTL_1_GATE_LEAF 0x10u
#define IA32_QOS_CORE_BW_THRTL_1_GATE_SUBLEAF 0x0u
#define IA32_QOS_CORE_BW_THRTL_1_GATE_REG 1u /* EBX */
#define IA32_QOS_CORE_BW_THRTL_1_GATE_BIT 5u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_STLB_QOS_INFO 0x1A8Fu


#endif /* VXP_COMMON_MSR_MEMORY_H */
