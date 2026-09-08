/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_misc.h
 * @brief Arquitectonicos que no caen en ninguna familia
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

#ifndef VXP_COMMON_MSR_MISC_H
#define VXP_COMMON_MSR_MISC_H

/** antes `P5_MC_ADDR`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_P5_MC_ADDR 0x0u

/** antes `P5_MC_TYPE`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_P5_MC_TYPE 0x1u

/** SDM Table 2-44. MSRs Supported         by  the 10th Generation Intel(R) CoreTM Processors (Ice          Lake  Microarchitecture) */
#define IA32_PREDICT_CMD 0x49u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EBX[0] */
#define IA32_PPIN_CTL 0x4Eu
#define IA32_PPIN_CTL_GATE_LEAF 0x7u
#define IA32_PPIN_CTL_GATE_SUBLEAF 0x1u
#define IA32_PPIN_CTL_GATE_REG 1u /* EBX */
#define IA32_PPIN_CTL_GATE_BIT 0u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EBX[0] */
#define IA32_PPIN 0x4Fu
#define IA32_PPIN_GATE_LEAF 0x7u
#define IA32_PPIN_GATE_SUBLEAF 0x1u
#define IA32_PPIN_GATE_REG 1u /* EBX */
#define IA32_PPIN_GATE_BIT 0u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_FEATURE_ACTIVATION 0x7Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCU_ENUMERATION 0x7Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCU_STATUS 0x7Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_FZM_RANGE_INDEX 0x82u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_FZM_DOMAIN_CONFIG 0x83u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_FZM_RANGE_ENDADDR 0x85u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_FZM_RANGE_WRITESTATUS 0x86u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define IA32_PBOPT_CTRL 0xBFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_UMWAIT_CONTROL 0xE1u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[9] */
#define IA32_MCU_OPT_CTRL 0x123u
#define IA32_MCU_OPT_CTRL_GATE_LEAF 0x7u
#define IA32_MCU_OPT_CTRL_GATE_SUBLEAF 0x0u
#define IA32_MCU_OPT_CTRL_GATE_REG 3u /* EDX */
#define IA32_MCU_OPT_CTRL_GATE_BIT 9u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_OVERCLOCKING_STATUS 0x195u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0DH.01H:EAX[4] */
#define IA32_XFD 0x1C4u
#define IA32_XFD_GATE_LEAF 0xDu
#define IA32_XFD_GATE_SUBLEAF 0x1u
#define IA32_XFD_GATE_REG 0u /* EAX */
#define IA32_XFD_GATE_BIT 4u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0DH.01H:EAX[4] */
#define IA32_XFD_ERR 0x1C5u
#define IA32_XFD_ERR_GATE_LEAF 0xDu
#define IA32_XFD_ERR_GATE_SUBLEAF 0x1u
#define IA32_XFD_ERR_GATE_REG 0u /* EAX */
#define IA32_XFD_ERR_GATE_BIT 4u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_RSP0 0x1CCu
#define IA32_FRED_RSP0_GATE_LEAF 0x7u
#define IA32_FRED_RSP0_GATE_SUBLEAF 0x1u
#define IA32_FRED_RSP0_GATE_REG 0u /* EAX */
#define IA32_FRED_RSP0_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_RSP1 0x1CDu
#define IA32_FRED_RSP1_GATE_LEAF 0x7u
#define IA32_FRED_RSP1_GATE_SUBLEAF 0x1u
#define IA32_FRED_RSP1_GATE_REG 0u /* EAX */
#define IA32_FRED_RSP1_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_RSP2 0x1CEu
#define IA32_FRED_RSP2_GATE_LEAF 0x7u
#define IA32_FRED_RSP2_GATE_SUBLEAF 0x1u
#define IA32_FRED_RSP2_GATE_REG 0u /* EAX */
#define IA32_FRED_RSP2_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_RSP3 0x1CFu
#define IA32_FRED_RSP3_GATE_LEAF 0x7u
#define IA32_FRED_RSP3_GATE_SUBLEAF 0x1u
#define IA32_FRED_RSP3_GATE_REG 0u /* EAX */
#define IA32_FRED_RSP3_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_STKLVLS 0x1D0u
#define IA32_FRED_STKLVLS_GATE_LEAF 0x7u
#define IA32_FRED_STKLVLS_GATE_SUBLEAF 0x1u
#define IA32_FRED_STKLVLS_GATE_REG 0u /* EAX */
#define IA32_FRED_STKLVLS_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_SSP1 0x1D1u
#define IA32_FRED_SSP1_GATE_LEAF 0x7u
#define IA32_FRED_SSP1_GATE_SUBLEAF 0x1u
#define IA32_FRED_SSP1_GATE_REG 0u /* EAX */
#define IA32_FRED_SSP1_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_SSP2 0x1D2u
#define IA32_FRED_SSP2_GATE_LEAF 0x7u
#define IA32_FRED_SSP2_GATE_SUBLEAF 0x1u
#define IA32_FRED_SSP2_GATE_REG 0u /* EAX */
#define IA32_FRED_SSP2_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_SSP3 0x1D3u
#define IA32_FRED_SSP3_GATE_LEAF 0x7u
#define IA32_FRED_SSP3_GATE_SUBLEAF 0x1u
#define IA32_FRED_SSP3_GATE_REG 0u /* EAX */
#define IA32_FRED_SSP3_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EAX[17] */
#define IA32_FRED_CONFIG 0x1D4u
#define IA32_FRED_CONFIG_GATE_LEAF 0x7u
#define IA32_FRED_CONFIG_GATE_SUBLEAF 0x1u
#define IA32_FRED_CONFIG_GATE_REG 0u /* EAX */
#define IA32_FRED_CONFIG_GATE_BIT 17u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[18] */
#define IA32_PLATFORM_DCA_CAP 0x1F8u
#define IA32_PLATFORM_DCA_CAP_GATE_LEAF 0x1u
#define IA32_PLATFORM_DCA_CAP_GATE_REG 2u /* ECX */
#define IA32_PLATFORM_DCA_CAP_GATE_BIT 18u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[18] */
#define IA32_CPU_DCA_CAP 0x1F9u
#define IA32_CPU_DCA_CAP_GATE_LEAF 0x1u
#define IA32_CPU_DCA_CAP_GATE_REG 2u /* ECX */
#define IA32_CPU_DCA_CAP_GATE_BIT 18u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[18] */
#define IA32_DCA_0_CAP 0x1FAu
#define IA32_DCA_0_CAP_GATE_LEAF 0x1u
#define IA32_DCA_0_CAP_GATE_REG 2u /* ECX */
#define IA32_DCA_0_CAP_GATE_BIT 18u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EDX[24] */
#define IA32_INTEGRITY_STATUS 0x2DCu
#define IA32_INTEGRITY_STATUS_GATE_LEAF 0x7u
#define IA32_INTEGRITY_STATUS_GATE_SUBLEAF 0x1u
#define IA32_INTEGRITY_STATUS_GATE_REG 3u /* EDX */
#define IA32_INTEGRITY_STATUS_GATE_BIT 24u

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E Microarchitecture) */
#define IA32_MC2MISC 0x467u

