/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/walk.c
 * @brief
 * \~english The walk: the carry, and the accounting of what got skipped.
 * \~spanish El recorrido: el acarreo, y la cuenta de lo que se salto.
 * \~
 *
 * \~english
 * All of the reasoning -- why it terminates, why the increment happens where it
 * does, why the filler is zero -- is in `walk.h`, where somebody deciding
 * whether to use this will read it.  Here is only what the code has to say for
 * itself.
 *
 * \~spanish
 * Todo el razonamiento -- por que termina, por que el incremento ocurre donde
 * ocurre, por que el relleno es cero -- esta en `walk.h`, que es donde lo leera
 * quien decida si usar esto.  Aqui solo lo que el codigo tiene que decir de si
 * mismo.
 */

#include "isa/walk.h"

/**
 * @brief
 * \~english How many candidates a jump from position `i` did not have to try.
 * \~spanish Cuantas candidatas no tuvo que probar un salto desde la posicion `i`.
 * \~
 *
 * \~english
 * Incrementing at `i` instead of at the last enumerated byte leaves out every
 * combination of the bytes between them: 256^k - 1 of them, where k is how many
 * bytes those are.  This is the number that says what tunneling is worth, and
 * the reason it is counted at all is that a search whose saving cannot be seen
 * cannot be told apart from one that is not saving anything.
 *
 * IT SATURATES instead of wrapping.  With `depth` at its ceiling the count does
 * not fit in sixty-four bits, and a counter that wraps reports a tiny saving
 * where there was an enormous one -- which is worse than reporting "at least
 * this much", because it reads as a real number.
 *
 * \~spanish
 * Incrementar en `i` en vez de en el ultimo byte enumerado deja fuera toda
 * combinacion de los bytes que hay en medio: 256^k - 1 de ellas, donde k es
 * cuantos bytes son.  Este es el numero que dice lo que vale el tunneling, y la
 * razon de contarlo es que una busqueda cuyo ahorro no se ve no se distingue de
 * una que no ahorra nada.
 *
 * SATURA en vez de dar la vuelta.  Con `depth` en su techo la cuenta no cabe en
 * sesenta y cuatro bits, y un contador que da la vuelta informa de un ahorro
 * minusculo donde hubo uno enorme -- que es peor que informar de "al menos
 * tanto", porque se lee como un numero de verdad.
 */
static u64 skipped_below(u32 i, u32 depth) {
    u64 n = 1;
    u32 k = depth - 1u - i;
    u32 j;
    /* \~english Eight bytes is where 256^k stops fitting.  \~spanish En ocho bytes
     * es donde 256^k deja de caber. \~ */
    if (k >= 8u) {
        return (u64)0xFFFFFFFFFFFFFFFFull;
    }
    for (j = 0; j < k; ++j) {
        n *= 256u;
    }
    return n - 1u;
}

status isa_walk_open(isa_walk *w, const u8 *prefix, u32 fixed, u8 lo, u8 hi,
                     u32 depth) {
    u32 i;

    if (w == 0 || depth == 0 || depth > ISA_WALK_DEPTH_MAX) {
        return ERR_INVALID;
    }
    /* \~english Freezing everything leaves nothing to walk; see the header.
     * \~spanish Congelarlo todo no deja nada que recorrer; ver la cabecera. \~ */
    if (fixed >= depth) {
        return ERR_INVALID;
    }
    if (fixed != 0 && prefix == 0) {
        return ERR_INVALID;
    }
    /* \~english An empty range is a caller's mistake, not zero results.
     * \~spanish Un rango vacio es un error de quien llama, no cero resultados. \~ */
    if (lo > hi) {
        return ERR_INVALID;
    }

    for (i = 0; i < ISA_MAX_LEN; ++i) {
        w->bytes[i] = 0;
    }
    for (i = 0; i < fixed; ++i) {
        w->bytes[i] = prefix[i];
    }
    w->bytes[fixed] = lo;
    w->fixed = (u8)fixed;
    w->lo = lo;
    w->hi = hi;
    w->depth = (u8)depth;
    w->done = 0;
    w->_pad[0] = 0;
    w->_pad[1] = 0;
    w->_pad[2] = 0;
    w->visited = 0;
    w->skipped = 0;
    return OK;
}

int isa_walk_step(isa_walk *w, u32 consumed) {
    u32 i;
    u32 j;

    if (w == 0 || w->done) {
        return 0;
    }
    w->visited += 1u;

    /*
     * \~english WHERE THE INCREMENT GOES: the last byte the decoder actually
     * read, never past what is being enumerated.  A consumption of zero is not a
     * thing a decoder can report -- it has to read at least one byte to say
     * anything -- but it is clamped instead of trusted, because the value comes
     * from another subsystem and a zero here would index before the buffer.
     *
     * \~spanish DONDE VA EL INCREMENTO: el ultimo byte que el decodificador leyo
     * de verdad, nunca mas alla de lo que se esta enumerando.  Un consumo de
     * cero no es algo que un decodificador pueda informar -- tiene que leer al
     * menos un byte para decir algo --, pero se acota en vez de fiarse, porque
     * el valor viene de otro subsistema y un cero aqui indexaria antes del
     * bufer.
     */
    i = (consumed < (u32)w->depth) ? consumed : (u32)w->depth;
    if (i == 0) {
        i = 1;
    }
    i -= 1;
    /* \~english Never above the frozen head: those bytes are somebody else's
     * subtree.  \~spanish Nunca por encima de la cabeza congelada: esos bytes son
     * el subarbol de otro. \~ */
    if (i < (u32)w->fixed) {
        i = (u32)w->fixed;
    }

    w->skipped += skipped_below(i, (u32)w->depth);

    /* \~english What is below the increment point is a fresh subtree, so it
     * starts at zero -- and this is also the half of the termination argument
     * that lowers the number.  \~spanish Lo que hay por debajo del punto de
     * incremento es un subarbol nuevo, asi que empieza en cero -- y esta es
     * tambien la mitad del argumento de terminacion que baja el numero. \~ */
    for (j = i + 1u; j < (u32)w->depth; ++j) {
        w->bytes[j] = 0;
    }

    /*
     * \~english THE CARRY, and it stops at the frozen head rather than at zero:
     * running out of room inside the owned subtree is the end of this walk, not
     * the end of the space.
     *
     * \~spanish EL ACARREO, y se detiene en la cabeza congelada y no en cero:
     * quedarse sin sitio dentro del subarbol propio es el final de ESTE
     * recorrido, no el final del espacio.
     */
    for (;;) {
        if (w->bytes[i] != 0xFFu) {
            w->bytes[i] += 1u;
            break;
        }
        w->bytes[i] = 0;
        if (i == (u32)w->fixed) {
            w->done = 1;
            return 0;
        }
        i -= 1u;
    }

    /* \~english And the range applies to the one byte the walk owns outright.
     * \~spanish Y el rango aplica al unico byte que el recorrido posee del todo. \~ */
    if (w->bytes[w->fixed] > w->hi) {
        w->done = 1;
        return 0;
    }
    return 1;
}
