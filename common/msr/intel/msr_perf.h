/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_perf.h
 * @brief Rendimiento: contadores, PEBS, LBR, Intel PT y Debug Store
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

#ifndef VXP_COMMON_MSR_PERF_H
#define VXP_COMMON_MSR_PERF_H

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_LASTBRANCH_4 0x44u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_LASTBRANCH_5 0x45u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_LASTBRANCH_6 0x46u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_LASTBRANCH_7 0x47u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MKTME_KEYID_PARTITIONING 0x87u

/** antes `PERFCTR0`.  SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC0 0xC1u
#define IA32_PMC0_GATE_LEAF 0xAu
#define IA32_PMC0_GATE_REG 0u /* EAX */
#define IA32_PMC0_GATE_SHIFT 8u
#define IA32_PMC0_GATE_MASK 0xFFu

/** antes `PERFCTR1`.  SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC1 0xC2u
#define IA32_PMC1_GATE_LEAF 0xAu
#define IA32_PMC1_GATE_REG 0u /* EAX */
#define IA32_PMC1_GATE_SHIFT 8u
#define IA32_PMC1_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC2 0xC3u
#define IA32_PMC2_GATE_LEAF 0xAu
#define IA32_PMC2_GATE_REG 0u /* EAX */
#define IA32_PMC2_GATE_SHIFT 8u
#define IA32_PMC2_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC3 0xC4u
#define IA32_PMC3_GATE_LEAF 0xAu
#define IA32_PMC3_GATE_REG 0u /* EAX */
#define IA32_PMC3_GATE_SHIFT 8u
#define IA32_PMC3_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC4 0xC5u
#define IA32_PMC4_GATE_LEAF 0xAu
#define IA32_PMC4_GATE_REG 0u /* EAX */
#define IA32_PMC4_GATE_SHIFT 8u
#define IA32_PMC4_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC5 0xC6u
#define IA32_PMC5_GATE_LEAF 0xAu
#define IA32_PMC5_GATE_REG 0u /* EAX */
#define IA32_PMC5_GATE_SHIFT 8u
#define IA32_PMC5_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC6 0xC7u
#define IA32_PMC6_GATE_LEAF 0xAu
#define IA32_PMC6_GATE_REG 0u /* EAX */
#define IA32_PMC6_GATE_SHIFT 8u
#define IA32_PMC6_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC7 0xC8u
#define IA32_PMC7_GATE_LEAF 0xAu
#define IA32_PMC7_GATE_REG 0u /* EAX */
#define IA32_PMC7_GATE_SHIFT 8u
#define IA32_PMC7_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC8 0xC9u
#define IA32_PMC8_GATE_LEAF 0xAu
#define IA32_PMC8_GATE_REG 0u /* EAX */
#define IA32_PMC8_GATE_SHIFT 8u
#define IA32_PMC8_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC9 0xCAu
#define IA32_PMC9_GATE_LEAF 0xAu
#define IA32_PMC9_GATE_REG 0u /* EAX */
#define IA32_PMC9_GATE_SHIFT 8u
#define IA32_PMC9_GATE_MASK 0xFFu

/** antes `PERFEVTSEL0`.  SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL0 0x186u
#define IA32_PERFEVTSEL0_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL0_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL0_GATE_SHIFT 8u
#define IA32_PERFEVTSEL0_GATE_MASK 0xFFu

/** antes `EVNTSEL0`.  SDM Table 2-68. MSRs in the P6 Family Processors */
#define PerfEvtSel0 0x186u

/** antes `PERFEVTSEL1`.  SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL1 0x187u
#define IA32_PERFEVTSEL1_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL1_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL1_GATE_SHIFT 8u
#define IA32_PERFEVTSEL1_GATE_MASK 0xFFu

/** antes `EVNTSEL1`.  SDM Table 2-68. MSRs in the P6 Family Processors */
#define PerfEvtSel1 0x187u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL2 0x188u
#define IA32_PERFEVTSEL2_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL2_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL2_GATE_SHIFT 8u
#define IA32_PERFEVTSEL2_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL3 0x189u
#define IA32_PERFEVTSEL3_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL3_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL3_GATE_SHIFT 8u
#define IA32_PERFEVTSEL3_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL4 0x18Au
#define IA32_PERFEVTSEL4_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL4_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL4_GATE_SHIFT 8u
#define IA32_PERFEVTSEL4_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL5 0x18Bu
#define IA32_PERFEVTSEL5_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL5_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL5_GATE_SHIFT 8u
#define IA32_PERFEVTSEL5_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL6 0x18Cu
#define IA32_PERFEVTSEL6_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL6_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL6_GATE_SHIFT 8u
#define IA32_PERFEVTSEL6_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL7 0x18Du
#define IA32_PERFEVTSEL7_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL7_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL7_GATE_SHIFT 8u
#define IA32_PERFEVTSEL7_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL8 0x18Eu
#define IA32_PERFEVTSEL8_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL8_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL8_GATE_SHIFT 8u
#define IA32_PERFEVTSEL8_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERFEVTSEL9 0x18Fu
#define IA32_PERFEVTSEL9_GATE_LEAF 0xAu
#define IA32_PERFEVTSEL9_GATE_REG 0u /* EAX */
#define IA32_PERFEVTSEL9_GATE_SHIFT 8u
#define IA32_PERFEVTSEL9_GATE_MASK 0xFFu

