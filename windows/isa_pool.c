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
    HANDLE read_end;  /**< \~english our end of its tally \~spanish nuestro extremo de su cuenta \~ */
    HANDLE notes_end; /**< \~english and of what it said out loud \~spanish y de lo que dijo en voz alta \~ */
    u64 deadline;     /**< \~english tick count past which it counts as hung \~spanish cuenta de tics pasada la cual cuenta como colgado \~ */
    isa_work work;    /**< \~english what it was given, to put back if it loses it \~spanish lo que se le dio, para devolverlo si lo pierde \~ */
    int busy;
} slot;

/**
 * @brief
 * \~english How much of a worker's spoken output is kept.
 * \~spanish Cuanto se guarda de lo que dice un trabajador.
 * \~
 *
 * \~english
 * A worker on a single candidate says at most fifteen short lines, one per attempt,
 * so this is generous.  It matters that it is SMALL and bounded: a pipe nobody is
 * reading blocks the writer once it fills, and a worker blocked on its own output
 * would look exactly like a worker hung on a candidate.
 *
 * \~spanish
 * Un trabajador con una sola candidata dice quince lineas cortas como mucho, una por
 * intento, asi que esto va sobrado.  Importa que sea PEQUENO y acotado: una tuberia
 * que nadie lee bloquea al que escribe cuando se llena, y un trabajador bloqueado en
 * su propia salida se veria exactamente igual que uno colgado en una candidata.
 */
#define POOL_NOTES_CAP 512u

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
                          const isa_work *w, const char *ref_path) {
    char hex[2u * ISA_MAX_LEN + 2u];
    u32 i;

    for (i = 0; i < (u32)w->fixed; ++i) {
        static const char digits[] = "0123456789ABCDEF";
        hex[2u * i] = digits[(w->prefix[i] >> 4) & 0xFu];
        hex[2u * i + 1u] = digits[w->prefix[i] & 0xFu];
    }
    hex[2u * (u32)w->fixed] = 0;
    /*
     * \~english AN EMPTY PREFIX IS WRITTEN AS `-`, AND IT HAS TO BE.  With a reference
     * path coming after it, a zero-length prefix would leave one fewer token and the
     * path would land where the prefix is read: the worker would take a filename for
     * hexadecimal, refuse it, and the parent would see a process that exits without
     * saying anything -- which is exactly what a lethal candidate looks like.  A whole
     * run would report findings that are a broken command line.
     *
     * \~spanish UN PREFIJO VACIO SE ESCRIBE `-`, Y TIENE QUE SER ASI.  Con una ruta de
     * referencia detras, un prefijo de longitud cero dejaria un token menos y la ruta
     * caeria donde se lee el prefijo: el trabajador tomaria un nombre de fichero por
     * hexadecimal, lo rechazaria, y el padre veria un proceso que sale sin decir nada --
     * que es exactamente como se ve una candidata letal.  Una corrida entera informaria
     * de hallazgos que son una linea de ordenes mal formada.
     */
    if (w->fixed == 0) {
        hex[0] = '-';
        hex[1] = 0;
    }

    /* \~english The quotes around the path are not optional: a program installed
     * somewhere with a space in the name would otherwise be read as two arguments,
     * and the failure would look like a bad work item.  \~spanish Las comillas
     * alrededor de la ruta no son opcionales: un programa instalado en un sitio con
     * un espacio en el nombre se leeria como dos argumentos, y el fallo pareceria un
     * trozo de trabajo mal formado. \~ */
    if (ref_path != 0 && ref_path[0] != 0) {
        snprintf(out, cap, "\"%s\" worker %u %u %02X %02X %s \"%s\"", exe,
                 (unsigned)w->depth, (unsigned)w->fixed, (unsigned)w->lo,
                 (unsigned)w->hi, hex, ref_path);
    } else {
        snprintf(out, cap, "\"%s\" worker %u %u %02X %02X %s", exe,
                 (unsigned)w->depth, (unsigned)w->fixed, (unsigned)w->lo,
                 (unsigned)w->hi, hex);
    }
    out[cap - 1u] = 0;
}

/** @brief
 *  \~english Starts one worker on one item.  Closes nothing it did not open.
 *  \~spanish Arranca un trabajador con un trozo.  No cierra nada que no abriera. \~ */
