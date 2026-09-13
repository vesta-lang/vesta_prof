/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file isa_win.c
 * @brief
 * \~english The Windows half of `common/isa/trial.h`: the pages and the net.
 * \~spanish La mitad de Windows de `common/isa/trial.h`: las paginas y la red.
 * \~
 *
 * \~english
 * TWO THINGS LIVE HERE and both are the system's, which is why they are not in
 * `common/`: reserving a pair of pages where the second one is never mapped, and
 * turning an exception record into a verdict.
 *
 * THE FILTER IS IN C AND NOT IN ASSEMBLY, which is the difference from
 * `msr_guard.S`.  That one's filter ignores its arguments and always says "handle
 * it", because all it needed to know was that something failed.  Here the whole
 * answer is INSIDE those arguments -- which exception, at which address, and above
 * all which instruction pointer -- so the filter has to read them, and reading a
 * structure is C's job.
 *
 * ONE ARENA PER WORKER, AND IT IS WHAT MAKES THIS PARALLELISABLE.  Nothing here
 * is shared: the pages are per arena, and what the filter writes down goes to
 * thread-local storage.  There is no lock and no coordination, and that is not an
 * optimisation -- a process-wide vectored handler would have to work out WHICH
 * worker faulted, whereas a scope table is resolved by the faulting address, so by
 * frame, so by thread.
 *
 * \~spanish
 * AQUI VIVEN DOS COSAS y las dos son del sistema, que es por lo que no estan en
 * `common/`: reservar una pareja de paginas donde la segunda no se mapea nunca, y
 * convertir un registro de excepcion en un veredicto.
 *
 * EL FILTRO VA EN C Y NO EN ENSAMBLADOR, que es la diferencia con `msr_guard.S`.
 * El de alli ignora sus argumentos y siempre dice "encargate", porque lo unico que
 * necesitaba saber era que algo habia fallado.  Aqui la respuesta entera esta
 * DENTRO de esos argumentos -- que excepcion, en que direccion, y sobre todo que
 * puntero de instruccion --, asi que el filtro tiene que leerlos, e interpretar
 * una estructura es trabajo de C.
 *
 * UNA ARENA POR TRABAJADOR, Y ES LO QUE HACE ESTO PARALELIZABLE.  Aqui no se
 * comparte nada: las paginas son de cada arena, y lo que el filtro apunta va a
 * almacenamiento local del hilo.  No hay cerrojo ni coordinacion, y eso no es una
 * optimizacion -- un manejador vectorizado del proceso entero tendria que averiguar
 * QUE trabajador fallo, mientras que una tabla de ambitos se resuelve por la
 * direccion que fallo, o sea por marco, o sea por hilo.
 */

#include "isa_win.h"

#include <windows.h>

/**
 * @brief
 * \~english Where the filter leaves what it saw, one per thread.
 * \~spanish Donde el filtro deja lo que vio, uno por hilo.
 * \~
 *
 * \~english
 * It is a global because a scope-table filter takes what
 * `__C_specific_handler` gives it and nothing else -- there is no room to pass a
 * destination.  Thread-local is what keeps that from being a shared mutable
 * variable, which is the whole reason the workers need no lock.
 *
 * \~spanish
 * Es global porque un filtro de tabla de ambitos recibe lo que le da
 * `__C_specific_handler` y nada mas -- no hay sitio para pasarle un destino.  Que
 * sea local del hilo es lo que evita que eso sea una variable compartida
 * mutable, que es toda la razon de que los trabajadores no necesiten cerrojo.
 */
typedef struct seen_fault {
    u32 code;
    u64 rip;
    u64 address;
    u64 kind; /**< \~english 0 read, 1 write, 8 execute \~spanish 0 leer, 1 escribir, 8 ejecutar \~ */
    isa_state after;
    int seen;
} seen_fault;

static _Thread_local seen_fault g_seen;

/*
 * \~english The layout `asm/x86_64/isa_trial.S` assumes when it loads the
 * registers.  It works out the offsets as multiples of eight, so what has to hold
 * is that `isa_state` really is sixteen 64-bit words followed by the flags.
 *
 * Pinned here and not trusted: getting it wrong does not fail to build and does
 * not fault -- the candidate would start from registers loaded out of the wrong
 * places, and the effects reported would be of an instruction nobody ran.
 *
 * \~spanish La disposicion que `asm/x86_64/isa_trial.S` da por hecha al cargar los
 * registros.  Calcula los desplazamientos como multiplos de ocho, asi que lo que
 * tiene que cumplirse es que `isa_state` sea de verdad dieciseis palabras de 64
 * bits seguidas de las banderas.
 *
 * Se fija aqui y no se confia: equivocarse no falla al construir ni da excepcion
 * -- el candidato arrancaria de registros cargados de los sitios equivocados, y los
 * efectos reportados serian de una instruccion que nadie ejecuto.
 */
STATIC_ASSERT(sizeof(u64) == 8, "isa_state assumes 64-bit words");
/* \~english `gpr` is the first member, so it is at offset zero by definition, and
 * no padding can appear between words that are all `u64`.  So this one size pins
 * everything the assembly works out: the array at zero and the flags right after
 * it, at 8 * ISA_GPR_COUNT.
 * \~spanish `gpr` es el primer miembro, asi que esta en el desplazamiento cero por
 * definicion, y entre palabras que son todas `u64` no puede aparecer relleno.  Asi
 * que este unico tamano fija todo lo que el ensamblador calcula: el array en cero
 * y las banderas justo detras, en 8 * ISA_GPR_COUNT. \~ */
STATIC_ASSERT(sizeof(isa_state) == 8 * (ISA_GPR_COUNT + 1),
              "isa_state must be 16 words plus the flags, with no padding");
