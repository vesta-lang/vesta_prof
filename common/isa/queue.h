/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/queue.h
 * @brief
 * \~english The work to be done, and what to do with the work that was lost.
 * \~spanish El trabajo por hacer, y que hacer con el trabajo que se perdio.
 * \~
 *
 * \~english
 * WHY A QUEUE AND NOT A SPLIT.  Handing each worker a fixed slice of the 256 first
 * bytes is the obvious thing and it does not work, because the subtrees are nothing
 * like each other.  Measured at depth two, of the 114 first bytes that got through
 * before the run jammed: 38 open 256 candidates, 25 open exactly one -- the first
 * byte is already the whole instruction -- and 50 killed the worker outright.  A
 * static split gives one worker a thousand times the work of another, and the run
 * takes as long as the unluckiest slice.
 *
 * AND THE OTHER REASON, which is not about speed at all.  A worker DIES.  Executing
 * arbitrary bytes, that is not a risk to be reduced but the normal case: `01 C4` is
 * `add esp, eax`, which writes the stack pointer and leaves the unwinder without a
 * frame to find, and `73 FE` is `jae -2`, which jumps to itself and never comes
 * back.  Neither can be caught from inside the process it happens in.  So work has
 * to be REASSIGNABLE, and something has to hold what was handed out but never came
 * back.  That is this file.
 *
 * WHAT MAKES A LOST ITEM USEFUL: it gets REFINED instead of thrown away.
 *
 *      a range of first bytes   splits in half, and the half that dies again
 *                               splits again -- ordinary bisection
 *      a single first byte      freezes it and opens the byte after it, which is
 *                               one level deeper into the same subtree
 *      a single candidate       there is nothing left to split: this is the one
 *                               that killed the worker, by name
 *
 * So a death is not a gap in the sweep, it is a finding with an address.  It is
 * also why the item is exactly what opens a walk: it has to survive being written
 * on a command line, handed to a fresh process, and split into smaller versions of
 * itself.
 *
 * \~spanish
 * POR QUE UNA COLA Y NO UN REPARTO.  Darle a cada trabajador una tajada fija de los
 * 256 primeros bytes es lo evidente y no funciona, porque los subarboles no se
 * parecen en nada.  Medido a profundidad dos, de los 114 primeros bytes que
 * pasaron antes de que la corrida se atascara: 38 abren 256 candidatas, 25 abren
 * exactamente una -- el primer byte ya es la instruccion entera -- y 50 mataron al
 * trabajador sin mas.  Un reparto estatico le da a un trabajador mil veces el
 * trabajo de otro, y la corrida tarda lo que la tajada mas desgraciada.
 *
 * Y LA OTRA RAZON, que no va de velocidad.  Un trabajador MUERE.  Ejecutando bytes
 * arbitrarios eso no es un riesgo que reducir sino el caso normal: `01 C4` es
 * `add esp, eax`, que escribe el puntero de pila y deja al desenrollador sin marco
 * que encontrar, y `73 FE` es `jae -2`, que salta a si misma y no vuelve nunca.
 * Ninguna de las dos se puede capturar desde dentro del proceso en el que pasa.
 * Asi que el trabajo tiene que ser REASIGNABLE, y algo tiene que guardar lo que se
 * entrego y no volvio.  Eso es este fichero.
 *
 * LO QUE HACE UTIL UN TROZO PERDIDO: se REFINA en vez de tirarse.
 *
 *      un rango de primeros bytes  se parte en dos, y la mitad que vuelve a morir
 *                                  se parte otra vez -- biseccion normal
 *      un primer byte solo         se congela y se abre el byte siguiente, que es
 *                                  un nivel mas adentro del mismo subarbol
 *      una sola candidata          no queda nada que partir: esta es la que mato al
 *                                  trabajador, con nombre y apellidos
 *
 * Asi que una muerte no es un hueco del barrido, es un hallazgo con direccion.  Es
 * tambien la razon de que el trozo sea exactamente lo que abre un recorrido: tiene
 * que sobrevivir a escribirse en una linea de ordenes, entregarse a un proceso
 * nuevo, y partirse en versiones mas pequenas de si mismo.
 */

#ifndef VXP_COMMON_ISA_QUEUE_H
#define VXP_COMMON_ISA_QUEUE_H

#include "isa/sweep.h"