/** SDM Table 2-7. MSRs Common to the Silvermont and Airmont Microarchitectures */
#define MSR_LBR_SELECT 0x1C8u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define DEBUGCTLMSR 0x1D9u

/** antes `MSR_DEBUGCTLA, MSR_DEBUGCTLB`.  SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[24] */
#define IA32_DEBUGCTL 0x1D9u
#define IA32_DEBUGCTL_GATE_LEAF 0x7u
#define IA32_DEBUGCTL_GATE_SUBLEAF 0x0u
#define IA32_DEBUGCTL_GATE_REG 2u /* ECX */
#define IA32_DEBUGCTL_GATE_BIT 24u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_DEBUGCTLA 0x1D9u

/** SDM Table 2-67. MSRs in Pentium M Processors */
#define MSR_DEBUGCTLB 0x1D9u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define LASTBRANCHFROMIP 0x1DBu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define LASTBRANCHTOIP 0x1DCu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_LER_FROM_IP 0x1DDu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define LASTINTFROMIP 0x1DDu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_LER_TO_IP 0x1DEu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define LASTINTTOIP 0x1DEu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[4] */
#define IA32_LER_INFO 0x1E0u
#define IA32_LER_INFO_GATE_LEAF 0x7u
#define IA32_LER_INFO_GATE_SUBLEAF 0x0u
#define IA32_LER_INFO_GATE_REG 1u /* EBX */
#define IA32_LER_INFO_GATE_BIT 4u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_FIXED_CTR0 0x309u
#define IA32_FIXED_CTR0_GATE_LEAF 0xAu
#define IA32_FIXED_CTR0_GATE_REG 3u /* EDX */
#define IA32_FIXED_CTR0_GATE_SHIFT 0u
#define IA32_FIXED_CTR0_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_FIXED_CTR1 0x30Au
#define IA32_FIXED_CTR1_GATE_LEAF 0xAu
#define IA32_FIXED_CTR1_GATE_REG 3u /* EDX */
#define IA32_FIXED_CTR1_GATE_SHIFT 0u
#define IA32_FIXED_CTR1_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_FIXED_CTR2 0x30Bu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_FIXED_CTR3 0x30Cu
#define IA32_FIXED_CTR3_GATE_LEAF 0xAu
#define IA32_FIXED_CTR3_GATE_REG 3u /* EDX */
#define IA32_FIXED_CTR3_GATE_SHIFT 0u
#define IA32_FIXED_CTR3_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_FIXED_CTR4 0x30Du
#define IA32_FIXED_CTR4_GATE_LEAF 0xAu
#define IA32_FIXED_CTR4_GATE_REG 3u /* EDX */
#define IA32_FIXED_CTR4_GATE_SHIFT 0u
#define IA32_FIXED_CTR4_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_FIXED_CTR5 0x30Eu
#define IA32_FIXED_CTR5_GATE_LEAF 0xAu
#define IA32_FIXED_CTR5_GATE_REG 3u /* EDX */
#define IA32_FIXED_CTR5_GATE_SHIFT 0u
#define IA32_FIXED_CTR5_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_FIXED_CTR6 0x30Fu
#define IA32_FIXED_CTR6_GATE_LEAF 0xAu
#define IA32_FIXED_CTR6_GATE_REG 3u /* EDX */
#define IA32_FIXED_CTR6_GATE_SHIFT 0u
#define IA32_FIXED_CTR6_GATE_MASK 0x1Fu

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_PERF_METRICS 0x329u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[15] */
#define IA32_PERF_CAPABILITIES 0x345u
#define IA32_PERF_CAPABILITIES_GATE_LEAF 0x1u
#define IA32_PERF_CAPABILITIES_GATE_REG 2u /* ECX */
#define IA32_PERF_CAPABILITIES_GATE_BIT 15u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_PERF_CAPABILITIES 0x345u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[7:0] */
#define IA32_FIXED_CTR_CTRL 0x38Du
#define IA32_FIXED_CTR_CTRL_GATE_LEAF 0xAu
#define IA32_FIXED_CTR_CTRL_GATE_REG 0u /* EAX */
#define IA32_FIXED_CTR_CTRL_GATE_SHIFT 0u
#define IA32_FIXED_CTR_CTRL_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[7:0] */
#define IA32_PERF_GLOBAL_STATUS 0x38Eu
#define IA32_PERF_GLOBAL_STATUS_GATE_LEAF 0xAu
#define IA32_PERF_GLOBAL_STATUS_GATE_REG 0u /* EAX */
#define IA32_PERF_GLOBAL_STATUS_GATE_SHIFT 0u
#define IA32_PERF_GLOBAL_STATUS_GATE_MASK 0xFFu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_PERF_GLOBAL_STATUS 0x38Eu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[7:0] */
#define IA32_PERF_GLOBAL_CTRL 0x38Fu
#define IA32_PERF_GLOBAL_CTRL_GATE_LEAF 0xAu
#define IA32_PERF_GLOBAL_CTRL_GATE_REG 0u /* EAX */
#define IA32_PERF_GLOBAL_CTRL_GATE_SHIFT 0u
#define IA32_PERF_GLOBAL_CTRL_GATE_MASK 0xFFu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_PERF_GLOBAL_CTRL 0x38Fu