STATIC_ASSERT(ISA_STACK_REG == 4, "the stack pointer is register number 4");

/**
 * @brief
 * \~english The filter `__C_specific_handler` calls.  Records and hands over.
 * \~spanish El filtro que llama `__C_specific_handler`.  Apunta y cede.
 * \~
 *
 * @return \~english always 1, `EXCEPTION_EXECUTE_HANDLER` \~spanish siempre 1, `EXCEPTION_EXECUTE_HANDLER` \~
 *
 * \~english
 * IT NEVER DECIDES ANYTHING, and that is deliberate.  A filter could say "not
 * mine, keep looking", and here that would be wrong: an attempt whose exception
 * nobody expected is precisely the interesting case, and declining it would send
 * it to whoever is next -- in practice, to the process dying.  Everything is taken
 * and the classification happens afterwards, in the open.
 *
 * THE ORDER OF THE FIELDS MATTERS.  `NumberParameters` is checked before reading
 * the two parameters: not every exception carries them, and an illegal
 * instruction carries none.  Reading them anyway would give the address and type
 * of whatever was left there from a previous exception -- a plausible value, which
 * is the worst kind of wrong.
 *
 * \~spanish
 * NUNCA DECIDE NADA, y es deliberado.  Un filtro podria decir "no es mio, sigue
 * buscando", y aqui eso seria un error: un intento cuya excepcion nadie esperaba
 * es justamente el caso interesante, y rechazarlo lo mandaria al siguiente -- en
 * la practica, a que el proceso muera.  Se coge todo y la clasificacion se hace
 * despues, a la vista.
 *
 * EL ORDEN DE LOS CAMPOS IMPORTA.  Se comprueba `NumberParameters` antes de leer
 * los dos parametros: no todas las excepciones los llevan, y una instruccion
 * ilegal no lleva ninguno.  Leerlos igualmente daria la direccion y el tipo de lo
 * que hubiera quedado ahi de una excepcion anterior -- un valor plausible, que es
 * la peor forma de estar equivocado.
 */
LONG isa_trial_filter(EXCEPTION_POINTERS *p, void *frame) {
    const CONTEXT *c;
    (void)frame;

    g_seen.seen = 1;
    g_seen.code = (u32)p->ExceptionRecord->ExceptionCode;
    g_seen.address = 0;
    g_seen.kind = 0;
    if (p->ExceptionRecord->NumberParameters >= 2) {
        g_seen.kind = (u64)p->ExceptionRecord->ExceptionInformation[0];
        g_seen.address = (u64)p->ExceptionRecord->ExceptionInformation[1];
    }

    /* \~english The context comes for free with the exception, and for a candidate
     * that ran it IS the state of the machine afterwards.  No extra mechanism:
     * the same trick that measures the length delivers this.
     * \~spanish El contexto viene gratis con la excepcion, y para un candidato que
     * corrio ES el estado de la maquina despues.  Ningun mecanismo extra: lo
     * entrega el mismo truco que mide la longitud. \~ */
    c = p->ContextRecord;
    g_seen.rip = (u64)c->Rip;
    g_seen.after.gpr[0] = (u64)c->Rax;
    g_seen.after.gpr[1] = (u64)c->Rcx;
    g_seen.after.gpr[2] = (u64)c->Rdx;
    g_seen.after.gpr[3] = (u64)c->Rbx;
    g_seen.after.gpr[4] = (u64)c->Rsp;
    g_seen.after.gpr[5] = (u64)c->Rbp;
    g_seen.after.gpr[6] = (u64)c->Rsi;
    g_seen.after.gpr[7] = (u64)c->Rdi;
    g_seen.after.gpr[8] = (u64)c->R8;
    g_seen.after.gpr[9] = (u64)c->R9;
    g_seen.after.gpr[10] = (u64)c->R10;
    g_seen.after.gpr[11] = (u64)c->R11;
    g_seen.after.gpr[12] = (u64)c->R12;
    g_seen.after.gpr[13] = (u64)c->R13;
    g_seen.after.gpr[14] = (u64)c->R14;
    g_seen.after.gpr[15] = (u64)c->R15;
    g_seen.after.flags = (u64)c->EFlags;

    return 1;
}

/* \~english Declared ahead because opening the arena calibrates itself with it,
 * and it is defined further down next to the rest of the attempt machinery.  It
 * has to be THIS one and not `isa_attempt`: see its own comment.
 * \~spanish Declarada por delante porque abrir la arena se calibra con ella, y
 * esta definida mas abajo junto al resto de la maquinaria de los intentos.  Tiene
 * que ser ESTA y no `isa_attempt`: ver su propio comentario. \~ */
static status raw_attempt_with(const isa_arena *a, const u8 *bytes, u32 n,
                               const isa_state *in, isa_result *out,
                               u64 *rsp_out);

