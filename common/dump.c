/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file dump.c
 * @brief Volcado de CPUID y del catalogo de MSR, recorriendo las tablas.
 *
 * Aqui no hay ni un nombre de campo ni un numero de bit: todo sale de las
 * tablas generadas de los manuales.  Ese es el punto -- un volcado escrito a
 * mano envejece con cada revision del manual y nadie se entera, porque un campo
 * que falta no se ve.
 *
 * NO DEPENDE DEL SISTEMA.  Ni `stdio` ni asignacion: escribe en el bufer del
 * llamante y la lectura de MSR entra por puntero a funcion.  Es lo que permite
 * que el mismo codigo corra dentro del driver y fuera.
 */

#include "dump.h"

#include "cpu_tables.h"
#include "cpuid_intrin.h"
#include "cpuid_table.h"
#include "msr_table.h"
#include "writer.h"

/** @brief El nombre del registro, para la columna. */
static const char *reg_name(u8 reg) {
    static const char *const names[4] = {"EAX", "EBX", "ECX", "EDX"};
    return names[reg & 3u];
}

/** @brief Extrae el campo `f` del valor ya leido `v`. */
static u32 field_value(const cpuid_field *f, u32 v) {
    /* El desplazamiento de 32 no esta definido en C, asi que un campo que ocupa
     * el registro entero se trata aparte en vez de fiarlo a que la maquina haga
     * lo razonable.  En x86 `shr` de 32 no desplaza NADA -- usa los cinco bits
     * bajos del contador --, con lo que la mascara saldria cero y el campo
     * entero se leeria como cero. */
    if (f->width >= 32) {
        return v >> f->lo;
    }
    return (v >> f->lo) & ((1u << f->width) - 1u);
}

/** @brief El registro que toca del resultado de CPUID. */
static u32 reg_of(const cpuid_regs *r, u8 reg) {
    switch (reg & 3u) {
    case CPUID_REG_EAX:
        return r->eax;
    case CPUID_REG_EBX:
        return r->ebx;
    case CPUID_REG_ECX:
        return r->ecx;
    default:
        return r->edx;
    }
}

/** @brief Un campo de texto y su coma. */
static void put_cell(writer *w, const char *s) {
    put_str(w, s);
    put_ch(w, ',');
}

/** @brief Un entero decimal y su coma. */
static void put_dec_cell(writer *w, u64 v) {
    put_u64(w, v);
    put_ch(w, ',');
}

/** @brief Un entero hexadecimal con prefijo y su coma. */
static void put_hex_cell(writer *w, u64 v, int digits) {
    put_hex(w, v, digits);
    put_ch(w, ',');
}

/**
 * @brief ¿Anuncia la pieza esta hoja?
 *
 * Preguntar por una hoja mas alta que el maximo NO falla: CPUID devuelve los
 * valores de otra.  Volcar eso seria producir un informe entero de datos
 * inventados con toda la pinta de ser buenos, que es peor que no volcarlos.
 */
static int leaf_supported(u32 leaf, u32 max_basic, u32 max_ext) {
    if (leaf >= 0x80000000u) {
        return leaf <= max_ext;
    }
    return leaf <= max_basic;
}

