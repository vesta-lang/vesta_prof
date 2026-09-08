/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/*
 * \~english
 * MSR read that does NOT take the machine down if the register does not exist.
 *
 * THE PROBLEM.  Of the 1,588 MSRs Intel documents, only 315 bring the CPUID
 * condition that says when they exist alongside them.  Reading one of the others
 * is a #GP -- a general protection fault -- and in ring 0 an unhandled #GP is
 * KMODE_EXCEPTION_NOT_HANDLED: a blue screen.  Without this file the dump can
 * only look at 315 of 1,588.
 *
 * WHY IN ASSEMBLY AND NOT IN C.  This is exactly the case `windows/asm/`
 * describes: the exact instruction matters.  The scope table points at a range
 * of ADDRESSES, so that range has to contain the instruction that can fault and
 * nothing else.  Written in C the compiler may move, duplicate or reorder what
 * is inside, and the range would stop telling the truth with nothing to warn.
 * Besides, mingw has no `__try`/`__except`.
 *
 * HOW IT WORKS.  On x64 exception handling is TABLE-DRIVEN, not a chain on the
 * stack.  `.seh_handler` hangs a language handler off this function --
 * `__C_specific_handler`, which `ntoskrnl.exe` exports -- and `.seh_handlerdata`
 * gives it a scope table with one entry:
 *
 *     .rva  begin, end, filter, target
 *
 *      +--------------------------------------------+
 *      |  msr_read_guarded                          |
 *      |    prologue                                |
 *      |    .Ltry_begin:  rdmsr   <-- may fault ----+---> #GP
 *      |    .Ltry_end:                              |      |
 *      |    ok path ------------------------.       |      v
 *      |    .Lfail: <-------- target -------|-------+---  filter says 1
 *      |    epilogue, ret                   |       |     (EXECUTE_HANDLER)
 *      +--------------------------------------------+
 *
 * THIS ONLY EXISTS IF THE BINARY HAS `.pdata`.  The driver used to carry
 * `-fno-asynchronous-unwind-tables` and its exception directory was EMPTY, so
 * this mechanism would have done nothing: the exception would have found no
 * table and it would have been a blue screen all the same.  See the comment in
 * `windows/CMakeLists.txt`.
 *
 * CHECKED IN RING 3 BEFORE USING IT HERE.  `rdmsr` in user mode faults ALWAYS,
 * so the same mechanism gets exercised where a fault kills a process instead of
 * the machine.
 *
 * SYNTAX: GNU as, not MASM.  This driver is built with MinGW and without the
 * WDK, so the assembler is `as` and the SEH directives are its own.
 *
 * \~spanish
 * Lectura de MSR que NO tumba la maquina si el registro no existe.
 *
 * EL PROBLEMA.  De los 1.588 MSR que documenta Intel, solo 315 traen al lado la
 * condicion de CPUID que dice cuando existen.  Leer uno de los otros es un #GP
 * -- una excepcion de proteccion general --, y en anillo cero un #GP sin
 * manejador es KMODE_EXCEPTION_NOT_HANDLED: pantalla azul.  Sin este fichero el
 * volcado solo puede mirar 315 de 1.588.
 *
 * POR QUE EN ENSAMBLADOR Y NO EN C.  Es justo el caso que `windows/asm/`
 * describe: importa la instruccion exacta.  La tabla de ambitos apunta a un
 * rango de DIRECCIONES, asi que ese rango tiene que contener la instruccion que
 * puede fallar y nada mas.  Escrito en C el compilador puede mover, duplicar o
 * reordenar lo de dentro, y el rango dejaria de decir la verdad sin que nada
 * avise.  Ademas mingw no tiene `__try`/`__except`.
 *
 * COMO FUNCIONA.  En x64 el manejo de excepciones va POR TABLAS, no por una
 * cadena en la pila.  `.seh_handler` cuelga de esta funcion un manejador de
 * lenguaje -- `__C_specific_handler`, que exporta `ntoskrnl.exe` -- y
 * `.seh_handlerdata` le da una tabla de ambitos con una entrada:
 *
 *     .rva  inicio, fin, filtro, destino
 *
 *      +--------------------------------------------+
 *      |  msr_read_guarded                          |
 *      |    prologo                                 |
 *      |    .Ltry_begin:  rdmsr   <-- puede fallar -+---> #GP
 *      |    .Ltry_end:                              |      |
 *      |    camino bueno --------------------.      |      v
 *      |    .Lfail: <-------- destino -------|------+---  el filtro dice 1
 *      |    epilogo, ret                     |      |     (EXECUTE_HANDLER)
 *      +--------------------------------------------+
 *
 * ESTO SOLO EXISTE SI EL BINARIO TIENE `.pdata`.  El driver llevaba
 * `-fno-asynchronous-unwind-tables` y su directorio de excepciones estaba
 * VACIO, con lo que este mecanismo no habria hecho nada: la excepcion no habria
 * encontrado tabla y habria sido pantalla azul igual.  Ver el comentario en
 * `windows/CMakeLists.txt`.
 *
 * COMPROBADO EN ANILLO 3 ANTES DE USARLO AQUI.  `rdmsr` en modo usuario falla
 * SIEMPRE, asi que el mismo mecanismo se ejercita donde un fallo mata un proceso
 * en vez de la maquina.
 *
 * SINTAXIS: GNU as, no MASM.  Este driver se construye con MinGW y sin WDK, asi
 * que el ensamblador es `as` y las directivas SEH son las suyas.
 * \~
 */

    .text