status isa_arena_open(isa_arena *a) {
    SYSTEM_INFO si;
    u8 *base;
    u8 *first;

    if (a == 0) {
        return ERR_INVALID;
    }
    a->base = 0;
    a->first = 0;
    a->boundary = 0;
    a->page = 0;

    GetSystemInfo(&si);
    if (si.dwPageSize == 0) {
        return ERR_STATE;
    }

    /* \~english Two pages RESERVED in one go, so they are guaranteed adjacent.
     * Reserving them separately could put something between them, and then the
     * boundary would not be a boundary -- the oracle would measure nothing and say
     * it measured something.
     * \~spanish Dos paginas RESERVADAS de una vez, para que queden garantizadamente
     * contiguas.  Reservarlas por separado podria meter algo en medio, y entonces
     * la frontera no seria una frontera -- el oraculo no mediria nada y diria que
     * midio algo. \~ */
    base = (u8 *)VirtualAlloc(0, (SIZE_T)si.dwPageSize * 2, MEM_RESERVE,
                              PAGE_NOACCESS);
    if (base == 0) {
        return ERR_NOSPACE;
    }
    /* \~english Only the FIRST one is committed, and the second is left reserved
     * for good: that it has no memory is not a leftover, it is the instrument.
     * \~spanish Solo se confirma la PRIMERA, y la segunda se deja reservada para
     * siempre: que no tenga memoria no es un descuido, es el instrumento. \~ */
    first = (u8 *)VirtualAlloc(base, si.dwPageSize, MEM_COMMIT,
                               PAGE_EXECUTE_READWRITE);
    if (first == 0) {
        VirtualFree(base, 0, MEM_RELEASE);
        return ERR_NOSPACE;
    }

    a->base = base;
    a->first = first;
    a->boundary = base + si.dwPageSize;
    a->page = (u32)si.dwPageSize;

    /* \~english And the calibration, which is also the first check that any of
     * this works.  A `nop` is one byte, it changes nothing, and what it leaves in
     * the stack pointer is what the mechanism leaves -- the return address the call
     * pushed.  From here on, a candidate whose stack pointer differs from this
     * moved the stack itself.
     *
     * If the `nop` does not come out as `ISA_RAN`, the apparatus is broken and the
     * arena refuses to open.  Far better here than as a million plausible answers.
     *
     * \~spanish Y la calibracion, que es ademas la primera comprobacion de que algo
     * de esto funciona.  Un `nop` mide un byte, no cambia nada, y lo que deja en el
     * puntero de pila es lo que deja el mecanismo -- la direccion de retorno que
     * empujo la llamada.  A partir de aqui, un candidato cuyo puntero de pila
     * difiera de este movio la pila el mismo.
     *
     * Si el `nop` no sale como `ISA_RAN`, el aparato esta roto y la arena se niega
     * a abrir.  Mucho mejor aqui que en forma de un millon de respuestas
     * plausibles. \~ */
    {
        const u8 nop = 0x90;
        isa_state zero;
        isa_result plain;
        isa_result with;
        u32 i;
        int bad;

        for (i = 0; i < sizeof(zero); ++i) {
            ((char *)&zero)[i] = 0;
        }

        /* \~english A `nop` measured, and the same `nop` from an all-zero state.
         * There is ONE guarded path now -- measuring loads a state too, since
         * measuring without one corrupts the process doing it -- so what the second
         * attempt adds is not another path but another state: all zeros is a very
         * different thing to load than the value measuring uses, and the assembly
         * that loads it is the part that would break.
         * \~spanish Un `nop` medido, y el mismo `nop` desde un estado todo a cero.
         * Ahora hay UN camino guardado -- medir tambien carga estado, porque medir
         * sin el corrompe al proceso que mide --, asi que lo que anade el segundo
         * intento no es otro camino sino otro estado: todo a cero es algo muy
         * distinto de cargar que el valor que usa la medida, y el ensamblador que
         * lo carga es la parte que se romperia. \~ */
        bad = (isa_attempt(a, &nop, 1, &plain) != OK ||
               plain.outcome != ISA_RAN);
        if (!bad) {
            bad = (raw_attempt_with(a, &nop, 1, &zero, &with, 0) != OK ||
                   with.outcome != ISA_RAN);
        }
        if (bad) {
            VirtualFree(base, 0, MEM_RELEASE);
            a->base = 0;
            a->first = 0;
            a->boundary = 0;
            a->page = 0;
            return ERR_STATE;
        }
    }
    return OK;
}

void isa_arena_close(isa_arena *a) {
    if (a == 0 || a->base == 0) {
        return;
    }
    VirtualFree(a->base, 0, MEM_RELEASE);
    a->base = 0;
    a->first = 0;
    a->boundary = 0;
    a->page = 0;
}

/**
 * @brief
 * \~english Turns what the filter saw into a verdict.
 * \~spanish Convierte lo que vio el filtro en un veredicto.
 * \~
 *
 * \~english
 * THE HARD CASE IS THE FIRST ONE, and it is the reason the apparatus exists.  An
 * access violation of execute type at the boundary means one of two opposite
 * things, and only the instruction pointer separates them: if it advanced to the
 * boundary the instruction decoded and RAN, and it was the next fetch that fell
 * off the edge; if it stayed on the candidate, the decoder never got going.
 *
 * ANYTHING NOT RECOGNISED COMES OUT AS `ISA_UNKNOWN` and not as invalid.  An
 * attempt whose outcome nobody understands is data, very possibly the interesting
 * kind.
 *
 * \~spanish
 * EL CASO DIFICIL ES EL PRIMERO, y es la razon de ser del aparato.  Una violacion
 * de acceso de tipo ejecucion en la frontera quiere decir una de dos cosas
 * opuestas, y solo el puntero de instruccion las separa: si avanzo a la frontera,
 * la instruccion decodifico y CORRIO, y fue el fetch siguiente el que se cayo por
 * el borde; si se quedo en el candidato, el decodificador no llego a arrancar.
 *
 * LO QUE NO SE RECONOZCA SALE COMO `ISA_UNKNOWN` y no como invalida.  Un intento
 * cuyo resultado nadie entiende es un dato, muy posiblemente del interesante.
 */