/** antes `also called IA32_FRED_SSP0`.  SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[7] */
#define IA32_PL0_SSP 0x6A4u
#define IA32_PL0_SSP_GATE_LEAF 0x7u
#define IA32_PL0_SSP_GATE_SUBLEAF 0x0u
#define IA32_PL0_SSP_GATE_REG 2u /* ECX */
#define IA32_PL0_SSP_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[7] */
#define IA32_PL3_SSP 0x6A7u
#define IA32_PL3_SSP_GATE_LEAF 0x7u
#define IA32_PL3_SSP_GATE_SUBLEAF 0x0u
#define IA32_PL3_SSP_GATE_REG 2u /* ECX */
#define IA32_PL3_SSP_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[7] */
#define IA32_INTERRUPT_SSP_TABLE_ADDR 0x6A8u
#define IA32_INTERRUPT_SSP_TABLE_ADDR_GATE_LEAF 0x7u
#define IA32_INTERRUPT_SSP_TABLE_ADDR_GATE_SUBLEAF 0x0u
#define IA32_INTERRUPT_SSP_TABLE_ADDR_GATE_REG 2u /* ECX */
#define IA32_INTERRUPT_SSP_TABLE_ADDR_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[31] */
#define IA32_PKRS 0x6E1u
#define IA32_PKRS_GATE_LEAF 0x7u
#define IA32_PKRS_GATE_SUBLEAF 0x0u
#define IA32_PKRS_GATE_REG 2u /* ECX */
#define IA32_PKRS_GATE_BIT 31u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCU_EXT_SERVICE 0x7A3u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCU_ROLLBACK_MIN_ID 0x7A4u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCU_STAGING_MBOX_ADDR 0x7A5u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_0 0x7B0u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_1 0x7B1u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_2 0x7B2u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_3 0x7B3u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_4 0x7B4u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_5 0x7B5u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_6 0x7B6u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_7 0x7B7u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_8 0x7B8u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_9 0x7B9u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_10 0x7BAu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_11 0x7BBu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_12 0x7BCu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_13 0x7BDu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_14 0x7BEu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_ROLLBACK_SIGN_ID_15 0x7BFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EDX[13] */
#define IA32_UINTR_RR 0x985u
#define IA32_UINTR_RR_GATE_LEAF 0x7u
#define IA32_UINTR_RR_GATE_SUBLEAF 0x1u
#define IA32_UINTR_RR_GATE_REG 3u /* EDX */
#define IA32_UINTR_RR_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EDX[13] */
#define IA32_UINTR_HANDLER 0x986u
#define IA32_UINTR_HANDLER_GATE_LEAF 0x7u
#define IA32_UINTR_HANDLER_GATE_SUBLEAF 0x1u
#define IA32_UINTR_HANDLER_GATE_REG 3u /* EDX */
#define IA32_UINTR_HANDLER_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EDX[13] */
#define IA32_UINTR_STACKADJUST 0x987u
#define IA32_UINTR_STACKADJUST_GATE_LEAF 0x7u
#define IA32_UINTR_STACKADJUST_GATE_SUBLEAF 0x1u
#define IA32_UINTR_STACKADJUST_GATE_REG 3u /* EDX */
#define IA32_UINTR_STACKADJUST_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EDX[13] */
#define IA32_UINTR_MISC 0x988u
#define IA32_UINTR_MISC_GATE_LEAF 0x7u
#define IA32_UINTR_MISC_GATE_SUBLEAF 0x1u
#define IA32_UINTR_MISC_GATE_REG 3u /* EDX */
#define IA32_UINTR_MISC_GATE_BIT 13u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define IA32_UINTR_NV 0x988u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EDX[13] */
#define IA32_UINTR_PD 0x989u
#define IA32_UINTR_PD_GATE_LEAF 0x7u
#define IA32_UINTR_PD_GATE_SUBLEAF 0x1u
#define IA32_UINTR_PD_GATE_REG 3u /* EDX */
#define IA32_UINTR_PD_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EDX[13] */
#define IA32_UINTR_TT 0x98Au
#define IA32_UINTR_TT_GATE_LEAF 0x7u
#define IA32_UINTR_TT_GATE_SUBLEAF 0x1u
#define IA32_UINTR_TT_GATE_REG 3u /* EDX */
#define IA32_UINTR_TT_GATE_BIT 13u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define IA32_COPY_STATUS 0x990u

