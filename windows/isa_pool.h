/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file isa_pool.h
 * @brief
 * \~english Workers that are PROCESSES, because a worker has to be able to die.
 * \~spanish Trabajadores que son PROCESOS, porque un trabajador tiene que poder
 *           morir.
 * \~
 *
 * \~english
 * WHY NOT THREADS, and this is not caution -- it is the only arrangement that
 * works.  Two things a candidate does cannot be survived by the thread running it,
 * and both were measured rather than feared:
 *
 *      `01 C4`   `add esp, eax`.  Writes the stack pointer, so the fault that comes
 *                next leaves the unwinder no frame to find.  There is nowhere for a
 *                handler to run
 *      `73 FE`   `jae -2`.  Jumps to itself and never comes back.  Nothing inside
 *                the process can interrupt it, because nothing inside the process
 *                is running
 *
 * A thread cannot be killed safely and a hung thread cannot be reclaimed at all.  A
 * process can be terminated from outside, has its own stack and its own pages, and
 * takes its damage with it.  The parent then knows exactly which item was in flight
 * and puts it back, smaller.
 *
 * THE TWO THINGS A PARENT MUST DO, and one of them is easy to forget:
 *
 *      notice a death     the exit code, or a result that never arrived
 *      notice a HANG      a deadline.  Without it, one `73 FE` stops the whole
 *                         sweep for as long as anyone lets it -- measured, a worker
 *                         spun for 647 seconds of CPU before being killed by hand
 *
 * WHAT TRAVELS BETWEEN THEM.  Down, a work item on the command line: it is the same
 * description a walk is opened with, so nothing gets translated.  Up, the tally as
 * RAW BYTES on the child's standard output -- not text, because then there is
 * nothing to parse and nothing to disagree about, and because a short read is
 * itself the proof that the child did not finish.
 *
 * \~spanish
 * POR QUE NO HILOS, y no es prudencia -- es la unica disposicion que funciona.  Hay
 * dos cosas que hace una candidata a las que el hilo que la ejecuta no sobrevive, y
 * las dos estan medidas y no temidas:
 *
 *      `01 C4`   `add esp, eax`.  Escribe el puntero de pila, asi que el fallo que
 *                viene detras deja al desenrollador sin marco que encontrar.  No hay
 *                donde corra un manejador
 *      `73 FE`   `jae -2`.  Salta a si misma y no vuelve nunca.  Nada de dentro del
 *                proceso puede interrumpirla, porque nada de dentro del proceso esta
 *                corriendo
 *
 * Un hilo no se puede matar con seguridad y un hilo colgado no se recupera de
 * ninguna forma.  Un proceso se puede terminar desde fuera, tiene su pila y sus
 * paginas propias, y se lleva su destrozo con el.  El padre sabe entonces
 * exactamente que trozo estaba en vuelo y lo vuelve a meter, mas pequeno.
 *
 * LAS DOS COSAS QUE UN PADRE TIENE QUE HACER, y una se olvida facil:
 *
 *      notar una muerte   el codigo de salida, o un resultado que no llego
 *      notar un CUELGUE   un plazo.  Sin el, un solo `73 FE` para el barrido entero
 *                         mientras alguien lo deje -- medido, un trabajador dio
 *                         vueltas 647 segundos de CPU antes de matarlo a mano
 *
 * QUE VIAJA ENTRE LOS DOS.  Hacia abajo, un trozo de trabajo en la linea de
 * ordenes: es la misma descripcion con la que se abre un recorrido, asi que no se
 * traduce nada.  Hacia arriba, la cuenta como BYTES CRUDOS por la salida estandar
 * del hijo -- no como texto, porque asi no hay nada que interpretar ni en lo que
 * discrepar, y porque una lectura corta es por si misma la prueba de que el hijo no
 * termino.
 */

#ifndef VXP_WINDOWS_ISA_POOL_H
#define VXP_WINDOWS_ISA_POOL_H

#include "isa/queue.h"

/**
 * @brief
 * \~english How a worker's turn ended.
 * \~spanish Como acabo el turno de un trabajador.
 */
typedef enum isa_worker_end {
    ISA_WORKER_DONE = 0, /**< \~english it swept the item and reported \~spanish barrio el trozo e informo \~ */
    ISA_WORKER_DIED = 1, /**< \~english it stopped existing \~spanish dejo de existir \~ */
    ISA_WORKER_HUNG = 2  /**< \~english it ran out of time and was terminated \~spanish se le acabo el plazo y se le termino \~ */
} isa_worker_end;

/**
 * @brief
 * \~english What the whole run came to.
 * \~spanish En que quedo la corrida entera.
 */
