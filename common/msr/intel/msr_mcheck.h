/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_mcheck.h
 * @brief Comprobacion de maquina
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

#ifndef VXP_COMMON_MSR_MCHECK_H
#define VXP_COMMON_MSR_MCHECK_H

/** antes `MCG_CAP`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCG_CAP 0x179u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MCG_CAP 0x179u

/** antes `MCG_STATUS`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCG_STATUS 0x17Au

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MCG_STATUS 0x17Au

/** antes `MCG_CTL`.  SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCG_CTL 0x17Bu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MCG_CTL 0x17Bu

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RAX 0x180u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RBX 0x181u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RCX 0x182u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RDX 0x183u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RSI 0x184u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RDI 0x185u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RBP 0x186u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RSP 0x187u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RFLAGS 0x188u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_RIP 0x189u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_MISC 0x18Au

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R8 0x190u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R9 0x191u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R10 0x192u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R11 0x193u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R12 0x194u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R13 0x195u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R14 0x196u

/** SDM Table 2-63. MSRs in the Pentium(R) 4 and Intel(R) Xeon(R) Processors */
#define MSR_MCG_R15 0x197u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC0_CTL2 0x280u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC1_CTL2 0x281u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC2_CTL2 0x282u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC3_CTL2 0x283u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC4_CTL2 0x284u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC5_CTL2 0x285u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC6_CTL2 0x286u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC7_CTL2 0x287u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC8_CTL2 0x288u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC9_CTL2 0x289u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC10_CTL2 0x28Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC11_CTL2 0x28Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC12_CTL2 0x28Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC13_CTL2 0x28Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC14_CTL2 0x28Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC15_CTL2 0x28Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC16_CTL2 0x290u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC17_CTL2 0x291u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC18_CTL2 0x292u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC19_CTL2 0x293u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC20_CTL2 0x294u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC21_CTL2 0x295u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC22_CTL2 0x296u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC23_CTL2 0x297u

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E  Microarchitecture) */
#define IA32_MC23_CTL2IA32_MC23_CTL2 0x297u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC24_CTL2 0x298u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC25_CTL2 0x299u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC26_CTL2 0x29Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC27_CTL2 0x29Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC28_CTL2 0x29Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC29_CTL2 0x29Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC30_CTL2 0x29Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC31_CTL2 0x29Fu

/** SDM Table 2-61. Selected MSRs Supported by Intel(R) Xeon PhiTM Processors with a CPUID                Signature */
#define MSR_MC0_RESIDENCY 0x3FCu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC0_CTL 0x400u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC0_CTL 0x400u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC0_STATUS 0x401u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC0_STATUS 0x401u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define IA32_MC0_ADDR 0x402u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC0_ADDR1 0x402u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC0_ADDR 0x402u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC0_MISC 0x403u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC0_MISC 0x403u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC1_CTL 0x404u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC1_CTL 0x404u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC1_STATUS 0x405u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC1_STATUS 0x405u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define IA32_MC1_ADDR 0x406u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC1_ADDR2 0x406u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC1_ADDR 0x406u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC1_MISC 0x407u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC1_MISC 0x407u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC2_CTL 0x408u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC2_CTL 0x408u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC2_STATUS 0x409u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC2_STATUS 0x409u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define IA32_MC2_ADDR 0x40Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC2_ADDR1 0x40Au

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC2_ADDR 0x40Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC2_MISC 0x40Bu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC2_MISC 0x40Bu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC4_CTL 0x40Cu

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC4_CTL 0x40Cu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC4_STATUS 0x40Du

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC4_STATUS 0x40Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC3_ADDR1 0x40Eu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC4_ADDR 0x40Eu

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC4_ADDR 0x40Eu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC4_MISC 0x40Fu

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC3_CTL 0x410u

/** SDM Table 2-67. MSRs in Pentium M Processors */
#define MSR_MC3_CTL 0x410u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC3_STATUS 0x411u

/** SDM Table 2-67. MSRs in Pentium M Processors */
#define MSR_MC3_STATUS 0x411u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC4_ADDR1 0x412u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC3_ADDR 0x412u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC3_ADDR 0x412u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC4_MISC 0x413u

/** SDM Table 2-68. MSRs in the P6 Family Processors */
#define MC3_MISC 0x413u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC3_MISC 0x413u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC5_CTL 0x414u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC5_CTL 0x414u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC5_STATUS 0x415u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC5_STATUS 0x415u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define IA32_MC5_ADDR 0x416u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC5_ADDR1 0x416u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC5_ADDR 0x416u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC5_MISC 0x417u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV */
#define MSR_MC5_MISC 0x417u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC6_CTL 0x418u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC6_STATUS 0x419u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define IA32_MC6_ADDR 0x41Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC6_ADDR1 0x41Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC6_MISC 0x41Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC7_CTL 0x41Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC7_STATUS 0x41Du

/** SDM Table 2-15. MSRs in Processors Based on Nehalem Microarchitecture */
#define IA32_MC7_ADDR 0x41Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC7_ADDR1 0x41Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC7_MISC 0x41Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC8_CTL 0x420u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC8_STATUS 0x421u

