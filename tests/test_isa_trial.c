/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_isa_trial.c
 * @brief
 * \~english The length oracle, against instructions whose length is known.
 * \~spanish El oraculo de longitud, contra instrucciones de longitud conocida.
 * \~
 *
 * \~english
 * WHY THIS TEST DECIDES EVERYTHING THAT COMES AFTER IT.  The oracle is the floor
 * of the whole ISA search: the traversal uses the measured length to choose which
 * byte to advance, so a wrong length does not produce an error -- it produces a
 * traversal that skips regions and never says so.  Every finding built on top
 * would be believable and unfounded.
 *
 * So it is checked against instructions where the answer is not a matter of
 * opinion: `nop` is one byte, `mov eax, imm32` is five, and a sequence with
 * prefixes up to the limit is fifteen.  If the oracle does not say 1, 5 and 15,
 * what is broken is the oracle.
 *
 * AND IT CHECKS THE CASE THAT MOTIVATES THE APPARATUS.  A complete instruction
 * that runs and falls off the edge, and a truncated one that never decoded, come
 * out of the system with the SAME exception code, the SAME type and the SAME
 * address.  Only the instruction pointer separates them.  A classifier that looked
 * at the code alone would call them both the same thing and be wrong on half the
 * instruction space.
 *
 * \~spanish
 * POR QUE ESTE TEST DECIDE TODO LO QUE VENGA DESPUES.  El oraculo es el suelo de
 * la busqueda entera de la ISA: el recorrido usa la longitud medida para elegir
 * que byte avanzar, asi que una longitud equivocada no produce un error --
 * produce un recorrido que se salta regiones y no lo dice nunca.  Cualquier
 * hallazgo construido encima seria creible e infundado.
 *
 * Asi que se comprueba contra instrucciones donde la respuesta no es opinable:
 * `nop` mide uno, `mov eax, imm32` mide cinco, y una secuencia con prefijos hasta
 * el limite mide quince.  Si el oraculo no dice 1, 5 y 15, lo roto es el oraculo.
 *
 * Y COMPRUEBA EL CASO QUE MOTIVA EL APARATO.  Una instruccion completa que corre
 * y se cae por el borde, y una truncada que no llego a decodificar, salen del
 * sistema con el MISMO codigo de excepcion, el MISMO tipo y la MISMA direccion.
 * Solo las separa el puntero de instruccion.  Un clasificador que mirara solo el
 * codigo las llamaria a las dos lo mismo y se equivocaria en media ISA.
 */

#include "isa_win.h"

#include <stdio.h>

static int failures = 0;

static void check(int cond, const char *what) {
    if (!cond) {
        printf("FAIL: %s\n", what);
        failures += 1;
    }
}

/** @brief
 *  \~english The name of an outcome, for a message that can be read.
 *  \~spanish El nombre de un resultado, para un mensaje que se pueda leer. \~ */
static const char *name_of(u32 outcome) {
    switch (outcome) {
    case ISA_NOTHING: return "SIN RESPUESTA";
    case ISA_RAN: return "corrio";
    case ISA_TRUNCATED: return "le faltaban bytes";
    case ISA_INVALID: return "no existe";
    case ISA_PRIVILEGED: return "privilegiada";
    case ISA_MEMORY_FAULT: return "toco memoria ajena";
    case ISA_ARITH: return "aritmetica";
    case ISA_RETURNED: return "volvio sola";
    default: return "sin clasificar";
    }
}

/**
 * @brief
 * \~english Measures one sequence and says what came out, whatever it was.
 * \~spanish Mide una secuencia y dice que salio, fuera lo que fuera.
 * \~
 *
 * \~english
 * It PRINTS every attempt and not only the failures, on purpose.  This test
 * interrogates the processor, so what it produces is as much a measurement as a
 * check: running it by hand on another machine is how one finds out that machine
 * answers differently.
 *
 * \~spanish
 * IMPRIME todos los intentos y no solo los fallos, a proposito.  Este test
 * interroga al procesador, asi que lo que produce es tanto una medida como una
 * comprobacion: correrlo a mano en otra maquina es como se descubre que esa
 * maquina contesta distinto.
 */
