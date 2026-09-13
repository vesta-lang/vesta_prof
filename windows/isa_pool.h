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
 * \~english What a run claims about a lethal subtree.
 * \~spanish Que afirma una corrida sobre un subarbol letal.
 * \~
 *
 * \~english
 * THE TWO MODES DIFFER IN WHAT THEY PROMISE, not in how fast they are, and that is
 * why it is a mode and not a heuristic.
 *
 *      CHARACTERISE   a lethal subtree is SAMPLED: so many values of the next byte
 *                     were tried, so many killed.  It cannot say "these are all of
 *                     them".  Cheap enough to sweep the machine
 *      EXHAUSTIVE     every lethal candidate is narrowed down and named.  It can say
 *                     "these are all of them", and it costs what that is worth: the
 *                     `C2` subtree alone -- `ret imm16`, where 65.280 of 65.536
 *                     candidates really do kill -- takes around 196.000 processes
 *
 * WHY BOTH, and it is not a compromise.  The question the compiler asks is "what does
 * this instruction touch", and `ret imm16` destroys the stack pointer is answered
 * ONCE; 65.280 confirmations serve nobody.  But the exhaustive one has to exist,
 * because it is the only one that can find what nothing describes: an undocumented
 * instruction is by definition one whose format is unknown, and sampling by what is
 * known skips exactly the region where it hides.
 *
 * So: characterise to find out WHERE to look, exhaustive to look.
 *
 * \~spanish
 * LOS DOS MODOS SE DIFERENCIAN EN LO QUE PROMETEN, no en lo rapidos que son, y por eso
 * es un modo y no una heuristica.
 *
 *      CARACTERIZAR   un subarbol letal se MUESTREA: se probaron tantos valores del
 *                     byte siguiente y tantos mataron.  No puede decir "estas son
 *                     todas".  Barato como para barrer la maquina
 *      EXHAUSTIVO     cada candidata letal se estrecha y se nombra.  Puede decir
 *                     "estas son todas", y cuesta lo que eso vale: el subarbol de
 *                     `C2` solo -- `ret imm16`, donde 65.280 de 65.536 candidatas
 *                     matan de verdad -- son unos 196.000 procesos
 *
 * POR QUE LOS DOS, y no es un apano.  La pregunta que hace el compilador es "que toca
 * esta instruccion", y `ret imm16` destruye el puntero de pila se responde UNA VEZ;
 * 65.280 confirmaciones no le sirven a nadie.  Pero el exhaustivo tiene que existir,
 * porque es el unico que puede encontrar lo que nada describe: una instruccion
 * indocumentada es por definicion una cuyo formato no se conoce, y muestrear por lo
 * conocido se salta justo la region donde se esconde.
 *
 * O sea: caracterizar para saber DONDE mirar, exhaustivo para mirar.
 */
typedef enum isa_mode {
    ISA_MODE_CHARACTERISE = 0,
    ISA_MODE_EXHAUSTIVE = 1
} isa_mode;

/**
 * @brief
 * \~english How many values of a byte a characterisation tries.
 * \~spanish Cuantos valores de un byte prueba una caracterizacion.
 * \~
 *
 * \~english
 * Sixteen, spread evenly -- `00`, `11`, `22` ... `FF` -- and the spread is the point.
 * A byte that selects a register does so with three bits of itself, so an even spread
 * lands on several different selections; sixteen adjacent values would all pick the
 * same one and report a subtree as uniform when it is not.
 *
 * It is a sample and it says so.  Sixteen of 256 catches a subtree where everything
 * kills and a subtree where nothing does; it can miss a pattern that only a handful
 * of values hit, and that is the price of the mode.
 *
 * \~spanish
 * Dieciseis, repartidos -- `00`, `11`, `22` ... `FF` -- y el reparto es lo importante.
 * Un byte que selecciona un registro lo hace con tres bits de si mismo, asi que un
 * reparto uniforme cae en varias selecciones distintas; dieciseis valores contiguos
 * elegirian todos la misma y reportarian como uniforme un subarbol que no lo es.
 *
 * Es una muestra y lo dice.  Dieciseis de 256 cazan un subarbol donde todo mata y uno
 * donde nada mata; puede perderse un patron al que solo den un punado de valores, y
 * ese es el precio del modo.
 */
#define ISA_POOL_PROFILE 16u

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
    /**
     * \~english Candidates represented by a finding because SAMPLING said the byte
     * about to be opened does not decide anything.
     *
     * KEPT APART FROM `covered` BECAUSE IT IS A WEAKER CLAIM, and putting the two in
     * one column would quietly turn a proof into a guess.  `covered` is established:
     * the byte is past the end of the instruction, so nothing reads it.  This one is
     * measured on four values -- and four is not 256.
     *
     * It exists because the alternative measured worse: at depth three, `C8`
     * (`enter imm16, imm8`) and `C2` (`ret imm16`) alone produced 33.578 of 35.910
     * findings, every one of them the same instruction with a different immediate.
     *
     * \~spanish Candidatas representadas por un hallazgo porque el MUESTREO dijo que el
     * byte que se iba a abrir no decide nada.
     *
     * APARTE DE `covered` PORQUE ES UNA AFIRMACION MAS FLOJA, y meter las dos en una
     * columna convertiria calladamente una demostracion en una suposicion.  `covered`
     * esta establecido: el byte cae pasado el final de la instruccion, asi que nadie lo
     * lee.  Este se mide sobre cuatro valores -- y cuatro no son 256.
     *
     * Existe porque la alternativa medida era peor: a profundidad tres, `C8`
     * (`enter imm16, imm8`) y `C2` (`ret imm16`) por si solos producian 33.578 de
     * 35.910 hallazgos, todos ellos la misma instruccion con otro inmediato.
     */
    u64 sampled;
    u64 samples; /**< \~english processes spent sampling \~spanish procesos gastados en muestrear \~ */
} isa_pool_stats;

