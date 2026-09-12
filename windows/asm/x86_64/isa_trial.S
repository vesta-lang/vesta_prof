/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file isa_trial.S
 * @brief
 * \~english Calling bytes nobody wrote, and coming back from it.
 * \~spanish Llamar a bytes que nadie escribio, y volver de ahi.
 * \~
 *
 * \~english
 * WHY THIS IS ASSEMBLY, and it is the second place in this tree where there is no
 * argument.  What is needed here cannot be written in C: a guarded region whose
 * scope table names a filter of our own, an indirect call into an address that
 * holds no function, and a landing point the unwinder can reach.  `__try` is not
 * a language construct in GCC, and the tables that implement it are what GNU as
 * emits with `.seh_*`.
 *
 * THE SAME RECIPE AS `msr_guard.S`, with one difference that cost a morning --
 * see the `nop` after the `call` below.
 *
 * WHAT IT DOES NOT DECIDE, on purpose: which state the candidate starts from.  It
 * loads one, because running arbitrary bytes from whatever the C code left in the
 * registers corrupts the process doing the running -- but WHICH one is the
 * caller's, because the caller is who compares afterwards.  Here there is no
 * policy, only the loading, the jump and the net.
 *
 * \~spanish
 * POR QUE ESTO ES ENSAMBLADOR, y es el segundo sitio de este arbol donde no hay
 * discusion.  Lo que hace falta aqui no se puede escribir en C: una region
 * guardada cuya tabla de ambitos nombre un filtro nuestro, una llamada indirecta
 * a una direccion que no contiene ninguna funcion, y un punto de aterrizaje al
 * que el desenrollador pueda llegar.  `__try` no es una construccion del lenguaje
 * en GCC, y las tablas que lo implementan son lo que GNU as emite con `.seh_*`.
 *
 * LA MISMA RECETA QUE `msr_guard.S`, con una diferencia que costo una manana --
 * ver el `nop` de despues del `call`, abajo.
 *
 * LO QUE NO DECIDE, a proposito: desde que estado arranca el candidato.  Carga
 * uno, porque ejecutar bytes arbitrarios desde lo que el codigo de C dejara en los
 * registros corrompe al proceso que los ejecuta -- pero CUAL es del llamante,
 * porque el llamante es quien compara despues.  Aqui no hay politica, solo la
 * carga, el salto y la red.
 */

    .text

