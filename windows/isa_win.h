/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file isa_win.h
 * @brief
 * \~english The apparatus for trying bytes: a pair of pages and one attempt.
 * \~spanish El aparato para probar bytes: una pareja de paginas y un intento.
 * \~
 *
 * \~english
 * WHY IT IS HERE AND NOT IN `common/`.  The shape of an answer -- what came out,
 * how long it was, what state it left -- is the processor's, and lives in
 * `common/isa/trial.h`.  Reserving pages and catching exceptions is the system's,
 * and that is this file.  It is the same seam as `msr/access.h` against
 * `msr_win.c`, and for the same reason: what it buys is that the deciding part can
 * be tested without any of this.
 *
 * \~spanish
 * POR QUE ESTA AQUI Y NO EN `common/`.  La forma de una respuesta -- que salio,
 * cuanto media, que estado dejo -- es del procesador, y vive en
 * `common/isa/trial.h`.  Reservar paginas y capturar excepciones es del sistema, y
 * eso es este fichero.  Es la misma costura que `msr/access.h` frente a
 * `msr_win.c`, y por lo mismo: lo que se compra es que la parte que decide se
 * pueda probar sin nada de esto.
 */

#ifndef VXP_WINDOWS_ISA_WIN_H
#define VXP_WINDOWS_ISA_WIN_H

#include "isa/sweep.h"
#include "isa/trial.h"

/**
 * @brief
 * \~english Where attempts are made: one page that runs, one that never maps.
 * \~spanish Donde se hacen los intentos: una pagina que ejecuta, otra que no se
 *           mapea nunca.
 * \~
 *
 * \~english
 * ONE PER WORKER, and it is what lets the search be parallel without a single
 * lock.  Two workers sharing an arena would overwrite each other's candidate
 * between placing it and executing it, and the answer would be about neither of
 * the two.
 *
 * \~spanish
 * UNA POR TRABAJADOR, y es lo que permite que la busqueda sea paralela sin un solo
 * cerrojo.  Dos trabajadores compartiendo una arena se sobreescribirian el
 * candidato entre ponerlo y ejecutarlo, y la respuesta no seria de ninguno de los
 * dos.
 */
typedef struct isa_arena {
    void *base;   /**< \~english the whole reservation, for releasing it \~spanish la reserva entera, para liberarla \~ */
    u8 *first;    /**< \~english the page that executes \~spanish la pagina que ejecuta \~ */
    u8 *boundary; /**< \~english first byte of the page that is not there \~spanish primer byte de la pagina que no esta \~ */
    u32 page;
} isa_arena;

/**
 * @brief
 * \~english Reserves the pair, and checks the apparatus works before anyone uses it.
 * \~spanish Reserva la pareja, y comprueba que el aparato funciona antes de que
 *           nadie lo use.
 * \~
 *
 * @return `OK`; `ERR_NOSPACE` \~english if the system would not give the pages; `ERR_STATE` if the apparatus does not work \~spanish si el sistema no da las paginas; `ERR_STATE` si el aparato no funciona \~
 *
 * \~english
 * OPENING IT RUNS ONE INSTRUCTION, through BOTH guarded paths -- the plain one and
 * the one that loads a state, because they are different code and both get used.
 * The instruction is a `nop`, whose outcome is not a matter of opinion, so a
 * failure here means the whole apparatus is broken; and finding that out at the
 * open is far better than finding it out as a million plausible answers.
 *
 * \~spanish
 * ABRIRLA EJECUTA UNA INSTRUCCION, por los DOS caminos guardados -- el simple y el
 * que carga un estado, porque son codigo distinto y los dos se usan.  La
 * instruccion es un `nop`, cuyo resultado no es opinable, asi que un fallo aqui
 * quiere decir que el aparato entero esta roto; y enterarse al abrir es mucho mejor
 * que enterarse en forma de un millon de respuestas plausibles.
 */
status isa_arena_open(isa_arena *a);

/** @brief
 *  \~english Gives it back.  Safe on an arena that was never opened.
 *  \~spanish La devuelve.  Segura sobre una arena que nunca se abrio. \~ */
void isa_arena_close(isa_arena *a);

/**
 * @brief
 * \~english One attempt with exactly `n` bytes available before the boundary.
 * \~spanish Un intento con exactamente `n` bytes disponibles antes de la frontera.
 * \~
 *
 * @param n \~english 1 to `ISA_MAX_LEN` \~spanish de 1 a `ISA_MAX_LEN` \~
 *
 * \~english
 * It answers about THESE `n` bytes, not about the instruction: if the decoder
 * wanted more, the outcome is `ISA_TRUNCATED` and there is no length.  Whoever
 * wants the length asks `isa_measure`, which is this in a loop.
 *
 * \~spanish
 * Responde sobre ESTOS `n` bytes, no sobre la instruccion: si el decodificador
 * queria mas, el resultado es `ISA_TRUNCATED` y no hay longitud.  Quien quiera la
 * longitud llama a `isa_measure`, que es esto en un bucle.
 */
status isa_attempt(const isa_arena *a, const u8 *bytes, u32 n, isa_result *out);

/**
 * @brief
 * \~english How long the instruction at `bytes` really is, asking the processor.
 * \~spanish Cuanto mide de verdad la instruccion de `bytes`, preguntandole al
 *           procesador.
 * \~
 *
 * @param bytes \~english at least `ISA_MAX_LEN` readable bytes \~spanish al menos `ISA_MAX_LEN` bytes legibles \~
 *
 * \~english
 * IT IS NOT A DISASSEMBLER AND THAT IS THE POINT.  A disassembler says how long
 * the instruction should be according to what someone wrote down; this says how
 * many bytes the silicon actually consumed.  When the two disagree, one of them is
 * wrong, and the one that runs the code is not it.
 *
 * \~spanish
 * NO ES UN DESENSAMBLADOR Y ESO ES EL ASUNTO.  Un desensamblador dice cuanto
 * deberia medir la instruccion segun lo que alguien apunto; esto dice cuantos
 * bytes consumio el silicio de verdad.  Cuando los dos discrepan, uno esta
 * equivocado, y no es el que ejecuta el codigo.
 */
