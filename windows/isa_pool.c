/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file isa_pool.c
 * @brief
 * \~english Spawning, waiting, killing, and putting the loss back smaller.
 * \~spanish Arrancar, esperar, matar, y devolver la perdida mas pequena.
 * \~
 *
 * \~english
 * Why workers are processes and what travels between them is in `isa_pool.h`.  Here
 * is the machinery, and the two places where Windows has an opinion: an inherited
 * pipe has to be inheritable on ONE end only, and a process that is terminated does
 * not get to flush anything.
 *
 * \~spanish
 * Por que los trabajadores son procesos y que viaja entre ellos esta en
 * `isa_pool.h`.  Aqui esta la maquinaria, y los dos sitios donde Windows tiene
 * opinion: una tuberia heredada tiene que ser heredable por UN extremo solo, y un
 * proceso al que se termina no llega a vaciar nada.
 */

#include "isa_pool.h"
#include "isa_win.h"

/*
 * \~english THE CLOCK HAS TO BE THE 64-BIT ONE, and asking for it means naming a
 * minimum Windows.  The 32-bit tick count wraps after about forty-nine days, and the
 * way it would fail here is the bad way: across a wrap, `now` is smaller than a
 * deadline, so a worker that IS hung is not noticed and the sweep stops for good.
 * Rare, silent, and impossible to reproduce on purpose.
 *
 * Declared here and not for the whole project on purpose: this is the only file that
 * needs it, and a floor imposed from the build would apply to code that does not.
 *
 * \~spanish EL RELOJ TIENE QUE SER EL DE 64 BITS, y pedirlo obliga a nombrar un
 * Windows minimo.  La cuenta de tics de 32 bits da la vuelta a los cuarenta y nueve
 * dias, y la forma en que fallaria aqui es la mala: al dar la vuelta, `now` es menor
 * que un plazo, asi que un trabajador que SI esta colgado no se nota y el barrido se
 * para para siempre.  Raro, callado, e imposible de reproducir a proposito.
 *
 * Se declara aqui y no para el proyecto entero a proposito: este es el unico fichero
 * que lo necesita, y un minimo impuesto desde la construccion se aplicaria a codigo
 * que no.
 */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief
 * \~english One worker in flight.
 * \~spanish Un trabajador en vuelo.
 */
typedef struct slot {
    HANDLE process;
    HANDLE read_end; /**< \~english our end of its output \~spanish nuestro extremo de su salida \~ */
    u64 deadline;    /**< \~english tick count past which it counts as hung \~spanish cuenta de tics pasada la cual cuenta como colgado \~ */
    isa_work work;   /**< \~english what it was given, to put back if it loses it \~spanish lo que se le dio, para devolverlo si lo pierde \~ */
    int busy;
} slot;

/**
 * @brief
 * \~english Writes the work item the way the command line carries it.
 * \~spanish Escribe el trozo de trabajo como lo lleva la linea de ordenes.
 * \~
 *
 * \~english
 * HEXADECIMAL AND POSITIONAL, with the prefix as one run of bytes.  It is meant to
 * be typed by a person as much as by the parent: a worker that cannot be run by hand
 * is a worker that cannot be looked at when it misbehaves, and the one thing this
 * program does is make processes misbehave.
 *
 *      vxp_fuzz worker <depth> <fixed> <lo> <hi> <prefix-hex>
 *
 * The prefix comes last because it is the only part that varies in length, and a
 * zero-length one has to be allowed -- the top-level items freeze nothing.
 *
 * \~spanish
 * HEXADECIMAL Y POSICIONAL, con el prefijo como una tirada de bytes.  Esta pensado
 * para que lo teclee una persona tanto como el padre: un trabajador que no se puede
 * ejecutar a mano es un trabajador que no se puede mirar cuando se porta mal, y lo
 * unico que hace este programa es que los procesos se porten mal.
 *
 *      vxp_fuzz worker <profundidad> <fijos> <lo> <hi> <prefijo-hex>
 *
 * El prefijo va al final porque es la unica parte de longitud variable, y hay que
 * admitir uno de longitud cero -- los trozos de arriba no congelan nada.
 */
