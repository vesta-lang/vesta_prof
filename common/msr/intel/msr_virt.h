/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_virt.h
 * @brief Virtualizacion y modo de gestion del sistema
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

#ifndef VXP_COMMON_MSR_VIRT_H
#define VXP_COMMON_MSR_VIRT_H

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_SMM_MONITOR_CTL 0x9Bu
#define IA32_SMM_MONITOR_CTL_GATE_LEAF 0x1u
#define IA32_SMM_MONITOR_CTL_GATE_REG 2u /* ECX */
#define IA32_SMM_MONITOR_CTL_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SMBASE 0x9Eu

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_SMRR_PHYSBASE 0xA0u

/** SDM Table 2-3. MSRs in Processors Based on Intel(R) CoreTM Microarchitecture */
#define MSR_SMRR_PHYSMASK 0xA1u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_SMM_MCA_CAP 0x17Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SMRR_PHYSBASE 0x1F2u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SMRR_PHYSMASK 0x1F3u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_BASIC 0x480u
#define IA32_VMX_BASIC_GATE_LEAF 0x1u
#define IA32_VMX_BASIC_GATE_REG 2u /* ECX */
#define IA32_VMX_BASIC_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_PINBASED_CTLS 0x481u
#define IA32_VMX_PINBASED_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_PINBASED_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_PINBASED_CTLS_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_PROCBASED_CTLS 0x482u
#define IA32_VMX_PROCBASED_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_PROCBASED_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_PROCBASED_CTLS_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_EXIT_CTLS 0x483u
#define IA32_VMX_EXIT_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_EXIT_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_EXIT_CTLS_GATE_BIT 5u

/** SDM Table 2-66. MSRs in Intel(R) CoreTM Solo, Intel(R) CoreTM Duo Processors, and Dual-Core Intel(R) Xeon(R) Processor LV.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_ENTRY_CTLS 0x484u
#define IA32_VMX_ENTRY_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_ENTRY_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_ENTRY_CTLS_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_MISC 0x485u
#define IA32_VMX_MISC_GATE_LEAF 0x1u
#define IA32_VMX_MISC_GATE_REG 2u /* ECX */
#define IA32_VMX_MISC_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_CR0_FIXED0 0x486u
#define IA32_VMX_CR0_FIXED0_GATE_LEAF 0x1u
#define IA32_VMX_CR0_FIXED0_GATE_REG 2u /* ECX */
#define IA32_VMX_CR0_FIXED0_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_CR0_FIXED1 0x487u
#define IA32_VMX_CR0_FIXED1_GATE_LEAF 0x1u
#define IA32_VMX_CR0_FIXED1_GATE_REG 2u /* ECX */
#define IA32_VMX_CR0_FIXED1_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_CR4_FIXED0 0x488u
#define IA32_VMX_CR4_FIXED0_GATE_LEAF 0x1u
#define IA32_VMX_CR4_FIXED0_GATE_REG 2u /* ECX */
#define IA32_VMX_CR4_FIXED0_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_CR4_FIXED1 0x489u
#define IA32_VMX_CR4_FIXED1_GATE_LEAF 0x1u
#define IA32_VMX_CR4_FIXED1_GATE_REG 2u /* ECX */
#define IA32_VMX_CR4_FIXED1_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_VMCS_ENUM 0x48Au
#define IA32_VMX_VMCS_ENUM_GATE_LEAF 0x1u
#define IA32_VMX_VMCS_ENUM_GATE_REG 2u /* ECX */
#define IA32_VMX_VMCS_ENUM_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_PROCBASED_CTLS2 0x48Bu
#define IA32_VMX_PROCBASED_CTLS2_GATE_LEAF 0x1u
#define IA32_VMX_PROCBASED_CTLS2_GATE_REG 2u /* ECX */
#define IA32_VMX_PROCBASED_CTLS2_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_EPT_VPID_CAP 0x48Cu
#define IA32_VMX_EPT_VPID_CAP_GATE_LEAF 0x1u
#define IA32_VMX_EPT_VPID_CAP_GATE_REG 2u /* ECX */
#define IA32_VMX_EPT_VPID_CAP_GATE_BIT 5u

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define IA32_VMX_EPT_VPID_ENUM 0x48Cu

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_TRUE_PINBASED_CTLS 0x48Du
#define IA32_VMX_TRUE_PINBASED_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_TRUE_PINBASED_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_TRUE_PINBASED_CTLS_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_TRUE_PROCBASED_CTLS 0x48Eu
#define IA32_VMX_TRUE_PROCBASED_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_TRUE_PROCBASED_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_TRUE_PROCBASED_CTLS_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_TRUE_EXIT_CTLS 0x48Fu
#define IA32_VMX_TRUE_EXIT_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_TRUE_EXIT_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_TRUE_EXIT_CTLS_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_TRUE_ENTRY_CTLS 0x490u
#define IA32_VMX_TRUE_ENTRY_CTLS_GATE_LEAF 0x1u
#define IA32_VMX_TRUE_ENTRY_CTLS_GATE_REG 2u /* ECX */
#define IA32_VMX_TRUE_ENTRY_CTLS_GATE_BIT 5u

/** SDM Table 2-6. MSRs Common to Intel Atom(R) Processors (Silvermont and Newer Microarchitectures) */
#define IA32_VMX_FMFUNC 0x491u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_VMFUNC 0x491u
#define IA32_VMX_VMFUNC_GATE_LEAF 0x1u
#define IA32_VMX_VMFUNC_GATE_REG 2u /* ECX */
#define IA32_VMX_VMFUNC_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_PROCBASED_CTLS3 0x492u
#define IA32_VMX_PROCBASED_CTLS3_GATE_LEAF 0x1u
#define IA32_VMX_PROCBASED_CTLS3_GATE_REG 2u /* ECX */
#define IA32_VMX_PROCBASED_CTLS3_GATE_BIT 5u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.01H:ECX[5] */
#define IA32_VMX_EXIT_CTLS2 0x493u
#define IA32_VMX_EXIT_CTLS2_GATE_LEAF 0x1u
#define IA32_VMX_EXIT_CTLS2_GATE_REG 2u /* ECX */
#define IA32_VMX_EXIT_CTLS2_GATE_BIT 5u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_SMM_FEATURE_CONTROL 0x4E0u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_SMM_DELAYED 0x4E2u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_SMM_BLOCKED 0x4E3u


#endif /* VXP_COMMON_MSR_VIRT_H */