/** SDM Table 2-45. Additional MSRs Supported by the 11th Generation Intel(R) CoreTM Processors Based on                           Tiger Lake */
#define IA32_COPY_STATUS1 0x990u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.19H:EBX[4] */
#define IA32_COPY_STATUS4 0x990u
#define IA32_COPY_STATUS4_GATE_LEAF 0x19u
#define IA32_COPY_STATUS4_GATE_REG 1u /* EBX */
#define IA32_COPY_STATUS4_GATE_BIT 4u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define IA32_IWKEYBACKUP_STATUS 0x991u

/** SDM Table 2-45. Additional MSRs Supported by the 11th Generation Intel(R) CoreTM Processors Based on                           Tiger Lake */
#define IA32_IWKEYBACKUP_STATUS1 0x991u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.19H:EBX[4] */
#define IA32_IWKEYBACKUP_STATUS5 0x991u
#define IA32_IWKEYBACKUP_STATUS5_GATE_LEAF 0x19u
#define IA32_IWKEYBACKUP_STATUS5_GATE_REG 1u /* EBX */
#define IA32_IWKEYBACKUP_STATUS5_GATE_BIT 4u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.01H:EBX[1] */
#define IA32_TSE_CAPABILITY 0x9F1u
#define IA32_TSE_CAPABILITY_GATE_LEAF 0x7u
#define IA32_TSE_CAPABILITY_GATE_SUBLEAF 0x1u
#define IA32_TSE_CAPABILITY_GATE_REG 1u /* EBX */
#define IA32_TSE_CAPABILITY_GATE_BIT 1u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[11] */
#define IA32_DEBUG_INTERFACE 0xC80u
#define IA32_DEBUG_INTERFACE_GATE_LEAF 0x1u
#define IA32_DEBUG_INTERFACE_GATE_REG 2u /* ECX */
#define IA32_DEBUG_INTERFACE_GATE_BIT 11u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_RESOURCE_PRIORITY 0xC88u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.10H.06H:EAX[1] */
#define IA32_RESOURCE_PRIORITY_PKG 0xC89u
#define IA32_RESOURCE_PRIORITY_PKG_GATE_LEAF 0x10u
#define IA32_RESOURCE_PRIORITY_PKG_GATE_SUBLEAF 0x6u
#define IA32_RESOURCE_PRIORITY_PKG_GATE_REG 0u /* EAX */
#define IA32_RESOURCE_PRIORITY_PKG_GATE_BIT 1u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[12] */
#define IA32_QM_EVTSEL 0xC8Du
#define IA32_QM_EVTSEL_GATE_LEAF 0x7u
#define IA32_QM_EVTSEL_GATE_SUBLEAF 0x0u
#define IA32_QM_EVTSEL_GATE_REG 1u /* EBX */
#define IA32_QM_EVTSEL_GATE_BIT 12u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[12] */
#define IA32_QM_CTR 0xC8Eu
#define IA32_QM_CTR_GATE_LEAF 0x7u
#define IA32_QM_CTR_GATE_SUBLEAF 0x0u
#define IA32_QM_CTR_GATE_REG 1u /* EBX */
#define IA32_QM_CTR_GATE_BIT 12u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[14] */
#define IA32_BNDCFGS 0xD90u
#define IA32_BNDCFGS_GATE_LEAF 0x7u
#define IA32_BNDCFGS_GATE_SUBLEAF 0x0u
#define IA32_BNDCFGS_GATE_REG 1u /* EBX */
#define IA32_BNDCFGS_GATE_BIT 14u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define IA32_COPY_LOCAL_TO_PLATFORM 0xD91u

