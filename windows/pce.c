/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file pce.c
 * @brief
 * \~english The `CR4.PCE` switch, one read-modify-write per processor.
 * \~spanish El interruptor `CR4.PCE`, un leer-modificar-escribir por procesador.
 * \~
 *
 * \~english
 * ONLY BIT 8 IS TOUCHED, and that is the whole safety story.  `CR4` holds the
 * switches the processor runs on: paging mode, SMEP, SMAP, the FS/GS base
 * instructions, virtualisation.  Writing a value you composed yourself instead
 * of the one that was there does not raise an exception -- it changes what the
 * machine IS, and the usual outcome is a triple fault, which is a reboot with no
 * message.  So: read, set one bit, write.  Never build a value.
 *
 * @code
 *      63                            8                       0
 *     +-------------------------------+-----+-----------------+
 *     |  everything else: UNTOUCHED   | PCE |  UNTOUCHED      |
 *     +-------------------------------+-----+-----------------+
 *                                        ^
 *                          the only bit this file writes
 * @endcode
 *
 * AND IT IS DONE HOLDING THE OTHER PROCESSORS.  `CR4` is per-processor, but the
 * sequence read-set-write is not atomic: preempted in the middle, somebody
 * else's change to another bit gets written back stale.  `KeIpiGenericCall`
 * raises to IPI_LEVEL and stops the rest of the machine while the function runs,
 * which is the only way to make the sequence indivisible.
 *
 * NO ASSEMBLY, AND CHECKED, same as `msr_win.c`: `<intrin.h>` brings
 * `__readcr4`/`__writecr4` in both toolchains and what comes out is what one
 * would write by hand -- `mov %cr4,%rax` and `mov %rcx,%cr4`.
 *
 * WHAT THIS FILE DOES NOT SOLVE, and it is worth knowing before trusting it:
 * Windows restores `CR4` from a saved context on some transitions -- sleep,
 * resume, a core going offline and coming back -- and the bit can be lost
 * without anybody saying so.  Whoever depends on `rdpmc` has to be able to
 * survive it faulting, or ask again.
 *
 * \~spanish
 * SOLO SE TOCA EL BIT 8, y en eso consiste toda la seguridad de esto.  `CR4`
 * lleva los interruptores con los que funciona el procesador: modo de
 * paginacion, SMEP, SMAP, las instrucciones de base de FS y GS, la
 * virtualizacion.  Escribir un valor compuesto por uno mismo en vez del que
 * habia no provoca una excepcion -- cambia lo que la maquina ES, y lo normal es
 * una triple falta, que es un reinicio sin mensaje.  Asi que: leer, poner un
 * bit, escribir.  Nunca construir un valor.
 *
 * Y SE HACE RETENIENDO A LOS DEMAS PROCESADORES.  `CR4` es por procesador, pero
 * la secuencia leer-poner-escribir no es atomica: si expropian en medio, el
 * cambio de otro a otro bit se vuelve a escribir viejo.  `KeIpiGenericCall` sube
 * a IPI_LEVEL y para el resto de la maquina mientras la funcion corre, que es la
 * unica forma de que la secuencia sea indivisible.
 *
 * SIN ENSAMBLADOR, Y COMPROBADO, igual que en `msr_win.c`: `<intrin.h>` trae
 * `__readcr4`/`__writecr4` en las dos cadenas y sale lo que se escribiria a
 * mano -- `mov %cr4,%rax` y `mov %rcx,%cr4`.
 *
 * LO QUE ESTE FICHERO NO RESUELVE, y conviene saberlo antes de fiarse: Windows
 * restaura `CR4` desde un contexto guardado en algunas transiciones -- suspender,
 * reanudar, un nucleo que se apaga y vuelve -- y el bit se puede perder sin que
 * nadie lo diga.  Quien dependa de `rdpmc` tiene que sobrevivir a que falle, o
 * volver a pedirlo.
 */

#include "pce.h"

#include "nt.h"

#include <intrin.h>

/** @brief
 *  \~english `CR4` bit 8: Performance-Monitoring Counter Enable.
 *  \~spanish Bit 8 de `CR4`: Performance-Monitoring Counter Enable. \~ */
#define CR4_PCE (((u64)1) << 8)

/** @brief
 *  \~english Its own pool tag, so `!poolused` tells the allocations apart.
 *  \~spanish Etiqueta de pool propia, para que `!poolused` distinga las
 *            reservas. \~ */
#define PCE_POOL_TAG                                                           \
    ((ULONG)('V' | ('X' << 8) | ('P' << 16) | ('E' << 24)))

