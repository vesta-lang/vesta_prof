/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file asm/x86_64/calib_kernels.S
 * @brief
 * \~english The measured kernels: exact sequences no compiler would keep.
 * \~spanish Los nucleos medidos: secuencias exactas que ningun compilador
 *           conservaria.
 * \~
 *
 * \~english
 * WHY THIS IS ASSEMBLY, and it is the one case where there is no argument.
 * Everything else in this tree that looked like it needed assembly turned out
 * not to -- `rdmsr` had an intrinsic, so did `CR4`.  Here the thing being
 * measured IS the instruction sequence: an optimiser that removes a dead `add`,
 * reorders two of them, or turns sixty-four into a shift is not making the code
 * better, it is deleting the experiment.  There is no C that pins this down.
 *
 * TWO WAYS OF MEASURING ONE INSTRUCTION, and they answer different questions:
 *
 * @code
 *   LATENCY                        THROUGHPUT
 *   each one needs the previous    all independent
 *
 *     add %rcx, %rax                 add %r8,  %rax
 *     add %rcx, %rax   <- waits      add %r8,  %rcx   <- do not wait
 *     add %rcx, %rax   <- waits      add %r8,  %rdx      for each other
 *     ...                            ...
 *   -> cycles / count = how long     -> cycles / count = how many fit
 *      ONE takes end to end             per cycle
 * @endcode
 *
 * The first is limited by the dependency chain; the second by how many
 * execution ports accept the operation.  A part where `add` has latency 1 and
 * throughput 0.25 does four per cycle, each taking one cycle: both numbers are
 * true and neither alone describes it.
 *
 * THE SAME FILE SERVES WINDOWS AND LINUX, and what makes that work is the
 * register discipline.  The latency kernels use only registers volatile in BOTH
 * conventions -- RAX, RCX, RDX, R8 to R11 -- so they save nothing and touch no
 * stack.  The throughput ones need more chains than that allows, so they borrow
 * the preserved ones and give them back, saving the UNION of what the two
 * conventions ask for.  Neither calls anything, so Windows' shadow space never
 * comes into it.
 *
 * See `common/asm/README.md`, which sets the test this file has to pass: it does
 * not mention a single thing the operating system defines.
 *
 * \~spanish
 * POR QUE ESTO ES ENSAMBLADOR, y es el unico caso en el que no hay discusion.
 * Todo lo demas de este arbol que parecia necesitarlo resulto que no -- `rdmsr`
 * tenia intrinseco, y `CR4` tambien.  Aqui lo que se mide ES la secuencia de
 * instrucciones: un optimizador que quita un `add` muerto, que reordena dos, o
 * que convierte sesenta y cuatro en un desplazamiento no esta mejorando el
 * codigo, esta borrando el experimento.  No hay C que fije esto.
 *
 * DOS FORMAS DE MEDIR UNA INSTRUCCION, y responden a preguntas distintas: ver el
 * diagrama de arriba.  La primera la limita la cadena de dependencias; la
 * segunda, cuantos puertos de ejecucion aceptan la operacion.  Una pieza donde
 * `add` tiene latencia 1 y throughput 0,25 hace cuatro por ciclo, tardando cada
 * uno un ciclo: los dos numeros son ciertos y ninguno solo la describe.
 *
 * SOLO REGISTROS VOLATILES EN LAS DOS ABI -- RAX, RCX, RDX y de R8 a R11 --, asi
 * que no se guarda ni se restaura nada y el mismo fichero sirve a Windows y a
 * Linux.  La pila no se toca, que es ademas por lo que el espacio de sombra de
 * Windows no aplica.  Ver `common/asm/README.md`, que fija la prueba que este
 * fichero tiene que pasar.
 */

#include "asm/x86_64/calib.h"

    .text

/*
 * \~english The loop that every kernel shares.  R11 is the counter, and it is
 * chosen on purpose: it is volatile in both conventions and it is not one of the
 * registers under test, so the loop's own dependency does not join the chain
 * being measured.
 * \~spanish El bucle que comparten todos los nucleos.  R11 es el contador, y se
 * elige a proposito: es volatil en las dos convenciones y no es ninguno de los
 * registros bajo prueba, asi que la dependencia del propio bucle no se suma a la
 * cadena que se mide. \~
 */
