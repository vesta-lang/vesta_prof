/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file asm/x86_64/calib.h
 * @brief
 * \~english The measured kernels: how many instructions each one runs.
 * \~spanish Los nucleos medidos: cuantas instrucciones ejecuta cada uno.
 * \~
 *
 * \~english
 * ONE HEADER FOR THE ASSEMBLER AND FOR C, and it is not a trick: the counts have
 * to be the same on both sides or every result is wrong by a constant nobody
 * notices.  A `.S` file goes through the preprocessor -- that is what the capital
 * letter means -- so it can include this, and `__ASSEMBLER__` hides from it the
 * part it would not understand.
 *
 * Two copies of these numbers would not fail to build and would not fault: the
 * bench would divide by the wrong count and report a latency that looks
 * perfectly plausible.
 *
 * \~spanish
 * UNA CABECERA PARA EL ENSAMBLADOR Y PARA C, y no es un truco: las cuentas
 * tienen que ser las mismas en los dos lados o todos los resultados salen mal
 * por un factor constante que nadie nota.  Un fichero `.S` pasa por el
 * preprocesador -- eso significa la mayuscula --, asi que puede incluir esto, y
 * `__ASSEMBLER__` le esconde la parte que no entenderia.
 *
 * Dos copias de estos numeros no fallarian al construir ni darian excepcion: el
 * banco dividiria por la cuenta equivocada y publicaria una latencia de aspecto
 * perfectamente plausible.
 */

#ifndef VXP_COMMON_ASM_X86_64_CALIB_H
#define VXP_COMMON_ASM_X86_64_CALIB_H

/**
 * @brief
 * \~english How many copies of the instruction go inside one loop iteration.
 * \~spanish Cuantas copias de la instruccion van dentro de una vuelta del bucle.
 * \~
 *
 * \~english
 * THE LOOP ITSELF COSTS, and this is what dilutes it.  Every iteration pays a
 * decrement and a branch -- about one cycle -- and that cost lands on whatever
 * is being measured.  With 64 copies per iteration the error is 1/64, about
 * 1.5%, which is below what the measurement is worth arguing about.  With 8 it
 * would be 12%, and a latency of 1 would read as 1.12.
 *
 * \~spanish
 * EL BUCLE CUESTA, y esto es lo que lo diluye.  Cada vuelta paga un decremento y
 * un salto -- alrededor de un ciclo -- y ese coste cae sobre lo que se este
 * midiendo.  Con 64 copias por vuelta el error es 1/64, un 1,5%, que esta por
 * debajo de lo que la medida da de si.  Con 8 seria del 12%, y una latencia de 1
 * se leeria como 1,12.
 */
#define CALIB_UNROLL 60

/**
 * @brief
 * \~english How many independent accumulators the throughput kernels use.
 * \~spanish Cuantos acumuladores independientes usan los nucleos de throughput.
 * \~
 *
 * \~english
 * TWELVE, and `CALIB_UNROLL` IS A MULTIPLE OF IT, which is why the unroll is 60
 * and not 64.  A throughput kernel repeats a group of this many, so with 64 the
 * assembler would divide, truncate, and emit fewer instructions than the C side
 * divides by.  Nothing fails: every throughput figure comes out low by a
 * perfectly believable amount.
 *
 * TWELVE AND NOT SIX, AND THE FIRST VERSION HAD SIX AND WAS WRONG.  There are
 * only seven registers volatile in both conventions, one of them holds the
 * counter and another the operand, so six groups meant repeating one destination
 * -- and a destination that repeats is a dependency chain.  The number it
 * produced was not the machine's throughput, it was that chain: 60 instructions
 * over a 20-deep chain is 0.333 cycles each, which is exactly what came out.
 *
 * WHAT GAVE IT AWAY: a P core and an E core measured the SAME.  Two different
 * microarchitectures cannot have the same throughput, so what was being measured
 * was not theirs.  It is worth writing down because the number looked entirely
 * reasonable -- three additions per cycle is a plausible thing for a processor to
 * do.
 *
 * Twelve needs registers the calling convention says must be preserved, so the
 * throughput kernels save and restore them.  That is the price, and it is the
 * right one to pay: with fewer independent chains than the part has ports, what
 * gets measured is the kernel and never the machine.
 *
 * \~spanish
 * DOCE, y `CALIB_UNROLL` ES MULTIPLO DE EL, que es por lo que el desenrollado es
 * 60 y no 64.  Un nucleo de throughput repite un grupo de estos, asi que con 64
 * el ensamblador dividiria, truncaria, y emitiria menos instrucciones de las que
 * el lado de C divide.  No falla nada: cada cifra de throughput sale baja por una
 * cantidad perfectamente creible.
 *
 * DOCE Y NO SEIS, Y LA PRIMERA VERSION TENIA SEIS Y ESTABA MAL.  Solo hay siete
 * registros volatiles en las dos convenciones, uno lleva el contador y otro el
 * operando, asi que seis grupos obligaban a repetir un destino -- y un destino
 * que se repite es una cadena de dependencias.  El numero que salia no era el
 * throughput de la maquina, era esa cadena: 60 instrucciones sobre una cadena de
 * 20 son 0,333 ciclos cada una, que es exactamente lo que dio.
 *
 * LO QUE LO DELATO: un nucleo P y uno E midieron LO MISMO.  Dos
 * microarquitecturas distintas no pueden tener el mismo throughput, asi que lo
 * que se estaba midiendo no era el suyo.  Merece quedar escrito porque el numero
 * tenia un aspecto de lo mas razonable -- tres sumas por ciclo es una cosa
 * plausible que haga un procesador.
 *
 * Doce necesita registros que la convencion de llamada obliga a preservar, asi
 * que los nucleos de throughput los guardan y los restauran.  Ese es el precio, y
 * es el correcto: con menos cadenas independientes que puertos tenga la pieza, lo
 * que se mide es el nucleo y nunca la maquina.
 */
