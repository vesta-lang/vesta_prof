/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/walk.h
 * @brief
 * \~english Walking the instruction space, skipping what the decoder never read.
 * \~spanish Recorrer el espacio de instrucciones saltando lo que el
 *           decodificador no leyo.
 * \~
 *
 * \~english
 * THE PROBLEM.  There are 256^15 byte strings of up to fifteen bytes, which is
 * more than there are atoms worth caring about.  Trying them one by one never
 * finishes, and almost all of them are the SAME instruction: if the processor
 * only read three bytes, the other twelve were never looked at, and the
 * 256^12 strings that share those three bytes all decode identically.
 *
 * THE IDEA, which is sandsifter's and is called tunneling: ask the processor how
 * many bytes it consumed, and carry the increment from THERE instead of from the
 * end.  Every string below that point is skipped in one step because it was
 * already answered.
 *
 *      candidate           the decoder read    what gets incremented
 *      --------------------------------------------------------------
 *      90 00 00 00 ...     1 byte  (`nop`)     byte 0   -> 91 00 00 ...
 *      0F 05 00 00 ...     2 bytes (`syscall`) byte 1   -> 0F 06 00 ...
 *      B8 11 22 33 44      5 bytes (`mov`)     see the depth bound below
 *
 * THE DEPTH BOUND, and it is what makes this a search and not an enumeration of
 * numbers.  What is being looked for is DECODINGS, not operand values: nobody
 * needs 2^32 confirmations that `mov eax, imm32` decodes.  So only the first
 * `depth` bytes are enumerated and the rest is filler; the increment happens at
 *
 *      min(consumed, depth) - 1
 *
 * WHAT IT DOES NOT DO, and the plainer version of this claimed otherwise: it caps
 * how many bytes get enumerated, it does not stop OPERAND bytes from being among
 * them.  With `depth` 3 the `mov` above is not answered once -- its opcode plus two
 * bytes of immediate are three distinct candidates each, so 2^32 confirmations
 * become 2^16 of them.  Measured on real silicon at depth three, two instructions
 * that touch the stack pointer -- `enter imm16, imm8` and `ret imm16` -- accounted
 * for 93% of everything the sweep reported.
 *
 * Telling an operand byte from an opcode byte needs to know the FORMAT, which is
 * exactly what a blind sweep refuses to assume, so it cannot be fixed here.  Where
 * it is dealt with is in whoever consumes a finding: the pool asks the processor
 * whether such a byte decides anything before it enumerates 256 values of it.
 *
 * Raising `depth` costs 256 times more per byte, so it is a knob and not a
 * constant.
 *
 * WHY IT TERMINATES, which is worth stating because "it eventually stops" is not
 * obvious for something that both moves forward and jumps backwards.  Read the
 * first `depth` bytes as one big-endian number N.  A step zeroes everything
 * below the increment point -- lowering N by at most 256^k - 1 -- and then adds
 * exactly 256^k at that point.  So N STRICTLY GROWS at every step: no candidate
 * can repeat, and the walk ends after at most 256^depth of them.
 *
 * THE FILLER IS PART OF THE QUESTION.  Bytes past what the decoder reads are
 * zero, so `mov eax, imm32` is asked with a zero immediate and a memory operand
 * is asked with a zero displacement.  A different filler is a DIFFERENT sweep,
 * not a better one -- sandsifter has a random mode for that reason -- and mixing
 * them would make two runs incomparable.
 *
 * WHAT THIS FILE DOES NOT DO: execute anything.  It does not know what a page is
 * or how a fault is caught; it is handed a number of consumed bytes and it says
 * which candidate comes next.  That is what lets the deciding part be tested
 * against a made-up decoder, with no pages, no exceptions and no privileges --
 * and it runs under MSVC, which the apparatus that actually executes cannot.
 *
 * \~spanish
 * EL PROBLEMA.  Hay 256^15 cadenas de hasta quince bytes, que son mas que los
 * atomos que merece la pena contar.  Probarlas una a una no termina nunca, y
 * casi todas son la MISMA instruccion: si el procesador solo leyo tres bytes,
 * los otros doce no los miro, y las 256^12 cadenas que comparten esos tres
 * decodifican igual.
 *
 * LA IDEA, que es de sandsifter y se llama tunneling: preguntarle al procesador
 * cuantos bytes consumio, y llevar el incremento DESDE AHI en vez de desde el
 * final.  Toda cadena por debajo de ese punto se salta en un paso porque ya
 * estaba respondida.
 *
 *      candidata           el decodificador leyo   que se incrementa
 *      ----------------------------------------------------------------
 *      90 00 00 00 ...     1 byte  (`nop`)          byte 0  -> 91 00 00 ...
 *      0F 05 00 00 ...     2 bytes (`syscall`)      byte 1  -> 0F 06 00 ...
 *      B8 11 22 33 44      5 bytes (`mov`)          ver la cota de abajo
 *
 * LA COTA DE PROFUNDIDAD, y es lo que convierte esto en una busqueda y no en una
 * enumeracion de numeros.  Lo que se busca son DECODIFICACIONES, no valores de
 * operando: nadie necesita 2^32 confirmaciones de que `mov eax, imm32`
 * decodifica.  Asi que solo se enumeran los primeros `depth` bytes y el resto es
 * relleno; el incremento ocurre en
 *
 *      min(consumidos, depth) - 1
 *
 * LO QUE NO HACE, y la version mas simple de esto afirmaba lo contrario: acota
 * cuantos bytes se enumeran, no evita que entre ellos haya bytes de OPERANDO.  Con
 * `depth` 3 el `mov` de arriba no queda respondido una vez -- su opcode mas dos
 * bytes de inmediato son tres candidatas distintas cada una, asi que las 2^32
 * confirmaciones se quedan en 2^16.  Medido sobre silicio de verdad a profundidad
 * tres, dos instrucciones que tocan el puntero de pila -- `enter imm16, imm8` y
 * `ret imm16` -- se llevaban el 93% de todo lo que informo el barrido.
 *
 * Distinguir un byte de operando de uno de opcode exige conocer el FORMATO, que es
 * justo lo que un barrido a ciegas se niega a suponer, asi que no se puede arreglar
 * aqui.  Donde se trata es en quien consume un hallazgo: el grupo de trabajadores le
 * pregunta al procesador si tal byte decide algo antes de enumerar 256 valores de el.
 *
 * Subir `depth` cuesta 256 veces mas por byte, asi que es un mando y no una
 * constante.
 *
 * POR QUE TERMINA, que merece decirse porque "al final para" no es evidente en
 * algo que avanza y a la vez salta hacia atras.  Leanse los primeros `depth`
 * bytes como un solo numero N en orden grande.  Un paso pone a cero todo lo que
 * hay por debajo del punto de incremento -- bajando N a lo sumo 256^k - 1 -- y
 * despues suma exactamente 256^k en ese punto.  Asi que N CRECE ESTRICTAMENTE en
 * cada paso: ninguna candidata puede repetirse, y el recorrido acaba despues de
 * 256^depth como mucho.
 *
 * EL RELLENO ES PARTE DE LA PREGUNTA.  Los bytes mas alla de lo que el
 * decodificador lee valen cero, asi que `mov eax, imm32` se pregunta con
 * inmediato cero y un operando de memoria con desplazamiento cero.  Otro relleno
 * es OTRO barrido, no uno mejor -- sandsifter tiene un modo aleatorio por ese
 * motivo -- y mezclarlos haria incomparables dos corridas.
 *
 * LO QUE ESTE FICHERO NO HACE: ejecutar nada.  No sabe que es una pagina ni como
 * se captura un fallo; se le da un numero de bytes consumidos y dice cual es la
 * candidata siguiente.  Eso es lo que permite probar la parte que decide contra
 * un decodificador inventado, sin paginas, sin excepciones y sin privilegios --
 * y corre con MSVC, que el aparato que ejecuta de verdad no puede.
 */