#define CALIB_BEGIN(name)                                                      \
    .globl name;                                                               \
    .p2align 4;                                                                \
name:;                                                                         \
    movq $CALIB_ITERS, %r11;                                                   \
    .p2align 4;                                                                \
9:

#define CALIB_END()                                                            \
    decq %r11;                                                                 \
    jnz 9b;                                                                    \
    ret

/*
 * \~english The same, for a kernel that needs MORE registers than are volatile.
 *
 * Seven are volatile in both conventions and that is not enough to keep twelve
 * independent chains going, so eight more are borrowed and given back.  Eight and
 * not seven so that the stack stays aligned to sixteen: the convention requires
 * it, and an odd number of pushes breaks it for whatever runs next.
 *
 * The set saved is the union of what the two conventions call preserved --
 * System V does not require RSI and RDI, Windows does -- because saving one too
 * many costs two instructions per call and getting it wrong costs a bug that only
 * shows on one of the two systems.
 *
 * \~spanish El mismo, para un nucleo que necesita MAS registros de los volatiles.
 *
 * Siete son volatiles en las dos convenciones y no bastan para llevar doce
 * cadenas independientes, asi que se toman prestados ocho mas y se devuelven.
 * Ocho y no siete para que la pila quede alineada a dieciseis: la convencion lo
 * exige, y un numero impar de empujes lo rompe para lo que venga despues.
 *
 * El conjunto que se guarda es la UNION de lo que las dos convenciones llaman
 * preservado -- System V no exige RSI ni RDI, Windows si -- porque guardar uno de
 * mas cuesta dos instrucciones por llamada y equivocarse cuesta un fallo que solo
 * aparece en uno de los dos sistemas. \~
 */
#define CALIB_BEGIN_WIDE(name)                                                 \
    .globl name;                                                               \
    .p2align 4;                                                                \
name:;                                                                         \
    pushq %rbx;                                                                \
    pushq %rbp;                                                                \
    pushq %rsi;                                                                \
    pushq %rdi;                                                                \
    pushq %r12;                                                                \
    pushq %r13;                                                                \
    pushq %r14;                                                                \
    pushq %r15;                                                                \
    movq $CALIB_ITERS, %r11;                                                   \
    .p2align 4;                                                                \
9:

#define CALIB_END_WIDE()                                                       \
    decq %r11;                                                                 \
    jnz 9b;                                                                    \
    popq %r15;                                                                 \
    popq %r14;                                                                 \
    popq %r13;                                                                 \
    popq %r12;                                                                 \
    popq %rdi;                                                                 \
    popq %rsi;                                                                 \
    popq %rbp;                                                                 \
    popq %rbx;                                                                 \
    ret

/* \~english Nothing under test.  What this kernel measures is the loop, and
 * subtracting it is what makes the rest mean the instruction and not the
 * scaffolding around it.
 * \~spanish Nada bajo prueba.  Lo que mide este nucleo es el bucle, y restarlo
 * es lo que hace que el resto signifique la instruccion y no el andamio que la
 * rodea. \~ */
CALIB_BEGIN(calib_empty)
    CALIB_END()

/* \~english LATENCY of `add`.  Every one needs the result of the one before, so
 * they cannot overlap however many ports the part has.
 * \~spanish LATENCIA de `add`.  Cada uno necesita el resultado del anterior, asi
 * que no pueden solaparse por muchos puertos que tenga la pieza. \~ */
CALIB_BEGIN(calib_add_lat)
    .rept CALIB_UNROLL
    addq %rcx, %rax
    .endr
    CALIB_END()