/** SDM Table 2-20. MSRs Supported by Intel(R) Processors Based on Sandy Bridge Microarchitecture.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PERF_GLOBAL_OVF_CTRL 0x390u
#define IA32_PERF_GLOBAL_OVF_CTRL_GATE_LEAF 0xAu
#define IA32_PERF_GLOBAL_OVF_CTRL_GATE_REG 0u /* EAX */
#define IA32_PERF_GLOBAL_OVF_CTRL_GATE_SHIFT 8u
#define IA32_PERF_GLOBAL_OVF_CTRL_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[7:0] */
#define IA32_PERF_GLOBAL_STATUS_RESET 0x390u
#define IA32_PERF_GLOBAL_STATUS_RESET_GATE_LEAF 0xAu
#define IA32_PERF_GLOBAL_STATUS_RESET_GATE_REG 0u /* EAX */
#define IA32_PERF_GLOBAL_STATUS_RESET_GATE_SHIFT 0u
#define IA32_PERF_GLOBAL_STATUS_RESET_GATE_MASK 0xFFu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_PERF_GLOBAL_OVF_CTRL 0x390u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[7:0] */
#define IA32_PERF_GLOBAL_STATUS_SET 0x391u
#define IA32_PERF_GLOBAL_STATUS_SET_GATE_LEAF 0xAu
#define IA32_PERF_GLOBAL_STATUS_SET_GATE_REG 0u /* EAX */
#define IA32_PERF_GLOBAL_STATUS_SET_GATE_SHIFT 0u
#define IA32_PERF_GLOBAL_STATUS_SET_GATE_MASK 0xFFu

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series */
#define MSR_UNCORE_PERF_GLOBAL_CTRL 0x391u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[7:0] */
#define IA32_PERF_GLOBAL_INUSE 0x392u
#define IA32_PERF_GLOBAL_INUSE_GATE_LEAF 0xAu
#define IA32_PERF_GLOBAL_INUSE_GATE_REG 0u /* EAX */
#define IA32_PERF_GLOBAL_INUSE_GATE_SHIFT 0u
#define IA32_PERF_GLOBAL_INUSE_GATE_MASK 0xFFu

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series */
#define MSR_UNCORE_PERF_GLOBAL_STATUS 0x392u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series */
#define MSR_UNCORE_PERF_GLOBAL_OVF_CTRL 0x393u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series */
#define MSR_UNCORE_FIXED_CTR0 0x394u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 816 en decimal, que no es 0x394 */
#define MSR_W_PMON_FIXED_CTR 0x394u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series */
#define MSR_UNCORE_FIXED_CTR_CTRL 0x395u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 817 en decimal, que no es 0x395 */
#define MSR_W_PMON_FIXED_CTR_CTL 0x395u

/** SDM Table 2-23. Additional MSRs Supported by the Intel(R) Xeon(R) Processors E5 Family Based on Sandy Bridge */
#define MSR_PEBS_NUM_ALT 0x39Cu

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 960 en decimal, que no es 0x3B0 */
#define MSR_UNCORE_PMC0 0x3B0u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 961 en decimal, que no es 0x3B1 */
#define MSR_UNCORE_PMC1 0x3B1u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 962 en decimal, que no es 0x3B2 */
#define MSR_UNCORE_PMC2 0x3B2u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors.  OJO: el manual da 944 en decimal, que no es 0x3B2 */
#define MSR_UNC_ARB_PERFEVTSEL0 0x3B2u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 963 en decimal, que no es 0x3B3 */
#define MSR_UNCORE_PMC3 0x3B3u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors.  OJO: el manual da 945 en decimal, que no es 0x3B3 */
#define MSR_UNC_ARB_PERFEVTSEL1 0x3B3u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 964 en decimal, que no es 0x3B4 */
#define MSR_UNCORE_PMC4 0x3B4u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 965 en decimal, que no es 0x3B5 */
#define MSR_UNCORE_PMC5 0x3B5u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 966 en decimal, que no es 0x3B6 */
#define MSR_UNCORE_PMC6 0x3B6u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 967 en decimal, que no es 0x3B7 */
#define MSR_UNCORE_PMC7 0x3B7u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 944 en decimal, que no es 0x3C0 */
#define MSR_UNCORE_PERFEVTSEL0 0x3C0u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 945 en decimal, que no es 0x3C1 */
#define MSR_UNCORE_PERFEVTSEL1 0x3C1u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 946 en decimal, que no es 0x3C2 */
#define MSR_UNCORE_PERFEVTSEL2 0x3C2u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 947 en decimal, que no es 0x3C3 */
#define MSR_UNCORE_PERFEVTSEL3 0x3C3u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 948 en decimal, que no es 0x3C4 */
#define MSR_UNCORE_PERFEVTSEL4 0x3C4u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 949 en decimal, que no es 0x3C5 */
#define MSR_UNCORE_PERFEVTSEL5 0x3C5u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 950 en decimal, que no es 0x3C6 */
#define MSR_UNCORE_PERFEVTSEL6 0x3C6u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series.  OJO: el manual da 951 en decimal, que no es 0x3C7 */
#define MSR_UNCORE_PERFEVTSEL7 0x3C7u