static void write_command(char *out, u32 cap, const char *exe,
                          const isa_work *w) {
    char hex[2u * ISA_MAX_LEN + 1u];
    u32 i;

    for (i = 0; i < (u32)w->fixed; ++i) {
        static const char digits[] = "0123456789ABCDEF";
        hex[2u * i] = digits[(w->prefix[i] >> 4) & 0xFu];
        hex[2u * i + 1u] = digits[w->prefix[i] & 0xFu];
    }
    hex[2u * (u32)w->fixed] = 0;

    /* \~english The quotes around the path are not optional: a program installed
     * somewhere with a space in the name would otherwise be read as two arguments,
     * and the failure would look like a bad work item.  \~spanish Las comillas
     * alrededor de la ruta no son opcionales: un programa instalado en un sitio con
     * un espacio en el nombre se leeria como dos argumentos, y el fallo pareceria un
     * trozo de trabajo mal formado. \~ */
    snprintf(out, cap, "\"%s\" worker %u %u %02X %02X %s", exe,
             (unsigned)w->depth, (unsigned)w->fixed, (unsigned)w->lo,
             (unsigned)w->hi, hex);
    out[cap - 1u] = 0;
}

/** @brief
 *  \~english Starts one worker on one item.  Closes nothing it did not open.
 *  \~spanish Arranca un trabajador con un trozo.  No cierra nada que no abriera. \~ */
static int spawn(const char *exe, const isa_work *w, u32 timeout_ms, slot *s) {
    SECURITY_ATTRIBUTES sa;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    HANDLE rd = 0;
    HANDLE wr = 0;
    char cmd[1024];

    memset(&sa, 0, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&rd, &wr, &sa, 0)) {
        return 0;
    }
    /* \~english OUR end must not be inherited, and forgetting this does not fail --
     * it hangs: the child would hold a copy of the read end open, so the pipe never
     * reports end of file and the parent waits for output from a process that has
     * already gone.  \~spanish NUESTRO extremo no debe heredarse, y olvidarlo no
     * falla -- cuelga: el hijo tendria una copia del extremo de lectura abierta, asi
     * que la tuberia nunca informa de fin de fichero y el padre espera salida de un
     * proceso que ya se fue. \~ */
    if (!SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(rd);
        CloseHandle(wr);
        return 0;
    }

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = wr;
    si.hStdError = wr;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    write_command(cmd, (u32)sizeof(cmd), exe, w);
    memset(&pi, 0, sizeof(pi));
    if (!CreateProcessA(0, cmd, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &si, &pi)) {
        CloseHandle(rd);
        CloseHandle(wr);
        return 0;
    }
    CloseHandle(pi.hThread);
    /* \~english The write end is closed HERE, in the parent, as soon as the child
     * has its copy.  Keeping it open would mean the pipe never reaches end of file
     * even after the child dies, because the parent itself is still a writer.
     * \~spanish El extremo de escritura se cierra AQUI, en el padre, en cuanto el
     * hijo tiene su copia.  Dejarlo abierto significaria que la tuberia no llega
     * nunca a fin de fichero ni despues de morir el hijo, porque el propio padre
     * sigue siendo escritor. \~ */
    CloseHandle(wr);

    s->process = pi.hProcess;
    s->read_end = rd;
    s->work = *w;
    s->deadline = (u64)GetTickCount64() + (u64)timeout_ms;
    s->busy = 1;
    return 1;
}

/**
 * @brief
 * \~english Reads the tally the worker wrote, if it wrote all of it.
 * \~spanish Lee la cuenta que escribio el trabajador, si la escribio entera.
 * \~
 *
 * @return \~english 1 when the whole thing arrived, 0 otherwise \~spanish 1 cuando llego entera, 0 si no \~
 *
 * \~english
 * A SHORT READ IS THE ANSWER, not an error to work around.  The worker writes the
 * tally as its last act, so anything less than all of it means it did not get there
 * -- which is exactly what the parent needs to know and needs no other channel to
 * find out.
 *
 * \~spanish
 * UNA LECTURA CORTA ES LA RESPUESTA, no un error que sortear.  El trabajador escribe
 * la cuenta como ultimo acto, asi que menos que toda ella quiere decir que no llego
 * -- que es justo lo que el padre necesita saber y no le hace falta otro canal para
 * averiguarlo.
 */