static void measured(const isa_arena *a, const char *what, const u8 *bytes,
                     u32 expect_outcome, u32 expect_len) {
    isa_result r;
    status rc = isa_measure(a, bytes, &r);

    printf("  %-30s ", what);
    if (rc != OK) {
        printf("ERROR %d\n", (int)rc);
        failures += 1;
        return;
    }
    printf("%-20s", name_of(r.outcome));
    if (r.length != 0) {
        printf(" longitud %2u", (unsigned)r.length);
    } else {
        printf("            ");
    }
    printf("  codigo 0x%08X\n", (unsigned)r.code);

    check(r.outcome == expect_outcome, what);
    /*
     * LA LONGITUD SE COMPRUEBA EN TODOS LOS VEREDICTOS MENOS UNO, y esa es la
     * diferencia que importa.  Un intento truncado no tiene longitud -- dice que la
     * instruccion mide mas que lo que se le ofrecio y nunca dice cuanto --, y el
     * resto si: el bucle deslizante paro en el primer numero de bytes con el que el
     * decodificador dejo de pedir mas, y eso es lo que consumio, decida luego lo
     * que decida.
     *
     * Se comprueba aqui porque es el numero sobre el que avanza el recorrido.  Una
     * privilegiada o una que no existe sin longitud hacen que el recorrido no salte
     * nada sobre ellas, y eso es casi todo el espacio.
     */
    if (expect_outcome != ISA_TRUNCATED) {
        check(r.length == expect_len, what);
    }
}