status cpuid_dump(u32 cpu, int header, char *buf, usize cap, usize *written) {
    writer w;
    const cpuid_table *t;
    const cpu_tables *v = cpu_tables_detect();
    u32 max_basic = cpuid_max_basic_leaf();
    u32 max_ext = cpuid_max_extended_leaf();
    u32 i;
    /* Lo ultimo consultado, para no repetir la instruccion por cada campo: una
     * hoja trae entre uno y treinta y dos campos, y CPUID es serializante. */
    u32 have_leaf = 0;
    u32 have_sub = 0;
    int have = 0;
    cpuid_regs r;

    writer_init(&w, buf, cap);
    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;

    /* La tabla la elige la FIRMA de la pieza, no un `if` sobre fabricantes
     * cableados aqui: ver `cpu_tables.c`.  Si no coincide ninguna, no se vuelca
     * -- volcar con la tabla de otro fabricante daria nombres de campo que no
     * corresponden a los bits leidos, y eso no falla, miente. */
    if (v == 0) {
        *written = 0;
        return ERR_UNSUPPORTED;
    }
    t = v->cpuid;

    /* La cabecera, y nada mas.  No hay preambulo con la hoja maxima ni con el
     * fabricante porque seria REDUNDANTE: `MAX_LEAF` es CPUID.0:EAX[31:0] y los
     * `VENDOR_ID_*` son CPUID.0:EBX/ECX/EDX, o sea que ya son filas.  Un
     * preambulo, ademas, romperia el formato para todo lo que lea CSV. */
    if (header) {
        put_str(&w, "cpu,leaf,subleaf,reg,lo,width,name,reserved,value\n");
    }

    for (i = 0; i < t->count; ++i) {
        const cpuid_field *f = &t->fields[i];
        u32 sub = (f->subleaf == CPUID_NO_SUBLEAF) ? 0u : (u32)f->subleaf;

        /* Lo que esta pieza NO tiene no sale.  Y no se consulta: una hoja por
         * encima del maximo que la pieza anuncia no da error, devuelve los
         * valores de otra hoja, y eso se leeria como un dato bueno. */
        if (!leaf_supported(f->leaf, max_basic, max_ext)) {
            continue;
        }
        if (!have || f->leaf != have_leaf || sub != have_sub) {
            cpuid_query(f->leaf, sub, &r);
            have_leaf = f->leaf;
            have_sub = sub;
            have = 1;
        }

        /* Las hojas y los valores en hexadecimal, las posiciones de bit en
         * decimal.  No es una eleccion nuestra: es como los escribe el manual,
         * y asi lo que sale del volcado se busca alli tal cual. */
        put_dec_cell(&w, (u64)cpu);
        put_hex_cell(&w, (u64)f->leaf, 8);
        if (f->subleaf != CPUID_NO_SUBLEAF) {
            put_hex(&w, (u64)f->subleaf, 2);
        }
        /* Celda VACIA cuando no hay subhoja, que no es lo mismo que la subhoja
         * cero: `Fn8000_001D_x[N:0]` es "una por cada nivel de cache". */
        put_ch(&w, ',');
        put_cell(&w, reg_name(f->reg));
        put_dec_cell(&w, (u64)f->lo);
        put_dec_cell(&w, (u64)f->width);
        put_cell(&w, cpuid_field_name(t, f));
        put_dec_cell(&w, (f->flags & CPUID_FIELD_RESERVED) ? 1u : 0u);
        put_hex(&w, (u64)field_value(f, reg_of(&r, f->reg)),
                (f->width + 3) / 4);
        put_ch(&w, '\n');
    }

    *written = w.len;
    return w.overflow ? ERR_NOSPACE : OK;
}

/**
 * @brief ¿Se cumple la condicion de CPUID que documenta este registro?
 *
 * Un campo de un bit se cumple si el bit esta puesto.  Uno de varios -- el
 * numero de contadores, por ejemplo -- se toma como cumplido si NO es cero, que
 * es lo unico que el manual permite deducir aqui: dice CUANTOS hay, no cual de
 * ellos corresponde a esta direccion.
 *
 * Y ESO SE PASA DE LARGO, no se queda corto.  Medido: `CPUID.0AH:EAX[15:8]`
 * vale 6 en esta pieza, asi que `IA32_PMC8` pasa una puerta que dice que hay
 * seis contadores.  De los 42 registros que fallaron en la primera corrida, 36
 * son de esta forma.  Aqui decia lo contrario, y era falso.
 *
 * NO SE ARREGLA ADIVINANDO.  Comparar el indice contra la cuenta exigiria saber
 * el indice, y no esta en la tabla: habria que sacarlo del NOMBRE a base de
 * manipular cadenas, que es adivinar con otro nombre.
 *
 * La puerta es una HEURISTICA sacada de prosa adyacente, con tres modos de
 * fallar ya conocidos: se pasa de largo con los indexados, no distingue "existe"
 * de "se puede leer" -- hay registros de solo escritura, y el manual los marca
 * `(WO)` --, y no captura las condiciones compuestas que el manual escribe con
 * `&&`.  Sirve para no intentar lo que seguro que no esta; la verdad la da el
 * guarda, que convierte "creo que existe" en "lo intente, y esto paso".
 */
static int gate_satisfied(const msr_reg *m, u32 max_basic, u32 max_ext) {
    cpuid_regs r;
    u32 v;
    u32 sub = (m->gate_subleaf == CPUID_NO_SUBLEAF) ? 0u
                                                    : (u32)m->gate_subleaf;
    if (!leaf_supported(m->gate_leaf, max_basic, max_ext)) {
        return 0;
    }
    cpuid_query(m->gate_leaf, sub, &r);
    v = reg_of(&r, m->gate_reg);
    if (m->gate_width >= 32) {
        return (v >> m->gate_lo) != 0u;
    }
    return ((v >> m->gate_lo) & ((1u << m->gate_width) - 1u)) != 0u;
}

