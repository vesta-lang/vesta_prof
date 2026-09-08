/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file user/vxarch.c
 * @brief
 * \~english The `.vxarch` parser: three kinds of line and nothing else.
 * \~spanish El analizador de `.vxarch`: tres clases de linea y nada mas.
 * \~
 *
 * \~english
 * THE SHAPE OF THE FILE, which is what makes a parser this small possible:
 *
 * @code
 *   vxarch 1 name=intel-alderlake-p family=intel ... classes=1616 mapped=15926
 *   ports: 0=p23A 1=p06 2=p01 ...
 *   # a comment
 *   class 0|0.33|1|0|0|-1.00|-|0*1.00
 *   class 1|0.50|2|0|0|-1.00|0:0:0:1.00,1:0:3:6.00:ub|0*1.00,1*1.00
 *   ...
 *   340|17
 *   1033|205
 * @endcode
 *
 * A line that starts with `class` describes a cost; one that is two numbers
 * separated by a bar maps a form to one of those costs.  Everything else --
 * header, port legend, comments -- is skipped, which is also what makes the
 * reader survive a file that grows a section it does not know.
 *
 * WHAT IT DOES NOT SURVIVE, on purpose, is a change of VERSION.  A new field in
 * the middle of a `class` line would still parse, and every cost would come out
 * shifted by one column: plausible numbers, wrong ones, nothing saying so.  So
 * the header is checked first and an unknown version is refused outright.
 *
 * NO STANDARD LIBRARY, same rule as the rest of the tree.  Parsing a decimal to
 * thousandths is fifteen lines and needs nothing; pulling in `strtod` to read
 * numbers that never have more than two decimals would be more dependency than
 * code.
 *
 * \~spanish
 * LA FORMA DEL FICHERO, que es lo que hace posible un analizador tan pequeno:
 * ver el ejemplo de arriba.
 *
 * Una linea que empieza por `class` describe un coste; una que son dos numeros
 * separados por barra asigna una forma a uno de esos costes.  Todo lo demas
 * -- cabecera, leyenda de puertos, comentarios -- se salta, que es ademas lo que
 * hace que el lector sobreviva a un fichero que anada una seccion que no conoce.
 *
 * A LO QUE NO SOBREVIVE, a proposito, es a un cambio de VERSION.  Un campo nuevo
 * en medio de una linea `class` se seguiria interpretando, y todos los costes
 * saldrian corridos una columna: numeros plausibles, equivocados, y nada que lo
 * diga.  Asi que la cabecera se comprueba primero y una version desconocida se
 * rechaza sin mas.
 *
 * SIN BIBLIOTECA ESTANDAR, misma regla que el resto del arbol.  Interpretar un
 * decimal a milesimas son quince lineas y no necesita nada; traerse `strtod`
 * para leer numeros que nunca tienen mas de dos decimales seria mas dependencia
 * que codigo.
 */

#include "vxarch.h"

/** @brief
 *  \~english Is `c` a digit?
 *  \~spanish ¿Es `c` un digito? \~ */
static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

/**
 * @brief
 * \~english Reads a decimal with up to three places, as thousandths.
 * \~spanish Lee un decimal de hasta tres cifras, en milesimas.
 * \~
 *
 * @param p   \~english where to read; advanced past the number \~spanish por donde va; se avanza pasado el numero \~
 * @param end \~english the limit \~spanish el limite \~
 * @return \~english the value times a thousand; a negative number comes back as 0 \~spanish el valor por mil; un numero negativo vuelve como 0 \~
 *
 * \~english
 * NEGATIVES COME BACK AS ZERO, and it is not laziness: in this file the only
 * negative is `-1.00` in the divider-cycles column, and it means "does not
 * apply" rather than a quantity.  Storing minus one as an unsigned would give a
 * gigantic number that then behaves like a very expensive instruction.
 *
 * \~spanish
 * LOS NEGATIVOS VUELVEN COMO CERO, y no es dejadez: en este fichero el unico
 * negativo es `-1.00` en la columna de ciclos del divisor, y quiere decir "no
 * aplica" y no una cantidad.  Guardar menos uno en un sin signo daria un numero
 * gigantesco que despues se comporta como una instruccion carisima.
 */
