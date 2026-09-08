#!/usr/bin/env python3
"""Emite las tablas de CPUID y MSR como DATOS, para poder recorrerlas.

POR QUE ADEMAS DE LAS MACROS.  Los generadores emiten una macro por campo, y
eso sirve para preguntar por uno que ya se conoce al escribir el codigo.  No
sirve para lo contrario: recorrerlos todos sin saber de antemano cuales hay,
que es lo que hace un volcado.  Una macro no existe en ejecucion.

Son las dos caras del MISMO dato, salen del mismo sitio en la misma pasada, y
por eso no pueden discrepar.  Si se generaran por separado acabarian
discrepando: es lo que pasa siempre que un hecho tiene dos productores.

POR QUE ESTA AQUI Y NO EN CADA GENERADOR.  Son cuatro generadores y una sola
forma de escribir una tabla.  Repetirla cuatro veces es garantizar que dentro
de un tiempo haya cuatro formas ligeramente distintas.

EL BLOQUE DE CADENAS.  Los nombres van en un unico bloque y el registro guarda
el DESPLAZAMIENTO, no un puntero.  Un puntero por campo son ocho bytes y una
reubicacion que el cargador resuelve al arrancar, una por campo; con mil
quinientos campos, eso es trabajo de sobra para imprimir texto.  Un
desplazamiento cuesta cuatro bytes, no lleva reubicacion y queda independiente
de la posicion.

Los nombres repetidos se comparten: el mismo nombre de campo aparece en muchas
hojas.
"""

import io

# Los registros de salida de CPUID, en el orden en que los numera el contrato
# de `common/cpuid/table.h`.
REG_INDEX = {"EAX": 0, "EBX": 1, "ECX": 2, "EDX": 3}
REG_MACRO = ("CPUID_REG_EAX", "CPUID_REG_EBX", "CPUID_REG_ECX", "CPUID_REG_EDX")


class Blob(object):
    """El bloque de cadenas, con los repetidos compartidos."""

    def __init__(self):
        self._off = {}
        self._order = []
        self.size = 0

    def add(self, s):
        """Anade una cadena y devuelve su desplazamiento."""
        if s in self._off:
            return self._off[s]
        # Solo se admiten nombres que sean identificadores.  No es una
        # restriccion caprichosa: emitirlos como literal de C sin escapar solo
        # es correcto si no traen comillas ni barras, y prefiero que reviente
        # aqui a que salga un fichero que no compila.
        for ch in s:
            if not (ch.isalnum() or ch == "_"):
                raise ValueError("nombre no emitible: %r" % s)
        off = self.size
        self._off[s] = off
        self._order.append(s)
        self.size += len(s) + 1
        return off

    def lines(self, indent="    "):
        """El bloque como ARRAY DE BYTES, un nombre por linea.

        No como literal de cadena, y el motivo es de portabilidad medida y no de
        purismo.  Los literales adyacentes se concatenan en UNO, asi que el
        bloque entero seria un solo literal de decenas de miles de caracteres, y
        ahi hay dos limites de verdad:

            C99 garantiza 4095 caracteres.  Mas no esta asegurado, y
                `-pedantic-errors` -- que este proyecto usa a proposito -- lo
                rechaza.
            MSVC corta en 65535 bytes, y eso es un limite DURO.  Los nombres de
                MSR ya rondan los treinta mil y esta tabla solo puede crecer:
                el dia que se anadan los campos de bits, revienta.

        Un array de bytes no tiene ninguno de los dos limites.  Cuesta mas
        fuente -- que nadie lee linea a linea -- y a cambio no se rompe.

        El desplazamiento va en el comentario porque es lo que guardan las
        filas: sin el, revisar una fila a mano obliga a contar caracteres.
        """
        out = []
        off = 0
        for s in self._order:
            body = ",".join("'%s'" % c for c in s)
            out.append("%s/* %6d */ %s, 0,\n" % (indent, off, body))
            off += len(s) + 1
        if not out:
            out.append("%s0\n" % indent)
        return out


HEADER = """\
/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file table.c
 * @brief %(what)s de %(vendor)s, como datos recorribles.
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python %(tool)s
 *
 * a partir del volcado de texto del manual, que no viaja en el repositorio.
 *
 * ES LA OTRA CARA DE LAS MACROS DE AL LADO, y sale de la misma pasada.  Las
 * macros sirven para preguntar por un campo que ya se conoce al escribir el
 * codigo; esta tabla, para recorrerlos todos sin saber de antemano cuales hay.
 * Una macro no existe en ejecucion: no se puede iterar ni imprimir su nombre.
 *
 * EL NOMBRE ES UN DESPLAZAMIENTO dentro del bloque de cadenas, no un puntero.
 * Un puntero por fila costaria ocho bytes y una reubicacion que el cargador
 * resuelve al arrancar, una por fila.  Ver `common/%(contract)s`.
 *
 * ES SU PROPIA UNIDAD DE TRADUCCION: quien no la referencie no la enlaza, asi
 * que tenerla en el arbol no le cuesta nada a quien no la use -- al driver,
 * sin ir mas lejos.
 */

"""