#ifndef VXP_COMMON_ISA_WALK_H
#define VXP_COMMON_ISA_WALK_H

#include "isa/trial.h"

/**
 * @brief
 * \~english How many leading bytes a walk enumerates, at most.
 * \~spanish Cuantos bytes de cabeza enumera un recorrido, como mucho.
 * \~
 *
 * \~english
 * It is `ISA_MAX_LEN` because beyond that there is no instruction: fifteen bytes
 * is the architectural limit and anything longer faults for being too long.  It
 * is a ceiling and not an advice -- a full sweep at this depth does not finish
 * in any useful amount of time.
 *
 * \~spanish
 * Es `ISA_MAX_LEN` porque mas alla no hay instruccion: quince bytes es el limite
 * de la arquitectura y algo mas largo falla por pasarse.  Es un techo y no un
 * consejo -- un barrido completo a esta profundidad no termina en ningun plazo
 * util.
 */
#define ISA_WALK_DEPTH_MAX ISA_MAX_LEN

/**
 * @brief
 * \~english One walk: the candidate it is on, and the subtree it owns.
 * \~spanish Un recorrido: la candidata en la que esta, y el subarbol que posee.
 * \~
 *
 * \~english
 * IT OWNS A SUBTREE, and that is the whole parallelism story.  The walk is
 * sequential by nature -- each step needs the length of the previous one -- so
 * what gets shared out is not steps but subtrees: `fixed` leading bytes are
 * frozen and the byte right after them is confined to `lo .. hi`.  A walk ends
 * when the carry tries to leave that, so two walks can never visit the same
 * candidate and neither needs to know the other exists.  No lock, no atomic, no
 * shared cursor.
 *
 *      what is wanted                       fixed  bytes      lo    hi
 *      ---------------------------------------------------------------------
 *      first bytes 0x00 .. 0x3F               0     --        0x00  0x3F
 *      everything under the prefix `0F 38`    2     0F 38     0x00  0xFF
 *
 * The first row is the static split of 256 first bytes between workers.  The
 * second is what the work queue hands out when that split comes out lopsided,
 * which it does: `0F` alone opens the whole two-byte space.
 *
 * \~spanish
 * POSEE UN SUBARBOL, y ahi esta toda la historia del paralelismo.  El recorrido
 * es secuencial por naturaleza -- cada paso necesita la longitud del anterior --,
 * asi que lo que se reparte no son pasos sino subarboles: se congelan `fixed`
 * bytes de cabeza y el byte justo despues queda confinado a `lo .. hi`.  Un
 * recorrido acaba cuando el acarreo intenta salirse de eso, asi que dos
 * recorridos no pueden visitar la misma candidata y ninguno necesita saber que
 * el otro existe.  Sin cerrojo, sin atomico, sin cursor compartido.
 *
 *      lo que se quiere                      fixed  bytes      lo    hi
 *      ---------------------------------------------------------------------
 *      primeros bytes 0x00 .. 0x3F             0     --        0x00  0x3F
 *      todo lo del prefijo `0F 38`             2     0F 38     0x00  0xFF
 *
 * La primera fila es el reparto estatico de 256 primeros bytes entre
 * trabajadores.  La segunda es lo que entrega la cola de trabajo cuando ese
 * reparto sale desequilibrado, que es lo que pasa: `0F` solo abre el espacio
 * entero de dos bytes.
 */