/**
 * @brief
 * \~english How many values of a byte get tried before calling it undecisive.
 * \~spanish Cuantos valores de un byte se prueban antes de declararlo no decisivo.
 * \~
 *
 * \~english
 * Four, and which four matters more than how many: `00`, `55`, `AA` and `FF`.  If the
 * byte happens to be a ModRM, `00` asks for a memory operand through `rax` and `FF`
 * for a register, which is about as different as two values of that byte can be -- so a
 * byte that decides anything is very likely to show it.  Two adjacent values would
 * agree on almost everything and prove nothing.
 *
 * \~spanish
 * Cuatro, y CUALES cuatro importa mas que cuantos: `00`, `55`, `AA` y `FF`.  Si el byte
 * resulta ser un ModRM, `00` pide un operando de memoria por `rax` y `FF` pide un
 * registro, que es todo lo distinto que pueden ser dos valores de ese byte -- asi que
 * un byte que decida algo es muy probable que lo ensene.  Dos valores contiguos
 * coincidirian en casi todo y no demostrarian nada.
 */
#define ISA_POOL_SAMPLES 4u

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
typedef struct isa_worker_outcome {
    u32 how;     /**< \~english an `isa_worker_end` \~spanish un `isa_worker_end` \~ */
    u32 reached; /**< \~english bytes it had got to offering; 0 if it never said \~spanish bytes que llevaba ofreciendo; 0 si no lo dijo \~ */
    /**
     * \~english How many other candidates this one finding answers for.  0 means it
     * speaks only for itself.
     *
     * \~spanish Por cuantas otras candidatas responde este hallazgo.  0 quiere decir que
     * habla solo por si misma.
     */
    u64 stands_for;
    /**
     * \~english Whether `stands_for` was established or sampled, and it is reported
     * rather than folded in: one says the bytes are never read, the other says four
     * values of them behaved alike.  A reader who cannot tell which is which has been
     * handed a proof and a guess in the same shape.
     *
     * \~spanish Si `stands_for` esta demostrado o muestreado, y se informa en vez de
     * mezclarse: uno dice que esos bytes no se leen nunca, el otro dice que cuatro
     * valores se portaron igual.  A quien no pueda distinguirlos se le ha dado una
     * demostracion y una suposicion con la misma pinta.
     */
    u32 by_sampling;
    u32 tried;  /**< \~english values of the next byte a characterisation tried \~spanish valores del byte siguiente que probo una caracterizacion \~ */
    u32 killed; /**< \~english how many of them took the worker down \~spanish cuantos de ellos se llevaron al trabajador \~ */
    u32 _pad;
} isa_worker_outcome;

typedef void (*isa_pool_report_fn)(void *ctx, const isa_work *w,
                                   const isa_worker_outcome *o,
                                   const isa_tally *t);

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
status isa_pool_run(const char *exe, u32 mode, const char *ref_path,
                    u32 workers, u32 timeout_ms, isa_queue *q,
                    isa_pool_report_fn report, void *ctx,
                    isa_pool_stats *stats);

/**
 * @brief
 * \~english Reads a reference table off disk.  The caller frees what it returns.
 * \~spanish Lee una tabla de referencia del disco.  Quien llama libera lo que devuelve.
 * \~
 *
 * @return \~english the buffer, or null having said why on standard output \~spanish el bufer, o nulo habiendo dicho por que por la salida estandar \~
 *
 * \~english
 * IT LIVES HERE AND NOT IN `common/`, which neither allocates nor does I/O -- the same
 * seam as the cost tables.  Both sides need it: the parent to check the table matches
 * the depth being asked for, and every worker to actually compare against it.  One
 * loader instead of two is one fewer place for them to read the same file differently.
 *
 * \~spanish
 * VIVE AQUI Y NO EN `common/`, que no reserva memoria ni hace E/S -- la misma costura que
 * las tablas de coste.  Los dos lados lo necesitan: el padre para comprobar que la tabla
 * casa con la profundidad que se pide, y cada trabajador para comparar de verdad contra
 * ella.  Un cargador en vez de dos es un sitio menos donde leer el mismo fichero de
 * forma distinta.
 */
u8 *isa_ref_load(const char *path, isa_ref *out);

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
int isa_worker_main(const isa_work *w, const char *ref_path);

/**
 * @brief
 * \~english Makes dying cheap: no crash reporting, no dialog, no wait.
 * \~spanish Hace que morir salga barato: sin informe de errores, sin dialogo, sin
 *           espera.
 * \~
 *
 * \~english
 * ANY MODE THAT EXECUTES ARBITRARY BYTES NEEDS THIS, which is why it is its own function
 * instead of being buried in the worker.  Dying is the normal outcome here, not an
 * incident: with the system's reporting on, each death cost about five seconds of being
 * asked about it, and that is what turned a sweep of 114 first bytes into ten minutes.
 *
 * \~spanish
 * CUALQUIER MODO QUE EJECUTE BYTES ARBITRARIOS LO NECESITA, y por eso es una funcion
 * propia en vez de estar enterrada en el trabajador.  Morir es el resultado normal aqui,
 * no un incidente: con el informe del sistema encendido cada muerte costaba unos cinco
 * segundos de que te preguntaran, y eso es lo que convirtio un barrido de 114 primeros
 * bytes en diez minutos.
 */
void isa_quiet_death(void);

#endif /* VXP_WINDOWS_ISA_POOL_H */
