/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_legacy.h
 * @brief Model-specific antiguos y nombres retirados
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

#ifndef VXP_COMMON_MSR_LEGACY_H
#define VXP_COMMON_MSR_LEGACY_H

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define P5_MC_ADDR 0x0u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define P5_MC_TYPE 0x1u

/** SDM Table 2-69. MSRs in the Pentium Processor */
#define CESR 0x11u

/** SDM Table 2-69. MSRs in the Pentium Processor */
#define CTR0 0x12u

/** SDM Table 2-69. MSRs in the Pentium Processor */
#define CTR1 0x13u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_MEMORY_CONTROL 0x33u

/** SDM Table 2-14. MSRs  in  Intel Atom(R) Processors Based on Tremont Microarchitecture */
#define MSR_MEMORY_CTRL 0x33u

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define MSR_SMI_COUNT 0x34u

/** SDM Table 2-32. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_CORE_THREAD_COUNT 0x35u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SOCKET_ID 0x39u

/** SDM Table 2-32. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_THREAD_ID_INFO 0x53u

/** SDM Table 2-41. Additional MSRs Supported by the 7th Generation and 8th Generation Intel(R) CoreTM Processors Based */
#define MSR_TRACE_HUB_STH_ACPIBAR_BASE 0x80u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_D1 0x89u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_D2 0x8Au

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SEAM_WBINVDP 0x98u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SEAM_WBNOINVDP 0x99u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SEAM_INTR_PENDING 0x9Au

/** antes `PERFCTR0`.  SDM Table 2-68. MSRs in the P6 Family Processors */
#define PerfCtr0 0xC1u

/** antes `PERFCTR1`.  SDM Table 2-68. MSRs in the P6 Family Processors */
#define PerfCtr1 0xC2u

/** SDM Table 2-53. Additional MSRs Supported by the Intel(R) CoreTM Ultra 7 Processors Supporting Performance Hybrid */
#define MSR_IO_CAPTURE_BASE 0xE4u

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define MSR_PMG_IO_CAPTURE_BASE 0xE4u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_RAR_CONTROL 0xEDu

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_RAR_ACTION_VECTOR_BASE 0xEEu

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_RAR_PAYLOAD_TABLE_BASE 0xEFu

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_RAR_INFO 0xF0u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_ARRAY_BIST 0x105u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_ARRAY_BIST_STATUS 0x105u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_CORE_BIST 0x105u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_ADDR 0x116u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_DECC 0x118u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_CTL 0x119u

/** SDM Table 2-67. MSRs in Pentium M Processors */
#define MSR_BBL_CR_CTL 0x119u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_TRIG 0x11Au

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_BUSY 0x11Bu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define BBL_CR_CTL3 0x11Eu

/** SDM Table 2-4. MSRs in the 45 nm and 32 nm Intel Atom(R) Processor Family.  OJO: el manual da 281 en decimal, que no es 0x11E */
#define MSR_BBL_CR_CTL3 0x11Eu

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define MSR_FEATURE_CONFIG 0x13Cu

/** SDM Table 2-61. Selected MSRs Supported by Intel(R) Xeon PhiTM Processors with a CPUID Signature */
#define MISC_FEATURE_ENABLES 0x140u

/** SDM Table 2-53. Additional MSRs Supported by the Intel(R) CoreTM Ultra 7 Processors Supporting Performance Hybrid */
#define MSR_FEATURE_ENABLES 0x140u

/** SDM Table 2-23. Additional MSRs Supported by the Intel(R) Xeon(R) Processors E5 Family Based on Sandy Bridge */
#define MSR_ERROR_CONTROL 0x17Fu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_PLATFORM_BRV 0x1A1u

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define MSR_TEMPERATURE_TARGET 0x1A2u

/** SDM Table 2-47. MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processor P-core */
#define MSR_PREFETCH_CONTROL 0x1A4u

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define MSR_OFFCORE_RSP_0 0x1A6u

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define MSR_OFFCORE_RSP_1 0x1A7u

/** SDM Table 2-15. MSRs in Processors Based on Nehalem Microarchitecture.  PUERTA: CPUID.06H:ECX[3] */
#define MSR_MISC_PWR_MGMT 0x1AAu
#define MSR_MISC_PWR_MGMT_GATE_LEAF 0x6u
#define MSR_MISC_PWR_MGMT_GATE_REG 2u /* ECX */
#define MSR_MISC_PWR_MGMT_GATE_BIT 3u

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_CRASHLOG_CONTROL 0x1F1u

