/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file isa/ref.c
 * @brief
 * \~english Reading the header, and turning bytes into an index.
 * \~spanish Leer la cabecera, y convertir bytes en un indice.
 * \~
 *
 * \~english
 * Why a second opinion at all is in `ref.h`.  Here is the container, and it is checked
 * field by field rather than trusted: a table that does not match what is being asked
 * produces confident disagreements that are nothing but a mismatch.
 *
 * \~spanish
 * Por que hay una segunda opinion esta en `ref.h`.  Aqui esta el contenedor, y se
 * comprueba campo a campo en vez de fiarse: una tabla que no case con lo que se pregunta
 * produce desacuerdos con aplomo que no son mas que un desajuste.
 */

#include "isa/ref.h"

/*
 * \~english The container the generator writes.  Eight bytes of magic ending in a
 * newline, so that looking at the file with anything at all shows what it is.
 *
 *      0   8   magic "VXPREF1\n"
 *      8   4   depth
 *     12   4   reserved, zero
 *     16   8   how many entries
 *     24   n   one length per candidate, 0 = does not decode
 *
 * \~spanish El contenedor que escribe el generador.  Ocho bytes de magia que acaban en
 * un salto de linea, para que mirar el fichero con cualquier cosa ensene lo que es.
 *
 *      0   8   magia "VXPREF1\n"
 *      8   4   profundidad
 *     12   4   reservado, cero
 *     16   8   cuantas entradas
 *     24   n   una longitud por candidata, 0 = no decodifica
 */
#define REF_HEADER 24u

static u32 read_u32(const u8 *p) {
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static u64 read_u64(const u8 *p) {
    return (u64)read_u32(p) | ((u64)read_u32(p + 4) << 32);
}

status isa_ref_parse(const u8 *buf, u64 n, isa_ref *out) {
    static const char magic[8] = {'V', 'X', 'P', 'R', 'E', 'F', '1', '\n'};
    u32 depth;
    u64 count;
    u64 expect;
    u32 i;

    if (buf == 0 || out == 0 || n < REF_HEADER) {
        return ERR_INVALID;
    }
    for (i = 0; i < 8u; ++i) {
        if ((char)buf[i] != magic[i]) {
            return ERR_INVALID;
        }
    }
    depth = read_u32(buf + 8);
    count = read_u64(buf + 16);
    if (depth < 1u || depth > ISA_WALK_DEPTH_MAX) {
        return ERR_INVALID;
    }

    /* \~english The count has to BE 256^depth, not merely fit: a table one entry short
     * would answer for every candidate but the last, and the one it could not answer for
     * would look like a reference that does not decode it -- a finding out of nothing.
     * \~spanish La cuenta tiene que SER 256^depth, no solo caber: una tabla a la que le
     * falte una entrada responderia por todas las candidatas menos la ultima, y esa
     * pareceria una referencia que no la decodifica -- un hallazgo salido de nada. \~ */
    expect = 1;
    for (i = 0; i < depth; ++i) {
        if (expect > (u64)0xFFFFFFFFFFFFFFFFull / 256ull) {
            return ERR_INVALID;
        }
        expect *= 256ull;
    }
    if (count != expect) {
        return ERR_INVALID;
    }
    if (n - REF_HEADER < count) {
        return ERR_INVALID;
    }

    out->len = buf + REF_HEADER;
    out->count = count;
    out->depth = depth;
    out->_pad = 0;
    return OK;
}

u32 isa_ref_length(const isa_ref *r, const u8 *bytes) {
    u64 index = 0;
    u32 i;

    if (r == 0 || r->len == 0 || bytes == 0) {
        return 0;
    }
    for (i = 0; i < r->depth; ++i) {
        index = (index << 8) | (u64)bytes[i];
    }
    if (index >= r->count) {
        return 0;
    }
    return (u32)r->len[index];
}
