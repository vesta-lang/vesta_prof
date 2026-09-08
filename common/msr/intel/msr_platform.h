/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_platform.h
 * @brief Plataforma, APIC, microcodigo y llamada al sistema
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

#ifndef VXP_COMMON_MSR_PLATFORM_H
#define VXP_COMMON_MSR_PLATFORM_H

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define IA32_MONITOR_FILTER_LINE_SIZE 0x6u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MONITOR_FILTER_SIZE 0x6u

/** antes `TSC`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_TIME_STAMP_COUNTER 0x10u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define TSC 0x10u

/** antes `MSR_PLATFORM_ID`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PLATFORM_ID 0x17u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_PLATFORM_ID 0x17u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define APIC_BASE 0x1Bu

/** antes `APIC_BASE`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_APIC_BASE 0x1Bu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[27] */
#define IA32_BARRIER 0x2Fu
#define IA32_BARRIER_GATE_LEAF 0x7u
#define IA32_BARRIER_GATE_SUBLEAF 0x1u
#define IA32_BARRIER_GATE_REG 0u /* EAX */
#define IA32_BARRIER_GATE_BIT 27u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_FEATURE_CONTROL 0x3Au
#define IA32_FEATURE_CONTROL_GATE_LEAF 0x1u
#define IA32_FEATURE_CONTROL_GATE_REG 2u /* ECX */
#define IA32_FEATURE_CONTROL_GATE_BIT 5u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_FEATURE_CONTROL 0x3Au

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[1] */
#define IA32_TSC_ADJUST 0x3Bu
#define IA32_TSC_ADJUST_GATE_LEAF 0x7u
#define IA32_TSC_ADJUST_GATE_SUBLEAF 0x0u
#define IA32_TSC_ADJUST_GATE_REG 1u /* EBX */
#define IA32_TSC_ADJUST_GATE_BIT 1u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BIOS_UPDT_TRIG 0x79u

/** antes `BIOS_UPDT_TRIG`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_BIOS_UPDT_TRIG 0x79u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_FZM_RANGE_STARTADDR 0x84u

/** antes `BIOS_SIGN/BBL_CR_D3`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_BIOS_SIGN_ID 0x8Bu

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_BIOS_MCU_ERRORCODE 0xA0u

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_FIT_BIOS_ERROR 0xA5u

/** SDM Table 2-45. Additional MSRs Supported by the 11th Generation Intel(R) CoreTM Processors Based on Tiger Lake */
#define MSR_BIOS_DEBUG 0xA7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[29] */
#define IA32_XAPIC_DISABLE_STATUS 0xBDu
#define IA32_XAPIC_DISABLE_STATUS_GATE_LEAF 0x7u
#define IA32_XAPIC_DISABLE_STATUS_GATE_SUBLEAF 0x0u
#define IA32_XAPIC_DISABLE_STATUS_GATE_REG 3u /* EDX */
#define IA32_XAPIC_DISABLE_STATUS_GATE_BIT 29u

/** SDM Table 2-7. MSRs Common to the Silvermont and Airmont Microarchitectures */
#define MSR_PLATFORM_INFO 0xCEu

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_BIOS_DONE 0x151u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SYSENTER_CS 0x174u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define SYSENTER_CS_MSR 0x174u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SYSENTER_ESP 0x175u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define SYSENTER_ESP_MSR 0x175u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SYSENTER_EIP 0x176u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define SYSENTER_EIP_MSR 0x176u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MISC_ENABLE 0x1A0u