/** SDM Table 2-41. Additional MSRs Supported by the 7th Generation and 8th Generation Intel(R) CoreTM Processors Based */
#define MSR_PRMRR_PHYS_BASE 0x1F4u

/** SDM Table 2-41. Additional MSRs Supported by the 7th Generation and 8th Generation Intel(R) CoreTM Processors Based */
#define MSR_PRMRR_PHYS_MASK 0x1F5u

/** SDM Table 2-41. Additional MSRs Supported by the 7th Generation and 8th Generation Intel(R) CoreTM Processors Based */
#define MSR_PRMRR_VALID_CONFIG 0x1FBu

/** SDM Table 2-14. MSRs in Intel Atom(R) Processors Based on Tremont Microarchitecture */
#define MSR_PRMRR_BASE_0 0x2A0u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_PRMRR_BASE_1 0x2A1u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_PRMRR_BASE_2 0x2A2u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_PRMRR_BASE_3 0x2A3u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_PRMRR_BASE_4 0x2A4u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_PRMRR_BASE_5 0x2A5u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_PRMRR_BASE_6 0x2A6u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_PRMRR_BASE_7 0x2A7u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_COPY_SBFT_HASHES 0x2B8u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SBFT_HASHES_STATUS 0x2B9u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_AUTHENTICATE_AND_COPY_SBFT_CHUNK 0x2BAu

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SBFT_CHUNKS_AUTHENTICATION_STATUS 0x2BBu

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_ACTIVATE_SBFT 0x2BCu

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SBFT_STATUS 0x2BDu

/** SDM Table 2-56. Additional MSRs        Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SBFT_MODULE_ID 0x2BEu

/** SDM Table 2-56. Additional MSRs  Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SBFTAF_LAST_WP 0x2BFu

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_COPY_SCAN_HASHES 0x2C2u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_SCAN_HASHES_STATUS 0x2C3u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_AUTHENTICATE_AND_COPY_CHUNK 0x2C4u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_CHUNKS_AUTHENTICATION_STATUS 0x2C5u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_ACTIVATE_SCAN 0x2C6u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_SCAN_STATUS 0x2C7u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_SCAN_MODULE_ID 0x2C8u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_LAST_SAF_WP 0x2C9u

/** SDM Table 2-57. Additional MSRs Supported by the Intel(R) Xeon(R) 6 E-Core Processors */
#define MSR_TRIGGER_PERIODIC_MEM_BIST 0x2D6u

/** SDM Table 2-57. Additional MSRs Supported by the Intel(R) Xeon(R) 6 E-Core Processors */
#define MSR_PERIODIC_MEM_BIST_STATUS 0x2D7u

/** SDM Table 2-52. Additional MSRs Supported by the 4th and 5th Generation Intel(R) Xeon(R) Scalable Processor Families */
#define MSR_INTEGRITY_CAPABILITIES 0x2D9u

/** SDM Table 2-41. Additional MSRs Supported by the 7th Generation and 8th Generation Intel(R) CoreTM Processors Based */
#define MSR_UNCORE_PRMRR_PHYS_BASE1 0x2F4u

/** SDM Table 2-41. Additional MSRs Supported by the 7th Generation and 8th Generation Intel(R) CoreTM Processors Based */
#define MSR_UNCORE_PRMRR_PHYS_MASK1 0x2F5u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_COUNTER0 0x300u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_COUNTER1 0x301u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series */
#define MSR_GQ_SNOOP_MESF 0x301u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_COUNTER2 0x302u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_COUNTER3 0x303u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_COUNTER0 0x304u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_COUNTER1 0x305u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_COUNTER2 0x306u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_COUNTER3 0x307u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_COUNTER0 0x308u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_COUNTER1 0x309u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_COUNTER2 0x30Au

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_COUNTER3 0x30Bu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_COUNTER0 0x30Cu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_COUNTER1 0x30Du

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_COUNTER2 0x30Eu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_COUNTER3 0x30Fu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_COUNTER4 0x310u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_COUNTER5 0x311u

/** SDM Table 2-42. Additional MSRs Supported by the 8th Generation Intel(R) CoreTM i3 Processors */
#define MSR_BR_DETECT_CTRL 0x350u

/** SDM Table 2-42. Additional MSRs Supported by the 8th Generation Intel(R) CoreTM i3 Processors */
#define MSR_BR_DETECT_STATUS 0x351u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_CCCR0 0x360u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_CCCR1 0x361u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_CCCR2 0x362u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_CCCR3 0x363u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_CCCR0 0x364u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_CCCR1 0x365u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_CCCR2 0x366u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_CCCR3 0x367u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_CCCR0 0x368u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_CCCR1 0x369u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_CCCR2 0x36Au

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_CCCR3 0x36Bu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_CCCR0 0x36Cu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_CCCR1 0x36Du

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_CCCR2 0x36Eu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_CCCR3 0x36Fu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_CCCR4 0x370u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_CCCR5 0x371u