/** antes `MSR_PEBS_ENABLE`.  SDM Table 2-14. MSRs in Intel Atom(R) Processors Based on Tremont Microarchitecture.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PEBS_ENABLE 0x3F1u
#define IA32_PEBS_ENABLE_GATE_LEAF 0xAu
#define IA32_PEBS_ENABLE_GATE_REG 0u /* EAX */
#define IA32_PEBS_ENABLE_GATE_SHIFT 8u
#define IA32_PEBS_ENABLE_GATE_MASK 0xFFu

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_PEBS_DATA_CFG 0x3F2u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_PEBS_MATRIX_VERT 0x3F2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.00H:EAX[5] */
#define IA32_PEBS_BASE 0x3F4u
#define IA32_PEBS_BASE_GATE_LEAF 0x23u
#define IA32_PEBS_BASE_GATE_SUBLEAF 0x0u
#define IA32_PEBS_BASE_GATE_REG 0u /* EAX */
#define IA32_PEBS_BASE_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.00H:EAX[5] */
#define IA32_PEBS_INDEX 0x3F5u
#define IA32_PEBS_INDEX_GATE_LEAF 0x23u
#define IA32_PEBS_INDEX_GATE_SUBLEAF 0x0u
#define IA32_PEBS_INDEX_GATE_REG 0u /* EAX */
#define IA32_PEBS_INDEX_GATE_BIT 5u