static u32 classify(const isa_arena *a, const u8 *candidate) {
    /* \~english 8 is the code Windows uses for an instruction-fetch access.
     * \~spanish 8 es el codigo que usa Windows para un acceso de busqueda de
     * instruccion. \~ */
    const u64 fetch = 8u;

    if (g_seen.code == (u32)STATUS_ACCESS_VIOLATION) {
        if (g_seen.kind == fetch && g_seen.address == (u64)(size_t)a->boundary) {
            if (g_seen.rip == (u64)(size_t)a->boundary) {
                return ISA_RAN;
            }
            if (g_seen.rip == (u64)(size_t)candidate) {
                return ISA_TRUNCATED;
            }
            /* \~english Fetch fault at the boundary from somewhere that is
             * neither: it jumped.  Not classifiable, and worth seeing.
             * \~spanish Fallo de busqueda en la frontera desde un sitio que no es
             * ninguno de los dos: salto.  No clasificable, y digno de verse. \~ */
            return ISA_UNKNOWN;
        }
        /* \~english It ran, and touched data it should not have.
         * \~spanish Corrio, y toco datos que no debia. \~ */
        return ISA_MEMORY_FAULT;
    }
    if (g_seen.code == (u32)STATUS_ILLEGAL_INSTRUCTION) {
        return ISA_INVALID;
    }
    if (g_seen.code == (u32)STATUS_PRIVILEGED_INSTRUCTION) {
        return ISA_PRIVILEGED;
    }
    if (g_seen.code == (u32)STATUS_INTEGER_DIVIDE_BY_ZERO ||
        g_seen.code == (u32)STATUS_INTEGER_OVERFLOW ||
        g_seen.code == (u32)STATUS_FLOAT_DIVIDE_BY_ZERO ||
        g_seen.code == (u32)STATUS_FLOAT_INVALID_OPERATION) {
        return ISA_ARITH;
    }
    return ISA_UNKNOWN;
}

/**
 * @brief
 * \~english The state a measurement runs from: nothing that could be an address.
 * \~spanish El estado desde el que corre una medida: nada que pueda ser una
 *           direccion.
 * \~
 *
 * \~english
 * WITHOUT THIS, MEASURING CORRUPTS THE PROCESS DOING THE MEASURING.  A candidate
 * is arbitrary bytes, and a great many of them touch memory through a register:
 * `00 00` is `add [rax], al`.  Run with whatever the C code happened to leave in
 * `rax`, that writes a byte into our own stack or heap -- it does not fault, it
 * does not fail, it quietly changes something and carries on.  Measured: sweeping
 * the `00` subtree that way, 152 of 256 candidates "ran", each one writing
 * somewhere inside us, and the next subtree killed the process outright.
 *
 * So every register is loaded with a value that has bit 63 set, which is NEVER a
 * valid user-mode address.  A memory operand then faults, and a fault is
 * information: the candidate decoded, the length is known, and nothing of ours was
 * touched.
 *
 * THE STACK POINTER IS NOT ONE OF THEM -- it is the way back, and the arithmetic
 * flags are left alone because this asks about lengths and not about effects.
 * That is also why this is a state of its own rather than the one `fill_state`
 * builds: that one wants values that can be told apart, this one wants values that
 * cannot be dereferenced, and tying the two together would mean a change made for
 * one silently moves the other.
 *
 * WHAT IT DOES NOT SAVE US FROM: a candidate that writes the stack pointer and
 * then faults leaves the unwinder no frame to find, and the process dies wherever
 * it stands.  No choice of register values prevents that, which is why a worker
 * has to be a PROCESS.
 *
 * \~spanish
 * SIN ESTO, MEDIR CORROMPE AL PROCESO QUE MIDE.  Una candidata son bytes
 * arbitrarios, y muchisimas tocan memoria a traves de un registro: `00 00` es
 * `add [rax], al`.  Corrida con lo que el codigo de C hubiera dejado en `rax`, eso
 * escribe un byte en nuestra propia pila o monticulo -- no falla, no da error,
 * cambia algo calladamente y sigue.  Medido: barriendo asi el subarbol de `00`,
 * 152 de 256 candidatas "corrieron", cada una escribiendo en algun sitio de dentro
 * de nosotros, y el subarbol siguiente mato el proceso sin mas.
 *
 * Asi que cada registro se carga con un valor que tiene el bit 63 puesto, que NUNCA
 * es una direccion valida de modo usuario.  Un operando de memoria falla entonces,
 * y un fallo es informacion: la candidata decodifico, la longitud se sabe, y no se
 * toco nada nuestro.
 *
 * EL PUNTERO DE PILA NO ES UNO DE ELLOS -- es la via de vuelta --, y las banderas
 * aritmeticas se dejan en paz porque esto pregunta por longitudes y no por
 * efectos.  Es tambien la razon de que sea un estado propio y no el que construye
 * `fill_state`: ese quiere valores que se distingan, este quiere valores que no se
 * puedan dereferenciar, y atar los dos significaria que un cambio hecho por uno
 * mueve el otro sin decirlo.
 *
 * DE LO QUE NO NOS SALVA: una candidata que escriba el puntero de pila y despues
 * falle deja al desenrollador sin marco que encontrar, y el proceso muere donde
 * este.  Ninguna eleccion de valores de registro evita eso, y por eso un
 * trabajador tiene que ser un PROCESO.
 */
static void measure_state(isa_state *s) {
    u32 i;
    for (i = 0; i < ISA_GPR_COUNT; ++i) {
        s->gpr[i] = 0xF0F0F0F0F0F0F0F0ull;
    }
    /* \~english Only the reserved bit that is always set; nothing else asked for.
     * \~spanish Solo el bit reservado que va siempre puesto; nada mas pedido. \~ */
    s->flags = 0x0002u;
}

status isa_attempt(const isa_arena *a, const u8 *bytes, u32 n, isa_result *out) {
    isa_state in;

    if (a == 0 || a->first == 0 || bytes == 0 || out == 0) {
        return ERR_INVALID;
    }
    if (n == 0 || n > ISA_MAX_LEN) {
        return ERR_INVALID;
    }
    measure_state(&in);
    return raw_attempt_with(a, bytes, n, &in, out, 0);
}

