/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file writer.h
 * @brief Cursor de escritura de texto sobre el bufer del llamante.
 *
 * POR QUE EXISTE APARTE.  Lo escribieron primero las capacidades del PMU para
 * su informe, y ahora lo necesita tambien el volcado de CPUID y MSR.  Copiarlo
 * seria tener dos, y dos se separan: uno aprende a decir que se ha desbordado y
 * el otro no, y el que no lo diga entrega un informe cortado por la mitad que
 * parece entero.
 *
 * POR QUE EN UNA CABECERA Y NO EN UN `.c`.  Son funciones de tres lineas en el
 * camino de escribir cada caracter.  El proyecto construye con LTO encendido a
 * proposito, pero `Debug` lo apaga, y ahi una llamada por caracter entre
 * unidades de traduccion se nota.  `static inline` las deja igual en los dos.
 *
 * NO DEPENDE DE NADA DEL SISTEMA.  Ni `stdio`, ni `snprintf`, ni asignacion.
 * Es lo que permite que el mismo codigo escriba desde el nucleo, donde no hay
 * ninguna de las tres.
 */

#ifndef VXP_COMMON_WRITER_H
#define VXP_COMMON_WRITER_H

#include "vxp_base.h"

/**
 * @brief Cursor de escritura sobre el bufer del llamante.
 *
 * Lleva su propio desbordamiento en vez de truncar en silencio: un informe
 * cortado por la mitad que no lo diga es peor que uno que falta.
 */
typedef struct writer {
    char *buf;   /**< el bufer del llamante; no se posee            */
    usize cap;   /**< cuanto cabe                                   */
    usize len;   /**< cuanto se lleva escrito                       */
    int overflow; /**< distinto de cero si no cupo todo             */
} writer;

/** @brief Deja el cursor listo sobre `buf`. */
static inline void writer_init(writer *w, char *buf, usize cap) {
    w->buf = buf;
    w->cap = cap;
    w->len = 0;
    w->overflow = 0;
}

/** @brief Anade un caracter. */
static inline void put_ch(writer *w, char c) {
    if (w->len + 1 > w->cap) {
        w->overflow = 1;
        return;
    }
    w->buf[w->len] = c;
    w->len += 1;
}

/** @brief Anade una cadena terminada en nul. */
static inline void put_str(writer *w, const char *s) {
    usize i = 0;
    while (s[i] != 0) {
        put_ch(w, s[i]);
        i += 1;
    }
}

/** @brief Anade `n` espacios, para cuadrar columnas. */
static inline void put_pad(writer *w, int n) {
    while (n > 0) {
        put_ch(w, ' ');
        n -= 1;
    }
}

/** @brief Anade un entero sin signo en decimal. */
static inline void put_u64(writer *w, u64 v) {
    char tmp[20];
    int n = 0;
    if (v == 0) {
        put_ch(w, '0');
        return;
    }
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
 * @brief Anade un entero en hexadecimal con `digits` cifras y prefijo `0x`.
 *
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
