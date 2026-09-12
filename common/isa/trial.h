/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/trial.h
 * @brief
 * \~english What one attempt at executing arbitrary bytes came to.
 * \~spanish En que quedo un intento de ejecutar bytes arbitrarios.
 * \~
 *
 * \~english
 * WHAT THIS IS FOR.  Walking the instruction space asking the processor itself
 * what exists: which byte sequences it accepts, how many bytes each one really
 * consumes, and what it does to the machine.  The answers go against what our
 * instruction database and a disassembler claim, and every disagreement is a
 * finding.
 *
 * THE PROCESSOR IS THE ONLY WITNESS THAT CANNOT BE WRONG.  A manual can be
 * incomplete, a disassembler can have bugs, a table can be stale -- the silicon
 * is what actually runs.  That is why this measures instead of reasoning, and it
 * is the same stance as the rest of this tree.
 *
 * HOW ONE ATTEMPT WORKS, and it is one trick doing three jobs at once.  Two
 * pages: the first executable, the second reserved and never mapped.  The
 * candidate bytes are placed so that they END exactly at the boundary, and then
 * called.
 *
 * @code
 *      ...executable page...  |  never mapped
 *      [ .. bytes .. C A N D ]|
 *                        ^    ^
 *                        |    boundary
 *                        the candidate starts here
 * @endcode
 *
 * Whatever happens, an exception comes out, and three things are read from it:
 *
 *   the LENGTH     if the decoder needed more bytes than there were before the
 *                  boundary, it faults reading across it.  If it did not, the
 *                  instruction ran and the NEXT fetch faults across it.  Both
 *                  fault at the same address with the same code -- what tells
 *                  them apart is the reported RIP, see `isa_outcome`
 *   the VERDICT    which exception: does not exist, privileged, needed more
 *                  bytes, or it ran
 *   the STATE      the exception carries a full register context, so for a
 *                  candidate that ran, that context IS the machine state
 *                  afterwards.  Comparing it with the state before gives which
 *                  registers and flags the instruction touched -- free, with no
 *                  extra mechanism
 *
 * WHY THE STATE MATTERS MORE THAN IT LOOKS.  To reason around an `asm` block the
 * compiler does not need to know that `add` adds: it needs to know that it writes
 * `rax` and the flags.  That set is what this produces, and for an instruction
 * nobody documented it is the only way to get it.
 *
 * A TRIAL AND NOT A PROBE, and the distinction is worth keeping.  `probe/` next
 * door ASKS the machine what it has -- CPUID, the MSR catalogue, the PMU's
 * capabilities -- and a question cannot bring the machine down.  This EXECUTES
 * bytes nobody wrote on purpose, so it needs a guarded region and it can take the
 * process with it.  Two different jobs and two different risks; calling them the
 * same would blur that.
 *
 * \~spanish
 * UN INTENTO Y NO UNA SONDA, y la distincion merece conservarse.  El `probe/` de
 * al lado PREGUNTA a la maquina que tiene -- CPUID, el catalogo de MSR, las
 * capacidades del PMU -- y una pregunta no puede tirar la maquina.  Esto EJECUTA
 * bytes que nadie escribio a proposito, asi que necesita una region guardada y
 * puede llevarse el proceso por delante.  Dos trabajos distintos y dos riesgos
 * distintos; llamarlos igual lo difuminaria.
 *
 * \~english
 *
 * \~spanish
 * PARA QUE SIRVE.  Recorrer el espacio de instrucciones preguntandole al propio
 * procesador que existe: que secuencias de bytes acepta, cuantos bytes consume
 * de verdad cada una, y que le hace a la maquina.  Las respuestas van contra lo
 * que afirman nuestra base de datos de instrucciones y un desensamblador, y cada
 * discrepancia es un hallazgo.
 *
 * EL PROCESADOR ES EL UNICO TESTIGO QUE NO PUEDE EQUIVOCARSE.  Un manual puede
 * estar incompleto, un desensamblador puede tener fallos, una tabla puede estar
 * vieja -- el silicio es lo que de verdad ejecuta.  Por eso esto mide en vez de
 * razonar, que es la misma postura que el resto de este arbol.
 *
 * COMO FUNCIONA UN INTENTO, y es un solo truco haciendo tres trabajos a la vez.
 * Dos paginas: la primera ejecutable, la segunda reservada y nunca mapeada.  Los
 * bytes candidatos se colocan de forma que ACABEN exactamente en la frontera, y
 * se llama ahi.  Ver el diagrama de arriba.
 *
 * Pase lo que pase sale una excepcion, y de ella se leen tres cosas:
 *
 *   la LONGITUD   si al decodificador le faltaban bytes, falla leyendo al cruzar
 *                 la frontera.  Si no le faltaban, la instruccion corrio y es el
 *                 fetch SIGUIENTE el que falla al cruzarla.  Los dos fallan en la
 *                 misma direccion y con el mismo codigo -- lo que los distingue
 *                 es el RIP que se reporta, ver `isa_outcome`
 *   el VEREDICTO  que excepcion: no existe, privilegiada, faltaban bytes, o
 *                 ejecuto
 *   el ESTADO     la excepcion lleva un contexto de registros completo, asi que
 *                 para un candidato que corrio, ese contexto ES el estado de la
 *                 maquina despues.  Compararlo con el de antes da que registros
 *                 y banderas toco la instruccion -- gratis, sin mecanismo nuevo
 *
 * POR QUE EL ESTADO IMPORTA MAS DE LO QUE PARECE.  Para razonar alrededor de un
 * bloque `asm` el compilador no necesita saber que `add` suma: necesita saber que
 * escribe `rax` y las banderas.  Ese conjunto es lo que esto produce, y para una
 * instruccion que nadie documento es la unica forma de obtenerlo.
 */

