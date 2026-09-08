/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_power.h
 * @brief Frecuencia, energia y control termico
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

#ifndef VXP_COMMON_MSR_POWER_H
#define VXP_COMMON_MSR_POWER_H

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define EBL_CR_POWERON 0x2Au

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_EBC_HARD_POWERON 0x2Au

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EBL_CR_POWERON 0x2Au

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_EBC_SOFT_POWERON 0x2Bu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_EBC_FREQUENCY_ID 0x2Cu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_FSB_FREQ 0xCDu

/** SDM Table 2-7. MSRs Common to the Silvermont and Airmont Microarchitectures */
#define MSR_PKG_CST_CONFIG_CONTROL 0xE2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:ECX[0] */
#define IA32_MPERF 0xE7u
#define IA32_MPERF_GATE_LEAF 0x6u
#define IA32_MPERF_GATE_REG 2u /* ECX */
#define IA32_MPERF_GATE_BIT 0u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:ECX[0] */
#define IA32_APERF 0xE8u
#define IA32_APERF_GATE_LEAF 0x6u
#define IA32_APERF_GATE_REG 2u /* ECX */
#define IA32_APERF_GATE_BIT 0u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PERF_STATUS 0x198u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_PERF_STATUS 0x198u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PERF_CTL 0x199u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[22] */
#define IA32_CLOCK_MODULATION 0x19Au
#define IA32_CLOCK_MODULATION_GATE_LEAF 0x1u
#define IA32_CLOCK_MODULATION_GATE_REG 3u /* EDX */
#define IA32_CLOCK_MODULATION_GATE_BIT 22u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[22] */
#define IA32_THERM_INTERRUPT 0x19Bu
#define IA32_THERM_INTERRUPT_GATE_LEAF 0x1u
#define IA32_THERM_INTERRUPT_GATE_REG 3u /* EDX */
#define IA32_THERM_INTERRUPT_GATE_BIT 22u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:EDX[22] */
#define IA32_THERM_STATUS 0x19Cu
#define IA32_THERM_STATUS_GATE_LEAF 0x1u
#define IA32_THERM_STATUS_GATE_REG 3u /* EDX */
#define IA32_THERM_STATUS_GATE_BIT 22u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_THERM2_CTL 0x19Du

/** SDM Table 2-15. MSRs in Processors Based on Nehalem Microarchitecture */
#define MSR_TURBO_POWER_CURRENT_LIMIT 0x1ACu

/** SDM Table 2-37. Additional MSRs Supported by Intel(R) Xeon(R) Processor D with a                    CPUID  Signature */
#define MSR_TURBO_RATIO_LIMIT3 0x1ACu

/** SDM Table 2-46. Additional MSRs Supported by the 12th and 13th Generation Intel(R) CoreTM Processors Supporting */
#define MSR_PRIMARY_TURBO_RATIO_LIMIT 0x1ADu

/** SDM Table 2-10. Specific MSRs Supported by Intel Atom(R) Processor C2000 Series with a CPUID Signature */
#define MSR_TURBO_RATIO_LIMIT 0x1ADu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_TURBO_GROUP_CORECNT 0x1AEu

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E Microarchitecture) */
#define MSR_TURBO_RATIO_LIMIT1 0x1AEu

/** SDM Table 2-50. MSRs Supported by the Intel(R) Xeon(R) Scalable Processor Family with a CPUID Signature */
#define MSR_TURBO_RATIO_LIMIT_CORES 0x1AEu

/** SDM Table 2-32. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_TURBO_RATIO_LIMIT2 0x1AFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:ECX[3] */
#define IA32_ENERGY_PERF_BIAS 0x1B0u
#define IA32_ENERGY_PERF_BIAS_GATE_LEAF 0x6u
#define IA32_ENERGY_PERF_BIAS_GATE_REG 2u /* ECX */
#define IA32_ENERGY_PERF_BIAS_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PACKAGE_THERM_STATUS 0x1B1u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[6] */
#define IA32_PACKAGE_THERM_INTERRUPT 0x1B2u
#define IA32_PACKAGE_THERM_INTERRUPT_GATE_LEAF 0x6u
#define IA32_PACKAGE_THERM_INTERRUPT_GATE_REG 0u /* EAX */
#define IA32_PACKAGE_THERM_INTERRUPT_GATE_BIT 6u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_POWER_CTL 0x1FCu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_PKG_C3_RESIDENCY 0x3F8u