status isa_measure(const isa_arena *a, const u8 *bytes, isa_result *out) {
    u32 n;

    if (a == 0 || bytes == 0 || out == 0) {
        return ERR_INVALID;
    }

    /* \~english Grow the number of bytes made available until the decoder stops
     * asking for more.  `ISA_TRUNCATED` is the only answer that means "give me
     * another byte"; anything else is already the answer, including the ones that
     * say the instruction does not exist.
     *
     * It goes UP from one and not down from fifteen on purpose: the shortest
     * reading is the true one.  Starting wide, a sequence whose first byte is a
     * complete instruction would be reported with the length of whatever came
     * after it.
     *
     * \~spanish Se va ampliando cuantos bytes hay disponibles hasta que el
     * decodificador deja de pedir mas.  `ISA_TRUNCATED` es la unica respuesta que
     * quiere decir "dame otro byte"; cualquier otra ya es la respuesta, incluidas
     * las que dicen que la instruccion no existe.
     *
     * Va de uno HACIA ARRIBA y no de quince hacia abajo a proposito: la lectura
     * mas corta es la verdadera.  Empezando ancho, una secuencia cuyo primer byte
     * ya es una instruccion completa se reportaria con la longitud de lo que
     * viniera detras. \~ */
    for (n = 1; n <= ISA_MAX_LEN; ++n) {
        status rc = isa_attempt(a, bytes, n, out);
        if (rc != OK) {
            return rc;
        }
        if (out->outcome != ISA_TRUNCATED) {
            return OK;
        }
    }

    /* \~english Fifteen bytes and still asking: that is past the architectural
     * limit, so the processor will refuse it however many more are offered.  The
     * last answer stands, and it says `ISA_TRUNCATED`, which is the truth.
     * \~spanish Quince bytes y sigue pidiendo: eso pasa del limite arquitectonico,
     * asi que el procesador lo rechazara por muchos mas que se le ofrezcan.  Vale
     * la ultima respuesta, y dice `ISA_TRUNCATED`, que es la verdad. \~ */
    return OK;
}

/**
 * @brief
 * \~english Which registers came out different from the ones that went in.
 * \~spanish Que registros salieron distintos de los que entraron.
 * \~
 *
 * \~english
 * THE STACK POINTER IS COMPARED AGAINST SOMETHING ELSE, and that is the whole
 * reason the arena calibrates itself.  Every other register is compared with what
 * was loaded into it; this one is compared with what the MECHANISM leaves in it,
 * because calling the candidate pushed a return address.  Comparing it with the
 * input would report that every instruction writes the stack pointer; not
 * comparing it at all would miss every `push` and every `pop`.
 *
 * \~spanish
 * EL PUNTERO DE PILA SE COMPARA CONTRA OTRA COSA, y es la razon entera de que la
 * arena se calibre.  Todos los demas registros se comparan con lo que se les
 * cargo; este se compara con lo que deja el MECANISMO, porque llamar al candidato
 * empujo una direccion de retorno.  Compararlo con la entrada reportaria que toda
 * instruccion escribe el puntero de pila; no compararlo reportaria ningun `push` y
 * ningun `pop`.
 */
static void mark_written(u64 rsp_at_call, const isa_state *in,
                         isa_result *out) {
    u32 i;

    out->wrote_gpr = 0;
    out->wrote_flags = 0;
    for (i = 0; i < ISA_GPR_COUNT; ++i) {
        /* \~english The eight the call itself pushed.  Exact, and independent of
         * how deep the C stack happened to be -- which is what a measured baseline
         * could not be.
         * \~spanish Los ocho que empujo la propia llamada.  Exacto, e independiente
         * de lo profunda que estuviera la pila de C -- que es lo que una linea base
         * medida no podia ser. \~ */
        u64 expected = (i == ISA_STACK_REG) ? (rsp_at_call - 8u) : in->gpr[i];
        if (out->after.gpr[i] != expected) {
            out->wrote_gpr |= (1u << i);
        }
    }
    /* \~english Only the flags that were asked for.  The mask is the one the
     * assembly applies when it loads them: comparing the rest would report a
     * change the system made, not the instruction.
     * \~spanish Solo las banderas que se pidieron.  La mascara es la que aplica el
     * ensamblador al cargarlas: comparar el resto reportaria un cambio del sistema
     * y no de la instruccion. \~ */
    if ((out->after.flags & 0x0CD7u) != (in->flags & 0x0CD7u)) {
        out->wrote_flags = 1;
    }
}

/**
 * @brief
 * \~english One attempt from a chosen state, WITHOUT working out what it wrote.
 * \~spanish Un intento desde un estado elegido, SIN calcular que escribio.
 * \~
 *
 * \~english
 * It exists so the calibration can use it.  Working out the written set needs the
 * baseline for the stack pointer, and measuring that baseline needs an attempt --
 * which would be circular.  Splitting the two breaks the circle, and it does it
 * without anyone having to call a function whose output is half meaningless.
 *
 * AND IT HAS TO BE THIS ONE, NOT `isa_attempt`.  The two guarded functions have
 * DIFFERENT prologues -- this one saves eight registers the other does not -- so
 * they leave different values in the stack pointer.  Calibrating with one and
 * comparing against the other is what made every instruction, `nop` included, look
 * like it wrote the stack pointer.
 *
 * \~spanish
 * Existe para que la calibracion pueda usarlo.  Calcular el conjunto escrito
 * necesita la linea base del puntero de pila, y medir esa linea base necesita un
 * intento -- lo que seria circular.  Partir las dos cosas rompe el circulo, y lo
 * hace sin que nadie tenga que llamar a una funcion cuya salida esta a medias sin
 * significado.
 *
 * Y TIENE QUE SER ESTA, NO `isa_attempt`.  Las dos funciones guardadas tienen
 * prologos DISTINTOS -- esta guarda ocho registros que la otra no -- asi que dejan
 * valores distintos en el puntero de pila.  Calibrar con una y comparar contra la
 * otra es lo que hacia que toda instruccion, `nop` incluido, pareciera escribir el
 * puntero de pila.
 */
