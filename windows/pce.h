/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file pce.h
 * @brief
 * \~english Letting ring 3 read the counters: `CR4.PCE`.
 * \~spanish Dejar que el anillo tres lea los contadores: `CR4.PCE`.
 * \~
 *
 * \~english
 * WHAT IT BUYS.  `rdpmc` reads a performance counter, and by default only ring 0
 * may execute it.  Bit 8 of the control register `CR4` -- Performance-Monitoring
 * Counter Enable -- makes it legal at any privilege level.  With it, a program
 * reads its own counters directly:
 *
 * @code
 *   without CR4.PCE          with CR4.PCE
 *   ---------------          ------------
 *   user asks the driver     rdpmc
 *   IOCTL -> ring 0          ~20-30 cycles, no transition
 *   read, return, ring 3     the window can be the region measured,
 *   microseconds             not the whole program
 * @endcode
 *
 * The second one is not "the same but faster": it changes what can be measured.
 * A window of microseconds barely catches anybody else's work on the core, while
 * one of seconds catches all of it -- which is exactly what widened the error
 * bar when a `hello world` was counted by loading and unloading the driver.
 *
 * IT IS A MACHINE-WIDE SWITCH, not a per-process one.  Turning it on lets ANY
 * program on the machine read the counters, which is a legitimate concern:
 * counters leak information about what other processes do.  That is why it is
 * turned off again on unload, and why it is not left on by default.
 *
 * \~spanish
 * QUE SE GANA.  `rdpmc` lee un contador de rendimiento, y por omision solo el
 * anillo cero puede ejecutarla.  El bit 8 del registro de control `CR4`
 * -- Performance-Monitoring Counter Enable -- la hace legal en cualquier nivel
 * de privilegio.  Con el, un programa lee sus propios contadores directamente:
 *
 * @code
 *   sin CR4.PCE                con CR4.PCE
 *   -----------                -----------
 *   usuario le pide al driver  rdpmc
 *   IOCTL -> anillo 0          ~20-30 ciclos, sin transicion
 *   leer, volver, anillo 3     la ventana puede ser la region que se mide,
 *   microsegundos              no el programa entero
 * @endcode
 *
 * Lo segundo no es "lo mismo pero mas rapido": cambia lo que se puede medir.
 * Una ventana de microsegundos apenas recoge trabajo ajeno del nucleo, mientras
 * que una de segundos lo recoge todo -- que es justo lo que ensancho la barra de
 * error al contar un `hola mundo` cargando y descargando el driver.
 *
 * ES UN INTERRUPTOR DE LA MAQUINA ENTERA, no de un proceso.  Encenderlo permite
 * que CUALQUIER programa de la maquina lea los contadores, y eso es una
 * preocupacion legitima: los contadores filtran informacion de lo que hacen
 * otros procesos.  Por eso se apaga al descargar, y por eso no se queda
 * encendido por omision.
 */

#ifndef VXP_WINDOWS_PCE_H
#define VXP_WINDOWS_PCE_H

#include "vxp_base.h"

/**
 * @brief
 * \~english Turns `rdpmc` on for every processor, remembering what it found.
 * \~spanish Enciende `rdpmc` en todos los procesadores, recordando lo que
 *           habia.
 * \~
 *
 * @return `OK`; `ERR_NOSPACE` \~english if there was no pool \~spanish si no hubo pool \~
 *
 * \~english
 * IT DOES NOT ASSUME THE WRITE WORKED.  After writing, `CR4` is read BACK on the
 * same processor and the result recorded.  Under a hypervisor a write to a
 * control register can be intercepted and quietly dropped, and then everything
 * would look fine until `rdpmc` faulted in user mode -- far from here, with
 * nothing to point at the cause.  `pce_enabled_count` says how many actually
 * took it.
 *
 * \~spanish
 * NO DA POR HECHO QUE LA ESCRITURA FUNCIONO.  Despues de escribir se RELEE `CR4`
 * en el mismo procesador y se apunta el resultado.  Bajo un hipervisor, una
 * escritura a un registro de control se puede interceptar y descartar en
 * silencio, y entonces todo pareceria bien hasta que `rdpmc` fallara en modo
 * usuario -- lejos de aqui y sin nada que senale la causa.
 * `pce_enabled_count` dice en cuantos entro de verdad.
 */
status pce_enable(void);

/**
 * @brief
 * \~english Puts back exactly what was there, processor by processor.
 * \~spanish Devuelve exactamente lo que habia, procesador a procesador.
 * \~
 *
 * \~english
 * The bit is only cleared where it was clear before.  Somebody else may have
 * turned it on for their own reasons -- another profiler, a benchmark harness --
 * and clearing it there would break them with no error and no clue.
 *
 * Safe to call when `pce_enable` was never called or failed: then there is
 * nothing recorded and nothing is touched.
 *
 * \~spanish
 * El bit solo se apaga donde estaba apagado antes.  Puede haberlo encendido otro
 * por sus motivos -- otro perfilador, un banco de medida -- y apagarselo lo
 * romperia sin error y sin pista.
 *
 * Es seguro llamarla si `pce_enable` no se llamo o fallo: entonces no hay nada
 * apuntado y no se toca nada.
 */
void pce_restore(void);

/** @brief
 *  \~english How many processors ended up with `rdpmc` really enabled.
 *  \~spanish En cuantos procesadores quedo `rdpmc` de verdad habilitado. \~ */
u32 pce_enabled_count(void);

/** @brief
 *  \~english How many were asked.  Different from the above means intercepted.
 *  \~spanish A cuantos se les pidio.  Que difiera del anterior quiere decir que
 *            se intercepto. \~ */
u32 pce_asked_count(void);

#endif /* VXP_WINDOWS_PCE_H */