/** SDM Table 2-5. MSRs Supported by Intel Atom(R) Processors with a CPUID Signature DisplayFamily_DisplayModel */
#define MSR_PKG_C4_RESIDENCY 0x3F9u

/** SDM Table 2-15. MSRs in Processors Based on Nehalem Microarchitecture */
#define MSR_PKG_C7_RESIDENCY 0x3FAu

/** SDM Table 2-58. Additional MSRs Supported by the Intel(R) Series 2 CoreTM Ultra Processors Supporting Performance */
#define MSR_PKG_POWER_LIMIT_4 0x601u

/** SDM Table 2-8. Specific MSRs Supported by Intel Atom(R) Processors with a CPUID Signature DisplayFamily_DisplayModel */
#define MSR_RAPL_POWER_UNIT 0x606u

/** SDM Table 2-8. Specific MSRs Supported by Intel Atom(R) Processors with a CPUID Signature DisplayFamily_DisplayModel */
#define MSR_PKG_POWER_LIMIT 0x610u

/** SDM Table 2-8. Specific MSRs Supported by Intel Atom(R) Processors with a CPUID Signature DisplayFamily_DisplayModel */
#define MSR_PKG_ENERGY_STATUS 0x611u

/** SDM Table 2-51. MSRs Supported by the 3rd Generation Intel(R) Xeon(R) Scalable Processor Family with a CPUID Signature */
#define MSR_PACKAGE_ENERGY_TIME_STATUS 0x612u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_PKG_PERF_STATUS 0x613u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_DRAM_POWER_LIMIT 0x618u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_DRAM_ENERGY_STATUS 0x619u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_DRAM_PERF_STATUS 0x61Bu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_DRAM_POWER_INFO 0x61Cu

/** SDM Table 2-31. Additional Residency MSRs Supported by 4th Generation Intel(R) CoreTM Processors                      with a */
#define MSR_PKG_C8_RESIDENCY 0x630u

/** SDM Table 2-31. Additional Residency MSRs Supported by 4th Generation Intel(R) CoreTM Processors                      with a */
#define MSR_PKG_C9_RESIDENCY 0x631u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_PKG_C10_RESIDENCY 0x632u

/** SDM Table 2-11. MSRs in Intel Atom(R) Processors Based on Airmont Microarchitecture */
#define MSR_PP0_POWER_LIMIT 0x638u

/** SDM Table 2-8. Specific MSRs Supported by Intel Atom(R) Processors with a CPUID Signature DisplayFamily_DisplayModel */
#define MSR_PP0_ENERGY_STATUS 0x639u

/** SDM Table 2-21. MSRs Supported by the 2nd Generation Intel(R) CoreTM Processors (Sandy Bridge Microarchitecture) */
#define MSR_PP1_POWER_LIMIT 0x640u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_PP1_ENERGY_STATUS 0x641u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_TURBO_ACTIVATION_RATIO 0x64Cu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_PLATFORM_ENERGY_COUNTER 0x64Du

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_PLATFORM_ENERGY_STATUS 0x64Du

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_PPERF 0x64Eu

/** SDM Table 2-46. Additional MSRs Supported by the 12th and 13th Generation Intel(R) CoreTM Processors                      Supporting */
#define MSR_SECONDARY_TURBO_RATIO_LIMIT 0x650u

