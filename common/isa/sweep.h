/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/sweep.h
 * @brief
 * \~english The walk joined to an oracle: one subtree swept, and the tally.
 * \~spanish El recorrido unido a un oraculo: un subarbol barrido, y la cuenta.
 * \~
 *
 * \~english
 * WHAT THIS ADDS TO `walk.h`, which already knows how to move: nothing but the
 * loop, and that is the point.  The walk says which candidate comes next given a
 * length; the oracle says what a candidate does; neither knows about the other.
 * This file is the three lines that put them together, plus what gets written
 * down along the way.
 *
 * THE ORACLE COMES IN AS AN OPERATION TABLE, the same seam as `msr/access.h`
 * against `msr_win.c` and for the same reason.  Measuring a candidate means
 * reserving pages, executing bytes and catching faults -- all of it the system's
 * business, none of it this file's -- so what arrives here is one function
 * pointer.  What that buys:
 *
 *      the sweep is testable       against a made-up oracle, with no pages, no
 *                                  exceptions and no privileges
 *      ring 0 costs nothing        the kernel side has a different way to try
 *                                  bytes and the same sweep drives it
 *      a worker cannot be a lie    the fake oracle and the real one are
 *                                  interchangeable, so a sweep that works on the
 *                                  fake is the same code in production
 *
 * \~spanish
 * QUE ANADE ESTO A `walk.h`, que ya sabe moverse: nada mas que el bucle, y en eso
 * esta la gracia.  El recorrido dice cual es la candidata siguiente dada una
 * longitud; el oraculo dice que hace una candidata; ninguno de los dos sabe del
 * otro.  Este fichero son las tres lineas que los juntan, mas lo que se va
 * apuntando por el camino.
 *
 * EL ORACULO ENTRA COMO TABLA DE OPERACIONES, la misma costura que `msr/access.h`
 * frente a `msr_win.c` y por lo mismo.  Medir una candidata es reservar paginas,
 * ejecutar bytes y capturar fallos -- todo ello asunto del sistema, nada de ello
 * asunto de este fichero --, asi que lo que llega aqui es un puntero a funcion.
 * Lo que eso compra:
 *
 *      el barrido se puede probar   contra un oraculo inventado, sin paginas, sin
 *                                   excepciones y sin privilegios
 *      el anillo 0 sale gratis      el lado de kernel tiene otra forma de probar
 *                                   bytes y el mismo barrido la conduce
 *      un trabajador no puede       el oraculo falso y el de verdad son
 *      mentir                       intercambiables, asi que un barrido que
 *                                   funciona con el falso es el mismo codigo en
 *                                   produccion
 */

#ifndef VXP_COMMON_ISA_SWEEP_H
#define VXP_COMMON_ISA_SWEEP_H

#include "isa/walk.h"

/**
 * @brief
 * \~english How many outcomes there are, for the histogram to have a size.
 * \~spanish Cuantos resultados hay, para que el histograma tenga tamano.
 * \~
 *
 * \~english
 * It has to be kept next to the enum in `trial.h`, and `sweep.c` has a static
 * assertion that pins it: a new outcome added there without touching this would
 * be counted into whichever bucket happens to follow, and a histogram that
 * quietly attributes findings to the wrong verdict is worse than one that does
 * not build.
 *
 * \~spanish
 * Tiene que ir a la par del enum de `trial.h`, y `sweep.c` lleva una asercion
 * estatica que lo sujeta: un resultado nuevo anadido alli sin tocar esto se
 * contaria en el cajon que venga detras, y un histograma que atribuye hallazgos
 * al veredicto equivocado sin decirlo es peor que uno que no compila.
 */
#define ISA_OUTCOME_COUNT 8u

/**
 * @brief
 * \~english How bytes get tried.  One function, so the sweep needs no system.
 * \~spanish Como se prueban unos bytes.  Una funcion, asi el barrido no necesita
 *           sistema.
 * \~
 *
 * \~english
 * `measure` answers about `ISA_MAX_LEN` readable bytes and fills in the outcome
 * and, when there is one, the length.  Returning anything but `OK` means the
 * apparatus itself failed -- not that the candidate was bad -- and the sweep
 * stops, because carrying on would produce a sweep whose gaps nobody can see.
 *
 * \~spanish
 * `measure` responde sobre `ISA_MAX_LEN` bytes legibles y rellena el resultado y,
 * cuando lo hay, la longitud.  Devolver algo distinto de `OK` quiere decir que
 * fallo el aparato -- no que la candidata fuera mala -- y el barrido se detiene,
 * porque seguir produciria un barrido cuyos huecos nadie puede ver.
 */
typedef struct isa_probe_ops {
    status (*measure)(void *ctx, const u8 *bytes, isa_result *out);
    void *ctx;
} isa_probe_ops;