/** SDM Table 2-15. MSRs in Processors Based on Nehalem Microarchitecture */
#define IA32_MC8_ADDR 0x422u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC8_ADDR1 0x422u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC8_MISC 0x423u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC9_CTL 0x424u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC9_STATUS 0x425u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC9_ADDR 0x426u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC9_ADDR1 0x426u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC9_MISC 0x427u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC10_CTL 0x428u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC10_STATUS 0x429u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC10_ADDR 0x42Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC10_ADDR1 0x42Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC10_MISC 0x42Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC11_CTL 0x42Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC11_STATUS 0x42Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC11_ADDR 0x42Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC11_ADDR1 0x42Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC11_MISC 0x42Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC12_CTL 0x430u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC12_STATUS 0x431u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC12_ADDR 0x432u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC12_ADDR1 0x432u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC12_MISC 0x433u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC13_CTL 0x434u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC13_STATUS 0x435u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC13_ADDR 0x436u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC13_ADDR1 0x436u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC13_MISC 0x437u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC14_CTL 0x438u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC14_STATUS 0x439u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC14_ADDR 0x43Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC14_ADDR1 0x43Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC14_MISC 0x43Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC15_CTL 0x43Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC15_STATUS 0x43Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC15_ADDR 0x43Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC15_ADDR1 0x43Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC15_MISC 0x43Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC16_CTL 0x440u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC16_STATUS 0x441u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC16_ADDR 0x442u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC16_ADDR1 0x442u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC16_MISC 0x443u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC17_CTL 0x444u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC17_STATUS 0x445u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC17_ADDR 0x446u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC17_ADDR1 0x446u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC17_MISC 0x447u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC18_CTL 0x448u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC18_STATUS 0x449u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC18_ADDR 0x44Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC18_ADDR1 0x44Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC18_MISC 0x44Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC19_CTL 0x44Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC19_STATUS 0x44Du

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC19_ADDR 0x44Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC19_ADDR1 0x44Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC19_MISC 0x44Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC20_CTL 0x450u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC20_STATUS 0x451u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC20_ADDR 0x452u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC20_ADDR1 0x452u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC20_MISC 0x453u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC21_CTL 0x454u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC21_STATUS 0x455u

/** SDM Table 2-17. Additional MSRs in the Intel(R) Xeon(R) Processor 7500 Series */
#define IA32_MC21_ADDR 0x456u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC21_ADDR1 0x456u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC21_MISC 0x457u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC22_CTL 0x458u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC22_STATUS 0x459u

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E          Microarchitecture) */
#define IA32_MC22_ADDR 0x45Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC22_ADDR1 0x45Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC22_MISC 0x45Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC23_CTL 0x45Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC23_STATUS 0x45Du

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E          Microarchitecture) */
#define IA32_MC23_ADDR 0x45Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC23_ADDR1 0x45Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC23_MISC 0x45Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC24_CTL 0x460u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC24_STATUS 0x461u

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E          Microarchitecture) */
#define IA32_MC24_ADDR 0x462u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC24_ADDR1 0x462u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC24_MISC 0x463u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC25_CTL 0x464u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC25_STATUS 0x465u

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E Microarchitecture) */
#define IA32_MC25_ADDR 0x466u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC25_ADDR1 0x466u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC25_MISC 0x467u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC26_CTL 0x468u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC26_STATUS 0x469u

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E Microarchitecture) */
#define IA32_MC26_ADDR 0x46Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC26_ADDR1 0x46Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC26_MISC 0x46Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC27_CTL 0x46Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC27_STATUS 0x46Du

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E Microarchitecture) */
#define IA32_MC27_ADDR 0x46Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC27_ADDR1 0x46Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC27_MISC 0x46Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC28_CTL 0x470u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC28_STATUS 0x471u

/** SDM Table 2-26. MSRs Supported by the Intel(R) Xeon(R) Processor E5 v2 Product Family (Ivy Bridge-E Microarchitecture) */
#define IA32_MC28_ADDR 0x472u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC28_ADDR1 0x472u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC28_MISC 0x473u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC29_CTL 0x474u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC29_STATUS 0x475u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC29_ADDR 0x476u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC29_MISC 0x477u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC30_CTL 0x478u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC30_STATUS 0x479u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC30_ADDR 0x47Au

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC30_MISC 0x47Bu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC31_CTL 0x47Cu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC31_STATUS 0x47Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC31_ADDR 0x47Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MC31_MISC 0x47Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_MCG_EXT_CTL 0x4D0u

/** SDM Table 2-7. MSRs Common to the Silvermont and Airmont Microarchitectures */
#define MSR_MC6_RESIDENCY_COUNTER 0x664u

/** SDM Table 2-9. Specific MSRs Supported by the Intel Atom(R) Processor E3000 Series with a CPUID Signature */
#define MSR_MC6_DEMOTION_POLICY_CONFIG 0x669u


#endif /* VXP_COMMON_MSR_MCHECK_H */