/** SDM Table 2-10. Specific MSRs Supported by Intel Atom(R) Processor C2000 Series with a CPUID Signature */
#define MSR_MISC_FEATURE_CONTROL 0x1A4u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[24] */
#define IA32_TSC_DEADLINE 0x6E0u
#define IA32_TSC_DEADLINE_GATE_LEAF 0x1u
#define IA32_TSC_DEADLINE_GATE_REG 2u /* ECX */
#define IA32_TSC_DEADLINE_GATE_BIT 24u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_APICID 0x802u
#define IA32_X2APIC_APICID_GATE_LEAF 0x1u
#define IA32_X2APIC_APICID_GATE_REG 2u /* ECX */
#define IA32_X2APIC_APICID_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_VERSION 0x803u
#define IA32_X2APIC_VERSION_GATE_LEAF 0x1u
#define IA32_X2APIC_VERSION_GATE_REG 2u /* ECX */
#define IA32_X2APIC_VERSION_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TPR 0x808u
#define IA32_X2APIC_TPR_GATE_LEAF 0x1u
#define IA32_X2APIC_TPR_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TPR_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_PPR 0x80Au
#define IA32_X2APIC_PPR_GATE_LEAF 0x1u
#define IA32_X2APIC_PPR_GATE_REG 2u /* ECX */
#define IA32_X2APIC_PPR_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_EOI 0x80Bu
#define IA32_X2APIC_EOI_GATE_LEAF 0x1u
#define IA32_X2APIC_EOI_GATE_REG 2u /* ECX */
#define IA32_X2APIC_EOI_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LDR 0x80Du
#define IA32_X2APIC_LDR_GATE_LEAF 0x1u
#define IA32_X2APIC_LDR_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LDR_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_SIVR 0x80Fu
#define IA32_X2APIC_SIVR_GATE_LEAF 0x1u
#define IA32_X2APIC_SIVR_GATE_REG 2u /* ECX */
#define IA32_X2APIC_SIVR_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR0 0x810u
#define IA32_X2APIC_ISR0_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR0_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR0_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR1 0x811u
#define IA32_X2APIC_ISR1_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR1_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR1_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR2 0x812u
#define IA32_X2APIC_ISR2_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR2_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR2_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR3 0x813u
#define IA32_X2APIC_ISR3_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR3_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR3_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR4 0x814u
#define IA32_X2APIC_ISR4_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR4_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR4_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR5 0x815u
#define IA32_X2APIC_ISR5_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR5_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR5_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR6 0x816u
#define IA32_X2APIC_ISR6_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR6_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR6_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ISR7 0x817u
#define IA32_X2APIC_ISR7_GATE_LEAF 0x1u
#define IA32_X2APIC_ISR7_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ISR7_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR0 0x818u
#define IA32_X2APIC_TMR0_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR0_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR0_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR1 0x819u
#define IA32_X2APIC_TMR1_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR1_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR1_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR2 0x81Au
#define IA32_X2APIC_TMR2_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR2_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR2_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR3 0x81Bu
#define IA32_X2APIC_TMR3_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR3_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR3_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR4 0x81Cu
#define IA32_X2APIC_TMR4_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR4_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR4_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR5 0x81Du
#define IA32_X2APIC_TMR5_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR5_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR5_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR6 0x81Eu
#define IA32_X2APIC_TMR6_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR6_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR6_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_TMR7 0x81Fu
#define IA32_X2APIC_TMR7_GATE_LEAF 0x1u
#define IA32_X2APIC_TMR7_GATE_REG 2u /* ECX */
#define IA32_X2APIC_TMR7_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR0 0x820u
#define IA32_X2APIC_IRR0_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR0_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR0_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR1 0x821u
#define IA32_X2APIC_IRR1_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR1_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR1_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR2 0x822u
#define IA32_X2APIC_IRR2_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR2_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR2_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR3 0x823u
#define IA32_X2APIC_IRR3_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR3_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR3_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR4 0x824u
#define IA32_X2APIC_IRR4_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR4_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR4_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR5 0x825u
#define IA32_X2APIC_IRR5_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR5_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR5_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR6 0x826u
#define IA32_X2APIC_IRR6_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR6_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR6_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_IRR7 0x827u
#define IA32_X2APIC_IRR7_GATE_LEAF 0x1u
#define IA32_X2APIC_IRR7_GATE_REG 2u /* ECX */
#define IA32_X2APIC_IRR7_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ESR 0x828u
#define IA32_X2APIC_ESR_GATE_LEAF 0x1u
#define IA32_X2APIC_ESR_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ESR_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LVT_CMCI 0x82Fu
#define IA32_X2APIC_LVT_CMCI_GATE_LEAF 0x1u
#define IA32_X2APIC_LVT_CMCI_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LVT_CMCI_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_ICR 0x830u
#define IA32_X2APIC_ICR_GATE_LEAF 0x1u
#define IA32_X2APIC_ICR_GATE_REG 2u /* ECX */
#define IA32_X2APIC_ICR_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LVT_TIMER 0x832u
#define IA32_X2APIC_LVT_TIMER_GATE_LEAF 0x1u
#define IA32_X2APIC_LVT_TIMER_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LVT_TIMER_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LVT_PMI 0x834u
#define IA32_X2APIC_LVT_PMI_GATE_LEAF 0x1u
#define IA32_X2APIC_LVT_PMI_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LVT_PMI_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LVT_LINT0 0x835u
#define IA32_X2APIC_LVT_LINT0_GATE_LEAF 0x1u
#define IA32_X2APIC_LVT_LINT0_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LVT_LINT0_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LVT_LINT1 0x836u
#define IA32_X2APIC_LVT_LINT1_GATE_LEAF 0x1u
#define IA32_X2APIC_LVT_LINT1_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LVT_LINT1_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LVT_ERROR 0x837u
#define IA32_X2APIC_LVT_ERROR_GATE_LEAF 0x1u
#define IA32_X2APIC_LVT_ERROR_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LVT_ERROR_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_INIT_COUNT 0x838u
#define IA32_X2APIC_INIT_COUNT_GATE_LEAF 0x1u
#define IA32_X2APIC_INIT_COUNT_GATE_REG 2u /* ECX */
#define IA32_X2APIC_INIT_COUNT_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_CUR_COUNT 0x839u
#define IA32_X2APIC_CUR_COUNT_GATE_LEAF 0x1u
#define IA32_X2APIC_CUR_COUNT_GATE_REG 2u /* ECX */
#define IA32_X2APIC_CUR_COUNT_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_DIV_CONF 0x83Eu
#define IA32_X2APIC_DIV_CONF_GATE_LEAF 0x1u
#define IA32_X2APIC_DIV_CONF_GATE_REG 2u /* ECX */
#define IA32_X2APIC_DIV_CONF_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_SELF_IPI 0x83Fu
#define IA32_X2APIC_SELF_IPI_GATE_LEAF 0x1u
#define IA32_X2APIC_SELF_IPI_GATE_REG 2u /* ECX */
#define IA32_X2APIC_SELF_IPI_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[20] */
#define IA32_EFER 0xC0000080u
#define IA32_EFER_GATE_LEAF 0x80000001u
#define IA32_EFER_GATE_REG 3u /* EDX */
#define IA32_EFER_GATE_BIT 20u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[29] */
#define IA32_STAR 0xC0000081u
#define IA32_STAR_GATE_LEAF 0x80000001u
#define IA32_STAR_GATE_REG 3u /* EDX */
#define IA32_STAR_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[29] */
#define IA32_LSTAR 0xC0000082u
#define IA32_LSTAR_GATE_LEAF 0x80000001u
#define IA32_LSTAR_GATE_REG 3u /* EDX */
#define IA32_LSTAR_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[29] */
#define IA32_CSTAR 0xC0000083u
#define IA32_CSTAR_GATE_LEAF 0x80000001u
#define IA32_CSTAR_GATE_REG 3u /* EDX */
#define IA32_CSTAR_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[29] */
#define IA32_FMASK 0xC0000084u
#define IA32_FMASK_GATE_LEAF 0x80000001u
#define IA32_FMASK_GATE_REG 3u /* EDX */
#define IA32_FMASK_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[29] */
#define IA32_FS_BASE 0xC0000100u
#define IA32_FS_BASE_GATE_LEAF 0x80000001u
#define IA32_FS_BASE_GATE_REG 3u /* EDX */
#define IA32_FS_BASE_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[29] */
#define IA32_GS_BASE 0xC0000101u
#define IA32_GS_BASE_GATE_LEAF 0x80000001u
#define IA32_GS_BASE_GATE_REG 3u /* EDX */
#define IA32_GS_BASE_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[29] */
#define IA32_KERNEL_GS_BASE 0xC0000102u
#define IA32_KERNEL_GS_BASE_GATE_LEAF 0x80000001u
#define IA32_KERNEL_GS_BASE_GATE_REG 3u /* EDX */
#define IA32_KERNEL_GS_BASE_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000001H:EDX[27] */
#define IA32_TSC_AUX 0xC0000103u
#define IA32_TSC_AUX_GATE_LEAF 0x80000001u
#define IA32_TSC_AUX_GATE_REG 3u /* EDX */
#define IA32_TSC_AUX_GATE_BIT 27u


#endif /* VXP_COMMON_MSR_PLATFORM_H */