#ifndef VXP_COMMON_ISA_TRIAL_H
#define VXP_COMMON_ISA_TRIAL_H

#include "vxp_base.h"

/**
 * @brief
 * \~english The longest an x86-64 instruction may be.
 * \~spanish Lo mas larga que puede ser una instruccion de x86-64.
 * \~
 *
 * \~english
 * Fifteen bytes, and it is an architectural limit rather than a property of any
 * encoding: a longer sequence raises a fault even if its prefixes would
 * otherwise be legal.  It is also what bounds the search -- without it, the space
 * would have no end.
 *
 * \~spanish
 * Quince bytes, y es un limite arquitectonico y no una propiedad de ninguna
 * codificacion: una secuencia mas larga provoca una excepcion aunque sus
 * prefijos fueran por lo demas legales.  Es ademas lo que acota la busqueda --
 * sin el, el espacio no tendria final.
 */
#define ISA_MAX_LEN 15u

/**
 * @brief
 * \~english How an attempt ended.
 * \~spanish Como acabo un intento.
 * \~
 *
 * \~english
 * THE FIRST TWO ARE THE SAME EXCEPTION, and telling them apart is the whole
 * point of the apparatus.  Both are an access violation, of execute type, at the
 * boundary address.  What separates them is the instruction pointer the exception
 * reports:
 *
 * @code
 *      RIP == boundary       the decoder finished, the instruction RAN, and it
 *                            was the next fetch that fell off the edge
 *                            -> the length is exactly what was placed
 *      RIP == candidate      the decoder never advanced: it needed bytes from
 *                            beyond the boundary
 *                            -> the length is greater than what was placed
 * @endcode
 *
 * Measured before writing this, and it is not obvious: `nop` and a bare `B8`
 * (`mov eax, imm32`, five bytes) placed one byte before the boundary produce the
 * same code, the same type and the same address.  Only the RIP differs.
 *
 * \~spanish
 * LOS DOS PRIMEROS SON LA MISMA EXCEPCION, y distinguirlos es la razon de ser
 * del aparato.  Los dos son una violacion de acceso, de tipo ejecucion, en la
 * direccion de la frontera.  Lo que los separa es el puntero de instruccion que
 * reporta la excepcion: ver el cuadro de arriba.
 *
 * Medido antes de escribir esto, y no es obvio: `nop` y un `B8` solo
 * (`mov eax, imm32`, cinco bytes) puestos un byte antes de la frontera producen
 * el mismo codigo, el mismo tipo y la misma direccion.  Solo difiere el RIP.
 */