/** SDM Table 2-16. Additional MSRs in the Intel(R) Xeon(R) Processor 5500 and 3400 Series */
#define MSR_UNCORE_ADDR_OPCODE_MATCH 0x396u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_CONFIG 0x396u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BSU_ESCR0 0x3A0u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BSU_ESCR1 0x3A1u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FSB_ESCR0 0x3A2u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FSB_ESCR1 0x3A3u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FIRM_ESCR0 0x3A4u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FIRM_ESCR1 0x3A5u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_ESCR0 0x3A6u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_FLAME_ESCR1 0x3A7u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_DAC_ESCR0 0x3A8u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_DAC_ESCR1 0x3A9u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MOB_ESCR0 0x3AAu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MOB_ESCR1 0x3ABu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_PMH_ESCR0 0x3ACu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_PMH_ESCR1 0x3ADu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_SAAT_ESCR0 0x3AEu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_SAAT_ESCR1 0x3AFu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_U2L_ESCR0 0x3B0u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors.  OJO: el manual da 946 en decimal, que no es 0x3B0 */
#define MSR_UNC_ARB_PERFCTR0 0x3B0u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_U2L_ESCR1 0x3B1u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors.  OJO: el manual da 947 en decimal, que no es 0x3B1 */
#define MSR_UNC_ARB_PERFCTR1 0x3B1u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_ESCR0 0x3B2u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_BPU_ESCR1 0x3B3u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IS_ESCR0 0x3B4u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IS_ESCR1 0x3B5u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_ITLB_ESCR0 0x3B6u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_ITLB_ESCR1 0x3B7u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_CRU_ESCR0 0x3B8u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_CRU_ESCR1 0x3B9u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_ESCR0 0x3BAu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IQ_ESCR1 0x3BBu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_RAT_ESCR0 0x3BCu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_RAT_ESCR1 0x3BDu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_SSU_ESCR0 0x3BEu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_ESCR0 0x3C0u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MS_ESCR1 0x3C1u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_TBPU_ESCR0 0x3C2u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_TBPU_ESCR1 0x3C3u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_TC_ESCR0 0x3C4u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_TC_ESCR1 0x3C5u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IX_ESCR0 0x3C8u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_IX_ESCR1 0x3C9u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_ALF_ESCR0 0x3CAu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_ALF_ESCR1 0x3CBu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_CRU_ESCR2 0x3CCu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_CRU_ESCR3 0x3CDu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_CRU_ESCR4 0x3E0u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_CRU_ESCR5 0x3E1u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_TC_PRECISE_EVENT 0x3F0u

/** SDM Table 2-20. MSRs Supported by Intel(R) Processors Based on Sandy Bridge Microarchitecture */
#define MSR_CORE_C7_RESIDENCY 0x3FEu

/** SDM Table 2-55. MSRs Supported by the Intel(R) CoreTM Ultra 7 Processor E-core */
#define MSR_SAF_CTRL 0x4F0u

/** SDM Table 2-56. Additional MSRs  Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_SBFT_CTRL 0x4F8u

/** SDM Table 2-47. MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processor P-core */
#define MSR_THREAD_UARCH_CTL 0x540u

/** SDM Table 2-45. Additional MSRs Supported by the 11th Generation Intel(R) CoreTM Processors Based on Tiger Lake */
#define MSR_VR_CURRENT_CONFIG 0x601u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_PKGC3_IRTL 0x60Au

/** SDM Table 2-20. MSRs Supported by Intel(R) Processors Based on Sandy Bridge Microarchitecture */
#define MSR_PKGC6_IRTL 0x60Bu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_PKGC_IRTL1 0x60Bu

/** SDM Table 2-21. MSRs Supported by the 2nd Generation Intel(R) CoreTM Processors (Sandy Bridge Microarchitecture) */
#define MSR_PKGC7_IRTL 0x60Cu

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_PKGC_IRTL2 0x60Cu

/** SDM Table 2-32. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_PCIE_PLL_RATIO 0x61Eu

/** SDM Table 2-41. Additional MSRs Supported by the 7th Generation and 8th Generation Intel(R) CoreTM Processors Based */
#define MSR_RING_RATIO_LIMIT 0x620u

/** SDM Table 2-32. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_UNCORE_RATIO_LIMIT 0x620u

/** SDM Table 2-21. MSRs Supported by the 2nd Generation Intel(R) CoreTM Processors (Sandy Bridge Microarchitecture) */
#define MSR_PP0_POLICY 0x63Au