def _table_c(what, vendor, tool, contract, blob, rows, symbol,
             struct_name, table_type):
    """El armazon comun de los dos: cabecera, bloque, filas y descriptor."""
    out = [HEADER % {"what": what, "vendor": vendor, "tool": tool,
                     "contract": contract}]
    out.append('#include "%s"\n\n' % contract)

    out.append("/* Los nombres, compartiendo los repetidos.  Array de bytes y no\n"
               " * literal de cadena: ver `Blob.lines` en tools/emit_table.py. */\n")
    out.append("static const char names[] = {\n")
    out.extend(blob.lines())
    out.append("};\n\n")

    out.append("static const %s rows[] = {\n" % struct_name)
    out.extend(rows)
    out.append("};\n\n")

    out.append("const %s %s = {\n" % (table_type, symbol))
    out.append("    rows,\n")
    out.append("    (u32)(sizeof rows / sizeof rows[0]),\n")
    out.append("    names\n")
    out.append("};\n")
    return "".join(out)


def cpuid_table_c(vendor, fields, tool):
    """La tabla de campos de CPUID de un fabricante.

    `fields` son diccionarios con `leaf`, `sub`, `reg`, `hi`, `lo`, `name` y
    `hole`, que es la forma que ya usan los dos generadores de CPUID.
    """
    blob = Blob()
    rows = []
    ordered = sorted(fields, key=lambda f: (
        f["leaf"], -1 if f["sub"] is None else f["sub"],
        REG_INDEX[f["reg"]], f["lo"]))
    for f in ordered:
        off = blob.add(f["name"])
        sub = "CPUID_NO_SUBLEAF" if f["sub"] is None else "%uu" % f["sub"]
        flags = "CPUID_FIELD_RESERVED" if f["hole"] else "0u"
        rows.append(
            "    { 0x%08Xu, %6uu, %-16s, %s, %2uu, %2uu, %-20s, 0u },"
            "  /* %s */\n" % (
                f["leaf"], off, sub, REG_MACRO[REG_INDEX[f["reg"]]],
                f["lo"], f["hi"] - f["lo"] + 1, flags, f["name"]))
    return _table_c("Los campos de CPUID", vendor.capitalize(), tool,
                    "cpuid/table.h", blob, rows,
                    "cpuid_table_%s" % vendor, "cpuid_field", "cpuid_table")


def msr_table_c(vendor, regs, tool):
    """La tabla de MSR de un fabricante.

    `regs` son diccionarios con `addr`, `name` y, si el manual la documenta
    adyacente, la puerta en `gate` como (leaf, subleaf, reg, lo, width).
    """
    blob = Blob()
    rows = []
    for r in sorted(regs, key=lambda x: (x["addr"], x["name"])):
        off = blob.add(r["name"])
        flags = "MSR_REG_INDEXED" if r.get("indexed") else "0u"
        g = r.get("gate")
        if g is None:
            # Sin puerta documentada.  NO es lo mismo que "no tiene": es que el
            # manual no la da al lado, y hay que poder distinguirlo.
            rows.append(
                "    { 0x%08Xu, %6uu, 0u, CPUID_NO_SUBLEAF, MSR_NO_GATE,"
                "  0u,  0u, %-15s, {0u, 0u} },  /* %s */\n"
                % (r["addr"], off, flags, r["name"]))
            continue
        leaf, sub, reg, lo, width = g
        subtxt = "CPUID_NO_SUBLEAF" if sub is None else "%uu" % sub
        rows.append(
            "    { 0x%08Xu, %6uu, 0x%Xu, %-16s, %s, %2uu, %2uu, %-15s,"
            " {0u, 0u} },  /* %s */\n"
            % (r["addr"], off, leaf, subtxt, REG_MACRO[REG_INDEX[reg]],
               lo, width, flags, r["name"]))
    return _table_c("Los MSR", vendor.capitalize(), tool,
                    "msr/table.h", blob, rows,
                    "msr_table_%s" % vendor, "msr_reg", "msr_table")


def write_if_changed(path, text, dry_run):
    """Escribe solo si cambia, y dice si difiere."""
    changed = True
    try:
        with io.open(path, encoding="ascii", newline="") as f:
            changed = f.read().replace("\r\n", "\n") != text
    except IOError:
        changed = True
    if changed and not dry_run:
        with io.open(path, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return changed