typedef enum isa_outcome {
    /** \~english it decoded and ran; the length is what was placed
     *  \~spanish decodifico y corrio; la longitud es lo que se puso \~ */
    ISA_RAN = 0,
    /** \~english the decoder needed bytes past the boundary
     *  \~spanish al decodificador le faltaban bytes pasada la frontera \~ */
    ISA_TRUNCATED = 1,
    /** \~english the processor does not know these bytes (#UD)
     *  \~spanish el procesador no conoce estos bytes (#UD) \~ */
    ISA_INVALID = 2,
    /** \~english it exists but not at this privilege level (#GP)
     *  \~spanish existe, pero no en este nivel de privilegio (#GP) \~ */
    ISA_PRIVILEGED = 3,
    /** \~english it ran and touched memory it should not have
     *  \~spanish corrio y toco memoria que no debia \~ */
    ISA_MEMORY_FAULT = 4,
    /** \~english it ran and raised something arithmetic (divide by zero...)
     *  \~spanish corrio y levanto algo aritmetico (division por cero...) \~ */
    ISA_ARITH = 5,
    /**
     * \~english Something the classifier does not know how to place.
     *
     * It is a value of its own and not folded into the others on purpose: an
     * attempt whose outcome nobody understands is DATA -- very possibly the
     * interesting kind -- and hiding it inside "invalid" would throw away the
     * findings this whole tool exists for.
     *
     * \~spanish Algo que el clasificador no sabe donde colocar.
     *
     * Es un valor propio y no se mete con los demas a proposito: un intento cuyo
     * resultado nadie entiende es un DATO -- muy posiblemente del interesante --
     * y esconderlo dentro de "invalida" tiraria justo los hallazgos para los que
     * existe esta herramienta.
     */
    ISA_UNKNOWN = 6,
    /**
     * \~english It returned on its own, without any exception.
     *
     * Rare and worth noticing: it means the candidate was a `ret` or something
     * that jumped back.  The state afterwards is trustworthy, but the length is
     * NOT measured -- nothing fell off the edge.
     *
     * \~spanish Volvio por su cuenta, sin excepcion ninguna.
     *
     * Raro y digno de notar: quiere decir que el candidato era un `ret` o algo
     * que salto de vuelta.  El estado de despues es fiable, pero la longitud NO
     * queda medida -- nada se cayo por el borde.
     */
    ISA_RETURNED = 7
} isa_outcome;

/**
 * @brief
 * \~english The registers, before and after, to see what the instruction moved.
 * \~spanish Los registros, antes y despues, para ver que movio la instruccion.
 * \~
 *
 * \~english
 * SIXTEEN GENERAL REGISTERS AND THE FLAGS, and no more for now.  The exception
 * context also carries the XMM, and the wider vector state needs asking for it
 * separately; that is more plumbing and it comes later.  What is here is what
 * answers the question the compiler actually asks about an `asm` block: which
 * registers does this clobber.
 *
 * THE ORDER IS THE ARCHITECTURE'S, not the context record's.  A register's number
 * is what appears in an encoding -- `rax` is 0, `rcx` is 1 -- so indexing by it
 * lets a finding be written as "writes register 2" without a translation table in
 * the middle.
 *
 * \~spanish
 * DIECISEIS REGISTROS GENERALES Y LAS BANDERAS, y de momento nada mas.  El
 * contexto de la excepcion lleva tambien los XMM, y el estado vectorial mas
 * ancho hay que pedirlo aparte; eso es mas fontaneria y viene despues.  Lo que
 * hay aqui es lo que responde a la pregunta que el compilador se hace de verdad
 * sobre un bloque `asm`: que registros pisa esto.
 *
 * EL ORDEN ES EL DE LA ARQUITECTURA, no el del registro de contexto.  El numero
 * de un registro es el que aparece en una codificacion -- `rax` es 0, `rcx` es
 * 1 --, asi que indexar por el permite escribir un hallazgo como "escribe el
 * registro 2" sin una tabla de traduccion por medio.
 */