typedef struct isa_pool_stats {
    u64 spawned;    /**< \~english processes started \~spanish procesos arrancados \~ */
    u64 done;       /**< \~english items swept end to end \~spanish trozos barridos de punta a punta \~ */
    u64 died;       /**< \~english items whose worker stopped existing \~spanish trozos cuyo trabajador dejo de existir \~ */
    u64 hung;       /**< \~english items whose worker ran out of time \~spanish trozos cuyo trabajador agoto el plazo \~ */
    u64 named;      /**< \~english single candidates pinned down as the culprit \~spanish candidatas sueltas fijadas como culpables \~ */
    u64 unspawnable; /**< \~english items no process could be started for \~spanish trozos para los que no se pudo arrancar proceso \~ */
    u64 probes;     /**< \~english extra processes spent asking how long an instruction is \~spanish procesos de mas gastados preguntando cuanto mide una instruccion \~ */
    /**
     * \~english Candidates a named instruction stands for, beyond the one named.
     *
     * WITHOUT THIS THE ACCOUNTING BREAKS, and it broke: when a probe establishes that a
     * whole subtree is one instruction, one candidate gets named and the other
     * `256^k - 1` are answered by it -- but nobody was counting them, so the sweep came
     * out 2550 candidates short of the space and said so.  That is the accounting doing
     * its job; the number it was missing is this one.
     *
     * It is NOT "spawns avoided", which is a different quantity and not worth inventing
     * a formula for: what the probes cost and save in processes is `probes` against the
     * measured spawn count of a run.
     *
     * \~spanish Candidatas que representa una instruccion nombrada, aparte de la
     * nombrada.
     *
     * SIN ESTO LA CONTABILIDAD SE ROMPE, y se rompio: cuando una sonda establece que un
     * subarbol entero es una instruccion, se nombra una candidata y las otras
     * `256^k - 1` quedan respondidas por ella -- pero nadie las contaba, asi que el
     * barrido salia 2550 candidatas por debajo del espacio y lo dijo.  Eso es la
     * contabilidad haciendo su trabajo; el numero que le faltaba es este.
     *
     * NO es "arranques evitados", que es otra cantidad y no merece inventarle una
     * formula: lo que cuestan y ahorran las sondas en procesos es `probes` frente a la
     * cuenta de arranques medida de una corrida.
     */
    u64 covered;
} isa_pool_stats;

/**
 * @brief
 * \~english Called for every item as it resolves, so the caller can report.
 * \~spanish Se llama por cada trozo al resolverse, para que quien llama informe.
 * \~
 *
 * @param how \~english an `isa_worker_end` \~spanish un `isa_worker_end` \~
 * @param t \~english only meaningful when `how` is `ISA_WORKER_DONE` \~spanish solo significa algo cuando `how` es `ISA_WORKER_DONE` \~
 * @param reached \~english how many bytes the worker had got to offering when it stopped existing; 0 if it never said \~spanish cuantos bytes llevaba ofreciendo el trabajador cuando dejo de existir; 0 si no lo dijo \~
 *
 * \~english
 * `reached` IS THE LENGTH OF A CANDIDATE THAT KILLS.  A worker given a single
 * candidate announces, before each attempt, how many bytes it is about to make
 * available -- so when it dies, the last thing it said is the first count at which
 * the decoder stopped asking for more, which is exactly the instruction's length.
 * It is the only way to learn it: the candidate destroys the process, so nothing
 * inside the process can report it afterwards.
 *
 * \~spanish
 * `reached` ES LA LONGITUD DE UNA CANDIDATA QUE MATA.  Un trabajador al que se le da
 * una sola candidata anuncia, antes de cada intento, cuantos bytes va a poner
 * disponibles -- asi que cuando muere, lo ultimo que dijo es el primer numero con el
 * que el decodificador dejo de pedir mas, que es exactamente la longitud de la
 * instruccion.  Es la unica forma de saberla: la candidata destruye el proceso, asi
 * que nada de dentro del proceso puede informarla despues.
 *
 * \~english
 * WHETHER AN ITEM IS A NAMED CULPRIT IS NOT PASSED, because it is already in the
 * item: one whose prefix is frozen up to `depth - 1` with `lo == hi` IS a single
 * candidate, so a death on it names it.  Passing a flag as well would be a second
 * copy of a fact, and two copies are somewhere for them to disagree.
 *
 * \~spanish
 * SI UN TROZO ES UNA CULPABLE NOMBRADA NO SE PASA, porque ya esta en el trozo: uno
 * con el prefijo congelado hasta `depth - 1` y `lo == hi` ES una sola candidata, asi
 * que una muerte sobre el la nombra.  Pasar ademas una bandera seria una segunda
 * copia de un hecho, y dos copias son un sitio donde discrepar.
 */