/** SDM Table 2-15. MSRs in Processors Based on Nehalem Microarchitecture */
#define MSR_PEBS_LD_LAT 0x3F6u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_PEBS_FRONTEND 0x3F7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC0 0x4C1u
#define IA32_A_PMC0_GATE_LEAF 0xAu
#define IA32_A_PMC0_GATE_REG 0u /* EAX */
#define IA32_A_PMC0_GATE_SHIFT 8u
#define IA32_A_PMC0_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC1 0x4C2u
#define IA32_A_PMC1_GATE_LEAF 0xAu
#define IA32_A_PMC1_GATE_REG 0u /* EAX */
#define IA32_A_PMC1_GATE_SHIFT 8u
#define IA32_A_PMC1_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC2 0x4C3u
#define IA32_A_PMC2_GATE_LEAF 0xAu
#define IA32_A_PMC2_GATE_REG 0u /* EAX */
#define IA32_A_PMC2_GATE_SHIFT 8u
#define IA32_A_PMC2_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC3 0x4C4u
#define IA32_A_PMC3_GATE_LEAF 0xAu
#define IA32_A_PMC3_GATE_REG 0u /* EAX */
#define IA32_A_PMC3_GATE_SHIFT 8u
#define IA32_A_PMC3_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC4 0x4C5u
#define IA32_A_PMC4_GATE_LEAF 0xAu
#define IA32_A_PMC4_GATE_REG 0u /* EAX */
#define IA32_A_PMC4_GATE_SHIFT 8u
#define IA32_A_PMC4_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC5 0x4C6u
#define IA32_A_PMC5_GATE_LEAF 0xAu
#define IA32_A_PMC5_GATE_REG 0u /* EAX */
#define IA32_A_PMC5_GATE_SHIFT 8u
#define IA32_A_PMC5_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC6 0x4C7u
#define IA32_A_PMC6_GATE_LEAF 0xAu
#define IA32_A_PMC6_GATE_REG 0u /* EAX */
#define IA32_A_PMC6_GATE_SHIFT 8u
#define IA32_A_PMC6_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC7 0x4C8u
#define IA32_A_PMC7_GATE_LEAF 0xAu
#define IA32_A_PMC7_GATE_REG 0u /* EAX */
#define IA32_A_PMC7_GATE_SHIFT 8u
#define IA32_A_PMC7_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC8 0x4C9u
#define IA32_A_PMC8_GATE_LEAF 0xAu
#define IA32_A_PMC8_GATE_REG 0u /* EAX */
#define IA32_A_PMC8_GATE_SHIFT 8u
#define IA32_A_PMC8_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_A_PMC9 0x4CAu
#define IA32_A_PMC9_GATE_LEAF 0xAu
#define IA32_A_PMC9_GATE_REG 0u /* EAX */
#define IA32_A_PMC9_GATE_SHIFT 8u
#define IA32_A_PMC9_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[25] */
#define IA32_RTIT_OUTPUT_BASE 0x560u
#define IA32_RTIT_OUTPUT_BASE_GATE_LEAF 0x7u
#define IA32_RTIT_OUTPUT_BASE_GATE_SUBLEAF 0x0u
#define IA32_RTIT_OUTPUT_BASE_GATE_REG 1u /* EBX */
#define IA32_RTIT_OUTPUT_BASE_GATE_BIT 25u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[25] */
#define IA32_RTIT_OUTPUT_MASK_PTRS 0x561u
#define IA32_RTIT_OUTPUT_MASK_PTRS_GATE_LEAF 0x7u
#define IA32_RTIT_OUTPUT_MASK_PTRS_GATE_SUBLEAF 0x0u
#define IA32_RTIT_OUTPUT_MASK_PTRS_GATE_REG 1u /* EBX */
#define IA32_RTIT_OUTPUT_MASK_PTRS_GATE_BIT 25u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[25] */
#define IA32_RTIT_CTL 0x570u
#define IA32_RTIT_CTL_GATE_LEAF 0x7u
#define IA32_RTIT_CTL_GATE_SUBLEAF 0x0u
#define IA32_RTIT_CTL_GATE_REG 1u /* EBX */
#define IA32_RTIT_CTL_GATE_BIT 25u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[25] */
#define IA32_RTIT_STATUS 0x571u
#define IA32_RTIT_STATUS_GATE_LEAF 0x7u
#define IA32_RTIT_STATUS_GATE_SUBLEAF 0x0u
#define IA32_RTIT_STATUS_GATE_REG 1u /* EBX */
#define IA32_RTIT_STATUS_GATE_BIT 25u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[25] */
#define IA32_RTIT_CR3_MATCH 0x572u
#define IA32_RTIT_CR3_MATCH_GATE_LEAF 0x7u
#define IA32_RTIT_CR3_MATCH_GATE_SUBLEAF 0x0u
#define IA32_RTIT_CR3_MATCH_GATE_REG 1u /* EBX */
#define IA32_RTIT_CR3_MATCH_GATE_BIT 25u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[2:0] */
#define IA32_RTIT_ADDR0_A 0x580u
#define IA32_RTIT_ADDR0_A_GATE_LEAF 0x7u
#define IA32_RTIT_ADDR0_A_GATE_SUBLEAF 0x1u
#define IA32_RTIT_ADDR0_A_GATE_REG 0u /* EAX */
#define IA32_RTIT_ADDR0_A_GATE_SHIFT 0u
#define IA32_RTIT_ADDR0_A_GATE_MASK 0x7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[2:0] */
#define IA32_RTIT_ADDR0_B 0x581u
#define IA32_RTIT_ADDR0_B_GATE_LEAF 0x7u
#define IA32_RTIT_ADDR0_B_GATE_SUBLEAF 0x1u
#define IA32_RTIT_ADDR0_B_GATE_REG 0u /* EAX */
#define IA32_RTIT_ADDR0_B_GATE_SHIFT 0u
#define IA32_RTIT_ADDR0_B_GATE_MASK 0x7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[2:0] */
#define IA32_RTIT_ADDR1_A 0x582u
#define IA32_RTIT_ADDR1_A_GATE_LEAF 0x7u
#define IA32_RTIT_ADDR1_A_GATE_SUBLEAF 0x1u
#define IA32_RTIT_ADDR1_A_GATE_REG 0u /* EAX */
#define IA32_RTIT_ADDR1_A_GATE_SHIFT 0u
#define IA32_RTIT_ADDR1_A_GATE_MASK 0x7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[2:0] */
#define IA32_RTIT_ADDR1_B 0x583u
#define IA32_RTIT_ADDR1_B_GATE_LEAF 0x7u
#define IA32_RTIT_ADDR1_B_GATE_SUBLEAF 0x1u
#define IA32_RTIT_ADDR1_B_GATE_REG 0u /* EAX */
#define IA32_RTIT_ADDR1_B_GATE_SHIFT 0u
#define IA32_RTIT_ADDR1_B_GATE_MASK 0x7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[2:0] */
#define IA32_RTIT_ADDR2_A 0x584u
#define IA32_RTIT_ADDR2_A_GATE_LEAF 0x7u
#define IA32_RTIT_ADDR2_A_GATE_SUBLEAF 0x1u
#define IA32_RTIT_ADDR2_A_GATE_REG 0u /* EAX */
#define IA32_RTIT_ADDR2_A_GATE_SHIFT 0u
#define IA32_RTIT_ADDR2_A_GATE_MASK 0x7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[2:0] */
#define IA32_RTIT_ADDR2_B 0x585u
#define IA32_RTIT_ADDR2_B_GATE_LEAF 0x7u
#define IA32_RTIT_ADDR2_B_GATE_SUBLEAF 0x1u
#define IA32_RTIT_ADDR2_B_GATE_REG 0u /* EAX */
#define IA32_RTIT_ADDR2_B_GATE_SHIFT 0u
#define IA32_RTIT_ADDR2_B_GATE_MASK 0x7u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_RTIT_ADDR3_A 0x586u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[2:0] */
#define IA32_RTIT_ADDR3_B 0x587u
#define IA32_RTIT_ADDR3_B_GATE_LEAF 0x7u
#define IA32_RTIT_ADDR3_B_GATE_SUBLEAF 0x1u
#define IA32_RTIT_ADDR3_B_GATE_REG 0u /* EAX */
#define IA32_RTIT_ADDR3_B_GATE_SHIFT 0u
#define IA32_RTIT_ADDR3_B_GATE_MASK 0x7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[21] */
#define IA32_DS_AREA 0x600u
#define IA32_DS_AREA_GATE_LEAF 0x1u
#define IA32_DS_AREA_GATE_REG 3u /* EDX */
#define IA32_DS_AREA_GATE_BIT 21u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_8_FROM_IP 0x688u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_9_FROM_IP 0x689u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_10_FROM_IP 0x68Au

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_11_FROM_IP 0x68Bu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_12_FROM_IP 0x68Cu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_13_FROM_IP 0x68Du

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_14_FROM_IP 0x68Eu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_15_FROM_IP 0x68Fu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_16_FROM_IP 0x690u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_17_FROM_IP 0x691u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_18_FROM_IP 0x692u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_19_FROM_IP 0x693u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_20_FROM_IP 0x694u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_21_FROM_IP 0x695u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_22_FROM_IP 0x696u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_23_FROM_IP 0x697u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_24_FROM_IP 0x698u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_25_FROM_IP 0x699u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_26_FROM_IP 0x69Au

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_27_FROM_IP 0x69Bu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_28_FROM_IP 0x69Cu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_29_FROM_IP 0x69Du

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_30_FROM_IP 0x69Eu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_31_FROM_IP 0x69Fu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_8_TO_IP 0x6C8u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_9_TO_IP 0x6C9u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_10_TO_IP 0x6CAu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_11_TO_IP 0x6CBu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_12_TO_IP 0x6CCu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_13_TO_IP 0x6CDu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_14_TO_IP 0x6CEu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_15_TO_IP 0x6CFu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_16_TO_IP 0x6D0u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_17_TO_IP 0x6D1u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_18_TO_IP 0x6D2u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_19_TO_IP 0x6D3u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_20_TO_IP 0x6D4u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_21_TO_IP 0x6D5u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_22_TO_IP 0x6D6u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_23_TO_IP 0x6D7u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_24_TO_IP 0x6D8u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_25_TO_IP 0x6D9u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_26_TO_IP 0x6DAu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_27_TO_IP 0x6DBu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_28_TO_IP 0x6DCu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_29_TO_IP 0x6DDu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_30_TO_IP 0x6DEu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_LASTBRANCH_31_TO_IP 0x6DFu

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_0_PERFEVTSEL2 0x702u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_0_PERFEVTSEL3 0x703u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_1_PERFEVTSEL2 0x712u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_1_PERFEVTSEL3 0x713u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_2_PERFEVTSEL2 0x722u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_2_PERFEVTSEL3 0x723u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_3_PERFEVTSEL2 0x732u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_3_PERFEVTSEL3 0x733u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_4_PERFEVTSEL2 0x742u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_4_PERFEVTSEL3 0x743u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture */
#define MSR_LASTBRANCH_INFO_0 0xDC0u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_0 0xDC0u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture */
#define MSR_LASTBRANCH_INFO_1 0xDC1u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_1 0xDC1u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_2 0xDC2u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_2 0xDC2u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_3 0xDC3u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_3 0xDC3u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_4 0xDC4u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_4 0xDC4u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_5 0xDC5u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_5 0xDC5u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_6 0xDC6u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_6 0xDC6u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_7 0xDC7u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_7 0xDC7u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_8 0xDC8u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_8 0xDC8u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_9 0xDC9u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_9 0xDC9u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_10 0xDCAu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_10 0xDCAu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_11 0xDCBu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_11 0xDCBu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_12 0xDCCu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_12 0xDCCu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_13 0xDCDu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_13 0xDCDu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus         Microarchitecture */
#define MSR_LASTBRANCH_INFO_14 0xDCEu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_14 0xDCEu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture */
#define MSR_LASTBRANCH_INFO_15 0xDCFu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_15 0xDCFu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture */
#define MSR_LASTBRANCH_INFO_16 0xDD0u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_16 0xDD0u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture */
#define MSR_LASTBRANCH_INFO_17 0xDD1u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_17 0xDD1u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture */
#define MSR_LASTBRANCH_INFO_18 0xDD2u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_18 0xDD2u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture */
#define MSR_LASTBRANCH_INFO_19 0xDD3u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_19 0xDD3u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3520 en decimal, que no es 0xDD4 */
#define MSR_LASTBRANCH_INFO_20 0xDD4u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_20 0xDD4u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3521 en decimal, que no es 0xDD5 */
#define MSR_LASTBRANCH_INFO_21 0xDD5u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_21 0xDD5u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3522 en decimal, que no es 0xDD6 */
#define MSR_LASTBRANCH_INFO_22 0xDD6u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_22 0xDD6u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3523 en decimal, que no es 0xDD7 */
#define MSR_LASTBRANCH_INFO_23 0xDD7u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_23 0xDD7u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3524 en decimal, que no es 0xDD8 */
#define MSR_LASTBRANCH_INFO_24 0xDD8u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_24 0xDD8u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3525 en decimal, que no es 0xDD9 */
#define MSR_LASTBRANCH_INFO_25 0xDD9u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_25 0xDD9u

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3526 en decimal, que no es 0xDDA */
#define MSR_LASTBRANCH_INFO_26 0xDDAu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_26 0xDDAu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3527 en decimal, que no es 0xDDB */
#define MSR_LASTBRANCH_INFO_27 0xDDBu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_27 0xDDBu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3528 en decimal, que no es 0xDDC */
#define MSR_LASTBRANCH_INFO_28 0xDDCu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_28 0xDDCu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3529 en decimal, que no es 0xDDD */
#define MSR_LASTBRANCH_INFO_29 0xDDDu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_29 0xDDDu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3530 en decimal, que no es 0xDDE */
#define MSR_LASTBRANCH_INFO_30 0xDDEu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_30 0xDDEu