#define ISA_GPR_COUNT 16u

typedef struct isa_state {
    u64 gpr[ISA_GPR_COUNT]; /**< \~english rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi, r8..r15 \~spanish rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi, r8..r15 \~ */
    u64 flags;
} isa_state;

/**
 * @brief
 * \~english Everything one attempt produced.
 * \~spanish Todo lo que produjo un intento.
 */
typedef struct isa_result {
    u32 outcome;   /**< `isa_outcome` */
    u32 length;    /**< \~english bytes consumed, when `ISA_RAN` says so \~spanish bytes consumidos, cuando `ISA_RAN` lo dice \~ */
    u32 code;      /**< \~english the system's exception code, raw \~spanish el codigo de excepcion del sistema, en crudo \~ */
    u32 _pad;
    u64 fault_at;  /**< \~english the address the access was to \~spanish la direccion a la que se accedio \~ */
    u64 rip;       /**< \~english what the exception reported; the discriminator \~spanish lo que reporto la excepcion; el discriminante \~ */
    /**
     * \~english The machine state AFTER, taken from the exception's context.
     *
     * There is no `before` here and no set of registers written, and that is not
     * an omission: computing which registers an instruction touched needs a
     * chosen input state, and choosing one needs loading the registers before the
     * call -- which this file's mechanism does not do yet.  A field that nobody
     * fills and reads as zero would say "it wrote nothing", which is a different
     * claim from "nobody looked".
     *
     * When that arrives, the comparison has a trap worth writing down in advance:
     * `rsp` and `rip` change on their own, because the call pushed a return
     * address.  Taking every difference at face value would report that every
     * instruction in the set writes the stack pointer.
     *
     * \~spanish El estado de la maquina DESPUES, tomado del contexto de la
     * excepcion.
     *
     * Aqui no hay un `antes` ni un conjunto de registros escritos, y no es un
     * olvido: calcular que registros toco una instruccion necesita un estado de
     * entrada elegido, y elegirlo necesita cargar los registros antes de la
     * llamada -- que es algo que el mecanismo de este fichero todavia no hace.  Un
     * campo que nadie rellena y se lee como cero diria "no escribio nada", que es
     * una afirmacion distinta de "nadie miro".
     *
     * Cuando llegue, la comparacion tiene una trampa que conviene dejar escrita de
     * antemano: `rsp` y `rip` cambian por su cuenta, porque la llamada empujo una
     * direccion de retorno.  Tomar cada diferencia al pie de la letra reportaria
     * que todas las instrucciones del juego escriben el puntero de pila.
     */
    isa_state after;

    /**
     * \~english Which general registers came out different, one bit per
     * register, and whether the flags moved.
     *
     * Filled only by the attempts that supply an input state -- with no `before`
     * there is nothing to compare against, and these stay at zero with the
     * outcome saying so.
     *
     * \~spanish Que registros generales salieron distintos, un bit por registro,
     * y si las banderas se movieron.
     *
     * Lo rellenan solo los intentos que aportan un estado de entrada -- sin un
     * `antes` no hay con que comparar, y estos se quedan a cero con el resultado
     * diciendolo.
     */
    u32 wrote_gpr;
    u32 wrote_flags;
} isa_result;

/**
 * @brief
 * \~english Which register the stack pointer is, because it does not play fair.
 * \~spanish Que registro es el puntero de pila, porque no juega limpio.
 * \~
 *
 * \~english
 * Every other register only changes if the instruction changed it.  This one
 * changes because CALLING the candidate pushed a return address, so comparing it
 * naively reports that every instruction in the set writes the stack pointer.
 *
 * What it is compared against instead is what the mechanism does to it on its
 * own, measured once by running an instruction known to be inert.  A deviation
 * from that is the candidate's doing; equality is the mechanism's.
 *
 * \~spanish
 * Todos los demas registros solo cambian si la instruccion los cambio.  Este
 * cambia porque LLAMAR al candidato empujo una direccion de retorno, asi que
 * compararlo a la ligera reporta que todas las instrucciones del juego escriben
 * el puntero de pila.
 *
 * Con lo que se compara en su lugar es con lo que el mecanismo le hace por su
 * cuenta, medido una vez ejecutando una instruccion que se sabe inerte.  Una
 * desviacion de eso es cosa del candidato; la igualdad es del mecanismo.
 */