status isa_measure(const isa_arena *a, const u8 *bytes, isa_result *out);

/**
 * @brief
 * \~english Loads a chosen state and calls `code` inside a guarded region.  Lives
 *           in `asm/x86_64/isa_trial.S`.
 * \~spanish Carga un estado elegido y llama a `code` dentro de una region
 *           guardada.  Vive en `asm/x86_64/isa_trial.S`.
 * \~
 *
 * @return \~english 0 if the bytes returned on their own, 1 if an exception came out \~spanish 0 si los bytes volvieron por su cuenta, 1 si salio una excepcion \~
 *
 * \~english
 * It is declared here and has no header of its own because it is one function and
 * nobody outside this pair of files should be calling it: on its own it says only
 * whether something failed, and the whole answer is in what the filter wrote down.
 *
 * \~spanish
 * Se declara aqui y no tiene cabecera propia porque es una sola funcion y nadie de
 * fuera de esta pareja de ficheros deberia llamarla: por si sola solo dice si algo
 * fallo, y la respuesta entera esta en lo que apunto el filtro.
 */
int isa_try_with(void *code, const isa_state *in, u64 *rsp_at_call);

/**
 * @brief
 * \~english One attempt from a chosen state, saying which registers moved.
 * \~spanish Un intento desde un estado elegido, diciendo que registros se movieron.
 * \~
 *
 * @param in \~english the state the candidate starts from \~spanish el estado del que arranca el candidato \~
 *
 * \~english
 * NOT EVERY REGISTER CAN BE CHOSEN: the stack pointer is the way back and the
 * instruction pointer is the candidate itself.  The rest are loaded, and so are
 * the arithmetic flags -- the ones belonging to the system are left alone, because
 * setting the trap flag would turn every attempt into a debug exception.
 *
 * \~spanish
 * NO TODOS LOS REGISTROS SE PUEDEN ELEGIR: el puntero de pila es la via de vuelta
 * y el de instruccion es el propio candidato.  El resto se cargan, y tambien las
 * banderas aritmeticas -- las del sistema se dejan en paz, porque poner la de
 * traza convertiria cada intento en una excepcion de depuracion.
 */
status isa_attempt_with(const isa_arena *a, const u8 *bytes, u32 n,
                        const isa_state *in, isa_result *out);

/**
 * @brief
 * \~english What the instruction does, from two runs with different inputs.
 * \~spanish Que hace la instruccion, a partir de dos corridas con entradas
 *           distintas.
 * \~
 *
 * @param n \~english how many bytes; use `isa_measure` first to know it \~spanish cuantos bytes; usar `isa_measure` antes para saberlo \~
 *
 * \~english
 * TWO RUNS BECAUSE ONE CANNOT TELL A CONSTANT FROM A COPY.  With a single input,
 * an instruction that writes zero and one that copies a register holding zero look
 * exactly the same.  The two inputs are chosen so that every register holds a
 * different, recognisable value in each -- with zeros everywhere, "wrote 0" and
 * "wrote nothing" would also be indistinguishable.
 *
 * \~spanish
 * DOS CORRIDAS PORQUE UNA NO DISTINGUE UNA CONSTANTE DE UNA COPIA.  Con una sola
 * entrada, una instruccion que escribe cero y otra que copia un registro que vale
 * cero se ven exactamente igual.  Las dos entradas se eligen para que cada
 * registro lleve un valor distinto y reconocible en cada una -- con ceros por todas
 * partes, "escribio 0" y "no escribio" tampoco se distinguirian.
 */
status isa_effects_of(const isa_arena *a, const u8 *bytes, u32 n,
                      isa_effects *out);

/**
 * @brief
 * \~english Puts this arena behind the oracle table a sweep drives.
 * \~spanish Pone esta arena detras de la tabla de oraculo que conduce un barrido.
 * \~
 *
 * \~english
 * IT IS THREE LINES AND IT EARNS ITS KEEP.  Without it, `isa_sweep` would have to
 * know what an arena is, and then the traversal could only ever be tested by
 * reserving pages and catching real exceptions -- which is to say, only on this
 * system, only with privileges to spare, and never against a case somebody chose.
 * With it, the same sweep drives the real processor here and a made-up decoder in
 * the test, and neither knows the difference.
 *
 * The arena is borrowed, not copied: it has to outlive the sweep, and one arena
 * per worker is what keeps the search lock-free.
 *
 * \~spanish
 * SON TRES LINEAS Y SE GANAN EL SITIO.  Sin esto, `isa_sweep` tendria que saber
 * que es una arena, y entonces el recorrido solo se podria probar reservando
 * paginas y capturando excepciones de verdad -- o sea, solo en este sistema, solo
 * con privilegios de sobra, y nunca contra un caso que alguien eligiera.  Con
 * esto, el mismo barrido conduce el procesador de verdad aqui y un decodificador
 * inventado en la prueba, y ninguno de los dos nota la diferencia.
 *
 * La arena se presta, no se copia: tiene que vivir mas que el barrido, y una arena
 * por trabajador es lo que mantiene la busqueda sin cerrojos.
 */
void isa_arena_ops(const isa_arena *a, isa_probe_ops *out);

#endif /* VXP_WINDOWS_ISA_WIN_H */