/** SDM Table 2-21. MSRs Supported by the 2nd Generation Intel(R) CoreTM Processors (Sandy Bridge Microarchitecture) */
#define MSR_PP1_POLICY 0x642u

/** SDM Table 2-25. Additional MSRs Supported by 3rd Generation Intel(R) CoreTM Processors Based on Ivy Bridge */
#define MSR_CONFIG_TDP_NOMINAL 0x648u

/** SDM Table 2-25. Additional MSRs Supported by 3rd Generation Intel(R) CoreTM Processors Based on Ivy Bridge */
#define MSR_CONFIG_TDP_LEVEL1 0x649u

/** SDM Table 2-25. Additional MSRs Supported by 3rd Generation Intel(R) CoreTM Processors Based on Ivy Bridge */
#define MSR_CONFIG_TDP_LEVEL2 0x64Au

/** SDM Table 2-25. Additional MSRs Supported by 3rd Generation Intel(R) CoreTM Processors Based on Ivy Bridge */
#define MSR_CONFIG_TDP_CONTROL 0x64Bu

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_CORE_HDC_RESIDENCY 0x653u

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture).  PUERTA: CPUID.06H:EAX[18] */
#define MSR_FAST_UNCORE_MSRS_CTL 0x657u
#define MSR_FAST_UNCORE_MSRS_CTL_GATE_LEAF 0x6u
#define MSR_FAST_UNCORE_MSRS_CTL_GATE_REG 0u /* EAX */
#define MSR_FAST_UNCORE_MSRS_CTL_GATE_BIT 18u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_WEIGHTED_CORE_C0 0x658u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_ANY_CORE_C0 0x659u

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_ANY_GFXE_C0 0x65Au

/** SDM Table 2-39. Additional MSRs Supported by the 6th--13th Generation Intel(R) CoreTM Processors, */
#define MSR_CORE_GFXE_OVERLAP_C0 0x65Bu

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_FAST_UNCORE_MSRS_STATUS 0x65Eu

/** SDM Table 2-44. MSRs Supported by the 10th Generation Intel(R) CoreTM Processors (Ice Lake Microarchitecture) */
#define MSR_FAST_UNCORE_MSRS_CAPABILITY 0x65Fu

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define MSR_CORE_C1_RESIDENCY 0x660u

/** SDM Table 2-9. Specific MSRs Supported by the Intel Atom(R) Processor E3000 Series with a CPUID Signature */
#define MSR_CC6_DEMOTION_POLICY_CONFIG 0x668u

/** SDM Table 2-30. MSRs Supported by 4th Generation Intel(R) CoreTM Processors (Haswell Microarchitecture) */
#define MSR_GRAPHICS_PERF_LIMIT_REASONS 0x6B0u

/** SDM Table 2-30. MSRs Supported by 4th Generation Intel(R) CoreTM Processors (Haswell Microarchitecture) */
#define MSR_RING_PERF_LIMIT_REASONS 0x6B1u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_0_UNIT_STATUS 0x705u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_0_PERFCTR2 0x708u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_0_PERFCTR3 0x709u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_1_UNIT_STATUS 0x715u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_1_PERFCTR2 0x718u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_1_PERFCTR3 0x719u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S0_PMON_BOX_CTL 0x720u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S0_PMON_EVNTSEL0 0x721u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S0_PMON_EVNTSEL1 0x722u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S0_PMON_EVNTSEL2 0x723u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S0_PMON_EVNTSEL3 0x724u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S0_PMON_BOX_FILTER 0x725u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_2_UNIT_STATUS 0x725u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S1_PMON_BOX_CTL 0x72Au

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S1_PMON_EVNTSEL0 0x72Bu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S1_PMON_EVNTSEL1 0x72Cu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S1_PMON_EVNTSEL2 0x72Du

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S1_PMON_EVNTSEL3 0x72Eu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S1_PMON_BOX_FILTER 0x72Fu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_BOX_CTL 0x734u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_EVNTSEL0 0x735u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_3_UNIT_STATUS 0x735u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_EVNTSEL1 0x736u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_EVNTSEL2 0x737u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_EVNTSEL3 0x738u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_BOX_FILTER 0x739u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_CTR0 0x73Au

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_CTR1 0x73Bu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_CTR2 0x73Cu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S2_PMON_CTR3 0x73Du

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_BOX_CTL 0x73Eu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_EVNTSEL0 0x73Fu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_EVNTSEL1 0x740u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_EVNTSEL2 0x741u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_EVNTSEL3 0x742u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_BOX_FILTER 0x743u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_CTR0 0x744u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_CTR1 0x745u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_4_UNIT_STATUS 0x745u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_CTR2 0x746u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_S3_PMON_CTR3 0x747u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_4_PERFCTR2 0x748u