static status raw_attempt_with(const isa_arena *a, const u8 *bytes, u32 n,
                               const isa_state *in, isa_result *out,
                               u64 *rsp_out) {
    u8 *candidate;
    u32 i;
    int faulted;
    u64 rsp_at_call = 0;

    if (a == 0 || a->first == 0 || bytes == 0 || in == 0 || out == 0) {
        return ERR_INVALID;
    }
    if (n == 0 || n > ISA_MAX_LEN) {
        return ERR_INVALID;
    }

    candidate = a->boundary - n;
    for (i = 0; i < n; ++i) {
        candidate[i] = bytes[i];
    }

    /* \~english ZEROING IT IS WHAT SAYS "NOBODY HAS ANSWERED", and it says so because
     * `ISA_NOTHING` is zero -- see the enum in `trial.h` for the candidate that made that
     * necessary.  There is nothing else to write here: if the flow never comes back to
     * fill the result in, what the caller reads is the truth.  \~spanish PONERLO A CERO ES
     * LO QUE DICE "NADIE HA RESPONDIDO", y lo dice porque `ISA_NOTHING` vale cero -- ver el
     * enum de `trial.h` para la candidata que lo hizo necesario.  No hay nada mas que
     * escribir aqui: si el flujo no vuelve a rellenar el resultado, lo que lea quien llama
     * es la verdad. \~ */
    for (i = 0; i < sizeof(*out); ++i) {
        ((char *)out)[i] = 0;
    }
    g_seen.seen = 0;
    g_seen.code = 0;
    g_seen.rip = 0;
    g_seen.address = 0;
    g_seen.kind = 0;

    faulted = isa_try_with(candidate, in, &rsp_at_call);
    if (rsp_out != 0) {
        *rsp_out = rsp_at_call;
    }

    if (!faulted) {
        out->outcome = ISA_RETURNED;
        out->length = n;
        return OK;
    }

    out->code = g_seen.code;
    out->rip = g_seen.rip;
    out->fault_at = g_seen.address;
    out->after = g_seen.after;
    out->outcome = classify(a, candidate);
    /*
     * \~english THE LENGTH IS `n` FOR EVERY ANSWER THAT IS NOT "GIVE ME MORE
     * BYTES", and reserving it for the ones that ran was leaving most of the
     * instruction space unmeasured.
     *
     * What the sliding loop in `isa_measure` establishes is this: at `n - 1` bytes
     * the decoder asked for another one, and at `n` it stopped asking.  That means
     * it consumed exactly `n` bytes BEFORE deciding -- and it is the deciding that
     * differs between outcomes, not the consuming.  An instruction that does not
     * exist consumed `n` bytes and then raised #UD; a privileged one consumed `n`
     * and then checked the ring; one with a memory operand consumed `n` and then
     * touched an address.  All three measured the same thing.
     *
     * It matters far more than it looks, because the number fed back here is what
     * the traversal advances on.  With a length only for the ones that ran, every
     * other candidate reported "I know nothing" and the walk skipped NOTHING over
     * them -- which is most of the space, since most byte strings do not exist.
     * The tunneling was switched off exactly where it saves the most.
     *
     * `ISA_TRUNCATED` is the one outcome with no length, and it keeps none: it says
     * the instruction is longer than what was offered and never says by how much.
     *
     * \~spanish LA LONGITUD ES `n` PARA TODA RESPUESTA QUE NO SEA "DAME MAS
     * BYTES", y reservarla para las que corrieron dejaba sin medir casi todo el
     * espacio de instrucciones.
     *
     * Lo que establece el bucle deslizante de `isa_measure` es esto: con `n - 1`
     * bytes el decodificador pidio otro, y con `n` dejo de pedir.  O sea que
     * consumio exactamente `n` bytes ANTES de decidir -- y lo que cambia entre
     * resultados es el decidir, no el consumir.  Una instruccion que no existe
     * consumio `n` bytes y despues levanto #UD; una privilegiada consumio `n` y
     * despues miro el anillo; una con operando de memoria consumio `n` y despues
     * toco una direccion.  Las tres midieron lo mismo.
     *
     * Importa mucho mas de lo que parece, porque el numero que se devuelve aqui es
     * sobre el que avanza el recorrido.  Con longitud solo para las que corrian,
     * cualquier otra candidata informaba "no se nada" y el recorrido no saltaba
     * NADA sobre ellas -- que es casi todo el espacio, porque casi ninguna cadena de
     * bytes existe.  El tunneling quedaba apagado justo donde mas ahorra.
     *
     * `ISA_TRUNCATED` es el unico resultado sin longitud, y sin longitud se queda:
     * dice que la instruccion mide mas que lo que se le ofrecio y nunca dice
     * cuanto mas.
     */
    if (out->outcome != ISA_TRUNCATED) {
        out->length = n;
    }
    return OK;
}

status isa_attempt_with(const isa_arena *a, const u8 *bytes, u32 n,
                        const isa_state *in, isa_result *out) {
    u64 rsp_at_call = 0;
    status rc = raw_attempt_with(a, bytes, n, in, out, &rsp_at_call);
    if (rc != OK) {
        return rc;
    }
    if (out->outcome == ISA_RETURNED) {
        /* \~english Nothing came back from the exception, so there is no state to
         * compare.  The outcome says so and the masks stay at zero.
         * \~spanish No volvio nada de la excepcion, asi que no hay estado que
         * comparar.  El resultado lo dice y las mascaras se quedan a cero. \~ */
        return OK;
    }
    /* \~english The written set is marked whatever the outcome: an instruction
     * that ran and then faulted touching memory HAD effects, and throwing them
     * away because the attempt ended badly would lose them.
     * \~spanish El conjunto escrito se marca sea cual sea el resultado: una
     * instruccion que corrio y despues fallo tocando memoria SI tuvo efectos, y
     * tirarlos porque el intento acabo mal los perderia. \~ */
    mark_written(rsp_at_call, in, out);
    return OK;
}