typedef struct isa_walk {
    u8 bytes[ISA_MAX_LEN]; /**< \~english the candidate; past `depth` it is filler \~spanish la candidata; mas alla de `depth` es relleno \~ */
    u8 fixed;              /**< \~english leading bytes that never move \~spanish bytes de cabeza que no se mueven \~ */
    u8 lo;                 /**< \~english lowest value of `bytes[fixed]` \~spanish valor menor de `bytes[fixed]` \~ */
    u8 hi;                 /**< \~english highest, inclusive \~spanish valor mayor, inclusive \~ */
    u8 depth;              /**< \~english how many bytes are enumerated \~spanish cuantos bytes se enumeran \~ */
    u8 done;               /**< \~english the subtree is exhausted \~spanish el subarbol se agoto \~ */
    u8 _pad[3];
    u64 visited;           /**< \~english candidates handed out \~spanish candidatas entregadas \~ */
    u64 skipped;           /**< \~english candidates the tunneling did not have to try \~spanish candidatas que el tunneling no tuvo que probar \~ */
} isa_walk;

/**
 * @brief
 * \~english Puts a walk on the first candidate of the subtree it will own.
 * \~spanish Pone un recorrido en la primera candidata del subarbol que va a
 *           poseer.
 * \~
 *
 * @param prefix \~english the frozen leading bytes, or null when `fixed` is 0 \~spanish los bytes de cabeza congelados, o nulo cuando `fixed` es 0 \~
 * @param fixed \~english how many of them, 0 to `depth - 1` \~spanish cuantos son, de 0 a `depth - 1` \~
 * @param depth \~english 1 to `ISA_WALK_DEPTH_MAX` \~spanish de 1 a `ISA_WALK_DEPTH_MAX` \~
 * @return `OK`; `ERR_INVALID` \~english if the subtree is empty or does not fit \~spanish si el subarbol es vacio o no cabe \~
 *
 * \~english
 * `fixed` MUST BE BELOW `depth`: freezing as many bytes as are enumerated leaves
 * nothing to walk, which is a caller's mistake and not an empty answer, so it is
 * refused rather than quietly returning one candidate.
 *
 * \~spanish
 * `fixed` TIENE QUE SER MENOR QUE `depth`: congelar tantos bytes como se
 * enumeran no deja nada que recorrer, y eso es un error de quien llama y no una
 * respuesta vacia, asi que se rechaza en vez de devolver una sola candidata sin
 * decir nada.
 */