/** SDM Table 2-22. Uncore PMU MSRs Supported by 2nd Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_4_PERFCTR3 0x749u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_U_PMON_GLOBAL_CTRL 0xC00u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_U_PMON_GLOBAL_STATUS 0xC01u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_U_PMON_GLOBAL_OVF_CTRL 0xC02u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_U_PMON_EVNT_SEL 0xC10u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_U_PMON_CTR 0xC11u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_BOX_CTRL 0xC20u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_BOX_STATUS 0xC21u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_BOX_OVF_CTRL 0xC22u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_EVNT_SEL0 0xC30u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_CTR0 0xC31u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_EVNT_SEL1 0xC32u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_CTR1 0xC33u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_EVNT_SEL2 0xC34u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_CTR2 0xC35u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_EVNT_SEL3 0xC36u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_CTR3 0xC37u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_BOX_CTRL 0xC40u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_BOX_STATUS 0xC41u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_BOX_OVF_CTRL 0xC42u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_EVNT_SEL0 0xC50u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_EVNT_SEL1 0xC52u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_EVNT_SEL2 0xC54u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_EVNT_SEL3 0xC56u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_BOX_CTRL 0xC60u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_BOX_STATUS 0xC61u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_BOX_OVF_CTRL 0xC62u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_EVNT_SEL0 0xC70u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_CTR0 0xC71u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_EVNT_SEL1 0xC72u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_CTR1 0xC73u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_EVNT_SEL2 0xC74u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_CTR2 0xC75u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_EVNT_SEL3 0xC76u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_CTR3 0xC77u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3120 en decimal, que no es 0xC80 */
#define MSR_W_PMON_BOX_CTRL 0xC80u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3121 en decimal, que no es 0xC81 */
#define MSR_W_PMON_BOX_STATUS 0xC81u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3122 en decimal, que no es 0xC82 */
#define MSR_W_PMON_BOX_OVF_CTRL 0xC82u

