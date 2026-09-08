/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file msr_amd_apic.h
 * @brief APIC, interrupciones y temporizadores locales
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_amd_msr_index.py
 *
 * a partir del Processor Programming Reference de AMD, que no viaja en el
 * repositorio.
 *
 * ALCANCE: **SOLO AMD**.  Los MSR de Intel estan en `common/msr/intel/`, y no
 * se mezclan porque los dos espacios se solapan sin coincidir: `0xC0000080` es
 * `IA32_EFER` para Intel y `EFER` para AMD -- el mismo registro con dos nombres
 * y dos fuentes --, mientras que `0xC0011030` solo existe aqui.
 *
 * Los nombres son los del PPR, para que se pueda buscar en el la misma cadena
 * que hay aqui.  Solo direcciones: los campos de bits de lo que se decodifique
 * se escriben a mano.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef VXP_COMMON_MSR_AMD_APIC_H
#define VXP_COMMON_MSR_AMD_APIC_H

/** APIC Base Address */
#define AMD_APIC_BAR 0x1Bu

/** APIC ID */
#define AMD_APIC_ID 0x802u

/** APIC Version */
#define AMD_ApicVersion 0x803u

/** Task Priority */
#define AMD_TPR 0x808u

/** Arbitration Priority */
#define AMD_ArbitrationPriority 0x809u

/** Processor Priority */
#define AMD_ProcessorPriority 0x80Au

/** End Of Interrupt */
#define AMD_EOI 0x80Bu

/** Logical Destination Register */
#define AMD_LDR 0x80Du

/** Spurious Interrupt Vector */
#define AMD_SVR 0x80Fu

/** Error Status Register */
#define AMD_ESR 0x828u

/** Interrupt Command */
#define AMD_InterruptCommand 0x830u

/** LVT Timer */
#define AMD_TimerLvtEntry 0x832u

/** LVT Thermal Sensor */
#define AMD_ThermalLvtEntry 0x833u

/** LVT Error */
#define AMD_ErrorLvtEntry 0x837u

/** Timer Initial Count */
#define AMD_TimerInitialCount 0x838u

/** Timer Current Count */
#define AMD_TimerCurrentCount 0x839u

/** Timer Divide Configuration */
#define AMD_TimerDivideConfiguration 0x83Eu

/** Self IPI */
#define AMD_SelfIPI 0x83Fu

/** Extended APIC Feature */
#define AMD_ExtendedApicFeature 0x840u

/** Extended APIC Control */
#define AMD_ExtendedApicControl 0x841u

/** Specific End Of Interrupt */
#define AMD_SpecificEndOfInterrupt 0x842u

/** Interrupt Enable 0 */
#define AMD_InterruptEnable0 0x848u

/** Reserved. */
#define AMD_IntPend 0xC0010055u

/** AVIC Doorbell */
#define AMD_AvicDoorbell 0xC001011Bu

/** Secure AVIC Control */
#define AMD_SecureAVIC 0xC0010138u


#endif /* VXP_COMMON_MSR_AMD_APIC_H */
