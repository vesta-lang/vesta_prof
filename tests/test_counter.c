/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_counter.c
 * @brief
 * \~english The arm / read / disarm sequence, exercised without a driver.
 * \~spanish La secuencia de armar / leer / desarmar, ejercitada sin driver.
 * \~
 *
 * \~english
 * WHY THIS CAN BE TESTED HERE AT ALL.  `count/counter.c` reaches the MSRs
 * through `msr_ops` and knows no system, so a make-believe bank of registers in
 * an ordinary array exercises the SAME code that runs in ring 0.
 *
 * WHAT IS CHECKED, and it is the part that does not fail on its own:
 *
 *   THE ORDER.  The manual requires the selector's switch to be OFF before
 *   writing a counter.  Getting it wrong raises no fault and returns no error --
 *   the counter takes a value nobody asked for and the measurement comes out
 *   wrong by a plausible amount.  So the bank RECORDS every write, and the test
 *   walks the log asserting that at the moment each counter was written, its
 *   selector had `EN` clear.  A test that only looked at the final state would
 *   pass with the order inverted.
 *
 *   THAT ONLY WHAT THE PART HAS IS ARMED.  The events are chosen by the table
 *   and filtered by `CPUID.0AH`, so a processor that enumerates four events must
 *   end up with four counters and not six programmed with something it does not
 *   have -- which would read as zero and look like "it happened zero times".
 *
 *   THAT DISARMING PUTS BACK EXACTLY WHAT WAS THERE.  Byte for byte, including
 *   the global register.  The PMU belongs to the whole machine and leaving it
 *   moved breaks whoever comes next, silently.
 *
 * \~spanish
 * POR QUE ESTO SE PUEDE PROBAR AQUI.  `count/counter.c` llega a los MSR por
 * `msr_ops` y no conoce ningun sistema, asi que un banco de registros de mentira
 * en un array corriente ejercita el MISMO codigo que corre en anillo cero.
 *
 * QUE SE COMPRUEBA, y es la parte que no falla sola:
 *
 *   EL ORDEN.  El manual exige que el interruptor del selector este APAGADO
 *   antes de escribir un contador.  Equivocarse no provoca excepcion ni devuelve
 *   error -- el contador coge un valor que nadie pidio y la medida sale mal por
 *   una cantidad plausible.  Asi que el banco APUNTA cada escritura, y el test
 *   recorre el registro comprobando que en el momento en que se escribio cada
 *   contador, su selector tenia el `EN` a cero.  Un test que solo mirara el
 *   estado final pasaria con el orden invertido.
 *
 *   QUE SOLO SE ARMA LO QUE LA PIEZA TIENE.  Los eventos se eligen por la tabla
 *   y se filtran con `CPUID.0AH`, asi que un procesador que enumere cuatro
 *   eventos tiene que acabar con cuatro contadores y no con seis programados con
 *   algo que no tiene -- que se leeria como cero y pareceria "paso cero veces".
 *
 *   QUE DESARMAR DEVUELVE EXACTAMENTE LO QUE HABIA.  Byte a byte, el registro
 *   global incluido.  El PMU es de la maquina entera y dejarlo movido rompe a
 *   quien venga despues, en silencio.
 */

#include "count/counter.h"

#include "msr/index.h"

#include <stdio.h>

static int failures = 0;

static void check(int cond, const char *what) {
    if (!cond) {
        printf("FAIL: %s\n", what);
        failures += 1;
    }
}

/* --- \~english the make-believe bank \~spanish el banco de mentira \~ ----- */

/* \~english How many writes the log holds.  Arming six counters is one global
 * write plus three per counter plus the final one: nineteen.  Room to spare so
 * that overflowing the log is a test failure and not a silent truncation.
 * \~spanish Cuantas escrituras caben en el registro.  Armar seis contadores son
 * una escritura global mas tres por contador mas la final: diecinueve.  Va
 * sobrado para que desbordarlo sea un fallo del test y no un truncado en
 * silencio. \~ */