#define CALIB_TP_WAYS 12

/**
 * @brief
 * \~english How many iterations one call runs.
 * \~spanish Cuantas vueltas da una llamada.
 * \~
 *
 * \~english
 * 64 x 156250 is ten million instructions, which at one cycle each is a few
 * milliseconds.  It is chosen against a floor that was measured, not guessed:
 * bracketing anything with `rdpmc` costs 49 to 71 cycles, so ten million cycles
 * puts the measurement overhead at one part in two hundred thousand.
 *
 * \~spanish
 * 64 x 156250 son diez millones de instrucciones, que a un ciclo cada una son
 * unos pocos milisegundos.  Se elige contra un suelo MEDIDO, no supuesto: acotar
 * cualquier cosa con `rdpmc` cuesta de 49 a 71 ciclos, asi que diez millones de
 * ciclos dejan el coste de medir en una parte entre doscientas mil.
 */
#define CALIB_ITERS 156250

/** @brief
 *  \~english Instructions under test that one call executes.
 *  \~spanish Instrucciones bajo prueba que ejecuta una llamada. \~ */
#define CALIB_INSTRS ((u64)CALIB_UNROLL * (u64)CALIB_ITERS)

#ifndef __ASSEMBLER__

#include "vxp_base.h"

/**
 * @brief
 * \~english The kernels.  No arguments, no return, and that is deliberate.
 * \~spanish Los nucleos.  Sin argumentos, sin retorno, y es deliberado.
 * \~
 *
 * \~english
 * WHY THEY TAKE NOTHING.  `common/asm/README.md` sets the test a file has to
 * pass to live there: does it mention anything the operating system defines?  A
 * routine with a C interface normally fails it, because in x86-64 the ABI is the
 * system's -- Windows passes in RCX and System V in RDI, and the preserved
 * register sets differ.
 *
 * A function with no arguments and no return value has none of that surface.  The
 * latency kernels go further and use only registers volatile in BOTH conventions
 * -- RAX, RCX, RDX and R8 to R11 -- so they save nothing and touch no stack; the
 * throughput ones need more independent chains than that allows, so they borrow
 * the preserved registers and give them back, saving the union of what the two
 * conventions ask for.  Neither calls anything, so Windows' shadow space does
 * not apply.  That is what lets one single file serve both systems instead of
 * two that drift.
 *
 * WHAT THEY DO NOT SOLVE: the assembler.  The syntax here is GNU's, so this
 * builds with GCC and with Clang and not with MSVC, which wants MASM.  It is the
 * third axis `common/asm/README.md` names, and it is the reason the calibration
 * tool is the one thing in this tree that is not built by the three compilers.
 * Writing it twice would be two versions of a measurement, which is worse than
 * one that not everybody can build.
 *
 * \~spanish
 * POR QUE NO TOMAN NADA.  `common/asm/README.md` fija la prueba que tiene que
 * pasar un fichero para vivir alli: ¿menciona algo que defina el sistema
 * operativo?  Una rutina con interfaz en C normalmente no la pasa, porque en
 * x86-64 la ABI la decide el sistema -- Windows pasa en RCX y System V en RDI, y
 * los conjuntos de registros preservados difieren.
 *
 * Una funcion sin argumentos y sin valor de retorno no tiene nada de esa
 * superficie, y estas usan solo registros volatiles en LAS DOS convenciones
 * -- RAX, RCX, RDX y de R8 a R11 --, asi que no guardan ni restauran nada.  No
 * tocan la pila, con lo que el espacio de sombra de Windows tampoco aplica.  Eso
 * es lo que permite que un solo fichero sirva a los dos sistemas en vez de dos
 * que se separan.
 *
 * LO QUE NO RESUELVE: el ensamblador.  La sintaxis de aqui es la de GNU, asi que
 * esto se construye con GCC y con Clang y no con MSVC, que quiere MASM.  Es el
 * tercer eje que nombra `common/asm/README.md`, y es la razon de que la
 * herramienta de calibracion sea lo unico de este arbol que no construyen los
 * tres compiladores.  Escribirla dos veces serian dos versiones de una medida,
 * que es peor que una que no todos puedan construir.
 */