/*
 * \~english
 * int isa_try_with(void *code, const isa_state *in, u64 *rsp_at_call)
 *
 * Loads a CHOSEN register state and calls into the bytes.  Win64: `code` in RCX,
 * `in` in RDX, `rsp_at_call` in R8.
 *
 * THE ONLY WAY IN, and there used to be a second one.  A plainer entry that loaded
 * nothing existed, on the argument that most attempts do not need a chosen state
 * and should not pay for eight pushes.  Both halves of that turned out to be
 * wrong: EVERY attempt needs one, because bytes run with whatever the C code left
 * in the registers write into the process doing the measuring -- measured, the `00`
 * subtree had 152 of 256 candidates "run" that way, each one writing somewhere
 * inside us, and the next subtree killed the process.  And the saving was not one:
 * every attempt already costs an exception, next to which eight pushes are nothing.
 *
 * IT SAVES EIGHT REGISTERS THE CONVENTION CALLS THE CALLER'S, which is unusual and
 * necessary: it destroys every register it can, so the ones it is supposed to
 * preserve have to be put back -- and put back on the EXCEPTION path too, which is
 * why they are pushed with `.seh_pushreg` and not just popped at the end.  Almost
 * no attempt reaches the end.
 *
 * WHAT IT CANNOT SET, and it is a real limit rather than an oversight:
 *
 *   RSP   the stack pointer is the only way back.  Loading it would leave the
 *         unwinder without a frame to walk, and an exception -- which is how
 *         almost every attempt ends -- would have nowhere to land
 *   RIP   it is the candidate's own address, by construction
 *   the flags that belong to the SYSTEM (trap, interrupt, privilege level).
 *         The trap flag is not an academic exclusion: setting it single-steps
 *         and every attempt would come back as a debug exception instead of as
 *         what the instruction did
 *
 * \~spanish
 * int isa_try_with(void *code, const isa_state *in, u64 *rsp_at_call)
 *
 * Carga un estado de registros ELEGIDO y llama a los bytes.  Win64: `code` en RCX,
 * `in` en RDX, `rsp_at_call` en R8.
 *
 * LA UNICA ENTRADA, y hubo una segunda.  Existia una entrada mas simple que no
 * cargaba nada, con el argumento de que la mayoria de los intentos no necesitan un
 * estado elegido y no deben pagar ocho empujes.  Las dos mitades resultaron falsas:
 * TODO intento necesita uno, porque unos bytes corridos con lo que el codigo de C
 * dejara en los registros escriben dentro del proceso que mide -- medido, en el
 * subarbol de `00` "corrieron" asi 152 de 256 candidatas, cada una escribiendo en
 * algun sitio de dentro de nosotros, y el subarbol siguiente mato el proceso.  Y el
 * ahorro no era ahorro: cada intento cuesta ya una excepcion, al lado de la cual
 * ocho empujes no son nada.
 *
 * GUARDA OCHO REGISTROS QUE LA CONVENCION LLAMA DEL LLAMANTE, que es raro y
 * necesario: destruye todos los registros que puede, asi que los que deberia
 * preservar hay que devolverlos -- y devolverlos tambien por el camino de la
 * EXCEPCION, que es la razon de empujarlos con `.seh_pushreg` y no solo sacarlos al
 * final.  Casi ningun intento llega al final.
 *
 * LO QUE NO PUEDE FIJAR, y es un limite de verdad y no un olvido:
 *
 *   RSP   el puntero de pila es la unica via de vuelta.  Cargarlo dejaria al
 *         desenrollador sin marco que recorrer, y una excepcion -- que es como
 *         acaba casi todo intento -- no tendria donde aterrizar
 *   RIP   es la direccion del propio candidato, por construccion
 *   las banderas que son del SISTEMA (traza, interrupcion, nivel de privilegio).
 *         La de traza no es una exclusion academica: ponerla hace ejecucion paso
 *         a paso y todos los intentos volverian como excepcion de depuracion en
 *         vez de como lo que hizo la instruccion
 */

/* \~english The offsets into `isa_state`.  It is `u64 gpr[16]` followed by
 * `u64 flags`, so they are just multiples of eight -- and the C side pins that
 * with a compile-time assertion, because a layout change here would load the
 * registers from the wrong places and report effects that never happened.
 * \~spanish Los desplazamientos dentro de `isa_state`.  Es `u64 gpr[16]` seguido
 * de `u64 flags`, asi que son multiplos de ocho -- y el lado de C lo fija con una
 * asercion en tiempo de compilacion, porque un cambio de disposicion aqui
 * cargaria los registros de los sitios equivocados y reportaria efectos que nunca
 * ocurrieron. \~ */
#define ST_GPR(i) (8 * (i))
#define ST_FLAGS  (8 * 16)

/* \~english Only the arithmetic flags plus the reserved bit that must be one.
 * Trap, interrupt, privilege level and the virtual-mode ones are left out on
 * purpose -- see the note above.
 * \~spanish Solo las banderas aritmeticas mas el bit reservado que debe valer
 * uno.  Traza, interrupcion, nivel de privilegio y las de modo virtual quedan
 * fuera a proposito -- ver la nota de arriba. \~ */
#define FLAGS_MASK 0x0CD7

    .globl  isa_try_with
    .seh_proc isa_try_with