/** SDM Table 2-13. MSRs in Intel Atom(R) Processors Based on Goldmont Plus Microarchitecture.  OJO: el manual da 3531 en decimal, que no es 0xDDF */
#define MSR_LASTBRANCH_INFO_31 0xDDFu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_LBR_INFO_31 0xDDFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_LBR_CTL 0x14CEu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.1CH.00H:EAX[7:0] */
#define IA32_LBR_DEPTH 0x14CFu
#define IA32_LBR_DEPTH_GATE_LEAF 0x1Cu
#define IA32_LBR_DEPTH_GATE_SUBLEAF 0x0u
#define IA32_LBR_DEPTH_GATE_REG 0u /* EAX */
#define IA32_LBR_DEPTH_GATE_SHIFT 0u
#define IA32_LBR_DEPTH_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP0_CTR 0x1900u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP0_CFG_A 0x1901u
#define IA32_PMC_GP0_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP0_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP0_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP0_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[0] */
#define IA32_PMC_GP0_CFG_C 0x1903u
#define IA32_PMC_GP0_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP0_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP0_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP0_CFG_C_GATE_BIT 0u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP1_CTR 0x1904u
#define IA32_PMC_GP1_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP1_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP1_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP1_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP1_CFG_A 0x1905u
#define IA32_PMC_GP1_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP1_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP1_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP1_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[1] */
#define IA32_PMC_GP1_CFG_C 0x1907u
#define IA32_PMC_GP1_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP1_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP1_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP1_CFG_C_GATE_BIT 1u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP2_CTR 0x1908u
#define IA32_PMC_GP2_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP2_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP2_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP2_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP2_CFG_A 0x1909u
#define IA32_PMC_GP2_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP2_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP2_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP2_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP2_CFG_B 0x190Au

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[2] */
#define IA32_PMC_GP2_CFG_C 0x190Bu
#define IA32_PMC_GP2_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP2_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP2_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP2_CFG_C_GATE_BIT 2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP3_CTR 0x190Cu
#define IA32_PMC_GP3_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP3_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP3_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP3_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP3_CFG_A 0x190Du
#define IA32_PMC_GP3_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP3_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP3_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP3_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP3_CFG_B 0x190Eu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[3] */
#define IA32_PMC_GP3_CFG_C 0x190Fu
#define IA32_PMC_GP3_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP3_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP3_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP3_CFG_C_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP4_CTR 0x1910u
#define IA32_PMC_GP4_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP4_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP4_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP4_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP4_CFG_A 0x1911u
#define IA32_PMC_GP4_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP4_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP4_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP4_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP4_CFG_B 0x1912u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[4] */
#define IA32_PMC_GP4_CFG_C 0x1913u
#define IA32_PMC_GP4_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP4_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP4_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP4_CFG_C_GATE_BIT 4u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP5_CTR 0x1914u
#define IA32_PMC_GP5_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP5_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP5_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP5_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP5_CFG_A 0x1915u
#define IA32_PMC_GP5_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP5_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP5_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP5_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP5_CFG_B 0x1916u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[5] */
#define IA32_PMC_GP5_CFG_C 0x1917u
#define IA32_PMC_GP5_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP5_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP5_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP5_CFG_C_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP6_CTR 0x1918u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP6_CFG_A 0x1919u
#define IA32_PMC_GP6_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP6_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP6_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP6_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP6_CFG_B 0x191Au

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[6] */
#define IA32_PMC_GP6_CFG_C 0x191Bu
#define IA32_PMC_GP6_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP6_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP6_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP6_CFG_C_GATE_BIT 6u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP7_CTR 0x191Cu
#define IA32_PMC_GP7_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP7_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP7_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP7_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP7_CFG_A 0x191Du
#define IA32_PMC_GP7_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP7_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP7_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP7_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_GP7_CFG_B 0x191Eu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:EAX[7] */
#define IA32_PMC_GP7_CFG_C 0x191Fu
#define IA32_PMC_GP7_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP7_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP7_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP7_CFG_C_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP8_CTR 0x1920u
#define IA32_PMC_GP8_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP8_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP8_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP8_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP8_CFG_A 0x1921u
#define IA32_PMC_GP8_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP8_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP8_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP8_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  OJO: el manual da 6430 en decimal, que no es 0x1922 */
#define IA32_PMC_GP8_CFG_B 0x1922u