#define ISA_STACK_REG 4u

/** @brief
 *  \~english What kind of value an instruction put in a register.
 *  \~spanish Que clase de valor puso una instruccion en un registro. \~ */
typedef enum isa_effect {
    /** \~english it did not change \~spanish no cambio \~ */
    ISA_EFFECT_NONE = 0,
    /**
     * \~english The same value from two different inputs: the instruction wrote
     * something that does not depend on what was there.
     * \~spanish El mismo valor desde dos entradas distintas: la instruccion
     * escribio algo que no depende de lo que hubiera.
     */
    ISA_EFFECT_CONSTANT = 1,
    /**
     * \~english Different values from different inputs: it read something.
     * \~spanish Valores distintos desde entradas distintas: leyo algo.
     */
    ISA_EFFECT_DEPENDS = 2
} isa_effect;

/**
 * @brief
 * \~english What an instruction does to the machine, from two runs.
 * \~spanish Que le hace una instruccion a la maquina, a partir de dos corridas.
 * \~
 *
 * \~english
 * WHY TWO RUNS AND NOT ONE.  One input state says which registers changed, and
 * that is already what a compiler needs to reason around an `asm` block.  It does
 * NOT say whether the instruction wrote a constant or copied something it read,
 * because with a single input those look identical.  Two different inputs
 * separate them, and it costs one more execution.
 *
 * WHAT IT STILL DOES NOT SAY: which input.  `mov rax, rcx` and `add rax, rcx`
 * both come out as "rax depends on the input", and telling them apart is
 * synthesis -- the part that is genuinely research-scale.  What is here is the
 * effect SET, which is what the ASA stores; the effect FUNCTION is not.
 *
 * \~spanish
 * POR QUE DOS CORRIDAS Y NO UNA.  Un estado de entrada dice que registros
 * cambiaron, y eso ya es lo que un compilador necesita para razonar alrededor de
 * un bloque `asm`.  NO dice si la instruccion escribio una constante o copio algo
 * que leyo, porque con una sola entrada las dos cosas se ven igual.  Dos entradas
 * distintas las separan, y cuesta una ejecucion mas.
 *
 * LO QUE SIGUE SIN DECIR: cual entrada.  `mov rax, rcx` y `add rax, rcx` salen
 * las dos como "rax depende de la entrada", y distinguirlas es sintesis -- la
 * parte que si es de escala de investigacion.  Lo que hay aqui es el CONJUNTO de
 * efectos, que es lo que guarda el ASA; la FUNCION de efecto, no.
 */
typedef struct isa_effects {
    u32 outcome; /**< `isa_outcome` \~english of the first run \~spanish de la primera corrida \~ */
    u32 length;
    u32 wrote_gpr;
    u32 flags_effect;                 /**< `isa_effect` */
    u8 gpr_effect[ISA_GPR_COUNT];     /**< `isa_effect` \~english per register \~spanish por registro \~ */
    /**
     * \~english Did the two runs agree on WHAT happened?
     *
     * If one ran and the other faulted, or the lengths differ, the instruction is
     * not behaving as a function of its inputs and no effect can be reported.  It
     * is its own field because that is a finding, not an error: a candidate that
     * answers differently to the same question is exactly what a fuzzer is looking
     * for.
     *
     * \~spanish ¿Coincidieron las dos corridas en QUE paso?
     *
     * Si una corrio y la otra fallo, o las longitudes difieren, la instruccion no
     * se comporta como funcion de sus entradas y no se puede reportar ningun
     * efecto.  Es un campo propio porque eso es un hallazgo y no un error: un
     * candidato que contesta distinto a la misma pregunta es justo lo que un
     * fuzzer busca.
     */
    u32 consistent;
} isa_effects;

#endif /* VXP_COMMON_ISA_TRIAL_H */