isa_try_with:
    .seh_handler __C_specific_handler, @except
    .seh_handlerdata
    .long   1
    .rva    .Lw_begin, .Lw_end, isa_trial_filter, .Lw_fail
    .text
    /*
     * \~english The eight registers Win64 calls the caller's.  They are saved
     * with `.seh_pushreg` and not by hand, and the difference matters: declaring
     * them puts them in the unwind information, so the unwinder restores them
     * when the exception takes us out -- which is the normal way out here.  Saved
     * by hand they would come back only on the path that returns, and every
     * attempt would leave the caller with another instruction's registers.
     *
     * Eight of them keeps the stack aligned to sixteen, which the convention
     * requires at the call.
     *
     * \~spanish Los ocho registros que Win64 considera del llamante.  Se guardan
     * con `.seh_pushreg` y no a mano, y la diferencia importa: declararlos los
     * mete en la informacion de desenrollado, asi que el desenrollador los
     * restaura cuando la excepcion nos saca -- que es la salida normal aqui.
     * Guardados a mano volverian solo por el camino que retorna, y cada intento
     * dejaria al llamante con los registros de otra instruccion.
     *
     * Ocho deja la pila alineada a dieciseis, que es lo que exige la convencion
     * en la llamada.
     */
    pushq   %rbx
    .seh_pushreg %rbx
    pushq   %rbp
    .seh_pushreg %rbp
    pushq   %rsi
    .seh_pushreg %rsi
    pushq   %rdi
    .seh_pushreg %rdi
    pushq   %r12
    .seh_pushreg %r12
    pushq   %r13
    .seh_pushreg %r13
    pushq   %r14
    .seh_pushreg %r14
    pushq   %r15
    .seh_pushreg %r15
    /* \~english 32 of shadow space plus 8 to stash the target, because by the time
     * we call it every register will hold something else.
     * \~spanish 32 de espacio de sombra mas 8 para guardar el destino, porque
     * cuando lo llamemos todos los registros llevaran otra cosa. \~ */
    subq    $40, %rsp
    .seh_stackalloc 40
    .seh_endprologue

    movq    %rcx, 32(%rsp)          /* el destino, fuera del alcance del estado */

    /*
     * \~english The stack pointer as it stands at the call, handed back to the
     * caller.  Without it there is no way to tell a candidate's stack effect from
     * the call's own: the absolute value depends on how deep the C stack happened
     * to be, so measuring it once and reusing it does not work -- it was measured
     * from a different depth.
     *
     * Stored BEFORE r8 is loaded from the state, because r8 is where the pointer
     * arrived.
     *
     * \~spanish El puntero de pila tal como esta en la llamada, devuelto al
     * llamante.  Sin el no hay forma de distinguir el efecto de pila de un
     * candidato del de la propia llamada: el valor absoluto depende de lo profunda
     * que estuviera la pila de C, asi que medirlo una vez y reutilizarlo no vale --
     * se midio desde otra profundidad.
     *
     * Se guarda ANTES de cargar r8 desde el estado, porque r8 es donde llego el
     * puntero.
     */
    movq    %rsp, (%r8)

    movq    %rdx, %rax              /* el estado, en rax hasta el final */

    /* \~english Everything except rax and rsp.  rax goes last because it is
     * holding the pointer we are reading from.
     * \~spanish Todo menos rax y rsp.  rax va el ultimo porque lleva el puntero
     * del que estamos leyendo. \~ */
    movq    ST_GPR(1)(%rax), %rcx
    movq    ST_GPR(2)(%rax), %rdx
    movq    ST_GPR(3)(%rax), %rbx
    movq    ST_GPR(5)(%rax), %rbp
    movq    ST_GPR(6)(%rax), %rsi
    movq    ST_GPR(7)(%rax), %rdi
    movq    ST_GPR(8)(%rax), %r8
    movq    ST_GPR(9)(%rax), %r9
    movq    ST_GPR(10)(%rax), %r10
    movq    ST_GPR(11)(%rax), %r11
    movq    ST_GPR(12)(%rax), %r12
    movq    ST_GPR(13)(%rax), %r13
    movq    ST_GPR(14)(%rax), %r14
    movq    ST_GPR(15)(%rax), %r15

    /* \~english The flags, masked.  `pushq`/`popfq` moves the stack and puts it
     * back, so it has to happen while rsp still means what the prologue left --
     * after the call it would be writing over the return address.
     * \~spanish Las banderas, enmascaradas.  `pushq`/`popfq` mueve la pila y la
     * devuelve, asi que tiene que ocurrir mientras rsp siga significando lo que
     * dejo el prologo -- despues de la llamada estaria escribiendo sobre la
     * direccion de retorno. \~ */
    pushq   ST_FLAGS(%rax)
    andq    $FLAGS_MASK, (%rsp)
    popfq

    movq    ST_GPR(0)(%rax), %rax