/**
 * @brief
 * \~english The two input states, and why they look like that.
 * \~spanish Los dos estados de entrada, y por que tienen esa pinta.
 * \~
 *
 * \~english
 * EVERY REGISTER HOLDS A DIFFERENT VALUE, and no register value is shared between
 * the two states.  Both things are needed and for different reasons:
 *
 *   different from each other   with zeros everywhere, "wrote 0" and "wrote
 *                               nothing" are the same observation
 *   different between states    it is what separates a constant from a copy: a
 *                               register that comes out the same from two
 *                               different inputs was written with something that
 *                               does not depend on them
 *
 * The values avoid being valid addresses on purpose: a candidate that dereferences
 * one should fault -- which is information -- rather than happen to read memory
 * that exists and quietly succeed.
 *
 * THE FLAGS ARE THE EXCEPTION and come in the same in both; the reason is right
 * where they are set, below.
 *
 * \~spanish
 * CADA REGISTRO LLEVA UN VALOR DISTINTO, y ningun valor de registro se comparte
 * entre los dos estados.  Las dos cosas hacen falta y por motivos distintos:
 *
 *   distintos entre si       con ceros por todas partes, "escribio 0" y "no
 *                            escribio" son la misma observacion
 *   distintos entre estados  es lo que separa una constante de una copia: un
 *                            registro que sale igual desde dos entradas distintas
 *                            se escribio con algo que no depende de ellas
 *
 * Los valores evitan ser direcciones validas a proposito: un candidato que
 * dereferencie uno deberia fallar -- que es informacion -- en vez de acertar a leer
 * memoria que existe y salir bien sin que se note.
 *
 * LAS BANDERAS SON LA EXCEPCION y entran iguales en los dos; el motivo esta justo
 * donde se ponen, mas abajo.
 */
static void fill_state(isa_state *s, u64 which) {
    u32 i;
    for (i = 0; i < ISA_GPR_COUNT; ++i) {
        /*
         * \~english DIFFERENT IN SIGN AND IN PARITY, not merely different.  The
         * first version changed only a high byte, and that is not enough: an
         * addition of two values whose low bytes agree produces the same carry, the
         * same parity and the same sign, so the flags came out identical from both
         * runs and every arithmetic instruction was reported as writing a CONSTANT.
         *
         * Two things are asked of each value: that bit 63 is set, so it is never a
         * valid user-mode address and a candidate that dereferences it faults --
         * which is information -- instead of happening to read memory that exists;
         * and that the register number is visible in the low bits, so a value that
         * turns up copied into another register can be recognised.
         *
         * \~spanish DISTINTOS EN SIGNO Y EN PARIDAD, no meramente distintos.  La
         * primera version cambiaba solo un byte alto, y eso no basta: una suma de
         * dos valores cuyos bytes bajos coinciden produce el mismo acarreo, la misma
         * paridad y el mismo signo, asi que las banderas salian identicas en las dos
         * corridas y toda instruccion aritmetica se reportaba escribiendo una
         * CONSTANTE.
         *
         * A cada valor se le piden dos cosas: que el bit 63 este puesto, para que
         * nunca sea una direccion valida de modo usuario y un candidato que la
         * dereferencie falle -- lo que es informacion -- en vez de acertar a leer
         * memoria que existe; y que el numero de registro se vea en los bits bajos,
         * para reconocer un valor que aparezca copiado en otro registro.
         */
        u64 tag = (u64)i * 0x11u;
        s->gpr[i] = (which == 0) ? (0xF000000000000000ull | tag)
                                 : (0x8FFFFFFFFFFFFFFFull ^ tag);
    }
    /*
     * \~english THE FLAGS GO IN THE SAME IN BOTH, which is the opposite of what the
     * registers do, and on purpose.  What these two states ask is what the
     * instruction does with the REGISTERS; every flag that differs afterwards has to
     * be its answer to them, and a flag arriving different would be an answer to
     * nothing.  It bit on `xor eax, eax`, reported as depending on the input because
     * the direction flag -- which it does not touch -- arrived set in one state and
     * clear in the other.
     *
     * WHAT IT GIVES UP, and it is worth writing down rather than rediscovering: an
     * instruction that READS a flag -- `adc` reads the carry -- is not exercised on
     * that.  It is a different question and it wants a third state, identical in the
     * registers and different in the flags.  Not here yet.
     *
     * \~spanish LAS BANDERAS ENTRAN IGUALES EN LOS DOS, que es lo contrario de lo
     * que hacen los registros, y a proposito.  Lo que preguntan estos dos estados es
     * que hace la instruccion con los REGISTROS; toda bandera que difiera despues
     * tiene que ser su respuesta a ellos, y una bandera que llegara distinta seria
     * una respuesta a nada.  Mordio con `xor eax, eax`, reportado dependiendo de la
     * entrada porque la bandera de direccion -- que no toca -- llegaba puesta en un
     * estado y a cero en el otro.
     *
     * LO QUE RENUNCIA, y merece quedar escrito en vez de volver a descubrirse: una
     * instruccion que LEE una bandera -- `adc` lee el acarreo -- no queda ejercitada
     * en eso.  Es otra pregunta y pide un tercer estado, identico en los registros y
     * distinto en las banderas.  Todavia no esta.
     */
    (void)which;
    s->flags = 0x0002u;
}