/**
 * @brief
 * \~english What each processor had, and what it ended up with.
 * \~spanish Que tenia cada procesador, y con que se quedo.
 * \~
 *
 * \~english
 * TWO ARRAYS AND NOT ONE, and the second is the honest part.  `had` is what
 * `pce_restore` puts back.  `now` is `CR4` READ AGAIN after writing it, and it
 * exists because a write to a control register can be intercepted by a
 * hypervisor and dropped without any error: assuming it worked would look
 * perfect here and fail far away, in user mode, with an illegal instruction and
 * nothing pointing back.
 *
 * \~spanish
 * DOS ARRAYS Y NO UNO, y el segundo es la parte honesta.  `had` es lo que
 * `pce_restore` devuelve a su sitio.  `now` es `CR4` RELEIDO despues de
 * escribirlo, y existe porque una escritura a un registro de control la puede
 * interceptar un hipervisor y descartarla sin dar ningun error: dar por hecho
 * que funciono se veria perfecto aqui y fallaria lejos, en modo usuario, con una
 * instruccion ilegal y nada que senale hacia atras.
 */
static u8 *g_had;
static u8 *g_now;
static ULONG g_cpus;

/**
 * @brief
 * \~english Runs on EVERY processor at once; `set` says which way.
 * \~spanish Corre en TODOS los procesadores a la vez; `set` dice en que
 *           sentido.
 * \~
 *
 * \~english
 * At IPI_LEVEL: no allocation, no lock, no paged memory.  Everything it touches
 * was allocated before and lives in non-paged pool.
 *
 * \~spanish
 * A IPI_LEVEL: ni reservar, ni cerrojos, ni memoria paginada.  Todo lo que toca
 * se reservo antes y vive en pool no paginado.
 */
static ULONG_PTR pce_worker(ULONG_PTR set) {
    ULONG i = KeGetCurrentProcessorNumberEx(0);
    u64 cr4;

    /* \~english A processor with no slot is not touched.  It would mean the
     * count changed between allocating and getting here -- a core coming
     * online -- and touching a control register with nowhere to record the
     * previous value is exactly how something is left switched on forever.
     * \~spanish Un procesador sin ranura no se toca.  Querria decir que la
     * cuenta cambio entre reservar y llegar aqui -- un nucleo que se enciende --
     * y tocar un registro de control sin donde apuntar el valor anterior es
     * justo como se deja algo encendido para siempre. \~ */
    if (i >= g_cpus) {
        return 0;
    }

    cr4 = __readcr4();
    if (set) {
        g_had[i] = (u8)((cr4 & CR4_PCE) != 0);
        if ((cr4 & CR4_PCE) == 0) {
            __writecr4(cr4 | CR4_PCE);
        }
        /* \~english Read back, do not assume.  See the comment on `g_now`.
         * \~spanish Releer, no suponer.  Ver el comentario de `g_now`. \~ */
        g_now[i] = (u8)((__readcr4() & CR4_PCE) != 0);
    } else {
        /* \~english Only where it was clear before: somebody else may have
         * turned it on for their own reasons.
         * \~spanish Solo donde estaba apagado antes: puede haberlo encendido
         * otro por sus motivos. \~ */
        if (!g_had[i] && (cr4 & CR4_PCE) != 0) {
            __writecr4(cr4 & ~CR4_PCE);
        }
    }
    return 0;
}

status pce_enable(void) {
    ULONG total = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    usize i;
    u8 *block;

    if (total == 0) {
        return ERR_STATE;
    }
    if (g_had != 0) {
        /* \~english Already on.  Enabling twice would overwrite the record of
         * what was there with what WE left, and then the restore would put back
         * our own state instead of the original.
         * \~spanish Ya esta encendido.  Encenderlo dos veces sobreescribiria el
         * apunte de lo que habia con lo que dejamos NOSOTROS, y entonces la
         * restauracion devolveria nuestro propio estado en vez del original. \~ */
        return ERR_STATE;
    }

    /* \~english One allocation split in two: the two arrays have the same life
     * and the same size, and two allocations would be two chances to leak.
     * \~spanish Una reserva partida en dos: los dos arrays tienen la misma vida
     * y el mismo tamano, y dos reservas serian dos ocasiones de fuga. \~ */
    block = (u8 *)ExAllocatePoolWithTag(NonPagedPoolNx, (usize)total * 2u,
                                        PCE_POOL_TAG);
    if (block == 0) {
        return ERR_NOSPACE;
    }
    for (i = 0; i < (usize)total * 2u; ++i) {
        block[i] = 0;
    }
    g_had = block;
    g_now = block + total;
    g_cpus = total;

    KeIpiGenericCall(pce_worker, 1);
    return OK;
}

void pce_restore(void) {
    if (g_had == 0) {
        return;
    }
    KeIpiGenericCall(pce_worker, 0);
    ExFreePoolWithTag(g_had, PCE_POOL_TAG);
    g_had = 0;
    g_now = 0;
    g_cpus = 0;
}

u32 pce_enabled_count(void) {
    ULONG i;
    u32 n = 0;
    if (g_now == 0) {
        return 0;
    }
    for (i = 0; i < g_cpus; ++i) {
        if (g_now[i]) {
            n += 1;
        }
    }
    return n;
}

u32 pce_asked_count(void) {
    return (u32)g_cpus;
}