/** SDM Table 2-45. Additional MSRs Supported by the 11th Generation Intel(R) CoreTM Processors Based on                     Tiger Lake */
#define IA32_COPY_LOCAL_TO_PLATFORM1 0xD91u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.19H:EBX[4] */
#define IA32_COPY_LOCAL_TO_PLATFORM5 0xD91u
#define IA32_COPY_LOCAL_TO_PLATFORM5_GATE_LEAF 0x19u
#define IA32_COPY_LOCAL_TO_PLATFORM5_GATE_REG 1u /* EBX */
#define IA32_COPY_LOCAL_TO_PLATFORM5_GATE_BIT 4u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define IA32_COPY_PLATFORM_TO_LOCAL 0xD92u

/** SDM Table 2-45. Additional MSRs Supported by the 11th Generation Intel(R) CoreTM Processors Based on                     Tiger Lake */
#define IA32_COPY_PLATFORM_TO_LOCAL1 0xD92u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.19H:EBX[4] */
#define IA32_COPY_PLATFORM_TO_LOCAL5 0xD92u
#define IA32_COPY_PLATFORM_TO_LOCAL5_GATE_LEAF 0x19u
#define IA32_COPY_PLATFORM_TO_LOCAL5_GATE_REG 1u /* EBX */
#define IA32_COPY_PLATFORM_TO_LOCAL5_GATE_BIT 4u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.0DH.01H:EAX[3] */
#define IA32_XSS 0xDA0u
#define IA32_XSS_GATE_LEAF 0xDu
#define IA32_XSS_GATE_SUBLEAF 0x1u
#define IA32_XSS_GATE_REG 0u /* EAX */
#define IA32_XSS_GATE_BIT 3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[13] */
#define IA32_PM_CTL1 0xDB1u
#define IA32_PM_CTL1_GATE_LEAF 0x6u
#define IA32_PM_CTL1_GATE_REG 0u /* EAX */
#define IA32_PM_CTL1_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[13] */
#define IA32_THREAD_STALL 0xDB2u
#define IA32_THREAD_STALL_GATE_LEAF 0x6u
#define IA32_THREAD_STALL_GATE_REG 0u /* EAX */
#define IA32_THREAD_STALL_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SEAMRR_BASE 0x1400u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.80000008H:EAX[7:0] */
#define IA32_SEAMRR_MASK 0x1401u
#define IA32_SEAMRR_MASK_GATE_LEAF 0x80000008u
#define IA32_SEAMRR_MASK_GATE_REG 0u /* EAX */
#define IA32_SEAMRR_MASK_GATE_SHIFT 0u
#define IA32_SEAMRR_MASK_GATE_MASK 0xFFu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[29] */
#define IA32_MCU_CONTROL 0x1406u
#define IA32_MCU_CONTROL_GATE_LEAF 0x7u
#define IA32_MCU_CONTROL_GATE_SUBLEAF 0x0u
#define IA32_MCU_CONTROL_GATE_REG 3u /* EDX */
#define IA32_MCU_CONTROL_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.06H:EAX[23] */
#define IA32_THREAD_FEEDBACK_CHAR 0x17D2u
#define IA32_THREAD_FEEDBACK_CHAR_GATE_LEAF 0x6u
#define IA32_THREAD_FEEDBACK_CHAR_GATE_REG 0u /* EAX */
#define IA32_THREAD_FEEDBACK_CHAR_GATE_BIT 23u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_HRESET_ENABLE 0x17DAu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_UARCH_MISC_CTL 0x1B01u


#endif /* VXP_COMMON_MSR_MISC_H */