#define LOG_MAX 128u

typedef struct write_entry {
    u32 addr;
    u64 value;
} write_entry;

typedef struct bank {
    u64 global_ctrl;
    u64 evtsel[COUNTER_MAX];
    u64 pmc[COUNTER_MAX];
    write_entry log[LOG_MAX];
    u32 logged;
    u32 overflowed;
    /* \~english Which address the bank answers for the counters.  It is set by
     * the test, and reading through the other one is a failure: it would mean
     * the module chose the wrong width.
     * \~spanish Por que direccion responde el banco para los contadores.  La fija
     * el test, y leer por la otra es un fallo: querria decir que el modulo eligio
     * el ancho equivocado. \~ */
    u32 counter_base;
} bank;

static bank g_bank;

/** @brief
 *  \~english Turns an address into a slot, or says it is not ours.
 *  \~spanish Convierte una direccion en una ranura, o dice que no es
 *            nuestra. \~ */
static u64 *slot_of(u32 addr) {
    if (addr == IA32_PERF_GLOBAL_CTRL) {
        return &g_bank.global_ctrl;
    }
    if (addr >= IA32_PERFEVTSEL0 && addr < IA32_PERFEVTSEL0 + COUNTER_MAX) {
        return &g_bank.evtsel[addr - IA32_PERFEVTSEL0];
    }
    if (addr >= g_bank.counter_base &&
        addr < g_bank.counter_base + COUNTER_MAX) {
        return &g_bank.pmc[addr - g_bank.counter_base];
    }
    return 0;
}

static status bank_read(u32 addr, u64 *out, void *ctx) {
    u64 *p = slot_of(addr);
    (void)ctx;
    if (p == 0) {
        return ERR_FAULT;
    }
    *out = *p;
    return OK;
}

static status bank_write(u32 addr, u64 value, void *ctx) {
    u64 *p = slot_of(addr);
    (void)ctx;
    if (p == 0) {
        return ERR_FAULT;
    }
    if (g_bank.logged < LOG_MAX) {
        g_bank.log[g_bank.logged].addr = addr;
        g_bank.log[g_bank.logged].value = value;
        g_bank.logged += 1;
    } else {
        g_bank.overflowed = 1;
    }
    *p = value;
    return OK;
}

static const msr_ops g_ops = {bank_read, bank_write, 0};

/**
 * @brief
 * \~english Leaves the bank as a machine where somebody else already wrote.
 * \~spanish Deja el banco como una maquina donde ya escribio alguien.
 * \~
 *
 * \~english
 * The previous values are NOT zeros on purpose.  With everything at zero,
 * "restored correctly" and "wiped to zero" look identical, and the test would
 * pass on a disarm that simply cleared the registers.
 *
 * \~spanish
 * Los valores previos NO son ceros a proposito.  Con todo a cero, "restaurado
 * bien" y "borrado a cero" se ven igual, y el test pasaria con un desarmado que
 * simplemente limpiara los registros.
 */
static void bank_reset(u32 counter_base) {
    u32 i;
    g_bank.counter_base = counter_base;
    g_bank.global_ctrl = 0x000000070000003Full;
    for (i = 0; i < COUNTER_MAX; ++i) {
        g_bank.evtsel[i] = 0x00000000DEAD0000ull + i;
        g_bank.pmc[i] = 0x0000123456780000ull + i;
    }
    g_bank.logged = 0;
    g_bank.overflowed = 0;
}

/** @brief
 *  \~english A part that enumerates `listed` events and lacks none.
 *  \~spanish Una pieza que enumera `listed` eventos y no le falta ninguno. \~ */
static void caps_reset(pmu_caps *c, u32 gp, u32 listed, u32 absent) {
    u32 i;
    char *p = (char *)c;
    for (i = 0; i < sizeof(*c); ++i) {
        p[i] = 0;
    }
    c->vendor = CPU_VENDOR_INTEL;
    c->pmu_version = 5;
    c->gp_counters = gp;
    c->arch_events_listed = listed;
    c->arch_events_absent = absent;
    c->counters_in_use = 0;
    c->full_width_write = 0;
}

