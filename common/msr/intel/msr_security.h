/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_security.h
 * @brief Capacidades y mitigaciones
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

#ifndef VXP_COMMON_MSR_SECURITY_H
#define VXP_COMMON_MSR_SECURITY_H

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[26] */
#define IA32_SPEC_CTRL 0x48u
#define IA32_SPEC_CTRL_GATE_LEAF 0x7u
#define IA32_SPEC_CTRL_GATE_SUBLEAF 0x0u
#define IA32_SPEC_CTRL_GATE_REG 3u /* EDX */
#define IA32_SPEC_CTRL_GATE_BIT 26u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[26] */
#define IA32_PRED_CMD 0x49u
#define IA32_PRED_CMD_GATE_LEAF 0x7u
#define IA32_PRED_CMD_GATE_SUBLEAF 0x0u
#define IA32_PRED_CMD_GATE_REG 3u /* EDX */
#define IA32_PRED_CMD_GATE_BIT 26u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.12H.00H:EAX[0] */
#define IA32_SGXLEPUBKEYHASH0 0x8Cu
#define IA32_SGXLEPUBKEYHASH0_GATE_LEAF 0x12u
#define IA32_SGXLEPUBKEYHASH0_GATE_SUBLEAF 0x0u
#define IA32_SGXLEPUBKEYHASH0_GATE_REG 0u /* EAX */
#define IA32_SGXLEPUBKEYHASH0_GATE_BIT 0u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SGXLEPUBKEYHASH1 0x8Du

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SGXLEPUBKEYHASH2 0x8Eu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SGXLEPUBKEYHASH3 0x8Fu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SGXLEPUBKEYHASH4 0x90u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_SGXLEPUBKEYHASH5 0x91u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[30] */
#define IA32_CORE_CAPABILITIES 0xCFu
#define IA32_CORE_CAPABILITIES_GATE_LEAF 0x7u
#define IA32_CORE_CAPABILITIES_GATE_SUBLEAF 0x0u
#define IA32_CORE_CAPABILITIES_GATE_REG 3u /* EDX */
#define IA32_CORE_CAPABILITIES_GATE_BIT 30u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[29] */
#define IA32_ARCH_CAPABILITIES 0x10Au
#define IA32_ARCH_CAPABILITIES_GATE_LEAF 0x7u
#define IA32_ARCH_CAPABILITIES_GATE_SUBLEAF 0x0u
#define IA32_ARCH_CAPABILITIES_GATE_REG 3u /* EDX */
#define IA32_ARCH_CAPABILITIES_GATE_BIT 29u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[28] */
#define IA32_FLUSH_CMD 0x10Bu
#define IA32_FLUSH_CMD_GATE_LEAF 0x7u
#define IA32_FLUSH_CMD_GATE_SUBLEAF 0x0u
#define IA32_FLUSH_CMD_GATE_REG 3u /* EDX */
#define IA32_FLUSH_CMD_GATE_BIT 28u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EDX[13] */
#define IA32_TSX_FORCE_ABORT 0x10Fu
#define IA32_TSX_FORCE_ABORT_GATE_LEAF 0x7u
#define IA32_TSX_FORCE_ABORT_GATE_SUBLEAF 0x0u
#define IA32_TSX_FORCE_ABORT_GATE_REG 3u /* EDX */
#define IA32_TSX_FORCE_ABORT_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_TSX_CTRL 0x122u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_SGXOWNEREPOCH0 0x300u

/** SDM Table 2-12. MSRs in Intel Atom(R) Processors Based on Goldmont Microarchitecture */
#define MSR_SGXOWNEREPOCH1 0x301u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:EBX[2] */
#define IA32_SGX_SVN_STATUS 0x500u
#define IA32_SGX_SVN_STATUS_GATE_LEAF 0x7u
#define IA32_SGX_SVN_STATUS_GATE_SUBLEAF 0x0u
#define IA32_SGX_SVN_STATUS_GATE_REG 1u /* EBX */
#define IA32_SGX_SVN_STATUS_GATE_BIT 2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[7] */
#define IA32_U_CET 0x6A0u
#define IA32_U_CET_GATE_LEAF 0x7u
#define IA32_U_CET_GATE_SUBLEAF 0x0u
#define IA32_U_CET_GATE_REG 2u /* ECX */
#define IA32_U_CET_GATE_BIT 7u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_S_CET 0x6A2u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[13] */
#define IA32_TME_CAPABILITY 0x981u
#define IA32_TME_CAPABILITY_GATE_LEAF 0x7u
#define IA32_TME_CAPABILITY_GATE_SUBLEAF 0x0u
#define IA32_TME_CAPABILITY_GATE_REG 2u /* ECX */
#define IA32_TME_CAPABILITY_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[13] */
#define IA32_TME_ACTIVATE 0x982u
#define IA32_TME_ACTIVATE_GATE_LEAF 0x7u
#define IA32_TME_ACTIVATE_GATE_SUBLEAF 0x0u
#define IA32_TME_ACTIVATE_GATE_REG 2u /* ECX */
#define IA32_TME_ACTIVATE_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[13] */
#define IA32_TME_EXCLUDE_MASK 0x983u
#define IA32_TME_EXCLUDE_MASK_GATE_LEAF 0x7u
#define IA32_TME_EXCLUDE_MASK_GATE_SUBLEAF 0x0u
#define IA32_TME_EXCLUDE_MASK_GATE_REG 2u /* ECX */
#define IA32_TME_EXCLUDE_MASK_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs.  PUERTA: CPUID.07H.00H:ECX[13] */
#define IA32_TME_EXCLUDE_BASE 0x984u
#define IA32_TME_EXCLUDE_BASE_GATE_LEAF 0x7u
#define IA32_TME_EXCLUDE_BASE_GATE_SUBLEAF 0x0u
#define IA32_TME_EXCLUDE_BASE_GATE_REG 2u /* ECX */
#define IA32_TME_EXCLUDE_BASE_GATE_BIT 13u

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_TME_CLEAR_SAVED_KEY 0x9FBu

/** SDM Table 2-53. Additional MSRs Supported by the Intel(R) CoreTM Ultra 7 Processors Supporting Performance Hybrid */
#define MSR_CORE_MKTME_ACTIVATE 0x9FFu

/** SDM Table 2-2. IA-32 Architectural MSRs */
#define IA32_PASID 0xD93u


#endif /* VXP_COMMON_MSR_SECURITY_H */