status isa_walk_open(isa_walk *w, const u8 *prefix, u32 fixed, u8 lo, u8 hi,
                     u32 depth);

/**
 * @brief
 * \~english Moves to the next candidate, told how many bytes this one used.
 * \~spanish Pasa a la candidata siguiente, dicho cuantos bytes gasto esta.
 * \~
 *
 * @param consumed \~english bytes the decoder read; `ISA_MAX_LEN` when it wanted more than there were \~spanish bytes que leyo el decodificador; `ISA_MAX_LEN` cuando quiso mas de los que habia \~
 * @return \~english 1 if there is a next candidate, 0 if the subtree is exhausted \~spanish 1 si hay candidata siguiente, 0 si el subarbol se agoto \~
 *
 * \~english
 * THE ANSWER TO THE PREVIOUS CANDIDATE IS THE INPUT TO THIS CALL, which is why
 * the walk cannot be a plain iterator: how far it jumps is not known until the
 * processor has spoken.  A caller that does not know the length -- because the
 * candidate wanted more bytes than the page had -- passes `ISA_MAX_LEN`, and
 * then no skipping happens, which is the safe direction: it tries more, never
 * fewer.
 *
 * \~spanish
 * LA RESPUESTA A LA CANDIDATA ANTERIOR ES LA ENTRADA DE ESTA LLAMADA, y por eso
 * el recorrido no puede ser un iterador normal: cuanto salta no se sabe hasta
 * que ha hablado el procesador.  Quien no sepa la longitud -- porque la
 * candidata queria mas bytes de los que habia en la pagina -- pasa
 * `ISA_MAX_LEN`, y entonces no se salta nada, que es el lado seguro: prueba de
 * mas, nunca de menos.
 */
int isa_walk_step(isa_walk *w, u32 consumed);

#endif /* VXP_COMMON_ISA_WALK_H */