status isa_effects_of(const isa_arena *a, const u8 *bytes, u32 n,
                      isa_effects *out) {
    isa_state in_a;
    isa_state in_b;
    isa_result ra;
    isa_result rb;
    status rc;
    u32 i;

    if (a == 0 || bytes == 0 || out == 0) {
        return ERR_INVALID;
    }
    for (i = 0; i < sizeof(*out); ++i) {
        ((char *)out)[i] = 0;
    }

    fill_state(&in_a, 0);
    fill_state(&in_b, 1);

    rc = isa_attempt_with(a, bytes, n, &in_a, &ra);
    if (rc != OK) {
        return rc;
    }
    rc = isa_attempt_with(a, bytes, n, &in_b, &rb);
    if (rc != OK) {
        return rc;
    }

    out->outcome = ra.outcome;
    out->length = ra.length;

    /* \~english If the two runs did not agree on WHAT happened, there is no effect
     * to report and the inconsistency is the finding.  It is checked before
     * anything else because everything below assumes both runs are the same
     * instruction doing the same thing.
     * \~spanish Si las dos corridas no coincidieron en QUE paso, no hay efecto que
     * reportar y la inconsistencia es el hallazgo.  Se comprueba antes que nada
     * porque todo lo de abajo da por hecho que las dos corridas son la misma
     * instruccion haciendo lo mismo. \~ */
    out->consistent = (ra.outcome == rb.outcome && ra.length == rb.length) ? 1u
                                                                          : 0u;
    if (!out->consistent) {
        return OK;
    }

    out->wrote_gpr = ra.wrote_gpr | rb.wrote_gpr;
    for (i = 0; i < ISA_GPR_COUNT; ++i) {
        if ((out->wrote_gpr & (1u << i)) == 0) {
            out->gpr_effect[i] = (u8)ISA_EFFECT_NONE;
            continue;
        }
        out->gpr_effect[i] = (ra.after.gpr[i] == rb.after.gpr[i])
                                     ? (u8)ISA_EFFECT_CONSTANT
                                     : (u8)ISA_EFFECT_DEPENDS;
    }
    /*
     * \~english WHICH FLAGS MOVED IS WORKED OUT FIRST, and it is not an
     * optimisation: it is the only thing that separates "wrote nothing" from "wrote
     * a constant".  The inputs agree on the flags, so a flag nobody touched comes
     * out equal in both runs -- exactly what a flag written to a fixed value looks
     * like.  Comparing the words alone cannot tell those two apart.
     *
     * Moved means: differs from its OWN input, in either of the two runs.  And with
     * the set in hand the comparison is restricted to it, so an untouched bit cannot
     * cast a vote.
     *
     * THERE IS A LIMIT HERE and it belongs to the method, not to this code: a flag
     * written to the value it already held did not move, so it is invisible.  For
     * `inc` the parity lands on its input value in one of the two runs, and it is
     * the OTHER run that puts it in the set.  Another instruction may not have that
     * luck, and then the report says less than the truth -- never more.
     *
     * \~spanish PRIMERO SE CALCULA QUE BANDERAS SE MOVIERON, y no es una
     * optimizacion: es lo unico que separa "no escribio nada" de "escribio una
     * constante".  Las entradas coinciden en las banderas, asi que una bandera que
     * nadie toco sale igual en las dos corridas -- que es exactamente como se ve una
     * bandera escrita a un valor fijo.  Comparar las palabras a secas no distingue
     * esas dos cosas.
     *
     * Moverse quiere decir: diferir de su PROPIA entrada, en cualquiera de las dos
     * corridas.  Y con el conjunto en la mano la comparacion se restringe a el, para
     * que un bit intacto no pueda votar.
     *
     * AQUI HAY UN LIMITE y es del metodo, no de este codigo: una bandera escrita al
     * valor que ya tenia no se movio, asi que es invisible.  Para `inc` la paridad
     * cae sobre su valor de entrada en una de las dos corridas, y es la OTRA la que
     * la mete en el conjunto.  Otra instruccion puede no tener esa suerte, y
     * entonces el informe dice menos que la verdad -- nunca mas.
     */
    {
        const u64 mask = 0x0CD7u;
        u64 moved_a = (ra.after.flags ^ in_a.flags) & mask;
        u64 moved_b = (rb.after.flags ^ in_b.flags) & mask;
        u64 written = moved_a | moved_b;

        if (written == 0) {
            out->flags_effect = (u32)ISA_EFFECT_NONE;
        } else if ((ra.after.flags & written) == (rb.after.flags & written)) {
            out->flags_effect = (u32)ISA_EFFECT_CONSTANT;
        } else {
            out->flags_effect = (u32)ISA_EFFECT_DEPENDS;
        }
    }
    return OK;
}

/** @brief
 *  \~english The adapter itself: the oracle table's shape over `isa_measure`.
 *  \~spanish El adaptador en si: la forma de la tabla de oraculo sobre
 *            `isa_measure`. \~ */
static status measure_through_arena(void *ctx, const u8 *bytes,
                                    isa_result *out) {
    return isa_measure((const isa_arena *)ctx, bytes, out);
}

void isa_arena_ops(const isa_arena *a, isa_probe_ops *out) {
    out->measure = measure_through_arena;
    /* \~english The cast drops the const the table cannot express; nothing here
     * writes to the arena -- measuring puts bytes in its page, which is what the
     * page is for.  \~spanish El casteo tira el const que la tabla no sabe
     * expresar; aqui nadie escribe en la arena -- medir pone bytes en su pagina,
     * que es para lo que esta. \~ */
    out->ctx = (void *)(size_t)a;
}