static int spawn(const char *exe, const isa_work *w, const char *ref_path,
                 u32 timeout_ms, slot *s) {
    SECURITY_ATTRIBUTES sa;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    HANDLE rd = 0;
    HANDLE wr = 0;
    HANDLE nrd = 0;
    HANDLE nwr = 0;
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

    /*
     * \~english TWO PIPES, AND NOT ONE, and having them share was a real defect.
     * What comes out of standard output is the tally as raw bytes; anything the
     * worker says out loud goes to standard error.  Pointed at the same pipe, one
     * stray line of text -- said by the worker on purpose, or by the runtime on its
     * way out -- lands in the middle of the tally and gets read as counts.  The
     * failure is a plausible number, which is the worst kind.
     *
     * \~spanish DOS TUBERIAS, Y NO UNA, y que las compartieran era un defecto de
     * verdad.  Lo que sale por la salida estandar es la cuenta como bytes crudos; lo
     * que el trabajador diga en voz alta va por la de error.  Apuntadas a la misma
     * tuberia, una linea de texto suelta -- dicha por el trabajador a proposito, o por
     * el tiempo de ejecucion al irse -- cae en medio de la cuenta y se lee como
     * cifras.  El fallo es un numero plausible, que es el peor.
     */
    if (!CreatePipe(&nrd, &nwr, &sa, 0)) {
        CloseHandle(rd);
        CloseHandle(wr);
        return 0;
    }
    if (!SetHandleInformation(nrd, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(rd);
        CloseHandle(wr);
        CloseHandle(nrd);
        CloseHandle(nwr);
        return 0;
    }

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = wr;
    si.hStdError = nwr;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    write_command(cmd, (u32)sizeof(cmd), exe, w, ref_path);
    memset(&pi, 0, sizeof(pi));
    if (!CreateProcessA(0, cmd, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &si, &pi)) {
        CloseHandle(rd);
        CloseHandle(wr);
        CloseHandle(nrd);
        CloseHandle(nwr);
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
    CloseHandle(nwr);

    s->process = pi.hProcess;
    s->read_end = rd;
    s->notes_end = nrd;
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

/**
 * @brief
 * \~english The last count a worker announced before it stopped existing.
 * \~spanish El ultimo numero que anuncio un trabajador antes de dejar de existir.
 * \~
 *
 * @return \~english the count, or 0 if it never said one \~spanish el numero, o 0 si no dijo ninguno \~
 *
 * \~english
 * IT LOOKS FOR THE LAST ONE, not the first, and that is the whole point: the worker
 * announces before every attempt, so the last thing it managed to say is where it
 * was when it died.  Reading it as text rather than as a number in a struct is
 * deliberate -- a person running the worker by hand sees the same thing the parent
 * does, and this whole program exists to make processes behave badly.
 *
 * \~spanish
 * BUSCA EL ULTIMO, no el primero, y ahi esta toda la gracia: el trabajador anuncia
 * antes de cada intento, asi que lo ultimo que consiguio decir es donde estaba cuando
 * murio.  Leerlo como texto y no como un numero dentro de una estructura es a
 * proposito -- una persona que ejecute el trabajador a mano ve lo mismo que ve el
 * padre, y este programa entero existe para que los procesos se porten mal.
 */
static u32 read_reached(HANDLE h) {
    char buf[POOL_NOTES_CAP];
    u32 have = 0;
    u32 i;
    u32 last = 0;

    while (have < POOL_NOTES_CAP - 1u) {
        DWORD got = 0;
        if (!ReadFile(h, buf + have, (DWORD)(POOL_NOTES_CAP - 1u - have), &got,
                      0)) {
            break;
        }
        if (got == 0) {
            break;
        }
        have += (u32)got;
    }
    buf[have] = 0;

    /* \~english `n=<digits>` is the whole format.  \~spanish `n=<digitos>` es el
     * formato entero. \~ */
    for (i = 0; i + 1u < have; ++i) {
        if (buf[i] == 'n' && buf[i + 1u] == '=') {
            u32 v = 0;
            u32 j = i + 2u;
            while (j < have && buf[j] >= '0' && buf[j] <= '9') {
                v = v * 10u + (u32)(buf[j] - '0');
                j += 1u;
            }
            if (j > i + 2u) {
                last = v;
            }
        }
    }
    return last;
}

/** @brief
 *  \~english Puts a finished slot back to being free.
 *  \~spanish Devuelve una ranura terminada a estar libre. \~ */
static void release(slot *s) {
    if (s->read_end != 0) {
        CloseHandle(s->read_end);
    }
    if (s->notes_end != 0) {
        CloseHandle(s->notes_end);
    }
    if (s->process != 0) {
        CloseHandle(s->process);
    }
    s->read_end = 0;
    s->notes_end = 0;
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
/**
 * @brief
 * \~english How long the instruction at the head of this subtree is.
 * \~spanish Cuanto mide la instruccion que encabeza este subarbol.
 * \~
 *
 * @param probe \~english filled in with the item that was measured \~spanish se rellena con el trozo que se midio \~
 * @return \~english the length, or 0 if it could not be obtained \~spanish la longitud, o 0 si no se pudo obtener \~
 *
 * \~english
 * WHAT IT IS FOR, and it saves 256 processes every time it answers.  A subtree that
 * killed a worker gets deepened: freeze the byte, open the next one, ask 256 more
 * questions.  But if the instruction at its head is only as long as the bytes already
 * frozen, the byte being opened IS NEVER READ -- so all 256 of those questions have
 * the same answer, and reporting 256 findings for one instruction contradicts the
 * very premise the traversal is built on.
 *
 * Measured before adding this: ten one-byte instructions -- eight `pop`s, `xchg esp,
 * eax` and `popfq` -- came out as 2560 separate lethal candidates.
 *
 * HOW IT ASKS.  The probe is an ordinary work item with the depth cut down to the
 * bytes that are frozen plus one, which makes it a SINGLE candidate -- and a worker
 * given a single candidate announces each attempt out loud, so the length arrives
 * whether it survives or not.  No new mode, no second protocol.
 *
 * IT IS SYNCHRONOUS, one process at a time, and that is affordable because it happens
 * once per lost subtree and replaces 256 spawns.  Making it concurrent would mean the
 * queue holding items that are questions about other items, and the queue's vocabulary
 * is worth more than the milliseconds.
 *
 * \~spanish
 * PARA QUE SIRVE, y ahorra 256 procesos cada vez que responde.  Un subarbol que mato a
 * un trabajador se profundiza: congelar el byte, abrir el siguiente, hacer 256
 * preguntas mas.  Pero si la instruccion que lo encabeza mide solo lo que ya esta
 * congelado, el byte que se abre NO LO LEE NADIE -- asi que esas 256 preguntas tienen
 * la misma respuesta, e informar de 256 hallazgos por una instruccion contradice la
 * premisa sobre la que esta construido el recorrido.
 *
 * Medido antes de anadir esto: diez instrucciones de un byte -- ocho `pop`, `xchg esp,
 * eax` y `popfq` -- salian como 2560 candidatas letales distintas.
 *
 * COMO PREGUNTA.  La sonda es un trozo de trabajo normal con la profundidad recortada
 * a los bytes congelados mas uno, lo que la convierte en UNA sola candidata -- y a un
 * trabajador con una sola candidata se le anuncia cada intento en voz alta, asi que la
 * longitud llega sobreviva o no.  Sin modo nuevo, sin segundo protocolo.
 *
 * ES SINCRONA, un proceso a la vez, y sale barata porque pasa una vez por subarbol
 * perdido y sustituye a 256 arranques.  Hacerla concurrente obligaria a que la cola
 * guardara trozos que son preguntas sobre otros trozos, y el vocabulario de la cola
 * vale mas que los milisegundos.
 */
static u32 probe_one(const char *exe, const isa_work *w, u32 timeout_ms,
                     u32 *len) {
    slot s;
    DWORD code = 1;
    int died;

    *len = 0;
    memset(&s, 0, sizeof(s));
    /* \~english A probe is asked ONE thing -- how long is this -- and comparing against a
     * reference is not it.  Not handing it the table matters at depth three, where the
     * table is sixteen megabytes and the probes and samples are the majority of the
     * processes: they would each load it to answer a question that does not use it.
     * \~spanish A una sonda se le pregunta UNA cosa -- cuanto mide esto -- y comparar
     * contra una referencia no es eso.  No darle la tabla importa a profundidad tres,
     * donde la tabla son dieciseis megabytes y las sondas y los muestreos son la mayoria
     * de los procesos: cada uno la cargaria para responder algo que no la usa. \~ */
    if (!spawn(exe, w, 0, timeout_ms, &s)) {
        return 2; /* no se pudo preguntar: ni vivo ni muerto */
    }
    if (WaitForSingleObject(s.process, (DWORD)timeout_ms) != WAIT_OBJECT_0) {
        /* \~english It hung on the probe too.  Terminated, and whatever it managed to
         * say still counts.  \~spanish Se colgo tambien en la sonda.  Terminado, y lo
         * que consiguiera decir vale igual. \~ */
        TerminateProcess(s.process, 2);
        WaitForSingleObject(s.process, 1000);
        died = 1;
    } else {
        if (!GetExitCodeProcess(s.process, &code)) {
            code = 1;
        }
        died = (code != 0) ? 1 : 0;
    }
    *len = read_reached(s.notes_end);
    release(&s);
    return died ? 1u : 0u;
}

/** @brief
 *  \~english The length of the instruction heading this subtree.
 *  \~spanish La longitud de la instruccion que encabeza este subarbol. \~ */
static u32 head_length(const char *exe, const isa_work *lost, u32 timeout_ms,
                       isa_work *probe) {
    u32 len = 0;
    *probe = *lost;
    probe->depth = (u8)((u32)lost->fixed + 1u);
    (void)probe_one(exe, probe, timeout_ms, &len);
    return len;
}

/**
 * @brief
 * \~english Does the byte about to be opened decide anything?  Asked, not assumed.
 * \~spanish Decide algo el byte que se va a abrir?  Preguntado, no supuesto.
 * \~
 *
 * @return \~english 1 if it decides (so the subtree must be split), 0 if four values behaved alike, 2 if it could not be asked \~spanish 1 si decide (asi que hay que partir el subarbol), 0 si cuatro valores se portaron igual, 2 si no se pudo preguntar \~
 *
 * \~english
 * WHY THIS QUESTION IS THE EXPENSIVE ONE TO GET WRONG.  A subtree that kills gets split
 * byte by byte until a single candidate is named -- and when the byte being split on is
 * an immediate, every one of its 256 values gives the same answer and the sweep reports
 * one instruction 65.536 times.  Measured at depth three: `enter imm16, imm8` and
 * `ret imm16` between them accounted for 93% of all findings.
 *
 * BUT THE BYTE CANNOT BE ASSUMED HARMLESS, and that is why this samples instead of
 * skipping.  If it happens to be a ModRM it chooses the operand, and choosing is
 * exactly what decides: `FF C4` is `inc esp` and takes the process down, `FF C0` is
 * `inc eax` and does not.  It showed up in the data -- `C1` produced 2048 findings,
 * which is 8 ModRM values times 256 immediates, not 65.536.
 *
 * WHAT IT PROVES AND WHAT IT DOES NOT.  It establishes that four chosen values of ONE
 * byte behave alike.  It does not establish that the 252 others do, nor that the bytes
 * after it are harmless -- for that it leans on the encoding order of x86, where a
 * ModRM and a SIB come BEFORE any displacement or immediate, so a byte that turned out
 * not to select anything is not followed by one that does.  That is an assumption about
 * the architecture, not a measurement, and it is why what this yields is counted as
 * `sampled` and never as `covered`.
 *
 * \~spanish
 * POR QUE ESTA PREGUNTA ES LA CARA DE FALLAR.  Un subarbol que mata se parte byte a byte
 * hasta nombrar una sola candidata -- y cuando el byte por el que se parte es un
 * inmediato, sus 256 valores dan la misma respuesta y el barrido informa de una
 * instruccion 65.536 veces.  Medido a profundidad tres: `enter imm16, imm8` y
 * `ret imm16` se llevaban entre las dos el 93% de los hallazgos.
 *
 * PERO EL BYTE NO SE PUEDE SUPONER INOFENSIVO, y por eso esto muestrea en vez de
 * saltarse.  Si resulta ser un ModRM elige el operando, y elegir es exactamente lo que
 * decide: `FF C4` es `inc esp` y se lleva el proceso, `FF C0` es `inc eax` y no.  Salio
 * en los datos -- `C1` produjo 2048 hallazgos, que son 8 valores de ModRM por 256
 * inmediatos, y no 65.536.
 *
 * QUE DEMUESTRA Y QUE NO.  Establece que cuatro valores elegidos de UN byte se portan
 * igual.  No establece que los otros 252 lo hagan, ni que los bytes de despues sean
 * inofensivos -- para eso se apoya en el orden de codificacion de x86, donde un ModRM y
 * un SIB van ANTES de cualquier desplazamiento o inmediato, asi que a un byte que
 * resulto no seleccionar nada no le sigue uno que si.  Esa es una suposicion sobre la
 * arquitectura y no una medida, y es la razon de que lo que sale de aqui se cuente como
 * `sampled` y nunca como `covered`.
 */
static u32 byte_decides(const char *exe, const isa_work *lost, u32 timeout_ms,
                        u32 *common_len, u64 *spent) {
    static const u8 values[ISA_POOL_SAMPLES] = {0x00u, 0x55u, 0xAAu, 0xFFu};
    u32 first_len = 0;
    u32 i;

    for (i = 0; i < ISA_POOL_SAMPLES; ++i) {
        isa_work w;
        u32 len = 0;
        u32 died;

        w = *lost;
        w.prefix[lost->fixed] = lost->lo;
        w.fixed = (u8)((u32)lost->fixed + 1u);
        w.lo = values[i];
        w.hi = values[i];
        w.depth = (u8)((u32)w.fixed + 1u);

        died = probe_one(exe, &w, timeout_ms, &len);
        *spent += 1u;
        if (died == 2u) {
            return 2u;
        }
        /* \~english One that survives is a byte that decides: the subtree is not all
         * the same thing.  \~spanish Una que sobreviva es un byte que decide: el
         * subarbol no es todo lo mismo. \~ */
        if (died == 0u) {
            return 1u;
        }
        if (i == 0) {
            first_len = len;
        } else if (len != first_len) {
            /* \~english Different lengths from different values: the byte is part of
             * what is being decoded, not a value inside it.  \~spanish Longitudes
             * distintas con valores distintos: el byte es parte de lo que se
             * decodifica, no un valor de dentro. \~ */
            return 1u;
        }
    }
    *common_len = first_len;
    return 0u;
}

/** @brief
 *  \~english How many candidates a subtree holds, saturating.
 *  \~spanish Cuantas candidatas tiene un subarbol, saturando. \~ */
static u64 subtree_size(const isa_work *w) {
    u64 n = 1;
    u32 k = (u32)w->depth - 1u - (u32)w->fixed;
    u32 j;
    for (j = 0; j < k && n <= (u64)0xFFFFFFFFFFFFFFFFull / 256ull; ++j) {
        n *= 256ull;
    }
    return n;
}

/**
 * @brief
 * \~english Says what a lethal subtree is like, instead of listing it.
 * \~spanish Dice como es un subarbol letal, en vez de listarlo.
 * \~
 *
 * @return \~english 1 if it could be characterised, 0 if not even that worked \~spanish 1 si se pudo caracterizar, 0 si ni eso salio \~
 *
 * \~english
 * THE CLAIM IS DELIBERATELY WEAKER THAN THE OTHER TWO COLLAPSES, and the numbers go
 * out with it so nobody has to take it on trust: `tried` values of the next byte were
 * run and `killed` of them took the worker down.  It does not say the other 240
 * behave the same way, and where all of them killed it does not say the subtree holds
 * nothing else.
 *
 * WHAT IT BUYS is the difference between minutes and hours.  A lethal subtree listed
 * exhaustively costs one process per lethal candidate plus the bisection above it --
 * around 196.000 for `ret imm16` alone, where 65.280 of 65.536 candidates really do
 * kill.  Characterising it costs sixteen.
 *
 * AND WHAT IT IS FOR is not the answer but the pointing: a subtree that comes out
 * mixed -- some values kill, some do not -- is exactly where the exhaustive mode
 * earns what it costs, and a uniform one is where it would be wasted.
 *
 * \~spanish
 * LA AFIRMACION ES A PROPOSITO MAS FLOJA QUE LOS OTROS DOS COLAPSOS, y los numeros
 * salen con ella para que nadie tenga que creersela: se corrieron `tried` valores del
 * byte siguiente y `killed` de ellos se llevaron al trabajador.  No dice que los otros
 * 240 se porten igual, y donde mataron todos no dice que el subarbol no tenga nada mas.
 *
 * LO QUE COMPRA es la diferencia entre minutos y horas.  Un subarbol letal listado
 * exhaustivamente cuesta un proceso por candidata letal mas la biseccion de encima --
 * unos 196.000 para `ret imm16` solo, donde 65.280 de 65.536 candidatas matan de
 * verdad.  Caracterizarlo cuesta dieciseis.
 *
 * Y PARA LO QUE SIRVE no es para responder sino para senalar: un subarbol que sale
 * mezclado -- unos valores matan y otros no -- es justo donde el modo exhaustivo se
 * gana lo que cuesta, y uno uniforme es donde se desperdiciaria.
 */
static int characterise(const char *exe, const isa_work *lost, u32 timeout_ms,
                        isa_worker_outcome *o, u64 *spent) {
    u32 i;
    u32 killed = 0;
    u32 asked = 0;
    u32 longest = 0;

    for (i = 0; i < ISA_POOL_PROFILE; ++i) {
        isa_work w;
        u32 len = 0;
        u32 died;
        /* \~english Evenly spread over the byte, which is what makes the sample land
         * on different register selections instead of all on one.  \~spanish
         * Repartidos por el byte, que es lo que hace que la muestra caiga en
         * selecciones de registro distintas en vez de todas en una. \~ */
        u32 value = (i * 255u) / (ISA_POOL_PROFILE - 1u);

        w = *lost;
        w.prefix[lost->fixed] = lost->lo;
        w.fixed = (u8)((u32)lost->fixed + 1u);
        w.lo = (u8)value;
        w.hi = (u8)value;
        w.depth = (u8)((u32)w.fixed + 1u);

        died = probe_one(exe, &w, timeout_ms, &len);
        *spent += 1u;
        if (died == 2u) {
            continue; /* no se pudo preguntar por este valor */
        }
        asked += 1u;
        if (died == 1u) {
            killed += 1u;
        }
        if (len > longest) {
            longest = len;
        }
    }
    if (asked == 0) {
        return 0;
    }
    o->tried = asked;
    o->killed = killed;
    if (longest != 0) {
        o->reached = longest;
    }
    return 1;
}

static void resolve_lost(slot *s, u32 how, isa_queue *q,
                         isa_pool_report_fn report, void *ctx,
                         isa_pool_stats *st, const char *exe, u32 mode,
                         u32 timeout_ms) {
    isa_tally t;
    isa_work single;
    isa_worker_outcome o;

    memset(&t, 0, sizeof(t));
    memset(&o, 0, sizeof(o));
    o.how = how;
    if (how == (u32)ISA_WORKER_DIED) {
        st->died += 1u;
    } else {
        st->hung += 1u;
    }
    o.reached = read_reached(s->notes_end);

    /*
     * \~english BEFORE DEEPENING, TWO QUESTIONS, and only when the subtree has come down
     * to one value of one byte -- above that the bisection is still cheap and the
     * questions would be about a range rather than about an instruction.
     *
     *   how long is the instruction     if it ends at or before the frozen bytes, the
     *   at the head                     byte about to be opened is never read, and the
     *                                   whole subtree is that one instruction.  Proved
     *   does the byte decide            if not, the byte falls INSIDE the instruction.
     *   anything                        Four values of it get tried: all alike, and the
     *                                   subtree is one instruction with different
     *                                   operand bytes.  Sampled, which is weaker, and
     *                                   counted separately for that reason
     *
     * \~spanish ANTES DE PROFUNDIZAR, DOS PREGUNTAS, y solo cuando el subarbol ya se ha
     * reducido a un valor de un byte -- por encima de eso la biseccion sigue siendo
     * barata y las preguntas serian sobre un rango y no sobre una instruccion.
     *
     *   cuanto mide la instruccion    si acaba en los bytes congelados o antes, el byte
     *   que lo encabeza               que se va a abrir no lo lee nadie, y el subarbol
     *                                 entero es esa instruccion.  Demostrado
     *   decide algo el byte           si no, el byte cae DENTRO de la instruccion.  Se
     *                                 prueban cuatro valores: todos iguales, y el
     *                                 subarbol es una instruccion con otros bytes de
     *                                 operando.  Muestreado, que es mas flojo, y se
     *                                 cuenta aparte por eso
     */
    if (s->work.lo == s->work.hi &&
        (u32)s->work.fixed + 1u < (u32)s->work.depth) {
        isa_work probe;
        u32 len = head_length(exe, &s->work, timeout_ms, &probe);

        st->probes += 1u;
        if (len != 0 && len <= (u32)s->work.fixed + 1u) {
            st->named += 1u;
            o.stands_for = subtree_size(&s->work) - 1ull;
            o.reached = len;
            st->covered += o.stands_for;
            if (report != 0) {
                report(ctx, &probe, &o, &t);
            }
            release(s);
            return;
        }
        if (len != 0) {
            u32 common = 0;
            if (byte_decides(exe, &s->work, timeout_ms, &common,
                             &st->samples) == 0u) {
                st->named += 1u;
                o.stands_for = subtree_size(&s->work) - 1ull;
                o.reached = (common != 0) ? common : len;
                o.by_sampling = 1u;
                st->sampled += o.stands_for;
                if (report != 0) {
                    report(ctx, &probe, &o, &t);
                }
                release(s);
                return;
            }
        }

        /*
         * \~english AND HERE IS WHERE THE MODE DECIDES.  The byte does decide
         * something, so the subtree is not one finding -- listing it is honest and
         * costly, characterising it is cheap and says less.  Which of those the run
         * promised is not something to work out from the data; it was chosen before it
         * started.
         *
         * \~spanish Y AQUI ES DONDE DECIDE EL MODO.  El byte decide algo, asi que el
         * subarbol no es un hallazgo -- listarlo es honesto y caro, caracterizarlo es
         * barato y dice menos.  Cual de las dos cosas prometio la corrida no es algo
         * que deducir de los datos; se eligio antes de empezar.
         */
        if (mode == (u32)ISA_MODE_CHARACTERISE) {
            if (characterise(exe, &s->work, timeout_ms, &o, &st->samples)) {
                st->named += 1u;
                o.stands_for = subtree_size(&s->work) - 1ull;
                o.by_sampling = 1u;
                st->sampled += o.stands_for;
                if (report != 0) {
                    report(ctx, &probe, &o, &t);
                }
                release(s);
                return;
            }
        }
    }

    if (isa_queue_refine(q, &s->work, &single)) {
        st->named += 1u;
    }
    if (report != 0) {
        report(ctx, &s->work, &o, &t);
    }
    release(s);
}

status isa_pool_run(const char *exe, u32 mode, const char *ref_path,
                    u32 workers, u32 timeout_ms, isa_queue *q,
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
            if (!spawn(exe, &w, ref_path, timeout_ms, &slots[i])) {
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
                        isa_worker_outcome o;
                        memset(&o, 0, sizeof(o));
                        o.how = (u32)ISA_WORKER_DONE;
                        report(ctx, &s->work, &o, &probe);
                    }
                    release(s);
                } else {
                    resolve_lost(s, (u32)ISA_WORKER_DIED, q, report, ctx, stats,
                                 exe, mode, timeout_ms);
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
                resolve_lost(s, (u32)ISA_WORKER_HUNG, q, report, ctx, stats, exe,
                             mode, timeout_ms);
            }
        }
    }
    return OK;
}

/**
 * @brief
 * \~english One candidate, measured out loud so a death still says its length.
 * \~spanish Una candidata, medida en voz alta para que una muerte diga aun su
 *           longitud.
 * \~
 *
 * \~english
 * IT DOES NOT USE THE SWEEP, and that is the point.  A sweep asks "what is in this
 * subtree"; this asks "how long is this one and how far does it get", which is a
 * different question and needs the count of offered bytes visible from outside.
 * `isa_measure` slides that count privately -- rightly, it is nobody's business --
 * so the loop is written out here instead of drilling a hole through the oracle.
 *
 * AND IT ANNOUNCES BEFORE, NOT AFTER.  A candidate that kills the process cannot
 * report anything afterwards; the last thing said before the silence is the answer.
 * The flush is not optional for the same reason: buffered output dies with the
 * buffer.
 *
 * \~spanish
 * NO USA EL BARRIDO, y en eso esta la gracia.  Un barrido pregunta "que hay en este
 * subarbol"; esto pregunta "cuanto mide esta y hasta donde llega", que es otra
 * pregunta y necesita que el numero de bytes ofrecidos se vea desde fuera.
 * `isa_measure` desliza ese numero en privado -- y con razon, no es asunto de nadie --,
 * asi que el bucle se escribe aqui en vez de abrirle un agujero al oraculo.
 *
 * Y ANUNCIA ANTES, NO DESPUES.  Una candidata que mata al proceso no puede informar
 * de nada despues; lo ultimo dicho antes del silencio es la respuesta.  El vaciado no
 * es opcional por lo mismo: la salida con bufer muere con el bufer.
 */
static int worker_one(const isa_arena *a, const isa_work *w, const isa_ref *ref,
                      isa_tally *t) {
    u8 bytes[ISA_MAX_LEN];
    u32 i;
    u32 n;

    for (i = 0; i < ISA_MAX_LEN; ++i) {
        bytes[i] = 0;
    }
    for (i = 0; i < (u32)w->fixed; ++i) {
        bytes[i] = w->prefix[i];
    }
    bytes[w->fixed] = w->lo;

    for (n = 1; n <= ISA_MAX_LEN; ++n) {
        isa_result r;

        fprintf(stderr, "n=%u\n", (unsigned)n);
        fflush(stderr);

        if (isa_attempt(a, bytes, n, &r) != OK) {
            return 1;
        }
        if (r.outcome == (u32)ISA_TRUNCATED) {
            continue;
        }
        /* \~english Survived: it is an ordinary answer after all, and the tally says
         * so in the same shape a sweep would.  \~spanish Sobrevivio: al final es una
         * respuesta normal, y la cuenta lo dice con la misma forma que tendria en un
         * barrido. \~ */
        t->candidates = 1u;
        if (r.outcome < ISA_OUTCOME_COUNT) {
            t->by_outcome[r.outcome] = 1u;
        }
        if (r.length >= 1u && r.length <= ISA_MAX_LEN) {
            t->by_length[r.length] = 1u;
        }
        /* \~english The SAME rule the sweep uses, asked of the same function.  This path
         * had no comparison at all for one run, and the exhaustive mode reported 256
         * candidates tried with zero of every quadrant.  \~spanish La MISMA regla que usa
         * el barrido, preguntada a la misma funcion.  Este camino no tenia comparacion
         * ninguna durante una corrida, y el modo exhaustivo informo de 256 candidatas
         * probadas con cero de cada cuadrante. \~ */
        if (ref != 0) {
            isa_quadrant(t, &r, isa_ref_length(ref, bytes));
        }
        return 0;
    }
    /* Quince bytes y seguia pidiendo: se queda como corta, que es la verdad. */
    t->candidates = 1u;
    t->truncated = 1u;
    t->by_outcome[ISA_TRUNCATED] = 1u;
    return 0;
}

u8 *isa_ref_load(const char *path, isa_ref *out) {
    FILE *f;
    long size;
    u8 *buf;

    if (path == 0 || out == 0) {
        return 0;
    }
    f = fopen(path, "rb");
    if (f == 0) {
        printf("no se puede abrir la referencia: %s\n", path);
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (size = ftell(f)) <= 0 ||
        fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        printf("la referencia no se puede medir: %s\n", path);
        return 0;
    }
    buf = (u8 *)malloc((size_t)size);
    if (buf == 0) {
        fclose(f);
        printf("no cabe la referencia en memoria\n");
        return 0;
    }
    if (fread(buf, 1, (size_t)size, f) != (size_t)size) {
        fclose(f);
        free(buf);
        printf("la referencia se leyo a medias\n");
        return 0;
    }
    fclose(f);
    if (isa_ref_parse(buf, (u64)size, out) != OK) {
        free(buf);
        printf("eso no es una tabla de referencia de las nuestras: %s\n", path);
        return 0;
    }
    return buf;
}

void isa_quiet_death(void) {
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX |
                 SEM_NOOPENFILEERRORBOX);
}

