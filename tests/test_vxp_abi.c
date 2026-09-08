/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 + excepcion de runtime (ver LICENSE).
 */

/**
 * @file test_vxp_abi.c
 * @brief Las tres capas con las que el contrato se describe a si mismo.
 *
 * Se comprueba con una estructura publicada de ejemplo -- `demo_sample` -- que
 * las tres cosas funcionan y, sobre todo, que **de una sola tabla salen las
 * dos**: las aserciones de compilacion y el descriptor de ejecucion.  Si se
 * pudieran separar, la que nadie mira acabaria mintiendo.
 *
 * Se compila como C porque es el lenguaje del lado que produce esto.
 */

#include "vxp_abi.h"

#include <stdio.h>
#include <string.h>

static int g_checks = 0, g_fails = 0;
#define CHECK(c)                                                               \
    do {                                                                       \
        ++g_checks;                                                            \
        if (!(c)) {                                                            \
            ++g_fails;                                                         \
            printf("  FAIL L%d: %s\n", __LINE__, #c);                          \
        }                                                                      \
    } while (0)

/* Una estructura publicada de ejemplo: la cabecera de registro por delante, que
 * es como iran las de verdad. */
typedef struct demo_sample {
    abi_record head;
    u64 ip;
    u32 tid;
    u32 cpu;
} demo_sample;

/* La tabla, usada para AFIRMAR la disposicion al compilar. */
#define ABI_SIZE(t, b) ABI_ASSERT_SIZE(t, b);
#define ABI_FIELD(t, f, o, id) ABI_ASSERT_FIELD(t, f, o);
#include "demo_abi.inc"
#undef ABI_SIZE
#undef ABI_FIELD

/* Y la MISMA tabla, para construir el descriptor de ejecucion. */
static const abi_field demo_fields[] = {
#define ABI_SIZE(t, b)
#define ABI_FIELD(t, f, o, id) ABI_DESC_FIELD(t, f, id),
#include "demo_abi.inc"
#undef ABI_SIZE
#undef ABI_FIELD
};

#define DEMO_STRUCT_ID 1u

static const abi_struct demo_desc = {DEMO_STRUCT_ID, (u32)sizeof(demo_sample),
                                     (u32)COUNT_OF(demo_fields), demo_fields};

int main(void) {
    printf("== the three self-describing layers ==\n");

    /* --- Capa 1: la cabecera de registro --------------------------------- */
    {
        /* Cuatro bytes exactos: es lo que se paga por cada muestra, millones de
         * veces, asi que crecer aqui no es gratis. */
        CHECK(sizeof(abi_record) == 4);
        /* Y va la PRIMERA en un registro publicado, o no se podria saltar sin
         * conocer el resto. */
        CHECK(ABI_OFFSET_OF(demo_sample, head) == 0);

        /* Recorrer saltando lo desconocido, que es la propiedad que compra. */
        u8 buf[64];
        memset(buf, 0, sizeof(buf));

        abi_record a = {7, 24};   /* uno que conocemos */
        abi_record b = {999, 16}; /* uno de una version mas nueva */
        abi_record c = {7, 24};
        memcpy(buf + 0, &a, sizeof(a));
        memcpy(buf + 24, &b, sizeof(b));
        memcpy(buf + 40, &c, sizeof(c));

        u32 known = 0, skipped = 0, pos = 0;
        while (pos + sizeof(abi_record) <= 64) {
            abi_record h;
            memcpy(&h, buf + pos, sizeof(h));
            if (h.kind == ABI_KIND_NONE) break; /* memoria sin escribir */
            CHECK(h.length >= sizeof(abi_record));
            if (h.kind == 7)
                ++known;
            else
                ++skipped; /* no se entiende, pero se puede pasar de largo */
            pos += h.length;
        }
        CHECK(known == 2);
        CHECK(skipped == 1);
        /* El de longitud cero corta el recorrido: si `length` pudiera ser 0, un
         * registro corrupto seria un bucle infinito dentro del consumidor. */
        CHECK(pos == 64);
    }

    /* --- Capa 2: las estructuras dicen su tamano ------------------------- */
    {
        /* Mas grande de lo esperado es CORRECTO: el otro extremo es mas nuevo y
         * trae campos que aqui no se conocen.  Se leen los conocidos. */
        CHECK(abi_sized_ok(32, 24));
        CHECK(abi_sized_ok(24, 24));
        /* Mas pequeno no: faltarian campos que se iban a leer. */
        CHECK(!abi_sized_ok(16, 24));
        CHECK(!abi_sized_ok(0, 24));
    }

    /* --- Capa 3: el descriptor ------------------------------------------- */
    {
        CHECK(demo_desc.id == DEMO_STRUCT_ID);
        CHECK(demo_desc.size == sizeof(demo_sample));
        CHECK(demo_desc.field_count == 4);

        /* Lo que se comprueba de verdad: que el descriptor dice la verdad sobre
         * la estructura.  Si se escribiera a mano, esto es lo que se separaria. */
        CHECK(demo_fields[0].id == 1 && demo_fields[0].offset == 0);
        CHECK(demo_fields[1].id == 2 &&
              demo_fields[1].offset == ABI_OFFSET_OF(demo_sample, ip));
        CHECK(demo_fields[1].size == sizeof(u64));
        CHECK(demo_fields[2].id == 3 &&
              demo_fields[2].offset == ABI_OFFSET_OF(demo_sample, tid));
        CHECK(demo_fields[3].id == 4 &&
              demo_fields[3].offset == ABI_OFFSET_OF(demo_sample, cpu));
        CHECK(demo_fields[3].size == sizeof(u32));

        /* Ningun campo se sale de la estructura que dice describir. */
        for (u32 i = 0; i < demo_desc.field_count; ++i)
            CHECK(demo_fields[i].offset + demo_fields[i].size <=
                  demo_desc.size);

        /* Los identificadores son unicos: dos campos con el mismo id harian que
         * un consumidor validara el equivocado y se diera por satisfecho. */
        for (u32 i = 0; i < demo_desc.field_count; ++i)
            for (u32 j = i + 1; j < demo_desc.field_count; ++j)
                CHECK(demo_fields[i].id != demo_fields[j].id);

        /* Y el formato del propio descriptor no puede cambiar nunca: es lo
         * unico que no tiene como describirse a si mismo. */
        CHECK(sizeof(abi_field) == 12);
    }

    /* --- Como lo usaria un consumidor al conectar ------------------------ */
    {
        /* Busca por ID, no por posicion: la posicion cambia al anadir campos,
         * el ID no.  Ese es todo el sentido de que exista el ID. */
        u32 found = 0;
        for (u32 i = 0; i < demo_desc.field_count; ++i) {
            if (demo_fields[i].id == 2) { /* el que a este consumidor le importa */
                found = 1;
                CHECK(demo_fields[i].offset == ABI_OFFSET_OF(demo_sample, ip));
                CHECK(demo_fields[i].size == sizeof(u64));
            }
        }
        CHECK(found);

        /* Un campo que este consumidor no conoce: no esta, y eso no es un
         * error -- es que el otro extremo es mas antiguo. */
        u32 unknown = 0;
        for (u32 i = 0; i < demo_desc.field_count; ++i)
            if (demo_fields[i].id == 99) unknown = 1;
        CHECK(!unknown);
    }

    printf("--- %d checks, %d failures ---\n", g_checks, g_fails);
    return g_fails ? 1 : 0;
}