/** SDM Table 2-58. Additional MSRs Supported by the Intel(R) Series 2 CoreTM Ultra Processors Supporting Performance */
#define MSR_SECONDARY_TURBO_RATIO_LIMIT_CORES 0x651u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_PKG_HDC_CONFIG 0x652u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_PKG_HDC_SHALLOW_RESIDENCY 0x655u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_PKG_HDC_DEEP_RESIDENCY 0x656u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors,.  PUERTA: CPUID.06H:EAX[4] */
#define MSR_PLATFORM_POWER_LIMIT 0x65Cu
#define MSR_PLATFORM_POWER_LIMIT_GATE_LEAF 0x6u
#define MSR_PLATFORM_POWER_LIMIT_GATE_REG 0u /* EAX */
#define MSR_PLATFORM_POWER_LIMIT_GATE_BIT 4u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_PLATFORM_POWER_INFO 0x665u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_PLATFORM_RAPL_SOCKET_PERF_STATUS 0x666u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[7] */
#define IA32_PL1_SSP 0x6A5u
#define IA32_PL1_SSP_GATE_LEAF 0x7u
#define IA32_PL1_SSP_GATE_SUBLEAF 0x0u
#define IA32_PL1_SSP_GATE_REG 2u /* ECX */
#define IA32_PL1_SSP_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[7] */
#define IA32_PL2_SSP 0x6A6u
#define IA32_PL2_SSP_GATE_LEAF 0x7u
#define IA32_PL2_SSP_GATE_SUBLEAF 0x0u
#define IA32_PL2_SSP_GATE_REG 2u /* ECX */
#define IA32_PL2_SSP_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[7] */
#define IA32_PM_ENABLE 0x770u
#define IA32_PM_ENABLE_GATE_LEAF 0x6u
#define IA32_PM_ENABLE_GATE_REG 0u /* EAX */
#define IA32_PM_ENABLE_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[7] */
#define IA32_HWP_CAPABILITIES 0x771u
#define IA32_HWP_CAPABILITIES_GATE_LEAF 0x6u
#define IA32_HWP_CAPABILITIES_GATE_REG 0u /* EAX */
#define IA32_HWP_CAPABILITIES_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[11] */
#define IA32_HWP_REQUEST_PKG 0x772u
#define IA32_HWP_REQUEST_PKG_GATE_LEAF 0x6u
#define IA32_HWP_REQUEST_PKG_GATE_REG 0u /* EAX */
#define IA32_HWP_REQUEST_PKG_GATE_BIT 11u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[8] */
#define IA32_HWP_INTERRUPT 0x773u
#define IA32_HWP_INTERRUPT_GATE_LEAF 0x6u
#define IA32_HWP_INTERRUPT_GATE_REG 0u /* EAX */
#define IA32_HWP_INTERRUPT_GATE_BIT 8u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[7] */
#define IA32_HWP_REQUEST 0x774u
#define IA32_HWP_REQUEST_GATE_LEAF 0x6u
#define IA32_HWP_REQUEST_GATE_REG 0u /* EAX */
#define IA32_HWP_REQUEST_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PECI_HWP_REQUEST_INFO 0x775u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[22] */
#define IA32_HWP_CTL 0x776u
#define IA32_HWP_CTL_GATE_LEAF 0x6u
#define IA32_HWP_CTL_GATE_REG 0u /* EAX */
#define IA32_HWP_CTL_GATE_BIT 22u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[7] */
#define IA32_HWP_STATUS 0x777u
#define IA32_HWP_STATUS_GATE_LEAF 0x6u
#define IA32_HWP_STATUS_GATE_REG 0u /* EAX */
#define IA32_HWP_STATUS_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[21] */
#define IA32_X2APIC_LVT_THERMAL 0x833u
#define IA32_X2APIC_LVT_THERMAL_GATE_LEAF 0x1u
#define IA32_X2APIC_LVT_THERMAL_GATE_REG 2u /* ECX */
#define IA32_X2APIC_LVT_THERMAL_GATE_BIT 21u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[13] */
#define IA32_PKG_HDC_CTL 0xDB0u
#define IA32_PKG_HDC_CTL_GATE_LEAF 0x6u
#define IA32_PKG_HDC_CTL_GATE_REG 0u /* EAX */
#define IA32_PKG_HDC_CTL_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[19] */
#define IA32_HW_FEEDBACK_PTR 0x17D0u
#define IA32_HW_FEEDBACK_PTR_GATE_LEAF 0x6u
#define IA32_HW_FEEDBACK_PTR_GATE_REG 0u /* EAX */
#define IA32_HW_FEEDBACK_PTR_GATE_BIT 19u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[19] */
#define IA32_HW_FEEDBACK_CONFIG 0x17D1u
#define IA32_HW_FEEDBACK_CONFIG_GATE_LEAF 0x6u
#define IA32_HW_FEEDBACK_CONFIG_GATE_REG 0u /* EAX */
#define IA32_HW_FEEDBACK_CONFIG_GATE_BIT 19u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_HW_FEEDBACK_THREAD_CONFIG 0x17D4u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_0_PERF_STATUS 0x2FD4u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_1_PERF_STATUS 0x2FDCu


#endif /* VXP_COMMON_MSR_POWER_H */