int isa_worker_main(const isa_work *w, const char *ref_path) {
    isa_arena arena;
    isa_probe_ops ops;
    isa_tally t;
    isa_ref ref;
    u8 *ref_buf = 0;

    memset(&ref, 0, sizeof(ref));

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
    isa_quiet_death();

    /* \~english Raw bytes go out, so the stream must not translate line endings: a
     * tally with a 0x0A byte in it would arrive one byte longer and be read as a
     * short one.  \~spanish Salen bytes crudos, asi que el flujo no debe traducir
     * fines de linea: una cuenta con un byte 0x0A dentro llegaria un byte mas larga y
     * se leeria como corta. \~ */
    _setmode(_fileno(stdout), _O_BINARY);

    /*
     * \~english THE TABLE IS LOADED AND ITS DEPTH CHECKED, and a mismatch is refused
     * rather than compared around.  A table built for another depth would be indexed with
     * numbers it does not have, and the run would come back full of confident
     * disagreements that are nothing but the wrong file.  Findings that loud and that
     * wrong are worse than none, so this exits instead.
     *
     * \~spanish LA TABLA SE CARGA Y SE LE COMPRUEBA LA PROFUNDIDAD, y un desajuste se
     * rechaza en vez de comparar de todas formas.  Una tabla hecha para otra profundidad
     * se indexaria con numeros que no tiene, y la corrida volveria llena de desacuerdos
     * con aplomo que no son mas que el fichero equivocado.  Hallazgos tan ruidosos y tan
     * falsos son peores que ninguno, asi que esto sale.
     */
    if (ref_path != 0) {
        ref_buf = isa_ref_load(ref_path, &ref);
        if (ref_buf == 0) {
            return 1;
        }
        if (ref.depth != (u32)w->depth) {
            free(ref_buf);
            return 1;
        }
    }

    if (isa_arena_open(&arena) != OK) {
        free(ref_buf);
        return 1;
    }

    /* \~english A single candidate is a different question, asked out loud.
     * \~spanish Una sola candidata es otra pregunta, y se hace en voz alta. \~ */
    if ((u32)w->fixed + 1u == (u32)w->depth && w->lo == w->hi) {
        memset(&t, 0, sizeof(t));
        if (worker_one(&arena, w, (ref_buf != 0) ? &ref : 0, &t) != 0) {
            isa_arena_close(&arena);
            free(ref_buf);
            return 1;
        }
        fwrite(&t, 1, sizeof(t), stdout);
        fflush(stdout);
        isa_arena_close(&arena);
        free(ref_buf);
        return 0;
    }

    isa_arena_ops(&arena, &ops);
    if (isa_sweep(&ops, w, (ref_buf != 0) ? &ref : 0, &t) != OK) {
        isa_arena_close(&arena);
        free(ref_buf);
        return 1;
    }
    fwrite(&t, 1, sizeof(t), stdout);
    fflush(stdout);
    isa_arena_close(&arena);
    free(ref_buf);
    return 0;
}