/**
 * @brief
 * \~english How many items fit at once.
 * \~spanish Cuantos trozos caben a la vez.
 * \~
 *
 * \~english
 * Enough for the initial split of 256 first bytes with room to spare, and the
 * refinement barely adds: it is depth-first and a lost item pushes at most two
 * pieces, so what is in flight at any moment is a handful of branches and not a
 * frontier.
 *
 * THERE IS NO GROWING, and that is on purpose in `common/`: nothing here allocates.
 * The queue filling up is an error that gets SAID -- see `dropped` -- because an
 * item that does not fit is a region of the instruction space that nobody sweeps,
 * and a sweep with a silent hole in it is worse than one that stops.
 *
 * \~spanish
 * De sobra para el reparto inicial de 256 primeros bytes, y el refinamiento apenas
 * anade: va en profundidad y un trozo perdido empuja dos piezas como mucho, asi que
 * lo que hay en vuelo en cada momento son un punado de ramas y no una frontera.
 *
 * NO CRECE, y eso es a proposito en `common/`: aqui nadie reserva memoria.  Que la
 * cola se llene es un error que se DICE -- ver `dropped` --, porque un trozo que no
 * cabe es una region del espacio de instrucciones que nadie barre, y un barrido con
 * un agujero callado dentro es peor que uno que se para.
 */
#define ISA_QUEUE_CAP 1024u

/**
 * @brief
 * \~english The pending work.  Last in, first out, and that is deliberate.
 * \~spanish El trabajo pendiente.  El ultimo que entra es el primero que sale, y es
 *           a proposito.
 * \~
 *
 * \~english
 * LIFO because refinement is a descent: when an item dies, what wants doing next is
 * its halves, while the failure is still the thing being chased.  Taking the oldest
 * item instead would interleave every descent with every other one, so the queue
 * would hold a whole frontier of half-narrowed failures and each one would take as
 * long as the entire sweep to resolve.
 *
 * \~spanish
 * El ultimo primero porque el refinamiento es un descenso: cuando un trozo muere, lo
 * que interesa hacer a continuacion son sus mitades, mientras el fallo sigue siendo
 * lo que se persigue.  Coger el mas antiguo intercalaria cada descenso con todos los
 * demas, asi que la cola guardaria una frontera entera de fallos a medio estrechar y
 * cada uno tardaria lo que el barrido completo en resolverse.
 */
typedef struct isa_queue {
    isa_work items[ISA_QUEUE_CAP];
    u32 count;
    u32 _pad;
    u64 pushed;  /**< \~english handed in, ever \~spanish metidos, en total \~ */
    u64 dropped; /**< \~english did not fit: a hole, and it must be reported \~spanish no cupieron: un agujero, y hay que informarlo \~ */
} isa_queue;

/** @brief
 *  \~english Empties it.  Safe on one that was never used.
 *  \~spanish La vacia.  Segura sobre una que no se uso nunca. \~ */
void isa_queue_open(isa_queue *q);

/**
 * @brief
 * \~english Adds one item.  Counts it as dropped if there is no room.
 * \~spanish Anade un trozo.  Lo cuenta como perdido si no hay sitio.
 * \~
 *
 * @return `OK`; `ERR_NOSPACE` \~english if it did not fit \~spanish si no cupo \~
 */
status isa_queue_push(isa_queue *q, const isa_work *w);

/** @brief
 *  \~english Takes the next item out.
 *  \~spanish Saca el trozo siguiente.
 *  \~
 *  @return \~english 1 if there was one, 0 if the queue is empty \~spanish 1 si habia, 0 si la cola esta vacia \~ */
int isa_queue_pop(isa_queue *q, isa_work *out);