/**
 * @brief
 * \~english Was every counter written while its selector had `EN` clear?
 * \~spanish ¿Se escribio cada contador con el `EN` de su selector a cero?
 * \~
 *
 * \~english
 * The log is replayed keeping a shadow of the selectors, and at each write to a
 * counter the shadow is asked.  Replaying rather than looking at the end state
 * is the whole point: the end state is identical whichever order the writes went
 * in.
 *
 *      write PERFEVTSEL0 = sel        shadow[0].EN = 0   ok
 *      write PMC0        = 0          <- asks the shadow: EN is 0, correct
 *      write PERFEVTSEL0 = sel | EN   shadow[0].EN = 1
 *
 * \~spanish
 * Se vuelve a recorrer el registro llevando una sombra de los selectores, y en
 * cada escritura a un contador se le pregunta a la sombra.  Recorrerlo en vez de
 * mirar el estado final es justo el asunto: el estado final es identico fuera
 * cual fuera el orden de las escrituras.
 *
 *      escribe PERFEVTSEL0 = sel        sombra[0].EN = 0   bien
 *      escribe PMC0        = 0          <- pregunta a la sombra: EN es 0, ok
 *      escribe PERFEVTSEL0 = sel | EN   sombra[0].EN = 1
 */
static int order_respected(void) {
    u64 shadow[COUNTER_MAX];
    u32 i;
    int ok = 1;

    for (i = 0; i < COUNTER_MAX; ++i) {
        shadow[i] = 0;
    }
    for (i = 0; i < g_bank.logged; ++i) {
        u32 addr = g_bank.log[i].addr;
        if (addr >= IA32_PERFEVTSEL0 && addr < IA32_PERFEVTSEL0 + COUNTER_MAX) {
            shadow[addr - IA32_PERFEVTSEL0] = g_bank.log[i].value;
        } else if (addr >= g_bank.counter_base &&
                   addr < g_bank.counter_base + COUNTER_MAX) {
            u32 n = addr - g_bank.counter_base;
            if ((shadow[n] & PERFEVTSEL_EN) != 0) {
                printf("FAIL: se escribio el contador %u con su selector "
                       "encendido (escritura %u del registro)\n",
                       (unsigned)n, (unsigned)i);
                ok = 0;
            }
        }
    }
    return ok;
}

/** @brief
 *  \~english How many times an address was written.
 *  \~spanish Cuantas veces se escribio una direccion. \~ */
static u32 writes_to(u32 addr) {
    u32 i;
    u32 n = 0;
    for (i = 0; i < g_bank.logged; ++i) {
        if (g_bank.log[i].addr == addr) {
            n += 1;
        }
    }
    return n;
}