static int read_tally(HANDLE h, isa_tally *out) {
    char *p = (char *)out;
    u32 have = 0;

    while (have < (u32)sizeof(*out)) {
        DWORD got = 0;
        if (!ReadFile(h, p + have, (DWORD)(sizeof(*out) - have), &got, 0)) {
            return 0;
        }
        if (got == 0) {
            return 0; /* fin de fichero antes de la cuenta entera */
        }
        have += (u32)got;
    }
    return 1;
}

/** @brief
 *  \~english Puts a finished slot back to being free.
 *  \~spanish Devuelve una ranura terminada a estar libre. \~ */
static void release(slot *s) {
    if (s->read_end != 0) {
        CloseHandle(s->read_end);
    }
    if (s->process != 0) {
        CloseHandle(s->process);
    }
    s->read_end = 0;
    s->process = 0;
    s->busy = 0;
}

/**
 * @brief
 * \~english Resolves one finished or overdue slot: report it, or put it back.
 * \~spanish Resuelve una ranura terminada o vencida: informar, o devolverla.
 * \~
 *
 * \~english
 * THE ITEM IS PUT BACK BEFORE IT IS REPORTED, and the order matters when the queue
 * is full: pushing first means a dropped item is counted by the queue as the hole it
 * is, instead of being reported as resolved and then quietly lost.
 *
 * \~spanish
 * EL TROZO SE DEVUELVE ANTES DE INFORMARLO, y el orden importa cuando la cola esta
 * llena: meterlo primero hace que un trozo que no cabe lo cuente la cola como el
 * agujero que es, en vez de informarse como resuelto y despues perderse en silencio.
 */
static void resolve_lost(slot *s, u32 how, isa_queue *q,
                         isa_pool_report_fn report, void *ctx,
                         isa_pool_stats *st) {
    isa_tally t;
    isa_work single;

    memset(&t, 0, sizeof(t));
    if (how == (u32)ISA_WORKER_DIED) {
        st->died += 1u;
    } else {
        st->hung += 1u;
    }
    if (isa_queue_refine(q, &s->work, &single)) {
        st->named += 1u;
    }
    if (report != 0) {
        report(ctx, &s->work, how, &t);
    }
    release(s);
}

