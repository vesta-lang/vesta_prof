/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file opaque_probe.c
 * @brief La implementacion del modulo de juguete.  Se compila como C.
 *
 * Es la mitad que importa del idioma: aqui esta la forma real, y fuera solo se
 * conoce el tamano.
 */

#include "opaque_probe.h"

/* La estructura de verdad.  No sale de este fichero. */
struct probe_impl {
    u32 value;
    u32 limit;
    u32 magic; /* para distinguir "sin inicializar" de "ya terminado" */
};

/* Un valor que no sale por casualidad de memoria a cero ni de basura. */
#define PROBE_MAGIC 0x50524F42u /* 'PROB' */

/* Que la estructura real quepa en lo publicado, comprobado AL COMPILAR.
 *
 * Sin esto, anadir un campo no da un error: da escrituras fuera del
 * almacenamiento que el llamante reservo, y el sintoma aparece lejos. */
OPAQUE_FITS(struct probe_impl, probe);

static struct probe_impl *impl(probe *p) {
    return (struct probe_impl *)(void *)p->opaque;
}
static const struct probe_impl *impl_const(const probe *p) {
    return (const struct probe_impl *)(const void *)p->opaque;
}

status probe_init(probe *p, u32 limit) {
    /* La comprobacion va en la FRONTERA, que es aqui: lo que entra de fuera del
     * modulo.  Dentro se confia en el contrato. */
    if (p == 0 || limit == 0) return ERR_INVALID;
    struct probe_impl *s = impl(p);
    s->value = 0;
    s->limit = limit;
    s->magic = PROBE_MAGIC;
    return OK;
}

status probe_add(probe *p, u32 n) {
    if (p == 0) return ERR_INVALID;
    struct probe_impl *s = impl(p);
    if (s->magic != PROBE_MAGIC) return ERR_STATE;
    /* Se comprueba ANTES de sumar, no despues: comprobar el desbordamiento
     * mirando si el resultado quedo mas pequeno es comportamiento indefinido
     * para con signo y una carrera con el optimizador para el resto. */
    if (n > s->limit - s->value) return ERR_NOSPACE;
    s->value += n;
    return OK;
}

status probe_value(const probe *p, u32 *out) {
    if (p == 0 || out == 0) return ERR_INVALID;
    const struct probe_impl *s = impl_const(p);
    if (s->magic != PROBE_MAGIC) return ERR_STATE;
    *out = s->value;
    return OK;
}

void probe_fini(probe *p) {
    if (p == 0) return;
    /* Se borra la marca para que usarlo despues devuelva ERR_STATE en vez
     * de leer un valor con pinta razonable. */
    impl(p)->magic = 0;
}

/* --- forma B: el tamano se sabe en ejecucion ----------------------------- */

struct probe_log_impl {
    u32 *data;  /* la region que aporto el llamante */
    u32 slots;  /* cuantas caben */
    u32 used;   /* cuantas hay */
    u32 magic;
};

#define PROBE_LOG_MAGIC 0x504C4F47u /* 'PLOG' */

OPAQUE_FITS(struct probe_log_impl, probe_log);

static struct probe_log_impl *log_impl(probe_log *g) {
    return (struct probe_log_impl *)(void *)g->opaque;
}
static const struct probe_log_impl *log_impl_const(const probe_log *g) {
    return (const struct probe_log_impl *)(const void *)g->opaque;
}

usize probe_log_bytes_needed(u32 slots) {
    return (usize)slots * sizeof(u32);
}

status probe_log_init(probe_log *g, void *storage, usize bytes, u32 slots) {
    if (g == 0 || storage == 0 || slots == 0) return ERR_INVALID;
    /* SE COMPRUEBA lo que dan, no se confia.  Si esto se saltara, un bufer
     * corto se convertiria en escrituras fuera de sitio -- y el llamante que se
     * equivoca al calcular el tamano es el caso normal, no el raro. */
    if (bytes < probe_log_bytes_needed(slots)) return ERR_NOSPACE;

    struct probe_log_impl *s = log_impl(g);
    s->data = (u32 *)storage;
    s->slots = slots;
    s->used = 0;
    s->magic = PROBE_LOG_MAGIC;
    return OK;
}

status probe_log_push(probe_log *g, u32 value) {
    if (g == 0) return ERR_INVALID;
    struct probe_log_impl *s = log_impl(g);
    if (s->magic != PROBE_LOG_MAGIC) return ERR_STATE;
    if (s->used >= s->slots) return ERR_NOSPACE;
    s->data[s->used++] = value;
    return OK;
}

status probe_log_count(const probe_log *g, u32 *out) {
    if (g == 0 || out == 0) return ERR_INVALID;
    const struct probe_log_impl *s = log_impl_const(g);
    if (s->magic != PROBE_LOG_MAGIC) return ERR_STATE;
    *out = s->used;
    return OK;
}

status probe_log_at(const probe_log *g, u32 index, u32 *out) {
    if (g == 0 || out == 0) return ERR_INVALID;
    const struct probe_log_impl *s = log_impl_const(g);
    if (s->magic != PROBE_LOG_MAGIC) return ERR_STATE;
    /* El limite se comprueba contra `used`, no contra `slots`: leer una ranura
     * reservada pero sin escribir devolveria lo que hubiera en la memoria del
     * llamante. */
    if (index >= s->used) return ERR_INVALID;
    *out = s->data[index];
    return OK;
}

void probe_log_fini(probe_log *g) {
    if (g == 0) return;
    struct probe_log_impl *s = log_impl(g);
    s->magic = 0;
    /* Se suelta la region tambien: el modulo no la reservo, pero seguir
     * apuntando a memoria del llamante despues de terminar es como se acaba
     * escribiendo en algo que ya se reutilizo. */
    s->data = 0;
    s->slots = 0;
    s->used = 0;
}

/* --- lo que ve el lado de C, para contrastar con el de C++ --------------- */

usize probe_sizeof_storage(void) {
    return sizeof(probe);
}
usize probe_alignof_storage(void) {
    return ALIGNOF(probe);
}
usize probe_sizeof_u64(void) {
    return sizeof(u64);
}
usize probe_sizeof_size(void) {
    return sizeof(usize);
}
int probe_ok_value(void) {
    return (int)OK;
}