/** SDM Table 2-56. Additional MSRs Supported by the Intel(R) Xeon(R) 6 P-Core Processors */
#define MSR_MBA_CFG 0xC84u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3136 en decimal, que no es 0xC90 */
#define MSR_W_PMON_EVNT_SEL0 0xC90u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3137 en decimal, que no es 0xC91 */
#define MSR_W_PMON_CTR0 0xC91u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3138 en decimal, que no es 0xC92 */
#define MSR_W_PMON_EVNT_SEL1 0xC92u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3139 en decimal, que no es 0xC93 */
#define MSR_W_PMON_CTR1 0xC93u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3140 en decimal, que no es 0xC94 */
#define MSR_W_PMON_EVNT_SEL2 0xC94u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3141 en decimal, que no es 0xC95 */
#define MSR_W_PMON_CTR2 0xC95u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3142 en decimal, que no es 0xC96 */
#define MSR_W_PMON_EVNT_SEL3 0xC96u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series.  OJO: el manual da 3143 en decimal, que no es 0xC97 */
#define MSR_W_PMON_CTR3 0xC97u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_BOX_CTRL 0xCA0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_BOX_STATUS 0xCA1u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_BOX_OVF_CTRL 0xCA2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_TIMESTAMP 0xCA4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_DSP 0xCA5u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_ISS 0xCA6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_MAP 0xCA7u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_MSC_THR 0xCA8u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_PGT 0xCA9u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_PLD 0xCAAu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_ZDP 0xCABu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_EVNT_SEL0 0xCB0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_CTR0 0xCB1u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_EVNT_SEL1 0xCB2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_CTR1 0xCB3u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_EVNT_SEL2 0xCB4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_CTR2 0xCB5u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_EVNT_SEL3 0xCB6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_CTR3 0xCB7u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_EVNT_SEL4 0xCB8u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_CTR4 0xCB9u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_EVNT_SEL5 0xCBAu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_CTR5 0xCBBu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_BOX_CTRL 0xCC0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_BOX_STATUS 0xCC1u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_BOX_OVF_CTRL 0xCC2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_EVNT_SEL0 0xCD0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_EVNT_SEL1 0xCD2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_EVNT_SEL2 0xCD4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_EVNT_SEL3 0xCD6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_BOX_CTRL 0xCE0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_BOX_STATUS 0xCE1u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_BOX_OVF_CTRL 0xCE2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_TIMESTAMP 0xCE4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_DSP 0xCE5u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_ISS 0xCE6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_MAP 0xCE7u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_MSC_THR 0xCE8u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_PGT 0xCE9u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_PLD 0xCEAu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_ZDP 0xCEBu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_EVNT_SEL0 0xCF0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_CTR0 0xCF1u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_EVNT_SEL1 0xCF2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_CTR1 0xCF3u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_EVNT_SEL2 0xCF4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_CTR2 0xCF5u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_EVNT_SEL3 0xCF6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_CTR3 0xCF7u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_EVNT_SEL4 0xCF8u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_CTR4 0xCF9u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_EVNT_SEL5 0xCFAu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_CTR5 0xCFBu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_BOX_CTRL 0xD00u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_BOX_OVF_CTRL 0xD02u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_EVNT_SEL0 0xD10u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_EVNT_SEL1 0xD12u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C0_PMON_BOX_FILTER 0xD14u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_EVNT_SEL2 0xD14u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_EVNT_SEL3 0xD16u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_EVNT_SEL4 0xD18u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_CTR4 0xD19u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_EVNT_SEL5 0xD1Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C0_PMON_CTR5 0xD1Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_BOX_CTRL 0xD20u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_BOX_OVF_CTRL 0xD22u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_EVNT_SEL0 0xD30u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_EVNT_SEL1 0xD32u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C1_PMON_BOX_FILTER 0xD34u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_EVNT_SEL2 0xD34u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_EVNT_SEL3 0xD36u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_EVNT_SEL4 0xD38u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_CTR4 0xD39u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_EVNT_SEL5 0xD3Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C4_PMON_CTR5 0xD3Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_BOX_CTRL 0xD40u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_BOX_OVF_CTRL 0xD42u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_EVNT_SEL0 0xD50u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_EVNT_SEL1 0xD52u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C2_PMON_BOX_FILTER 0xD54u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_EVNT_SEL2 0xD54u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_EVNT_SEL3 0xD56u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_EVNT_SEL4 0xD58u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_CTR4 0xD59u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_EVNT_SEL5 0xD5Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C2_PMON_CTR5 0xD5Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_BOX_CTRL 0xD60u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_BOX_OVF_CTRL 0xD62u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_EVNT_SEL0 0xD70u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_EVNT_SEL1 0xD72u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C3_PMON_BOX_FILTER 0xD74u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_EVNT_SEL2 0xD74u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_EVNT_SEL3 0xD76u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_EVNT_SEL4 0xD78u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_CTR4 0xD79u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_EVNT_SEL5 0xD7Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C6_PMON_CTR5 0xD7Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_BOX_CTRL 0xD80u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_BOX_OVF_CTRL 0xD82u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_EVNT_SEL0 0xD90u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_EVNT_SEL1 0xD92u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_EVNT_SEL2 0xD94u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C4_PMON_BOX_FILTER 0xD94u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_EVNT_SEL3 0xD96u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_EVNT_SEL4 0xD98u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_CTR4 0xD99u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_EVNT_SEL5 0xD9Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C1_PMON_CTR5 0xD9Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_BOX_CTRL 0xDA0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_BOX_OVF_CTRL 0xDA2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_EVNT_SEL0 0xDB0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_EVNT_SEL1 0xDB2u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C5_PMON_BOX_FILTER 0xDB4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_EVNT_SEL2 0xDB4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_EVNT_SEL3 0xDB6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_EVNT_SEL4 0xDB8u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_CTR4 0xDB9u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_EVNT_SEL5 0xDBAu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C5_PMON_CTR5 0xDBBu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_BOX_CTRL 0xDC0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_BOX_OVF_CTRL 0xDC2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_EVNT_SEL0 0xDD0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_EVNT_SEL1 0xDD2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_EVNT_SEL2 0xDD4u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C6_PMON_BOX_FILTER 0xDD4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_EVNT_SEL3 0xDD6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_EVNT_SEL4 0xDD8u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_CTR4 0xDD9u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_EVNT_SEL5 0xDDAu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C3_PMON_CTR5 0xDDBu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_BOX_CTRL 0xDE0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_BOX_OVF_CTRL 0xDE2u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_EVNT_SEL0 0xDF0u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_EVNT_SEL1 0xDF2u