/** @brief
 *  \~english Nothing under test: only the loop.  It is the floor to subtract.
 *  \~spanish Nada bajo prueba: solo el bucle.  Es el suelo que hay que
 *            restar. \~ */
void calib_empty(void);

/** @brief
 *  \~english `add` chained on itself: measures LATENCY.
 *  \~spanish `add` encadenado consigo mismo: mide LATENCIA. \~ */
void calib_add_lat(void);

/** @brief
 *  \~english Six independent `add`: measures THROUGHPUT.
 *  \~spanish Seis `add` independientes: mide THROUGHPUT. \~ */
void calib_add_tp(void);

/** @brief
 *  \~english `imul` chained on itself: latency of the multiplier.
 *  \~spanish `imul` encadenado consigo mismo: latencia del multiplicador. \~ */
void calib_imul_lat(void);

/** @brief
 *  \~english Six independent `imul`: how many fit per cycle.
 *  \~spanish Seis `imul` independientes: cuantos caben por ciclo. \~ */
void calib_imul_tp(void);

/**
 * @brief
 * \~english The same load, through the three ways of writing an address.
 * \~spanish La misma carga, por las tres formas de escribir una direccion.
 * \~
 *
 * \~english
 * A FORM OF THE TABLE DOES NOT PIN DOWN THE ADDRESSING MODE, and that is what
 * these three are for.  `MOV_GPRv_MEMv/64x64` is one row -- one latency -- but
 * the machine does not have to take the same time for `(%rax)`, for
 * `8(%rax)` and for `(%rax,%rcx,8)`: several parts have a fast path for the
 * simple form and charge an extra cycle for an index.
 *
 * So when a measurement disagrees with that row, there are two explanations and
 * they are not the same thing:
 *
 *   the table is off for this part      -> the three measure the same
 *   the row describes another mode      -> the three come out different, and
 *                                          one of them matches the row
 *
 * Without the three, a discrepancy has no way of telling which it is, and
 * calling a table wrong when what differs is the way one wrote the address is
 * exactly the kind of confident mistake this whole tree is arranged to avoid.
 *
 * \~spanish
 * UNA FORMA DE LA TABLA NO FIJA EL MODO DE DIRECCIONAMIENTO, y para eso estan
 * estas tres.  `MOV_GPRv_MEMv/64x64` es una fila -- una latencia -- pero la
 * maquina no tiene por que tardar lo mismo con `(%rax)`, con `8(%rax)` y con
 * `(%rax,%rcx,8)`: varias piezas tienen un camino rapido para la forma simple y
 * cobran un ciclo de mas por un indice.
 *
 * Asi que cuando una medida discrepa de esa fila hay dos explicaciones, y no son
 * la misma cosa: ver el cuadro de arriba.  Sin las tres, un desvio no tiene como
 * decir cual de las dos es, y llamar equivocada a una tabla cuando lo que
 * difiere es como se escribio la direccion es justo la clase de error confiado
 * que este arbol entero esta dispuesto para no cometer.
 */

/** @brief
 *  \~english `mov (%rax), %rax`: base register and nothing else.
 *  \~spanish `mov (%rax), %rax`: registro base y nada mas. \~ */
void calib_load_lat(void);

/** @brief
 *  \~english `mov 8(%rax), %rax`: base plus a small displacement.
 *  \~spanish `mov 8(%rax), %rax`: base mas un desplazamiento pequeno. \~ */
void calib_load_disp(void);

/** @brief
 *  \~english `mov (%rax,%rcx,8), %rax`: base plus scaled index.
 *  \~spanish `mov (%rax,%rcx,8), %rax`: base mas indice escalado. \~ */
void calib_load_index(void);

#endif /* __ASSEMBLER__ */

#endif /* VXP_COMMON_ASM_X86_64_CALIB_H */