.Lw_begin:
    /*
     * \~english CALL and not JMP, and it matters twice.  A candidate that turns out
     * to be a `ret` returns here cleanly instead of popping an address that is not
     * its own; and the pushed return address is what lets the unwinder climb out of
     * a fault that happened in bytes with no unwind information of their own -- it
     * treats them as a leaf and takes the return address from the stack.
     *
     * \~spanish LLAMADA y no salto, y importa por dos cosas.  Un candidato que
     * resulte ser un `ret` vuelve aqui limpiamente en vez de sacar de la pila una
     * direccion que no es suya; y la direccion de retorno empujada es lo que
     * permite al desenrollador salir de un fallo ocurrido en bytes que no tienen
     * informacion de desenrollado propia -- los trata como hoja y toma la direccion
     * de retorno de la pila.
     */
    call    *32(%rsp)
    /*
     * \~english THIS `nop` IS NOT PADDING, and without it nothing works.
     *
     * The scope range is HALF-OPEN: `[begin, end)`.  When the fault happens inside
     * the CALLED code, what gets looked up in the table is not the faulting address
     * -- it is the RETURN address of this frame, which points just past the `call`.
     * With nothing here, that address equals `.Lw_end` and falls OUTSIDE the range
     * by one: no handler is found and the process dies, with the tables perfectly
     * well formed.
     *
     * It is why `msr_guard.S` never ran into this: there the faulting instruction
     * is its own, so its address lands inside the range without help.  A recipe
     * that works for an instruction that faults itself does NOT transfer as-is to a
     * fault in a callee.
     *
     * \~spanish ESTE `nop` NO ES RELLENO, y sin el no funciona nada.
     *
     * El rango del ambito es MEDIO ABIERTO: `[begin, end)`.  Cuando el fallo ocurre
     * dentro del codigo LLAMADO, lo que se busca en la tabla no es la direccion que
     * fallo -- es la direccion de RETORNO de este marco, que apunta justo despues
     * del `call`.  Sin nada aqui, esa direccion coincide con `.Lw_end` y cae FUERA
     * del rango por uno: no se encuentra manejador y el proceso muere, con las
     * tablas perfectamente bien formadas.
     *
     * Es por lo que `msr_guard.S` no se topo nunca con esto: alli la instruccion
     * que falla es la propia, asi que su direccion cae dentro del rango sin ayuda.
     * Una receta que vale para una instruccion que se autofalla NO transfiere tal
     * cual a un fallo en una llamada.
     */
    nop
.Lw_end:
    xorl    %eax, %eax
    addq    $40, %rsp
    popq    %r15
    popq    %r14
    popq    %r13
    popq    %r12
    popq    %rdi
    popq    %rsi
    popq    %rbp
    popq    %rbx
    ret

.Lw_fail:
    movl    $1, %eax
    addq    $40, %rsp
    popq    %r15
    popq    %r14
    popq    %r13
    popq    %r12
    popq    %rdi
    popq    %rsi
    popq    %rbp
    popq    %rbx
    ret
    .seh_endproc