typedef void (*isa_pool_report_fn)(void *ctx, const isa_work *w, u32 how,
                                   const isa_tally *t, u32 reached);

/**
 * @brief
 * \~english Empties the queue, spawning workers and refining what they lose.
 * \~spanish Vacia la cola, arrancando trabajadores y refinando lo que pierden.
 * \~
 *
 * @param exe \~english the path to this same program; it spawns itself \~spanish la ruta a este mismo programa; se arranca a si mismo \~
 * @param workers \~english how many at once, 1 to `ISA_POOL_MAX` \~spanish cuantos a la vez, de 1 a `ISA_POOL_MAX` \~
 * @param timeout_ms \~english how long an item may take before it counts as hung \~spanish cuanto puede tardar un trozo antes de contar como colgado \~
 * @return `OK`; `ERR_INVALID` \~english on arguments that make no sense \~spanish con argumentos sin sentido \~
 *
 * \~english
 * IT SPAWNS ITSELF, which is worth saying plainly because it looks odd.  There is no
 * second program: the worker is this one, told to sweep one item, and that is what
 * keeps the two sides from drifting -- a separate worker binary would be a second
 * place where the oracle, the walk and the tally layout all have to agree.
 *
 * IT RETURNS WHEN THE QUEUE IS EMPTY, and the queue grows while it works: every
 * death or hang puts back something smaller.  That terminates because refinement
 * terminates -- see `isa_queue_refine` -- and the worst case is bounded by the
 * depth, not by how murderous the instruction space turns out to be.
 *
 * \~spanish
 * SE ARRANCA A SI MISMO, que merece decirse claro porque suena raro.  No hay un
 * segundo programa: el trabajador es este, al que se le dice que barra un trozo, y
 * eso es lo que evita que los dos lados se separen -- un binario de trabajador
 * aparte seria un segundo sitio donde el oraculo, el recorrido y la forma de la
 * cuenta tienen que coincidir.
 *
 * VUELVE CUANDO LA COLA ESTA VACIA, y la cola crece mientras trabaja: cada muerte o
 * cuelgue devuelve algo mas pequeno.  Eso termina porque el refinamiento termina
 * -- ver `isa_queue_refine` -- y el peor caso lo acota la profundidad, no lo
 * asesino que resulte ser el espacio de instrucciones.
 */
status isa_pool_run(const char *exe, u32 workers, u32 timeout_ms, isa_queue *q,
                    isa_pool_report_fn report, void *ctx,
                    isa_pool_stats *stats);

/**
 * @brief
 * \~english The most workers at once.
 * \~spanish El maximo de trabajadores a la vez.
 * \~
 *
 * \~english
 * It is 64 because that is what one `WaitForMultipleObjects` accepts, and going
 * past it would mean waiting in groups -- which is a different program.  Sixty-four
 * is already more than the logical processors of any machine this runs on.
 *
 * \~spanish
 * Es 64 porque es lo que acepta un solo `WaitForMultipleObjects`, y pasar de ahi
 * obligaria a esperar por grupos -- que es otro programa.  Sesenta y cuatro ya son
 * mas que los procesadores logicos de cualquier maquina donde esto corra.
 */
#define ISA_POOL_MAX 64u

/**
 * @brief
 * \~english The worker side: sweeps one item and writes the tally out raw.
 * \~spanish El lado del trabajador: barre un trozo y escribe la cuenta en crudo.
 * \~
 *
 * @return \~english 0 if it finished, non-zero if it could not even start \~spanish 0 si termino, distinto de cero si no pudo ni empezar \~
 *
 * \~english
 * IT ALSO TURNS OFF THE CRASH REPORTING, and that is not cosmetic.  A worker dying
 * is the normal case here, and with the system's error reporting left on, each death
 * took about five seconds -- measured, sweeping 114 first bytes one process at a
 * time took over ten minutes with fifty of them dying.  The dying is free; being
 * asked about it is not.
 *
 * \~spanish
 * TAMBIEN APAGA EL INFORME DE ERRORES, y eso no es cosmetico.  Que un trabajador
 * muera es el caso normal aqui, y con el informe de errores del sistema encendido
 * cada muerte costaba unos cinco segundos -- medido, barrer 114 primeros bytes de
 * proceso en proceso paso de diez minutos con cincuenta de ellos muriendo.  Morir
 * sale gratis; que te pregunten por ello no.
 */
int isa_worker_main(const isa_work *w);

#endif /* VXP_WINDOWS_ISA_POOL_H */