/**
 * @brief Las dos ultimas celdas de una fila de MSR: el estado y el valor.
 *
 * @param ok_state que poner en `state` cuando la lectura sale bien.  No es
 *                 siempre "ok": un registro sin puerta documentada que SI se
 *                 pudo leer sigue siendo un registro sin puerta, y perder ese
 *                 matiz seria decir que el manual documenta algo que no
 *                 documenta.
 */
static void put_msr_value(writer *w, msr_value v, const char *ok_state) {
    if (v.rc == OK) {
        put_cell(w, ok_state);
        put_hex(w, v.value, 16);
    } else if (v.rc == ERR_FAULT) {
        /* El procesador la rechazo.  Con guarda esto es INFORMACION -- dice que
         * el registro no existe en esta pieza --, no un fallo. */
        put_str(w, "faulted,");
    } else {
        put_str(w, "not_read,");
    }
    put_ch(w, '\n');
}

status msr_dump(u32 cpu, int header, int guarded, msr_read_fn read, void *ctx,
                char *buf, usize cap, usize *written) {
    writer w;
    const msr_table *t;
    const cpu_tables *v = cpu_tables_detect();
    u32 max_basic = cpuid_max_basic_leaf();
    u32 max_ext = cpuid_max_extended_leaf();
    u32 i;

    writer_init(&w, buf, cap);
    /* Misma regla que en CPUID: la tabla la elige la firma de la pieza. */
    if (v == 0) {
        *written = 0;
        return ERR_UNSUPPORTED;
    }
    t = v->msr;

    /* `state` dice POR QUE una fila no trae valor, y por eso es una columna y
     * no una frase: asi se puede filtrar.  Los cuatro valores no son matices
     * del mismo hecho, son cuatro hechos distintos:
     *
     *   no_gate      el manual no documenta la condicion al lado.  NO dice que
     *                el registro no exista: dice que por esta via no se sabe
     *   unsatisfied  hay condicion, y esta pieza no la cumple
     *   no_reader    la cumple, pero quien vuelca no puede leer -- modo usuario
     *   ok           leido, y el valor esta en la ultima columna
     *   faulted      se intento y el procesador la rechazo */
    if (header) {
        put_str(&w,
                "cpu,addr,name,indexed,gate_leaf,gate_subleaf,gate_reg,gate_lo,"
                "gate_width,state,value\n");
    }

    for (i = 0; i < t->count; ++i) {
        const msr_reg *m = &t->regs[i];

        put_dec_cell(&w, (u64)cpu);
        put_hex_cell(&w, (u64)m->addr, 8);
        put_cell(&w, msr_reg_name(t, m));
        /* La direccion es una base y el indice se suma.  Va en columna propia
         * porque quien procese el volcado tiene que ver que ahi hay una familia
         * entera y no un registro suelto. */
        put_dec_cell(&w, (m->flags & MSR_REG_INDEXED) ? 1u : 0u);

        if (!msr_reg_has_gate(m)) {
            /* Las cinco celdas de la puerta van vacias, que es lo que
             * corresponde a "no aplica" en CSV.
             *
             * Sin puerta documentada la lectura se INTENTA IGUAL, porque el
             * lector puede traer guarda: no saber si un registro existe no es
             * razon para no mirarlo, mientras mirar sea seguro.  Quien no
             * tenga guarda pone `guarded` a cero y aqui no se toca nada. */
            if (read == 0 || !guarded) {
                put_str(&w, ",,,,,no_gate,\n");
                continue;
            }
            put_str(&w, ",,,,,");
            put_msr_value(&w, read(m->addr, ctx), "no_gate");
            continue;
        }
        put_hex_cell(&w, (u64)m->gate_leaf, 2);
        if (m->gate_subleaf != CPUID_NO_SUBLEAF) {
            put_hex(&w, (u64)m->gate_subleaf, 2);
        }
        put_ch(&w, ',');
        put_cell(&w, reg_name(m->gate_reg));
        put_dec_cell(&w, (u64)m->gate_lo);
        put_dec_cell(&w, (u64)m->gate_width);

        if (!gate_satisfied(m, max_basic, max_ext)) {
            put_str(&w, "unsatisfied,\n");
            continue;
        }
        if (read == 0) {
            put_str(&w, "no_reader,\n");
            continue;
        }
        put_msr_value(&w, read(m->addr, ctx), "ok");
    }

    *written = w.len;
    return w.overflow ? ERR_NOSPACE : OK;
}