status isa_pool_run(const char *exe, u32 workers, u32 timeout_ms, isa_queue *q,
                    isa_pool_report_fn report, void *ctx,
                    isa_pool_stats *stats) {
    slot slots[ISA_POOL_MAX];
    u32 i;

    if (exe == 0 || q == 0 || stats == 0) {
        return ERR_INVALID;
    }
    if (workers < 1u || workers > ISA_POOL_MAX || timeout_ms == 0) {
        return ERR_INVALID;
    }

    memset(slots, 0, sizeof(slots));
    memset(stats, 0, sizeof(*stats));

    for (;;) {
        HANDLE waiting[ISA_POOL_MAX];
        u32 which[ISA_POOL_MAX];
        u32 count = 0;
        u64 now;
        int in_flight = 0;

        /* \~english Fill every free slot that has work waiting.  \~spanish Llenar
         * cada ranura libre que tenga trabajo esperando. \~ */
        for (i = 0; i < workers; ++i) {
            isa_work w;
            if (slots[i].busy) {
                continue;
            }
            if (!isa_queue_pop(q, &w)) {
                break;
            }
            if (!spawn(exe, &w, timeout_ms, &slots[i])) {
                /* \~english No process: the item is NOT put back, because putting it
                 * back would spin on the same failure forever.  It is counted, which
                 * is the honest thing -- the sweep has a hole and says so.
                 * \~spanish Sin proceso: el trozo NO se devuelve, porque devolverlo
                 * daria vueltas sobre el mismo fallo para siempre.  Se cuenta, que es
                 * lo honesto -- el barrido tiene un agujero y lo dice. \~ */
                stats->unspawnable += 1u;
                continue;
            }
            stats->spawned += 1u;
        }

        for (i = 0; i < workers; ++i) {
            if (slots[i].busy) {
                in_flight = 1;
                waiting[count] = slots[i].process;
                which[count] = i;
                count += 1u;
            }
        }
        if (!in_flight) {
            /* \~english Nothing running and nothing to run: the queue emptied, and
             * refinement has stopped producing.  \~spanish Nada corriendo y nada que
             * correr: la cola se vacio, y el refinamiento dejo de producir. \~ */
            if (q->count == 0) {
                break;
            }
            continue;
        }

        /*
         * \~english A SHORT WAIT AND THEN LOOK AT THE CLOCK, rather than waiting for
         * a process to exit.  A hung worker never exits, so a wait with no deadline
         * of its own would be the whole sweep stopping on one `73 FE`.  Fifty
         * milliseconds is short enough that a killed worker is reclaimed promptly and
         * long enough that the parent is not the thing using the processor.
         *
         * \~spanish UNA ESPERA CORTA Y DESPUES MIRAR EL RELOJ, en vez de esperar a
         * que un proceso salga.  Un trabajador colgado no sale nunca, asi que una
         * espera sin plazo propio seria el barrido entero parandose en un `73 FE`.
         * Cincuenta milisegundos es bastante corto para recuperar pronto a un
         * trabajador matado y bastante largo para que el padre no sea quien gasta el
         * procesador.
         */
        (void)WaitForMultipleObjects((DWORD)count, waiting, FALSE, 50);

        now = (u64)GetTickCount64();
        for (i = 0; i < count; ++i) {
            slot *s = &slots[which[i]];
            DWORD code = 0;

            if (WaitForSingleObject(s->process, 0) == WAIT_OBJECT_0) {
                isa_tally probe;
                int full;
                if (!GetExitCodeProcess(s->process, &code)) {
                    code = 1;
                }
                /* \~english Both conditions, and neither alone is enough: a worker
                 * can exit zero having written nothing (killed between the sweep and
                 * the write), and one can write a full tally and then die on the way
                 * out.  \~spanish Las dos condiciones, y ninguna basta sola: un
                 * trabajador puede salir con cero sin haber escrito nada (matado
                 * entre el barrido y la escritura), y otro puede escribir la cuenta
                 * entera y morir al salir. \~ */
                full = read_tally(s->read_end, &probe);
                if (code == 0 && full) {
                    stats->done += 1u;
                    if (report != 0) {
                        report(ctx, &s->work, (u32)ISA_WORKER_DONE, &probe);
                    }
                    release(s);
                } else {
                    resolve_lost(s, (u32)ISA_WORKER_DIED, q, report, ctx, stats);
                }
                continue;
            }
            if (now > s->deadline) {
                /* \~english Terminated from outside, which is the only thing that
                 * works on a worker that is not running any code of ours.
                 * \~spanish Terminado desde fuera, que es lo unico que funciona con
                 * un trabajador que no esta corriendo ningun codigo nuestro. \~ */
                TerminateProcess(s->process, 2);
                WaitForSingleObject(s->process, 1000);
                resolve_lost(s, (u32)ISA_WORKER_HUNG, q, report, ctx, stats);
            }
        }
    }
    return OK;
}

int isa_worker_main(const isa_work *w) {
    isa_arena arena;
    isa_probe_ops ops;
    isa_tally t;

    /*
     * \~english THE CRASH REPORTING GOES OFF FIRST, before anything can crash.  Not
     * cosmetic: dying is the normal outcome here, and with it on each death cost
     * about five seconds of the system deciding what to tell somebody.  Measured
     * sweeping 114 first bytes one at a time -- over ten minutes, fifty of them
     * deaths.
     *
     * \~spanish EL INFORME DE ERRORES SE APAGA PRIMERO, antes de que nada pueda
     * caerse.  No es cosmetico: morir es el resultado normal aqui, y con el encendido
     * cada muerte costaba unos cinco segundos del sistema decidiendo que contarle a
     * alguien.  Medido barriendo 114 primeros bytes de uno en uno -- mas de diez
     * minutos, cincuenta de ellos muertes.
     */
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX |
                 SEM_NOOPENFILEERRORBOX);

    /* \~english Raw bytes go out, so the stream must not translate line endings: a
     * tally with a 0x0A byte in it would arrive one byte longer and be read as a
     * short one.  \~spanish Salen bytes crudos, asi que el flujo no debe traducir
     * fines de linea: una cuenta con un byte 0x0A dentro llegaria un byte mas larga y
     * se leeria como corta. \~ */
    _setmode(_fileno(stdout), _O_BINARY);

    if (isa_arena_open(&arena) != OK) {
        return 1;
    }
    isa_arena_ops(&arena, &ops);
    if (isa_sweep(&ops, w, &t) != OK) {
        isa_arena_close(&arena);
        return 1;
    }
    fwrite(&t, 1, sizeof(t), stdout);
    fflush(stdout);
    isa_arena_close(&arena);
    return 0;
}