/*
 * \~english
 * The filter.  `__C_specific_handler` calls it with (EXCEPTION_POINTERS*, frame)
 * and it decides.  Always 1: here any exception inside the window means "the
 * register does not exist", which is exactly what we wanted to find out.
 *
 * \~spanish
 * El filtro.  Lo llama `__C_specific_handler` con (EXCEPTION_POINTERS*, frame) y
 * decide.  Siempre 1: aqui cualquier excepcion en la ventana es "el registro no
 * existe", que es exactamente lo que se queria averiguar.
 * \~
 */
    .globl  vxp_msr_guard_filter
    .def    vxp_msr_guard_filter; .scl 2; .type 32; .endef
    .seh_proc vxp_msr_guard_filter
vxp_msr_guard_filter:
    .seh_endprologue
    movl    $1, %eax                /* EXCEPTION_EXECUTE_HANDLER */
    ret
    .seh_endproc

/*
 * int msr_read_guarded(u32 addr, u64 *out)
 *
 * \~english  Win64: addr in ecx, out in rdx.
 *            Returns 1 if it read, 0 if the processor refused the read.
 * \~spanish  Win64: addr en ecx, out en rdx.
 *            Devuelve 1 si leyo, 0 si el procesador rechazo la lectura.
 * \~
 */
    .globl  msr_read_guarded
    .def    msr_read_guarded; .scl 2; .type 32; .endef
    .seh_proc msr_read_guarded
msr_read_guarded:
    .seh_handler __C_specific_handler, @except
    .seh_handlerdata
    .long   1                       /* \~english one scope table entry
                                     * \~spanish una entrada en la tabla de
                                     * ambitos \~ */
    .rva    .Ltry_begin, .Ltry_end, vxp_msr_guard_filter, .Lfail
    .text
    subq    $48, %rsp
    .seh_stackalloc 48
    .seh_endprologue

    /* \~english The output pointer goes on the STACK and not in a register: when
     * unwinding to the target there is no guarantee whatsoever about which
     * register survives.
     * \~spanish El puntero de salida va a la PILA y no a un registro: al
     * desenrollar hasta el destino no hay ninguna garantia sobre que registro
     * sobrevive. \~ */
    movq    %rdx, 40(%rsp)

    /* \~english The guarded window: ONLY the instruction that can fault.  The
     * narrower it is, the less it can hide -- a fault from something else inside
     * the range would read as "the MSR does not exist".
     * \~spanish La ventana guardada: SOLO la instruccion que puede fallar.
     * Cuanto mas estrecha, menos puede esconder -- un fallo de otra cosa dentro
     * del rango se leeria como "el MSR no existe". \~ */
.Ltry_begin:
    rdmsr                           /* \~english ecx = address, returns edx:eax
                                     * \~spanish ecx = direccion, devuelve
                                     * edx:eax \~ */
.Ltry_end:

    shlq    $32, %rdx
    orq     %rdx, %rax
    movq    40(%rsp), %rcx
    movq    %rax, (%rcx)
    movl    $1, %eax
    jmp     .Ldone

.Lfail:
    /* \~english This is where control lands when the filter accepts.  The zero
     * destination is on purpose: whoever receives a failure must not find the
     * previous read's value there, which would be another register's data.
     * \~spanish Se sale por aqui cuando el filtro acepta.  El destino cero es a
     * proposito: quien reciba un fallo no debe encontrarse el valor de la
     * lectura anterior, que seria un dato de otro registro. \~ */
    movq    40(%rsp), %rcx
    movq    $0, (%rcx)
    xorl    %eax, %eax

.Ldone:
    addq    $48, %rsp
    ret
    .seh_endproc