/**
 * @brief
 * \~english Fills it with the split of one byte, under a prefix that stays put.
 * \~spanish La llena con el reparto de un byte, bajo un prefijo que no se mueve.
 * \~
 *
 * @param prefix \~english the frozen head, or null when `fixed` is 0 \~spanish la cabeza congelada, o nulo cuando `fixed` es 0 \~
 * @param fixed \~english how many bytes are frozen; 0 seeds the whole space \~spanish cuantos bytes se congelan; 0 siembra el espacio entero \~
 * @param chunk \~english how many values per item; 1 gives 256 items \~spanish cuantos valores por trozo; 1 da 256 trozos \~
 * @return `OK`; `ERR_INVALID` \~english on a chunk or depth that makes no sense; `ERR_NOSPACE` if they do not fit \~spanish con un trozo o una profundidad sin sentido; `ERR_NOSPACE` si no caben \~
 *
 * \~english
 * THE PREFIX IS WHAT MAKES A RUN AIMABLE.  With none, this seeds the whole
 * instruction space -- which is what a sweep of the machine wants.  With one, it
 * seeds a single subtree, and that is how somebody asks for the expensive, exhaustive
 * treatment of the one region a cheap sweep flagged as worth it.
 *
 * \~spanish
 * EL PREFIJO ES LO QUE PERMITE APUNTAR UNA CORRIDA.  Sin el, esto siembra el espacio
 * de instrucciones entero -- que es lo que quiere un barrido de la maquina.  Con el,
 * siembra un solo subarbol, y asi es como alguien pide el trato caro y exhaustivo de
 * la unica region que un barrido barato senalo como interesante.
 *
 * \~english
 * THE CHUNK IS A TRADE AND NEITHER END IS FREE.  Big chunks mean fewer processes
 * and less overhead, but a death costs the whole chunk and the bisection that
 * follows; small chunks pay a process per item and lose almost nothing to a death.
 * Given that around half the subtrees kill the worker, small is the side to err on.
 *
 * \~spanish
 * EL TAMANO DEL TROZO ES UN CANJE Y NINGUN EXTREMO SALE GRATIS.  Trozos grandes son
 * menos procesos y menos coste, pero una muerte cuesta el trozo entero y la
 * biseccion que viene detras; trozos pequenos pagan un proceso por trozo y pierden
 * casi nada con una muerte.  Dado que cerca de la mitad de los subarboles matan al
 * trabajador, lo pequeno es el lado por el que equivocarse.
 */
status isa_queue_seed(isa_queue *q, const u8 *prefix, u32 fixed, u32 depth,
                      u32 chunk);

/**
 * @brief
 * \~english What was lost, made smaller.  Or named, when it cannot get smaller.
 * \~spanish Lo que se perdio, hecho mas pequeno.  O nombrado, cuando ya no puede
 *           hacerse mas pequeno.
 * \~
 *
 * @param lost \~english the item a worker never came back from \~spanish el trozo del que un trabajador no volvio \~
 * @param single \~english written only when the return value is 1 \~spanish se escribe solo cuando el valor devuelto es 1 \~
 * @return \~english 1 when `lost` is a single candidate and therefore the culprit, 0 when it was split and pushed \~spanish 1 cuando `lost` es una sola candidata y por tanto la culpable, 0 cuando se partio y se metio en la cola \~
 *
 * \~english
 * IT ALWAYS TERMINATES, and the cost is `9 * depth` refinements at worst.  Worth
 * deriving rather than asserting, since it is a loop that feeds itself:
 *
 *      per level    8 halvings take a full 256-wide range down to one value
 *                   (256, 128, 64, 32, 16, 8, 4, 2, 1), plus 1 step to either
 *                   freeze that value and open the next byte, or name the
 *                   candidate when there is no next byte
 *      in total     9 per level, and there are `depth` levels
 *
 * Measured against that, driving the descent by hand: 18 steps to name a two-byte
 * candidate and 27 for a three-byte one, from a queue seeded as one single item.
 * Seeding it in smaller chunks skips part of the first level -- 10 steps for a
 * two-byte candidate when each item starts as one first byte.
 *
 * \~spanish
 * SIEMPRE TERMINA, y cuesta `9 * depth` refinamientos en el peor caso.  Merece
 * derivarse en vez de afirmarse, siendo un bucle que se alimenta a si mismo:
 *
 *      por nivel    8 particiones bajan un rango de 256 a un solo valor
 *                   (256, 128, 64, 32, 16, 8, 4, 2, 1), mas 1 paso para congelar
 *                   ese valor y abrir el byte siguiente, o para nombrar la
 *                   candidata cuando no hay byte siguiente
 *      en total     9 por nivel, y hay `depth` niveles
 *
 * Medido contra eso, conduciendo el descenso a mano: 18 pasos para nombrar una
 * candidata de dos bytes y 27 para una de tres, partiendo de una cola sembrada como
 * un unico trozo.  Sembrarla en trozos mas pequenos se salta parte del primer nivel
 * -- 10 pasos para una de dos bytes cuando cada trozo empieza siendo un primer byte.
 */
int isa_queue_refine(isa_queue *q, const isa_work *lost, isa_work *single);

#endif /* VXP_COMMON_ISA_QUEUE_H */