/** SDM Table 2-2. IA-32 Architectural MSRs.  OJO: el manual da 6431 en decimal, que no es 0x1923.  PUERTA: CPUID.23H.05H:EAX[8] */
#define IA32_PMC_GP8_CFG_C 0x1923u
#define IA32_PMC_GP8_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP8_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP8_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP8_CFG_C_GATE_BIT 8u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP9_CTR 0x1924u
#define IA32_PMC_GP9_CTR_GATE_LEAF 0xAu
#define IA32_PMC_GP9_CTR_GATE_REG 0u /* EAX */
#define IA32_PMC_GP9_CTR_GATE_SHIFT 8u
#define IA32_PMC_GP9_CTR_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EAX[15:8] */
#define IA32_PMC_GP9_CFG_A 0x1925u
#define IA32_PMC_GP9_CFG_A_GATE_LEAF 0xAu
#define IA32_PMC_GP9_CFG_A_GATE_REG 0u /* EAX */
#define IA32_PMC_GP9_CFG_A_GATE_SHIFT 8u
#define IA32_PMC_GP9_CFG_A_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  OJO: el manual da 6430 en decimal, que no es 0x1926 */
#define IA32_PMC_GP9_CFG_B 0x1926u

/** SDM Table 2-2. IA-32 Architectural MSRs.  OJO: el manual da 6431 en decimal, que no es 0x1927.  PUERTA: CPUID.23H.05H:EAX[9] */
#define IA32_PMC_GP9_CFG_C 0x1927u
#define IA32_PMC_GP9_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_GP9_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_GP9_CFG_C_GATE_REG 0u /* EAX */
#define IA32_PMC_GP9_CFG_C_GATE_BIT 9u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_PMC_FX0_CTR 0x1980u
#define IA32_PMC_FX0_CTR_GATE_LEAF 0xAu
#define IA32_PMC_FX0_CTR_GATE_REG 3u /* EDX */
#define IA32_PMC_FX0_CTR_GATE_SHIFT 0u
#define IA32_PMC_FX0_CTR_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_FX0_CFG_B 0x1982u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:ECX[0] */
#define IA32_PMC_FX0_CFG_C 0x1983u
#define IA32_PMC_FX0_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_FX0_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_FX0_CFG_C_GATE_REG 2u /* ECX */
#define IA32_PMC_FX0_CFG_C_GATE_BIT 0u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_PMC_FX1_CTR 0x1984u
#define IA32_PMC_FX1_CTR_GATE_LEAF 0xAu
#define IA32_PMC_FX1_CTR_GATE_REG 3u /* EDX */
#define IA32_PMC_FX1_CTR_GATE_SHIFT 0u
#define IA32_PMC_FX1_CTR_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_FX1_CFG_B 0x1986u