int main(void) {
    pmu_caps caps;
    counter_state st;
    u64 values[COUNTER_MAX];
    u32 i;

    printf("--- las negativas ---\n");
    {
        /* \~english Each refusal says a different thing, and none of them is a
         * fallback that measures anyway.
         * \~spanish Cada negativa dice una cosa distinta, y ninguna es un
         * respaldo que mida igualmente. \~ */
        bank_reset(IA32_PMC0);
        caps_reset(&caps, 6, 7, 0);
        caps.vendor = CPU_VENDOR_AMD;
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) ==
                      ERR_UNSUPPORTED,
              "en AMD no se programan los MSR de Intel");
        check(g_bank.logged == 0, "y no se escribio nada");

        caps_reset(&caps, 6, 7, 0);
        caps.pmu_version = 0;
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) ==
                      ERR_UNSUPPORTED,
              "sin PMU arquitectonico no se arma");

        caps_reset(&caps, 6, 7, 0);
        caps.counters_in_use = 1;
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) == ERR_STATE,
              "si ya cuenta otro, se cede en vez de pisar");
        check(g_bank.logged == 0, "y tampoco se escribio nada");

        caps_reset(&caps, 6, 0, 0);
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) ==
                      ERR_UNSUPPORTED,
              "sin ningun evento enumerado no hay nada que armar");

        check(counter_arm(0, 0, &g_ops, &st) == ERR_INVALID,
              "sin capacidades es un error de uso");
        check(counter_arm(&caps, 0, 0, &st) == ERR_INVALID,
              "sin forma de llegar al MSR, tambien");
    }

    printf("--- armar ---\n");
    {
        bank_reset(IA32_PMC0);
        caps_reset(&caps, 6, 7, 0);
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) == OK,
              "se arma en una pieza con seis contadores y siete eventos");
        check(!g_bank.overflowed, "el registro de escrituras no se desbordo");
        check(st.armed == 6, "seis contadores, seis eventos");
        check(st.saved == 6, "y se guardaron los seis que habia");

        /* \~english The first six encoded events, in the manual's order.
         * \~spanish Los seis primeros eventos codificados, en el orden del
         * manual. \~ */
        for (i = 0; i < st.armed; ++i) {
            const arch_event *e = &arch_events[st.event[i]];
            u64 want = (u64)e->event | ((u64)e->umask << 8) |
                       (u64)PERFEVTSEL_USR | (u64)PERFEVTSEL_EN;
            check(st.event[i] == i, "se eligieron en el orden de la tabla");
            check(g_bank.evtsel[i] == want,
                  "el selector lleva evento, umask, USR y EN");
            check((g_bank.evtsel[i] & PERFEVTSEL_OS) == 0,
                  "y NO lleva OS, que no se pidio");
            check(g_bank.pmc[i] == 0, "el contador quedo a cero");
        }
        check(g_bank.global_ctrl == 0x3Fu,
              "la compuerta abre exactamente los seis armados");

        /* LA COMPROBACION QUE IMPORTA. */
        check(order_respected(),
              "cada contador se escribio con su selector apagado");

        /* \~english The global register is written twice: zero to stop, and the
         * mask to start.  If the second one were missing nothing would count and
         * every reading would be zero, which is a plausible-looking result.
         * \~spanish El registro global se escribe dos veces: cero para parar, y
         * la mascara para arrancar.  Si faltara la segunda no contaria nada y
         * toda lectura seria cero, que es un resultado de aspecto plausible. \~ */
        check(writes_to(IA32_PERF_GLOBAL_CTRL) == 2,
              "la compuerta se cierra antes de programar y se abre al final");
        check(g_bank.log[0].addr == IA32_PERF_GLOBAL_CTRL &&
                      g_bank.log[0].value == 0,
              "lo primero es parar");
        check(g_bank.log[g_bank.logged - 1].addr == IA32_PERF_GLOBAL_CTRL &&
                      g_bank.log[g_bank.logged - 1].value == 0x3Fu,
              "lo ultimo es arrancarlos todos a la vez");
    }

    printf("--- leer ---\n");
    {
        for (i = 0; i < COUNTER_MAX; ++i) {
            g_bank.pmc[i] = 1000u + i;
            values[i] = 0;
        }
        check(counter_read(&st, &g_ops, values) == OK, "la lectura sale bien");
        for (i = 0; i < st.armed; ++i) {
            check(values[i] == 1000u + i,
                  "cada contador se lee en el orden en que se armo");
        }
        check(values[st.armed] == 0,
              "y no se toca mas alla de los armados");
    }

    printf("--- desarmar ---\n");
    {
        bank g_before = g_bank;
        check(counter_disarm(&st, &g_ops) == OK, "el desarmado sale bien");
        check(g_bank.global_ctrl == 0x000000070000003Full,
              "la compuerta vuelve a como estaba");
        for (i = 0; i < st.saved; ++i) {
            check(g_bank.evtsel[i] == 0x00000000DEAD0000ull + i,
                  "el selector vuelve a como estaba");
            check(g_bank.pmc[i] == 0x0000123456780000ull + i,
                  "y el contador tambien");
        }
        check(order_respected(),
              "tambien al desarmar se apaga antes de tocar la cuenta");
        (void)g_before;
    }

    printf("--- lo que la pieza no tiene, no se arma ---\n");
    {
        /* \~english Four enumerated and the third one absent: three armable, and
         * the six counters must not be filled with the rest.
         * \~spanish Cuatro enumerados y el tercero ausente: tres armables, y los
         * seis contadores no deben rellenarse con el resto. \~ */
        bank_reset(IA32_PMC0);
        caps_reset(&caps, 6, 4, 1u << 2);
        check(counter_arm(&caps, PERFEVTSEL_USR | PERFEVTSEL_OS, &g_ops, &st) ==
                      OK,
              "se arma con lo que hay");
        check(st.armed == 3, "tres eventos utilizables, tres contadores");
        check(st.event[0] == 0 && st.event[1] == 1 && st.event[2] == 3,
              "el ausente se salta, no se programa y se lee como cero");
        check(g_bank.global_ctrl == 0x7u,
              "la compuerta abre tres, no seis");
        check((g_bank.evtsel[0] & PERFEVTSEL_OS) != 0,
              "esta vez si se pidio OS");
        check(order_respected(), "y el orden se respeta igual");
    }

    printf("--- mas eventos que contadores ---\n");
    {
        /* \~english Two counters and eight events: it must arm two, not walk off
         * the end of the array.
         * \~spanish Dos contadores y ocho eventos: tiene que armar dos, no
         * salirse del array. \~ */
        bank_reset(IA32_PMC0);
        caps_reset(&caps, 2, 8, 0);
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) == OK,
              "se arma con menos contadores que eventos");
        check(st.armed == 2, "manda el numero de contadores");
        check(g_bank.evtsel[2] == 0x00000000DEAD0002ull,
              "el tercero ni se toco");
    }

    printf("--- el alias de ancho completo ---\n");
    {
        /* \~english With full-width writes the counters are reached through
         * `IA32_A_PMC0`.  The bank only answers on the base it was given, so
         * choosing the wrong one is a fault and not a silent success.
         * \~spanish Con escrituras de ancho completo los contadores se alcanzan
         * por `IA32_A_PMC0`.  El banco solo responde por la base que se le dio,
         * asi que elegir la equivocada es un fallo y no un exito silencioso. \~ */
        bank_reset(IA32_A_PMC0);
        caps_reset(&caps, 4, 7, 0);
        caps.full_width_write = 1;
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) == OK,
              "con ancho completo se usa el alias");
        check(st.counter_msr == IA32_A_PMC0, "y queda anotado en el estado");
        check(order_respected(), "el orden tampoco cambia por el alias");

        /* \~english And without it, the narrow one.  Same check the other way
         * round, because a module that always used the alias would pass the
         * previous one.
         * \~spanish Y sin el, el estrecho.  La misma comprobacion al reves,
         * porque un modulo que usara siempre el alias pasaria la anterior. \~ */
        bank_reset(IA32_PMC0);
        caps_reset(&caps, 4, 7, 0);
        check(counter_arm(&caps, PERFEVTSEL_USR, &g_ops, &st) == OK,
              "sin ancho completo se usa el registro normal");
        check(st.counter_msr == IA32_PMC0, "y tambien queda anotado");
    }

    printf("--- desarmar lo que nunca se armo ---\n");
    {
        counter_state empty;
        char *p = (char *)&empty;
        for (i = 0; i < sizeof(empty); ++i) {
            p[i] = 0;
        }
        bank_reset(IA32_PMC0);
        check(counter_disarm(&empty, &g_ops) == OK,
              "sobre un estado a ceros no hace nada");
        check(g_bank.logged == 0,
              "y no escribe: es el DriverUnload que no llego a armar");
    }

    if (failures == 0) {
        printf("OK\n");
        return 0;
    }
    printf("%d fallos\n", failures);
    return 1;
}