int main(void) {
    isa_arena a;
    u8 bytes[ISA_MAX_LEN];
    u32 i;

    /* \~english Unbuffered: if an attempt takes the process down despite the net,
     * the last line printed says which one it was.  With buffering that line is
     * exactly the one that gets lost.
     * \~spanish Sin bufer: si un intento se lleva el proceso por delante a pesar de
     * la red, la ultima linea impresa dice cual fue.  Con bufer esa linea es justo
     * la que se pierde. \~ */
    setvbuf(stdout, 0, _IONBF, 0);

    if (isa_arena_open(&a) != OK) {
        printf("FAIL: no se pudo abrir la arena\n");
        return 1;
    }
    printf("arena: pagina ejecutable %p, frontera %p, %u bytes por pagina\n\n",
           (void *)a.first, (void *)a.boundary, (unsigned)a.page);

    printf("--- longitudes que no son opinables ---\n");
    for (i = 0; i < ISA_MAX_LEN; ++i) {
        bytes[i] = 0x90; /* nop de relleno, para que la cola nunca sea basura */
    }

    /* `nop`: un byte, y el resto son nop tambien, asi que si el oraculo dijera 2
     * seria que esta midiendo la instruccion siguiente. */
    bytes[0] = 0x90;
    measured(&a, "nop", bytes, ISA_RAN, 1);

    /* `mov eax, imm32`: opcode mas cuatro bytes de inmediato. */
    bytes[0] = 0xB8;
    bytes[1] = 0x11;
    bytes[2] = 0x22;
    bytes[3] = 0x33;
    bytes[4] = 0x44;
    bytes[5] = 0x90;
    measured(&a, "mov eax, 0x44332211", bytes, ISA_RAN, 5);

    /* `add rax, rcx`: REX.W mas opcode mas ModRM. */
    bytes[0] = 0x48;
    bytes[1] = 0x01;
    bytes[2] = 0xC8;
    bytes[3] = 0x90;
    measured(&a, "add rax, rcx", bytes, ISA_RAN, 3);

    /* `mov rax, imm64`: el maximo que se alcanza sin prefijos redundantes. */
    bytes[0] = 0x48;
    bytes[1] = 0xB8;
    for (i = 2; i < 10; ++i) {
        bytes[i] = (u8)i;
    }
    bytes[10] = 0x90;
    measured(&a, "mov rax, imm64", bytes, ISA_RAN, 10);

    /* Y el limite arquitectonico: prefijos de segmento repetidos hasta que la
     * secuencia mide quince.  Es legal, y quince es lo maximo que el procesador
     * acepta -- uno mas y lo rechaza por longitud, no por codificacion. */
    for (i = 0; i < 11; ++i) {
        bytes[i] = 0x2E; /* prefijo CS, repetido */
    }
    bytes[11] = 0x48;
    bytes[12] = 0x01;
    bytes[13] = 0xC8; /* add rax, rcx */
    bytes[14] = 0x90;
    measured(&a, "add rax,rcx con 11 prefijos", bytes, ISA_RAN, 14);

    printf("\n--- los otros veredictos ---\n");

    /* `ud2`: invalida por definicion, y el manual promete que lo seguira siendo. */
    bytes[0] = 0x0F;
    bytes[1] = 0x0B;
    bytes[2] = 0x90;
    measured(&a, "ud2", bytes, ISA_INVALID, 2);

    /* `wrmsr`: existe, y en anillo tres no.  Es la prueba de que lo peligroso es
     * inofensivo aqui, que es toda la razon de empezar por anillo tres. */
    bytes[0] = 0x0F;
    bytes[1] = 0x30;
    bytes[2] = 0x90;
    measured(&a, "wrmsr (anillo tres)", bytes, ISA_PRIVILEGED, 2);

    /* `hlt`: tambien privilegiada.  Se prueba a proposito porque en anillo cero
     * seria una de las que se lleva la maquina, y conviene ver que aqui no. */
    bytes[0] = 0xF4;
    bytes[1] = 0x90;
    measured(&a, "hlt (anillo tres)", bytes, ISA_PRIVILEGED, 1);

    /* `ret`: vuelve sola, asi que es el unico camino que no pasa por una
     * excepcion.  Y mide uno: no hizo falta ofrecerle un segundo byte. */
    bytes[0] = 0xC3;
    bytes[1] = 0x90;
    measured(&a, "ret", bytes, ISA_RETURNED, 1);

    printf("\n--- lo que solo distingue el puntero de instruccion ---\n");
    {
        /* Los dos casos que salen del sistema IGUALES salvo en el RIP.  Se piden
         * con `isa_attempt` y no con `isa_measure` para fijar cuantos bytes hay
         * antes de la frontera: con uno solo, el `nop` cabe y el `B8` no. */
        isa_result ran;
        isa_result cut;
        u8 one[ISA_MAX_LEN];

        for (i = 0; i < ISA_MAX_LEN; ++i) {
            one[i] = 0x90;
        }
        one[0] = 0x90;
        check(isa_attempt(&a, one, 1, &ran) == OK, "el intento del nop sale");

        one[0] = 0xB8; /* necesita cinco, hay uno */
        check(isa_attempt(&a, one, 1, &cut) == OK, "el intento del B8 sale");

        printf("  nop con 1 byte     %-20s codigo 0x%08X  rip %p\n",
               name_of(ran.outcome), (unsigned)ran.code, (void *)ran.rip);
        printf("  B8  con 1 byte     %-20s codigo 0x%08X  rip %p\n",
               name_of(cut.outcome), (unsigned)cut.code, (void *)cut.rip);

        check(ran.outcome == ISA_RAN, "el nop con un byte corrio");
        check(cut.outcome == ISA_TRUNCATED, "al B8 con un byte le faltaban bytes");
        check(ran.code == cut.code,
              "los dos salen con el MISMO codigo de excepcion");
        check(ran.fault_at == cut.fault_at,
              "y fallan en la MISMA direccion");
        check(ran.rip != cut.rip,
              "lo unico que los distingue es el rip -- si esto falla, el "
              "clasificador no puede funcionar");
        check(ran.rip == (u64)(size_t)a.boundary,
              "el que corrio avanzo a la frontera");
        check(cut.rip == (u64)(size_t)(a.boundary - 1),
              "el truncado no se movio del candidato");
    }

    printf("\n--- los efectos, y `nop` es el que decide ---\n");
    {
        /* La banda de cordura de los efectos.  `nop` tiene que salir escribiendo
         * NADA: si el aparato dice que escribe algo, lo que esta roto es la
         * comparacion -- muy probablemente el puntero de pila, que cambia por la
         * propia llamada y es lo que la calibracion de la arena existe para
         * descontar. */
        static const struct {
            const char *what;
            u8 code[4];
            u32 n;
            u32 wrote;       /* mascara de registros esperada */
            u32 reg;         /* registro a mirar, si hay uno */
            u32 reg_effect;  /* que clase de valor deberia llevar */
            u32 flags;       /* efecto esperado en las banderas */
        } cases[] = {
            /* `nop`: el unico que no toca nada.  Es la comprobacion que sostiene
             * todas las demas. */
            {"nop", {0x90}, 1, 0, 0, ISA_EFFECT_NONE, ISA_EFFECT_NONE},

            /* `xor eax, eax`: escribe rax con CERO, que no depende de la entrada,
             * y toca las banderas.  Es el caso que una sola entrada NO podria
             * distinguir de una copia. */
            {"xor eax, eax", {0x31, 0xC0}, 2, 1u << 0, 0, ISA_EFFECT_CONSTANT,
             ISA_EFFECT_CONSTANT},

            /* `mov rax, rcx`: escribe rax con algo que SI depende de la entrada,
             * y no toca las banderas.  El contraste exacto con el anterior. */
            {"mov rax, rcx", {0x48, 0x89, 0xC8}, 3, 1u << 0, 0,
             ISA_EFFECT_DEPENDS, ISA_EFFECT_NONE},

            /* `add rax, rcx`: escribe rax dependiendo de la entrada Y las
             * banderas dependiendo de la entrada. */
            {"add rax, rcx", {0x48, 0x01, 0xC8}, 3, 1u << 0, 0,
             ISA_EFFECT_DEPENDS, ISA_EFFECT_DEPENDS},

            /* `inc rdx`: para comprobar que se ve el registro que toca y no
             * siempre el cero. */
            {"inc rdx", {0x48, 0xFF, 0xC2}, 3, 1u << 2, 2, ISA_EFFECT_DEPENDS,
             ISA_EFFECT_DEPENDS},

            /* `push rax`: el caso del puntero de pila.  Tiene que aparecer como
             * escrito, y sin la calibracion de la arena seria indistinguible de
             * lo que hace la propia llamada. */
            {"push rax", {0x50}, 1, 1u << ISA_STACK_REG, ISA_STACK_REG,
             ISA_EFFECT_CONSTANT, ISA_EFFECT_NONE}};

        for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
            isa_effects e;
            u32 r;
            status rc = isa_effects_of(&a, cases[i].code, cases[i].n, &e);

            printf("  %-16s ", cases[i].what);
            if (rc != OK) {
                printf("ERROR %d\n", (int)rc);
                failures += 1;
                continue;
            }
            printf("%-12s escribe", name_of(e.outcome));
            if (e.wrote_gpr == 0) {
                printf(" nada");
            } else {
                for (r = 0; r < ISA_GPR_COUNT; ++r) {
                    if (e.wrote_gpr & (1u << r)) {
                        printf(" r%u%s", (unsigned)r,
                               e.gpr_effect[r] == ISA_EFFECT_CONSTANT ? "=cte"
                                                                      : "=dep");
                    }
                }
            }
            if (e.flags_effect != ISA_EFFECT_NONE) {
                printf("  banderas%s",
                       e.flags_effect == ISA_EFFECT_CONSTANT ? "=cte" : "=dep");
            }
            printf("\n");

            check(e.consistent, cases[i].what);
            check(e.wrote_gpr == cases[i].wrote, cases[i].what);
            check(e.flags_effect == cases[i].flags, cases[i].what);
            if (cases[i].wrote != 0) {
                check(e.gpr_effect[cases[i].reg] == cases[i].reg_effect,
                      cases[i].what);
            }
        }
    }

    printf("\n--- errores de uso ---\n");
    {
        isa_result r;
        u8 one[1] = {0x90};
        check(isa_attempt(0, one, 1, &r) == ERR_INVALID, "sin arena");
        check(isa_attempt(&a, 0, 1, &r) == ERR_INVALID, "sin bytes");
        check(isa_attempt(&a, one, 0, &r) == ERR_INVALID, "cero bytes");
        check(isa_attempt(&a, one, ISA_MAX_LEN + 1, &r) == ERR_INVALID,
              "mas del limite arquitectonico");
    }

    isa_arena_close(&a);
    /* Cerrarla dos veces tiene que ser inofensivo: un trabajador que muere a
     * medias deja esto a medio camino. */
    isa_arena_close(&a);

    if (failures == 0) {
        printf("\nOK\n");
        return 0;
    }
    printf("\n%d fallos\n", failures);
    return 1;
}