/* \~english THROUGHPUT of `add`.  Twelve accumulators that never look at each
 * other, so what limits this is how many the machine issues per cycle and
 * nothing else.  Every destination appears ONCE per group: a destination that
 * repeats is a dependency chain, and then the number measured is the chain and
 * not the part.
 * \~spanish THROUGHPUT de `add`.  Doce acumuladores que no se miran nunca entre
 * si, asi que lo que limita esto es cuantos emite la maquina por ciclo y nada
 * mas.  Cada destino aparece UNA vez por grupo: un destino que se repite es una
 * cadena de dependencias, y entonces el numero medido es la cadena y no la
 * pieza. \~ */
CALIB_BEGIN_WIDE(calib_add_tp)
    .rept CALIB_UNROLL / CALIB_TP_WAYS
    addq %r8, %rax
    addq %r8, %rcx
    addq %r8, %rdx
    addq %r8, %r9
    addq %r8, %r10
    addq %r8, %rbx
    addq %r8, %rbp
    addq %r8, %rsi
    addq %r8, %rdi
    addq %r8, %r12
    addq %r8, %r13
    addq %r8, %r14
    .endr
    CALIB_END_WIDE()

/* \~english LATENCY of `imul`.  The classic check that the bench works: it is
 * several cycles on every part, so a result of one would mean the chain broke.
 * \~spanish LATENCIA de `imul`.  La comprobacion clasica de que el banco
 * funciona: son varios ciclos en cualquier pieza, asi que un resultado de uno
 * querria decir que la cadena se rompio. \~ */
CALIB_BEGIN(calib_imul_lat)
    .rept CALIB_UNROLL
    imulq %rcx, %rax
    .endr
    CALIB_END()

/* \~english THROUGHPUT of `imul`.  Usually one per cycle: there tends to be a
 * single multiplier even where there are four adders.
 * \~spanish THROUGHPUT de `imul`.  Normalmente uno por ciclo: suele haber un
 * solo multiplicador donde hay cuatro sumadores. \~ */
CALIB_BEGIN_WIDE(calib_imul_tp)
    .rept CALIB_UNROLL / CALIB_TP_WAYS
    imulq %r8, %rax
    imulq %r8, %rcx
    imulq %r8, %rdx
    imulq %r8, %r9
    imulq %r8, %r10
    imulq %r8, %rbx
    imulq %r8, %rbp
    imulq %r8, %rsi
    imulq %r8, %rdi
    imulq %r8, %r12
    imulq %r8, %r13
    imulq %r8, %r14
    .endr
    CALIB_END_WIDE()

/*
 * \~english LATENCY of a load that always hits the first level of cache.
 *
 * The cell holds its OWN address, so every load feeds the next one's address and
 * the chain cannot be predicted away: the machine cannot start the second load
 * before the first one has landed, which is exactly the definition of the
 * latency being measured.
 *
 * @code
 *      cell:  [ &cell ]
 *               ^   |
 *               '---'   mov (%rax), %rax  ->  rax = &cell, again
 * @endcode
 *
 * Eight bytes, so it never leaves the first cache level.  It is loaded with a
 * RIP-relative address, which needs no relocation at run time and works the same
 * wherever the image lands.
 *
 * \~spanish LATENCIA de una carga que siempre acierta en el primer nivel de
 * cache.
 *
 * La celda guarda su PROPIA direccion, asi que cada carga alimenta la direccion
 * de la siguiente y la cadena no se puede adivinar: la maquina no puede empezar
 * la segunda carga antes de que la primera haya llegado, que es exactamente la
 * definicion de la latencia que se mide.
 *
 * Ocho bytes, asi que no sale nunca del primer nivel de cache.  Se carga con una
 * direccion relativa a RIP, que no necesita reubicacion en ejecucion y funciona
 * igual caiga donde caiga la imagen.
 */
    .globl calib_load_lat
    .p2align 4
calib_load_lat:
    movq $CALIB_ITERS, %r11
    leaq calib_cell(%rip), %rax
    .p2align 4
9:
    .rept CALIB_UNROLL
    movq (%rax), %rax
    .endr
    decq %r11
    jnz 9b
    ret

    .data
    .p2align 3
calib_cell:
    .quad calib_cell