/** SDM Table 2-24. Uncore PMU MSRs in Intel(R) Xeon(R) Processor E5 Family */
#define MSR_C7_PMON_BOX_FILTER 0xDF4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_EVNT_SEL2 0xDF4u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_EVNT_SEL3 0xDF6u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_EVNT_SEL4 0xDF8u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_CTR4 0xDF9u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_EVNT_SEL5 0xDFAu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_C7_PMON_CTR5 0xDFBu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_BOX_CTRL 0xE00u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_BOX_STATUS 0xE01u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_BOX_OVF_CTRL 0xE02u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P0 0xE04u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C0_PMON_BOX_FILTER0 0xE05u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P1 0xE05u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P2 0xE06u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P3 0xE07u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P4 0xE08u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P5 0xE09u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P6 0xE0Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_IPERF0_P7 0xE0Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_QLX_P0 0xE0Cu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_QLX_P1 0xE0Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_QLX_P2 0xE0Eu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_QLX_P3 0xE0Fu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL0 0xE10u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR0 0xE11u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL1 0xE12u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR1 0xE13u

/** SDM Table 2-28. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v2 and                      E7 v2 Families */
#define MSR_C8_PMON_BOX_FILTER 0xE14u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL2 0xE14u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C1_PMON_BOX_FILTER0 0xE15u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR2 0xE15u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL3 0xE16u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR3 0xE17u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL4 0xE18u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR4 0xE19u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL5 0xE1Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR5 0xE1Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL6 0xE1Cu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR6 0xE1Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_EVNT_SEL7 0xE1Eu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R0_PMON_CTR7 0xE1Fu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_BOX_CTRL 0xE20u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_BOX_STATUS 0xE21u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_BOX_OVF_CTRL 0xE22u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P8 0xE24u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C2_PMON_BOX_FILTER0 0xE25u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P9 0xE25u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P10 0xE26u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P11 0xE27u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P12 0xE28u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P13 0xE29u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P14 0xE2Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_IPERF1_P15 0xE2Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_QLX_P4 0xE2Cu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_QLX_P5 0xE2Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_QLX_P6 0xE2Eu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_QLX_P7 0xE2Fu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL8 0xE30u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR8 0xE31u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL9 0xE32u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR9 0xE33u

/** SDM Table 2-28. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v2 and                      E7 v2 Families */
#define MSR_C9_PMON_BOX_FILTER 0xE34u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL10 0xE34u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C3_PMON_BOX_FILTER0 0xE35u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR10 0xE35u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL11 0xE36u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR11 0xE37u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL12 0xE38u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR12 0xE39u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL13 0xE3Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR13 0xE3Bu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL14 0xE3Cu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR14 0xE3Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_EVNT_SEL15 0xE3Eu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_R1_PMON_CTR15 0xE3Fu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_MATCH 0xE45u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C4_PMON_BOX_FILTER0 0xE45u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B0_PMON_MASK 0xE46u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_MATCH 0xE49u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S0_PMON_MASK 0xE4Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_MATCH 0xE4Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_B1_PMON_MASK 0xE4Eu

/** SDM Table 2-28. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v2 and       E7 v2 Families */
#define MSR_C10_PMON_BOX_FILTER 0xE54u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_MM_CONFIG 0xE54u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C5_PMON_BOX_FILTER0 0xE55u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_ADDR_MATCH 0xE55u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M0_PMON_ADDR_MASK 0xE56u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_MATCH 0xE59u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_S1_PMON_MASK 0xE5Au

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_MM_CONFIG 0xE5Cu

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_ADDR_MATCH 0xE5Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define MSR_M1_PMON_ADDR_MASK 0xE5Eu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C6_PMON_BOX_FILTER0 0xE65u

/** SDM Table 2-28. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v2 and       E7 v2 Families */
#define MSR_C11_PMON_BOX_FILTER 0xE74u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C7_PMON_BOX_FILTER0 0xE75u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C8_PMON_BOX_FILTER0 0xE85u

/** SDM Table 2-28. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v2 and                     E7 v2 Families */
#define MSR_C12_PMON_BOX_FILTER 0xE94u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C9_PMON_BOX_FILTER0 0xE95u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C10_PMON_BOX_FILTER0 0xEA5u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C10_PMON_BOX_STATUS 0xEA7u