static u32 read_milli(const char **p, const char *end) {
    u32 whole = 0;
    u32 frac = 0;
    u32 scale = 1000;
    int negative = 0;

    if (*p < end && **p == '-') {
        negative = 1;
        *p += 1;
    }
    while (*p < end && is_digit(**p)) {
        whole = whole * 10u + (u32)(**p - '0');
        *p += 1;
    }
    if (*p < end && **p == '.') {
        *p += 1;
        while (*p < end && is_digit(**p)) {
            if (scale > 1) {
                scale /= 10u;
                frac += (u32)(**p - '0') * scale;
            }
            *p += 1;
        }
    }
    if (negative) {
        return 0;
    }
    return whole * 1000u + frac;
}

/** @brief
 *  \~english Reads a plain integer.
 *  \~spanish Lee un entero a secas. \~ */
static u32 read_u32(const char **p, const char *end) {
    u32 v = 0;
    while (*p < end && is_digit(**p)) {
        v = v * 10u + (u32)(**p - '0');
        *p += 1;
    }
    return v;
}

/** @brief
 *  \~english Skips to just past the next bar, or to the end of the line.
 *  \~spanish Salta hasta pasada la siguiente barra, o al final de la linea. \~ */
static void skip_field(const char **p, const char *end) {
    while (*p < end && **p != '|') {
        *p += 1;
    }
    if (*p < end) {
        *p += 1;
    }
}

/**
 * @brief
 * \~english The longest latency of a class, out of all its edges.
 * \~spanish La latencia mas larga de una clase, de entre todas sus aristas.
 * \~
 *
 * \~english
 * A class does not have ONE latency: it has one per pair of operands -- from
 * input X to output Y -- because they are not all the same.  What a cost model
 * uses for the critical path is the longest, so that is what is kept.
 *
 * The field is `so:to:kind:cycles[:ub]` per edge, separated by commas, and the
 * cycles are the third colon.  A `-` means the class has no timed edges.
 *
 * \~spanish
 * Una clase no tiene UNA latencia: tiene una por pareja de operandos -- de la
 * entrada X a la salida Y -- porque no son todas iguales.  Lo que un modelo de
 * coste usa para el camino critico es la mas larga, asi que es la que se guarda.
 *
 * El campo es `so:to:kind:ciclos[:ub]` por arista, separadas por comas, y los
 * ciclos van tras los dos puntos terceros.  Un `-` quiere decir que la clase no
 * tiene ninguna arista cronometrada.
 */
static u32 read_latency_max(const char **p, const char *end) {
    u32 best = 0;

    while (*p < end && **p != '|' && **p != '\n') {
        u32 colons = 0;
        /* \~english Walk to the third colon, which is where the cycles are.
         * \~spanish Avanzar hasta los dos puntos terceros, que es donde estan
         * los ciclos. \~ */
        while (*p < end && **p != ',' && **p != '|' && **p != '\n') {
            if (**p == ':') {
                colons += 1;
                *p += 1;
                if (colons == 3) {
                    u32 v = read_milli(p, end);
                    if (v > best) {
                        best = v;
                    }
                }
                continue;
            }
            *p += 1;
        }
        if (*p < end && **p == ',') {
            *p += 1;
        }
    }
    return best;
}

/** @brief
 *  \~english Does `text` start with `lit`?
 *  \~spanish ¿Empieza `text` por `lit`? \~ */
static int starts_with(const char *text, const char *end, const char *lit) {
    usize i = 0;
    while (lit[i] != 0) {
        if (text + i >= end || text[i] != lit[i]) {
            return 0;
        }
        i += 1;
    }
    return 1;
}

