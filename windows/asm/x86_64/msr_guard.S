/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/*
 * Lectura de MSR que NO tumba la maquina si el registro no existe.
 *
 * EL PROBLEMA.  De los 1.588 MSR que documenta Intel, solo 315 traen al lado la
 * condicion de CPUID que dice cuando existen.  Leer uno de los otros es un
 * #GP, y en anillo cero un #GP sin manejador es KMODE_EXCEPTION_NOT_HANDLED --
 * pantalla azul.  Sin este fichero, el volcado solo puede mirar 315.
 *
 * POR QUE EN ENSAMBLADOR Y NO EN C.  Es justo el caso que `windows/asm/`
 * describe: importa la instruccion exacta.  La tabla de ambitos apunta a un
 * rango [inicio, fin) de DIRECCIONES, asi que ese rango tiene que contener la
 * instruccion que puede fallar y nada mas.  Escrito en C, el compilador puede
 * mover, duplicar o reordenar lo de dentro, y el rango dejaria de decir la
 * verdad sin que nada avise.  Ademas mingw no tiene `__try`/`__except`.
 *
 * COMO FUNCIONA.  En x64 el manejo de excepciones va POR TABLAS, no por una
 * cadena en la pila.  `.seh_handler` cuelga de esta funcion un manejador de
 * lenguaje -- `__C_specific_handler`, que exporta `ntoskrnl.exe` --, y
 * `.seh_handlerdata` le da una tabla de ambitos con una entrada:
 *
 *     .rva  inicio, fin, filtro, destino
 *
 * Si algo falla entre `inicio` y `fin`, se llama al filtro; si devuelve 1
 * (EXCEPTION_EXECUTE_HANDLER), la ejecucion sigue en `destino`.
 *
 * ESTO SOLO EXISTE SI EL BINARIO TIENE `.pdata`.  El driver llevaba
 * `-fno-asynchronous-unwind-tables` y su directorio de excepciones estaba
 * VACIO, con lo que este mecanismo no habria hecho nada: la excepcion no habria
 * encontrado tabla y habria sido pantalla azul igual.  Ver el comentario en
 * `windows/CMakeLists.txt`.
 *
 * COMPROBADO EN ANILLO 3 ANTES DE USARLO AQUI.  `rdmsr` en modo usuario falla
 * SIEMPRE, asi que el mismo mecanismo se ejercita donde un fallo mata un
 * proceso en vez de la maquina.  Ver `tests/test_msr_guard.c`.
 *
 * SINTAXIS: GNU as, no MASM.  Este driver se construye con MinGW y sin WDK, asi
 * que el ensamblador es `as` y las directivas SEH son las suyas.
 */

    .text

/*
 * El filtro.  Lo llama __C_specific_handler con (EXCEPTION_POINTERS*, frame) y
 * decide que hacer.  Siempre 1: aqui cualquier excepcion en la ventana es "el
 * registro no existe", que es exactamente lo que se queria averiguar.
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
 * Win64: addr en ecx, out en rdx.
 * Devuelve 1 si leyo, 0 si el procesador rechazo la lectura.
 */
    .globl  msr_read_guarded
    .def    msr_read_guarded; .scl 2; .type 32; .endef
    .seh_proc msr_read_guarded
msr_read_guarded:
    .seh_handler __C_specific_handler, @except
    .seh_handlerdata
    .long   1                       /* una entrada en la tabla de ambitos */
    .rva    .Ltry_begin, .Ltry_end, vxp_msr_guard_filter, .Lfail
    .text
    subq    $48, %rsp
    .seh_stackalloc 48
    .seh_endprologue

    /* El puntero de salida va a la PILA y no a un registro: al desenrollar
     * hasta el destino no hay ninguna garantia sobre que registro sobrevive. */
    movq    %rdx, 40(%rsp)

    /* La ventana guardada: SOLO la instruccion que puede fallar.  Cuanto mas
     * estrecha, menos puede esconder -- un fallo de otra cosa dentro del rango
     * se leeria como "el MSR no existe". */
.Ltry_begin:
    rdmsr                           /* ecx = direccion, devuelve edx:eax */
.Ltry_end:

    shlq    $32, %rdx
    orq     %rdx, %rax
    movq    40(%rsp), %rcx
    movq    %rax, (%rcx)
    movl    $1, %eax
    jmp     .Ldone

.Lfail:
    /* Se sale por aqui cuando el filtro acepta.  El destino cero es a
     * proposito: quien reciba un fallo no debe encontrarse el valor de la
     * lectura anterior, que seria un dato de otro registro. */
    movq    40(%rsp), %rcx
    movq    $0, (%rcx)
    xorl    %eax, %eax

.Ldone:
    addq    $48, %rsp
    ret
    .seh_endproc