/**
 * @brief
 * \~english One piece of work: exactly what opening a walk needs.
 * \~spanish Un trozo de trabajo: exactamente lo que necesita abrir un recorrido.
 * \~
 *
 * \~english
 * IT IS THE WALK'S ARGUMENTS AND NOT A SECOND VOCABULARY, and that is deliberate.
 * This is the thing a work queue hands out and the thing a worker process is
 * started with, so it has to travel; making it anything other than "what the walk
 * is opened with" would mean translating between two descriptions of the same
 * subtree, and a translation is somewhere for the two to stop agreeing.
 *
 * \~spanish
 * SON LOS ARGUMENTOS DEL RECORRIDO Y NO UN SEGUNDO VOCABULARIO, y es a proposito.
 * Esto es lo que reparte una cola de trabajo y con lo que se arranca un proceso
 * trabajador, asi que tiene que viajar; hacerlo otra cosa distinta de "aquello con
 * lo que se abre el recorrido" obligaria a traducir entre dos descripciones del
 * mismo subarbol, y una traduccion es un sitio donde las dos pueden dejar de
 * coincidir.
 */
typedef struct isa_work {
    u8 prefix[ISA_MAX_LEN]; /**< \~english the frozen head \~spanish la cabeza congelada \~ */
    u8 fixed;
    u8 lo;
    u8 hi;
    u8 depth;
} isa_work;

/**
 * @brief
 * \~english What a swept subtree turned out to contain.
 * \~spanish Lo que resulto contener un subarbol barrido.
 * \~
 *
 * \~english
 * THE HISTOGRAMS ARE THE FINDING, not a progress report.  Two numbers out of here
 * decide design questions that are otherwise guesswork:
 *
 *      `candidates` per subtree    how lopsided the space is, which is the whole
 *                                  argument for a work queue instead of a static
 *                                  split across workers
 *      `by_length`                 whether the oracle is answering sensibly at
 *                                  all: a sweep where nothing is longer than one
 *                                  byte is an oracle that is not measuring
 *
 * `truncated` is kept apart from the length histogram because a truncated attempt
 * HAS no length -- putting it in the bucket for fifteen would invent a measurement
 * that was never made.
 *
 * \~spanish
 * LOS HISTOGRAMAS SON EL HALLAZGO, no un informe de avance.  Dos numeros de aqui
 * deciden preguntas de diseno que de otro modo son adivinar:
 *
 *      `candidates` por subarbol   lo desequilibrado que esta el espacio, que es
 *                                  todo el argumento para una cola de trabajo en
 *                                  vez de un reparto estatico entre trabajadores
 *      `by_length`                 si el oraculo esta respondiendo con sentido: un
 *                                  barrido donde nada mide mas de un byte es un
 *                                  oraculo que no esta midiendo
 *
 * `truncated` se guarda aparte del histograma de longitudes porque un intento
 * truncado NO tiene longitud -- meterlo en el cajon del quince inventaria una
 * medida que nunca se hizo.
 */
typedef struct isa_tally {
    u64 candidates;                       /**< \~english tried \~spanish probadas \~ */
    u64 skipped;                          /**< \~english what the tunneling avoided \~spanish lo que evito el tunneling \~ */
    u64 by_outcome[ISA_OUTCOME_COUNT];
    u64 by_length[ISA_MAX_LEN + 1u];      /**< \~english only where a length was measured \~spanish solo donde hubo longitud medida \~ */
    u64 truncated;                        /**< \~english no length: the decoder wanted more \~spanish sin longitud: el decodificador queria mas \~ */
} isa_tally;

/**
 * @brief
 * \~english Sweeps one subtree from end to end, filling in the tally.
 * \~spanish Barre un subarbol de punta a punta, rellenando la cuenta.
 * \~
 *
 * @return `OK`; `ERR_INVALID` \~english if the work does not describe a subtree; whatever the oracle failed with \~spanish si el trabajo no describe un subarbol; lo que fallara el oraculo \~
 *
 * \~english
 * AN ORACLE FAILURE STOPS IT, and does not get counted as a verdict.  The two are
 * completely different things: "this candidate is invalid" is the answer being
 * looked for, and "the apparatus could not ask" is the answer being worthless.
 * Folding the second into the first is how a sweep comes out looking complete
 * after having measured nothing.
 *
 * \~spanish
 * UN FALLO DEL ORACULO LO DETIENE, y no se cuenta como veredicto.  Son dos cosas
 * completamente distintas: "esta candidata no es valida" es la respuesta que se
 * busca, y "el aparato no pudo preguntar" es que la respuesta no vale nada.
 * Meter la segunda dentro de la primera es como un barrido acaba pareciendo
 * completo despues de no haber medido nada.
 */
status isa_sweep(const isa_probe_ops *ops, const isa_work *work,
                 isa_tally *out);

#endif /* VXP_COMMON_ISA_SWEEP_H */
