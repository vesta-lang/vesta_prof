/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/ref.h
 * @brief
 * \~english What somebody else's decoder says, so the measurement has a rival.
 * \~spanish Lo que dice el decodificador de otra gente, para que la medida tenga
 *           con quien discutir.
 * \~
 *
 * \~english
 * WHY A SECOND OPINION IS THE WHOLE POINT.  The sweep measures the processor, and a
 * measurement with nothing to compare against cannot be wrong out loud: an oracle that
 * measures badly looks exactly like a machine that decodes strangely.  A decoder
 * written by other people, from the same manuals but with a different reading, is the
 * only thing that catches that -- and where the two disagree is where the findings are.
 *
 *      CONFLICT        both decode and give DIFFERENT LENGTHS.  One of them is wrong,
 *                      and for a disassembler that is serious: a wrong length
 *                      misaligns everything after it
 *      ONLY SILICON    the processor runs it and the decoder does not know it.  This
 *                      is what the sweep exists to find
 *      ONLY THE TABLE  the other way round.  Usually an extension this part does not
 *                      have, which is ALSO data -- AVX-512 is fused off on Alder Lake
 *                      because the small cores lack it
 *
 * IT IS A TABLE AND NOT A CALL, and that is what keeps this tree free of dependencies.
 * The expensive side is the processor -- one attempt costs an exception, and half the
 * subtrees take the process with them -- while the decoder's side is cheap and can be
 * had in full beforehand.  So it is precomputed by a tool and read here as bytes.
 *
 * INDEXED THE WAY THE TRAVERSAL NUMBERS CANDIDATES: the first `depth` bytes read as
 * one big-endian number.  Nothing gets translated between the two, which is one fewer
 * place for them to stop agreeing.
 *
 * \~spanish
 * POR QUE UNA SEGUNDA OPINION ES TODO EL ASUNTO.  El barrido mide el procesador, y una
 * medida sin nada con lo que compararse no puede equivocarse en voz alta: un oraculo
 * que mida mal se ve exactamente igual que una maquina que decodifique raro.  Un
 * decodificador escrito por otra gente, de los mismos manuales pero con otra lectura,
 * es lo unico que caza eso -- y donde los dos discrepan es donde estan los hallazgos.
 *
 *      CONFLICTO        los dos decodifican y dan LONGITUD DISTINTA.  Uno de los dos
 *                       esta mal, y para un desensamblador eso es grave: una longitud
 *                       equivocada desalinea todo lo que viene detras
 *      SOLO EL SILICIO  el procesador la ejecuta y el decodificador no la conoce.
 *                       Esto es lo que el barrido existe para encontrar
 *      SOLO LA TABLA    al reves.  Suele ser una extension que esta pieza no tiene, y
 *                       eso TAMBIEN es un dato -- AVX-512 esta fusionado en Alder Lake
 *                       porque los nucleos pequenos no lo tienen
 *
 * ES UNA TABLA Y NO UNA LLAMADA, y eso es lo que mantiene este arbol sin dependencias.
 * El lado caro es el procesador -- un intento cuesta una excepcion, y la mitad de los
 * subarboles se llevan el proceso --, mientras que el lado del decodificador es barato
 * y se puede tener entero de antemano.  Asi que lo precalcula una herramienta y aqui se
 * lee como bytes.
 *
 * INDEXADA COMO NUMERA LAS CANDIDATAS EL RECORRIDO: los primeros `depth` bytes leidos
 * como un numero en orden grande.  Entre los dos no se traduce nada, que es un sitio
 * menos donde dejar de coincidir.
 */

#ifndef VXP_COMMON_ISA_REF_H
#define VXP_COMMON_ISA_REF_H

#include "isa/walk.h"

/**
 * @brief
 * \~english One length per candidate, and the depth it was built for.
 * \~spanish Una longitud por candidata, y la profundidad con la que se hizo.
 * \~
 *
 * \~english
 * THE DEPTH IS CARRIED SO IT CAN BE CHECKED.  A table built for two bytes and used to
 * answer about three would be indexed with numbers it does not have, and the failure
 * would not be a crash -- it would be a run reporting thousands of disagreements that
 * are only a mismatched table.  Findings that loud and that wrong are worse than none.
 *
 * \~spanish
 * LA PROFUNDIDAD VIAJA PARA PODER COMPROBARLA.  Una tabla hecha para dos bytes y usada
 * para responder de tres se indexaria con numeros que no tiene, y el fallo no seria una
 * caida -- seria una corrida informando de miles de desacuerdos que solo son una tabla
 * que no toca.  Hallazgos tan ruidosos y tan falsos son peores que ninguno.
 */
typedef struct isa_ref {
    const u8 *len; /**< \~english borrowed, not copied \~spanish prestada, no copiada \~ */
    u64 count;
    u32 depth;
    u32 _pad;
} isa_ref;

/**
 * @brief
 * \~english Reads the table out of a buffer somebody else loaded.
 * \~spanish Lee la tabla de un bufer que cargo alguien mas.
 * \~
 *
 * @return `OK`; `ERR_INVALID` \~english if it is not one of these tables, or says a depth this build cannot use \~spanish si no es una de estas tablas, o dice una profundidad que esta construccion no puede usar \~
 *
 * \~english
 * NO FILE IS OPENED HERE, the same rule as the cost tables in `user/vxarch.c`: `common/`
 * neither allocates nor does I/O, so whoever has a file gives it as bytes.  It is also
 * what lets a worker in ring zero be handed the same table without a filesystem.
 *
 * \~spanish
 * AQUI NO SE ABRE NINGUN FICHERO, la misma regla que las tablas de coste de
 * `user/vxarch.c`: `common/` no reserva memoria ni hace E/S, asi que quien tenga un
 * fichero lo pasa como bytes.  Es tambien lo que permite darle la misma tabla a un
 * trabajador de anillo cero, donde no hay sistema de ficheros.
 */
status isa_ref_parse(const u8 *buf, u64 n, isa_ref *out);

/**
 * @brief
 * \~english What the reference says about these bytes.
 * \~spanish Lo que dice la referencia de estos bytes.
 * \~
 *
 * @return \~english the length, or 0 when the reference does not decode them \~spanish la longitud, o 0 cuando la referencia no los decodifica \~
 */
u32 isa_ref_length(const isa_ref *r, const u8 *bytes);

#endif /* VXP_COMMON_ISA_REF_H */