/** SDM Table 2-28. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v2 and       E7 v2 Families */
#define MSR_C13_PMON_BOX_FILTER 0xEB4u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C11_PMON_BOX_FILTER0 0xEB5u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C11_PMON_BOX_STATUS 0xEB7u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C12_PMON_BOX_FILTER0 0xEC5u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C12_PMON_BOX_STATUS 0xEC7u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C13_PMON_BOX_FILTER0 0xED5u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C13_PMON_BOX_STATUS 0xED7u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C14_PMON_BOX_STATUS 0xEE7u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_BOX_CTL 0xEF0u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_EVNTSEL0 0xEF1u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_EVNTSEL1 0xEF2u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_EVNTSEL2 0xEF3u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_EVNTSEL3 0xEF4u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_BOX_FILTER0 0xEF5u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_BOX_FILTER1 0xEF6u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_BOX_STATUS 0xEF7u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_CTR0 0xEF8u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_CTR1 0xEF9u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_CTR2 0xEFAu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C15_PMON_CTR3 0xEFBu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_BOX_CTL 0xF00u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_EVNTSEL0 0xF01u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_EVNTSEL1 0xF02u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_EVNTSEL2 0xF03u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_EVNTSEL3 0xF04u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_BOX_FILTER0 0xF05u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_BOX_FILTER1 0xF06u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_BOX_STATUS 0xF07u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_CTR0 0xF08u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_CTR1 0xF09u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_CTR2 0xF0Au

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C16_PMON_CTR3 0xF0Bu

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_BOX_CTL 0xF10u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_EVNTSEL0 0xF11u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_EVNTSEL1 0xF12u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_EVNTSEL2 0xF13u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_EVNTSEL3 0xF14u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_BOX_FILTER0 0xF15u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_BOX_FILTER1 0xF16u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_BOX_STATUS 0xF17u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_CTR0 0xF18u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_CTR1 0xF19u

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_CTR2 0xF1Au

/** SDM Table 2-33. Uncore PMU MSRs in the Intel(R) Xeon(R) Processor E5 v3 Family */
#define MSR_C17_PMON_CTR3 0xF1Bu

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_BOX_CTRL 0xF40u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_BOX_OVF_CTRL 0xF42u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_EVNT_SEL0 0xF50u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_EVNT_SEL1 0xF52u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_EVNT_SEL2 0xF54u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_EVNT_SEL3 0xF56u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_EVNT_SEL4 0xF58u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_CTR4 0xF59u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_EVNT_SEL5 0xF5Au

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C8_PMON_CTR5 0xF5Bu

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_BOX_CTRL 0xFC0u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_BOX_OVF_CTRL 0xFC2u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_EVNT_SEL0 0xFD0u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_EVNT_SEL1 0xFD2u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_EVNT_SEL2 0xFD4u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_EVNT_SEL3 0xFD6u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_EVNT_SEL4 0xFD8u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_CTR4 0xFD9u

/** SDM Table 2-19. Additional MSRs Supported by the Intel(R) Xeon(R) Processor E7 Family */
#define MSR_C9_PMON_EVNT_SEL5 0xFDAu

/** SDM Table 2-19. Additional MSRs Supported by            the Intel(R) Xeon(R) Processor E7   Family */
#define MSR_C9_PMON_CTR5 0xFDBu

/** SDM Table 2-60. MSRs Supported  by  the Intel(R) Series 2 CoreTM Ultra Processor E-core */
#define MSR_WORK_CONSERVING_CLOS 0x1878u

/** SDM Table 2-55. MSRs Supported by the Intel(R) CoreTM Ultra 7 Processor E-core */
#define MSR_STLB_FILL_TRANSLATION 0x1A8Eu

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_8_PERFCTR0 0x2042u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_8_PERFCTR1 0x2043u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R) CoreTM Processors */
#define MSR_UNC_CBO_9_PERFCTR0 0x204Au

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_CBO_9_PERFCTR1 0x204Bu

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_0_PERFCTR0 0x2FD2u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_0_PERFCTR1 0x2FD3u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_0_PERF_CTRL 0x2FD5u

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_1_PERFCTR0 0x2FDAu

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_1_PERFCTR1 0x2FDBu

/** SDM Table 2-49. Uncore PMU MSRs Supported by 12th and 13th Generation Intel(R)          CoreTM  Processors */
#define MSR_UNC_ARB_1_PERF_CTRL 0x2FDDu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL0 0x107CCu

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_IFSB_BUSQ0 0x107CCu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL1 0x107CDu

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_IFSB_BUSQ1 0x107CDu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL2 0x107CEu

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_IFSB_SNPQ0 0x107CEu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL3 0x107CFu

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_IFSB_SNPQ1 0x107CFu

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_EFSB_DRDY0 0x107D0u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL4 0x107D0u

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_EFSB_DRDY1 0x107D1u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL5 0x107D1u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL6 0x107D2u

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_IFSB_CTL6 0x107D2u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_CTR_CTL7 0x107D3u

/** SDM Table 2-64. MSRs Unique to 64-bit Intel(R) Xeon(R) Processor             MP with Up to an    8  MB L3 Cache */
#define MSR_IFSB_CNTR7 0x107D3u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_EMON_L3_GL_CTL 0x107D8u


#endif /* VXP_COMMON_MSR_LEGACY_H */