status vxarch_parse(const char *text, usize len, vxarch *out) {
    const char *p = text;
    const char *end = text + len;
    u32 i;

    if (text == 0 || out == 0 || out->classes == 0 || out->form_class == 0) {
        return ERR_INVALID;
    }

    out->class_count = 0;
    out->form_count = 0;
    for (i = 0; i < VXARCH_NAME_MAX; ++i) {
        out->name[i] = 0;
    }
    for (i = 0; i < VXARCH_HASH_MAX; ++i) {
        out->xml_sha256[i] = 0;
    }
    for (i = 0; i < out->form_cap; ++i) {
        out->form_class[i] = VXARCH_NO_CLASS;
    }

    /* --- \~english the header, and the version comes first
     *     \~spanish la cabecera, y la version va primero \~ --------------- */
    if (!starts_with(p, end, "vxarch ")) {
        return ERR_INVALID;
    }
    p += 7;
    if (read_u32(&p, end) != VXARCH_VERSION) {
        /* \~english A format this reader does not know.  See the file comment:
         * carrying on would give plausible costs from the wrong columns.
         * \~spanish Un formato que este lector no conoce.  Ver el comentario del
         * fichero: seguir daria costes plausibles de las columnas
         * equivocadas. \~ */
        return ERR_INVALID;
    }
    /* \~english The name, for whoever wants to say which part these numbers
     * describe.  A report that gives costs without saying whose they are cannot
     * be checked by anybody.
     * \~spanish El nombre, para quien quiera decir de que pieza son estos
     * numeros.  Un informe que da costes sin decir de quien son no lo puede
     * comprobar nadie. \~ */
    while (p < end && *p != '\n') {
        if (starts_with(p, end, "name=")) {
            p += 5;
            i = 0;
            while (p < end && *p != ' ' && *p != '\n' &&
                   i + 1 < VXARCH_NAME_MAX) {
                out->name[i] = *p;
                i += 1;
                p += 1;
            }
            continue;
        }
        /* \~english And the hash of the data it was built from, which is what
         * lets anybody who wrote down a form number find out that the numbers
         * have moved under them.  See `VXARCH_HASH_MAX`.
         * \~spanish Y el resumen de los datos de los que salio, que es lo que
         * permite a quien apunto un numero de forma enterarse de que los numeros
         * se le han movido debajo.  Ver `VXARCH_HASH_MAX`. \~ */
        if (starts_with(p, end, "xml_sha256=")) {
            p += 11;
            i = 0;
            while (p < end && *p != ' ' && *p != '\n' &&
                   i + 1 < VXARCH_HASH_MAX) {
                out->xml_sha256[i] = *p;
                i += 1;
                p += 1;
            }
            continue;
        }
        p += 1;
    }
    while (p < end && *p != '\n') {
        p += 1;
    }

    /* --- \~english the body \~spanish el cuerpo \~ ---------------------- */
    while (p < end) {
        const char *line;

        if (*p == '\n') {
            p += 1;
            continue;
        }
        line = p;
        while (p < end && *p != '\n') {
            p += 1;
        }

        if (starts_with(line, p, "class ")) {
            const char *q = line + 6;
            u32 cid = read_u32(&q, p);
            vxarch_class c;

            if (q < p && *q == '|') {
                q += 1;
            }
            c.recip_tp = read_milli(&q, p);
            skip_field(&q, p);
            c.uops = (u16)read_u32(&q, p);
            skip_field(&q, p);
            c.flags = (read_u32(&q, p) != 0) ? VXARCH_MICROCODED : 0u;
            skip_field(&q, p);
            if (read_u32(&q, p) != 0) {
                c.flags |= VXARCH_MACRO_FUSIBLE;
            }
            skip_field(&q, p);
            skip_field(&q, p); /* \~english divider cycles \~spanish ciclos del divisor \~ */
            c.latency = read_latency_max(&q, p);
            c._pad = 0;

            if (cid >= out->class_cap) {
                return ERR_NOSPACE;
            }
            out->classes[cid] = c;
            if (cid + 1u > out->class_count) {
                out->class_count = cid + 1u;
            }
        } else if (is_digit(*line)) {
            const char *q = line;
            u32 form = read_u32(&q, p);
            u32 cid;

            if (q >= p || *q != '|') {
                continue; /* \~english not a mapping line \~spanish no es una linea de mapeo \~ */
            }
            q += 1;
            cid = read_u32(&q, p);
            if (form >= out->form_cap) {
                return ERR_NOSPACE;
            }
            if (cid >= out->class_cap) {
                return ERR_NOSPACE;
            }
            out->form_class[form] = (u16)cid;
            out->form_count += 1;
        }
        /* \~english Anything else -- the port legend, a comment, a section this
         * reader does not know -- is skipped instead of being an error: a format
         * that grows should not break a reader that does not need the new part.
         * \~spanish Cualquier otra cosa -- la leyenda de puertos, un comentario,
         * una seccion que este lector no conozca -- se salta en vez de ser un
         * error: un formato que crece no deberia romper a un lector que no
         * necesita la parte nueva. \~ */
    }

    return OK;
}

const vxarch_class *vxarch_cost(const vxarch *a, u32 form) {
    u16 cid;

    if (a == 0 || form >= a->form_cap) {
        return 0;
    }
    cid = a->form_class[form];
    if (cid == VXARCH_NO_CLASS || cid >= a->class_count) {
        return 0;
    }
    return &a->classes[cid];
}
