/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file writer.h
 * @brief
 * \~english A text-writing cursor over the caller's buffer.
 * \~spanish Cursor de escritura de texto sobre el bufer del llamante.
 * \~
 *
 * \~english
 * WHY IT EXISTS SEPARATELY.  The PMU capabilities wrote it first for their
 * report, and now the CPUID and MSR dump needs it too.  Copying it would mean
 * having two, and two drift apart: one learns to say it overflowed and the other
 * does not, and the one that does not hands over a report cut in half that looks
 * whole.
 *
 * WHY IN A HEADER AND NOT A `.c`.  They are three-line functions on the path of
 * writing every single character.  The project builds with LTO on for a reason,
 * but `Debug` turns it off, and there a call per character across translation
 * units shows.  `static inline` leaves them the same in both.
 *
 * IT DEPENDS ON NOTHING FROM THE SYSTEM.  No `stdio`, no `snprintf`, no
 * allocation.  That is what lets the same code write from inside the kernel,
 * where there is none of the three.
 *
 * \~spanish
 * POR QUE EXISTE APARTE.  Lo escribieron primero las capacidades del PMU para su
 * informe, y ahora lo necesita tambien el volcado de CPUID y MSR.  Copiarlo
 * seria tener dos, y dos se separan: uno aprende a decir que se ha desbordado y
 * el otro no, y el que no lo diga entrega un informe cortado por la mitad que
 * parece entero.
 *
 * POR QUE EN UNA CABECERA Y NO EN UN `.c`.  Son funciones de tres lineas en el
 * camino de escribir cada caracter.  El proyecto construye con LTO encendido a
 * proposito, pero `Debug` lo apaga, y ahi una llamada por caracter entre
 * unidades de traduccion se nota.  `static inline` las deja igual en los dos.
 *
 * NO DEPENDE DE NADA DEL SISTEMA.  Ni `stdio`, ni `snprintf`, ni asignacion.  Es
 * lo que permite que el mismo codigo escriba desde el nucleo, donde no hay
 * ninguna de las tres.
 */

#ifndef VXP_COMMON_WRITER_H
#define VXP_COMMON_WRITER_H

#include "vxp_base.h"

/**
 * @brief
 * \~english A write cursor over the caller's buffer.
 * \~spanish Cursor de escritura sobre el bufer del llamante.
 * \~
 *
 * \~english
 * It carries its own overflow flag instead of truncating silently: a report cut
 * in half that does not say so is worse than one that is missing.
 *
 * \~spanish
 * Lleva su propio desbordamiento en vez de truncar en silencio: un informe
 * cortado por la mitad que no lo diga es peor que uno que falta.
 */
typedef struct writer {
    char *buf;    /**< \~english the caller's buffer; not owned \~spanish el bufer del llamante; no se posee \~ */
    usize cap;    /**< \~english how much fits \~spanish cuanto cabe \~ */
    usize len;    /**< \~english how much is written \~spanish cuanto se lleva escrito \~ */
    int overflow; /**< \~english non-zero if it did not all fit \~spanish distinto de cero si no cupo todo \~ */
} writer;

/** @brief
 *  \~english Gets the cursor ready over `buf`.
 *  \~spanish Deja el cursor listo sobre `buf`. \~ */
static inline void writer_init(writer *w, char *buf, usize cap) {
    w->buf = buf;
    w->cap = cap;
    w->len = 0;
    w->overflow = 0;
}

/** @brief
 *  \~english Appends one character.
 *  \~spanish Anade un caracter. \~ */
static inline void put_ch(writer *w, char c) {
    if (w->len + 1 > w->cap) {
        w->overflow = 1;
        return;
    }
    w->buf[w->len] = c;
    w->len += 1;
}

/** @brief
 *  \~english Appends a nul-terminated string.
 *  \~spanish Anade una cadena terminada en nul. \~ */
static inline void put_str(writer *w, const char *s) {
    usize i = 0;
    while (s[i] != 0) {
        put_ch(w, s[i]);
        i += 1;
    }
}

/** @brief
 *  \~english Appends `n` spaces, to line columns up.
 *  \~spanish Anade `n` espacios, para cuadrar columnas. \~ */
static inline void put_pad(writer *w, int n) {
    while (n > 0) {
        put_ch(w, ' ');
        n -= 1;
    }
}

/** @brief
 *  \~english Appends an unsigned integer in decimal.
 *  \~spanish Anade un entero sin signo en decimal. \~ */
static inline void put_u64(writer *w, u64 v) {
    char tmp[20];
    int n = 0;
    if (v == 0) {
        put_ch(w, '0');
        return;
    }
    /* \~english Digits come out backwards -- the remainder gives the last one
     * first -- so they are collected and then poured out in reverse.
     * \~spanish Las cifras salen al reves -- el resto da la ultima primero --,
     * asi que se recogen y luego se vuelcan en orden inverso. \~ */
    while (v > 0 && n < 20) {
        tmp[n] = (char)('0' + (int)(v % 10));
        v /= 10;
        n += 1;
    }
    while (n > 0) {
        n -= 1;
        put_ch(w, tmp[n]);
    }
}

/**
 * @brief
 * \~english Appends an integer in hexadecimal with `digits` digits and a `0x`
 *           prefix.
 * \~spanish Anade un entero en hexadecimal con `digits` cifras y prefijo `0x`.
 * \~
 *
 * \~english
 * Fixed width on purpose: MSR values get compared by eye across cores, and with
 * a variable width the columns shift and the eye stops being useful.
 *
 * \~spanish
 * Ancho fijo a proposito: los valores de MSR se comparan a ojo entre nucleos, y
 * con ancho variable las columnas se descolocan y el ojo deja de servir.
 */
static inline void put_hex(writer *w, u64 v, int digits) {
    static const char d[] = "0123456789ABCDEF";
    int i;
    put_str(w, "0x");
    for (i = digits - 1; i >= 0; --i) {
        put_ch(w, d[(v >> (i * 4)) & 0xFu]);
    }
}

#endif /* VXP_COMMON_WRITER_H */