/** SDM Table 2-2. IA-32 Architectural MSRs.  OJO: el manual da 6532 en decimal, que no es 0x1987.  PUERTA: CPUID.23H.05H:ECX[1] */
#define IA32_PMC_FX1_CFG_C 0x1987u
#define IA32_PMC_FX1_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_FX1_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_FX1_CFG_C_GATE_REG 2u /* ECX */
#define IA32_PMC_FX1_CFG_C_GATE_BIT 1u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_PMC_FX2_CTR 0x1988u
#define IA32_PMC_FX2_CTR_GATE_LEAF 0xAu
#define IA32_PMC_FX2_CTR_GATE_REG 3u /* EDX */
#define IA32_PMC_FX2_CTR_GATE_SHIFT 0u
#define IA32_PMC_FX2_CTR_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_FX2_CFG_B 0x198Au

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:ECX[2] */
#define IA32_PMC_FX2_CFG_C 0x198Bu
#define IA32_PMC_FX2_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_FX2_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_FX2_CFG_C_GATE_REG 2u /* ECX */
#define IA32_PMC_FX2_CFG_C_GATE_BIT 2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_PMC_FX3_CTR 0x198Cu
#define IA32_PMC_FX3_CTR_GATE_LEAF 0xAu
#define IA32_PMC_FX3_CTR_GATE_REG 3u /* EDX */
#define IA32_PMC_FX3_CTR_GATE_SHIFT 0u
#define IA32_PMC_FX3_CTR_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_FX3_CFG_B 0x198Eu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.23H.05H:ECX[3] */
#define IA32_PMC_FX3_CFG_C 0x198Fu
#define IA32_PMC_FX3_CFG_C_GATE_LEAF 0x23u
#define IA32_PMC_FX3_CFG_C_GATE_SUBLEAF 0x5u
#define IA32_PMC_FX3_CFG_C_GATE_REG 2u /* ECX */
#define IA32_PMC_FX3_CFG_C_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_PMC_FX4_CTR 0x1990u
#define IA32_PMC_FX4_CTR_GATE_LEAF 0xAu
#define IA32_PMC_FX4_CTR_GATE_REG 3u /* EDX */
#define IA32_PMC_FX4_CTR_GATE_SHIFT 0u
#define IA32_PMC_FX4_CTR_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_FX4_CFG_C 0x1993u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_PMC_FX5_CTR 0x1994u
#define IA32_PMC_FX5_CTR_GATE_LEAF 0xAu
#define IA32_PMC_FX5_CTR_GATE_REG 3u /* EDX */
#define IA32_PMC_FX5_CTR_GATE_SHIFT 0u
#define IA32_PMC_FX5_CTR_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_FX5_CFG_C 0x1997u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0AH:EDX[4:0] */
#define IA32_PMC_FX6_CTR 0x1998u
#define IA32_PMC_FX6_CTR_GATE_LEAF 0xAu
#define IA32_PMC_FX6_CTR_GATE_REG 3u /* EDX */
#define IA32_PMC_FX6_CTR_GATE_SHIFT 0u
#define IA32_PMC_FX6_CTR_GATE_MASK 0x1Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PMC_FX6_CFG_C 0x199Bu

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_8_PERFEVTSEL0 0x2040u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_8_PERFEVTSEL1 0x2041u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_9_PERFEVTSEL0 0x2048u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_9_PERFEVTSEL1 0x2049u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_0_PERFEVTSEL0 0x2FD0u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_0_PERFEVTSEL1 0x2FD1u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_1_PERFEVTSEL0 0x2FD8u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_1_PERFEVTSEL1 0x2FD9u


#endif /* VXP_COMMON_MSR_PERF_H */
