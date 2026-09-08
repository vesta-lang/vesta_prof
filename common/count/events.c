/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file count/events.c
 * @brief
 * \~english The thirteen architectural events, copied from the manual.
 * \~spanish Los trece eventos arquitectonicos, copiados del manual.
 * \~
 *
 * \~english
 * WHERE THIS COMES FROM.  Intel SDM Volume 3B, Table 22-3, "UMask and Event
 * Select Encodings for Pre-Defined Architectural Performance Events".  The order
 * is the manual's, which is the order of the bits in `CPUID.0AH:EBX`, and it is
 * not free to change: the position in this array is what the processor is asked
 * about.
 *
 * IT IS ITS OWN TRANSLATION UNIT so that whoever only wants to PRINT the events
 * and whoever wants to ARM them link the same bytes.  It is thirteen entries; it
 * costs nothing and it removes the possibility of two versions of the same
 * table.
 *
 * \~spanish
 * DE DONDE SALE ESTO.  Manual de Intel volumen 3B, tabla 22-3, "UMask and Event
 * Select Encodings for Pre-Defined Architectural Performance Events".  El orden
 * es el del manual, que es el de los bits de `CPUID.0AH:EBX`, y no es libre de
 * cambiar: la posicion en este array es por lo que se le pregunta al procesador.
 *
 * ES SU PROPIA UNIDAD DE TRADUCCION para que quien solo quiera IMPRIMIR los
 * eventos y quien quiera ARMARLOS enlacen los mismos bytes.  Son trece entradas;
 * no cuesta nada y quita la posibilidad de que haya dos versiones de la misma
 * tabla.
 */

#include "count/events.h"

const arch_event arch_events[ARCH_EVENT_MAX] = {
        /* \~english The seven classic ones: encoding checked against Table 22-3.
         * \~spanish Los siete clasicos: codificacion contrastada con la tabla
         * 22-3. \~ */
        {"CORE_CYC", "core cycles", 0x3Cu, 0x00u, ARCH_EVENT_ENCODED, 0},
        {"INTR_RET", "instructions retired", 0xC0u, 0x00u, ARCH_EVENT_ENCODED,
         0},
        {"REF_CYC", "reference cycles", 0x3Cu, 0x01u, ARCH_EVENT_ENCODED, 0},
        {"LLC_CYC", "LLC references", 0x2Eu, 0x4Fu, ARCH_EVENT_ENCODED, 0},
        {"LLC_MISSES", "LLC misses", 0x2Eu, 0x41u, ARCH_EVENT_ENCODED, 0},
        {"BR_INSTR_RET", "branch instructions retired", 0xC4u, 0x00u,
         ARCH_EVENT_ENCODED, 0},
        {"BR_MISPRED_RET", "branch mispredicts retired", 0xC5u, 0x00u,
         ARCH_EVENT_ENCODED, 0},

        /* \~english Topdown slots IS an ordinary event with an architectural
         * encoding, and it counts on its own: issue slots.  What is not read
         * through a selector is the BREAKDOWN that follows it -- how those slots
         * were spent -- which comes out of the metrics register.  The two things
         * get confused because they share a name.
         * \~spanish Las ranuras de topdown SON un evento corriente con
         * codificacion arquitectonica, y cuenta por si solo: ranuras de emision.
         * Lo que no se lee por un selector es el REPARTO que viene detras -- en
         * que se gastaron esas ranuras --, que sale del registro de metricas.
         * Las dos cosas se confunden porque comparten nombre. \~ */
        {"SLOTS", "topdown slots", 0xA4u, 0x01u, ARCH_EVENT_ENCODED, 0},

        /* \~english The breakdown, and the LBR insertions: microarchitecture
         * -specific, so no plausible number goes here.
         * \~spanish El reparto, y las inserciones de LBR: propios de cada
         * microarquitectura, asi que aqui no va ningun numero plausible. \~ */
        {"BACKEND", "topdown backend bound", 0, 0, 0, 0},
        {"BADSPEC", "topdown bad speculation", 0, 0, 0, 0},
        {"FRONTEND", "topdown frontend bound", 0, 0, 0, 0},
        {"RETIRING", "topdown retiring", 0, 0, 0, 0},
        {"LBR_INSERTS", "LBR inserts", 0, 0, 0, 0}};
