#!/usr/bin/env python3
"""Genera la tabla de campos de CPUID de AMD, de su manual.

POR QUE ES UN GENERADOR APARTE.  Porque AMD tabula CPUID de otra forma, y las
diferencias no son cosmeticas -- el extractor de Intel se equivocaria en las
cuatro:

    CPUID Fn8000_001B_EAX Instruction-Based Sampling Feature Indicators
    Bits   Field Name                Description
    31:20                            Reserved
    19     IbsUpdtdDtlbStats         Simplified DTLB page size reporting...

  1. el REGISTRO va en la cabecera, no en cada fila.  Intel escribe `ECX[15]`
     en la propia fila;
  2. los reservados dejan **la columna de nombre VACIA**, mientras que Intel
     escribe `Reserved` donde iria el nombre;
  3. los nombres son CamelCase (`IbsUpdtdDtlbStats`), no MAYUSCULAS_CON_GUION.
     El discriminador del extractor de Intel -- "o lleva guion bajo, o va en
     mayusculas" -- los rechazaria TODOS;
  4. y hay una forma para varios registros a la vez: `Fn8000_001B_E[D,C,B]X`.

De ahi que la columna de nombre se localice por POSICION y no partiendo por
espacios: una celda vacia no deja ninguna marca, y un nombre no se distingue de
la primera palabra de una descripcion por su forma.

QUE PRODUCE
-----------
Un fichero por familia de hojas en `common/cpuid/amd/`, mas su `index.h`.

ALCANCE
-------
Solo AMD.  Los de Intel estan en `common/cpuid/intel/`, y no se mezclan: **el
rango extendido lo reparten los dos y no dicen lo mismo**.  Intel llega a
`80000008H`; de ahi en adelante manda AMD, y ahi estan `Fn8000_001B` (IBS),
`Fn8000_001D`/`1E` (cache y topologia) y `Fn8000_0022` (extensiones de
monitorizacion).

DE DONDE SALE
-------------
Del AMD64 Architecture Programmer's Manual, volumen 3.  Es un PDF y no viaja en
el repositorio; se vuelca a texto y `VESTA_AMD_DIR` dice donde esta:

    pdftotext -table <amd64-vol3.pdf> amd64.table.txt

Uso:
    python tools/gen_amd_cpuid_index.py            genera
    python tools/gen_amd_cpuid_index.py --dry-run  cuenta y no escribe
    python tools/gen_amd_cpuid_index.py --check    falla si el arbol esta viejo
    python tools/gen_amd_cpuid_index.py --report   el detalle de lo descartado
"""

import argparse
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR = os.path.join(ROOT, "common", "cpuid", "amd")

# Se busca en `manual/`, que esta en las exclusiones locales, o donde diga la
# variable de entorno.  Misma convencion que el lector de `tools/sdm/`.
MANUAL_DIR = os.environ.get("VESTA_AMD_DIR") or os.path.join(ROOT, "manual")
TABLE_DUMP = os.path.join(MANUAL_DIR, "amd64.table.txt")

# `pdftotext` emite Latin-1.
ENCODING = "latin-1"

# La cabecera de una hoja.  Dos formas:
#
#   CPUID Fn8000_001B_EAX Instruction-Based Sampling Feature Indicators
#   CPUID Fn8000_001B_E[D,C,B]X      Reserved
#
# La segunda dice que varios registros estan reservados a la vez, y hay que
# tratarla: si se ignora, esos registros quedan sin documentar y parece que la
# hoja no los tiene.
# Se reconoce el PREFIJO y se parsea el sufijo aparte, en vez de exigir una
# forma fija.  El manual usa al menos cinco, y con un patron rigido se pierden
# hojas enteras SIN QUE NADA FALLE -- asi se estaban perdiendo `Fn8000_001D`
# (topologia de cache) y `Fn8000_001C`:
#
#   _EAX              la comun
#   _EAX_x0  _EAX_x1  con subhoja explicita
#   _EAX_x[N:0]       con subhoja parametrica
#   _E[D,C,B]X        varios registros reservados a la vez
#   _x1               subhoja sin registro
# La lista entre corchetes puede llevar ESPACIOS -- `_E[A, B, C, D]X` --, y
# entonces el sufijo deja de ser una sola palabra.  Se reconoce aparte para que
# no se parta por el primer espacio: con `\S*` a secas, esa forma se leia como
# el sufijo `_E[A,` y el resto se iba a la descripcion, con lo que la hoja se
# perdia entera.
LEAF = re.compile(
    r"^CPUID\s+Fn([0-9A-F]{4})_([0-9A-F]{4})"
    r"((?:_E\[[^\]]*\]X)?\S*)\s+(\S.*?)\s*$")

# Y el sufijo, ya suelto.
SUF_REG = re.compile(r"^_E([A-D])X")
SUF_MULTI = re.compile(r"^_E\[([A-D,\s]+)\]X")
SUF_SUB = re.compile(r"^_x(?:([0-9A-F]+)|\[[^\]]*\])")

# La fila de encabezado de la tabla de bits.  De ella salen las columnas.
#
# El manual usa CUATRO formas, y durante un tiempo solo se reconocia una.  Las
# otras tres no daban error: dejaban la columna del nombre sin fijar, con lo
# que la tabla ENTERA se descartaba y la hoja parecia no existir.  Asi se
# perdian `Fn8000_0008_EBX` -- IBPB, IBRS, STIBP, SSBD y el muestreo de saltos,
# que es justo lo que le interesa a un perfilador --, `Fn8000_0001_EAX`
# (familia, modelo y stepping) y las subhojas de calidad de servicio.  Lo
# encontro el contraste contra una tercera fuente, no nuestras pruebas.
#
#   Bits      Field Name   Description               113 tablas
#   Bit       Field Name   Description                 2  (una sola, a dos paginas)
#   Bits      Field Names  Description                 1
#   Register  Bits         Field Name  Description    11
#
# La ultima es distinta de las otras tres en el FONDO y no solo en la forma: el
# registro deja de venir de la cabecera de la hoja y pasa a venir de cada fila.
BITS_HEADER = re.compile(
    r"^(\s*)(?:(Register)\s+)?(Bits?)\s+(Field Names?)\s+(Description)\s*$")

# La celda de la columna de registro, cuando la tabla la trae.
REG_CELL = re.compile(r"^E([A-D])X$")

# Una fila de campo empieza por su rango de bits.
BITS = re.compile(r"^\s*(\d+)(?::(\d+))?\s")

# Un nombre de campo de AMD.
#
# Admite EMPEZAR POR CIFRA -- `8BitBrandId` es un campo real del manual --, y
# eso no rompe nada porque la macro lleva prefijo (`AMD_CPUID_00000001_EBX_`),
# de modo que el identificador de C nunca empieza por un digito.  Exigir letra
# inicial, que era lo primero que escribi, tiraba ese campo en silencio.
IDENT = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_]*$")

# ...pero tiene que llevar ALGUNA LETRA.  Sin esta condicion, permitir la cifra
# inicial abre la puerta a los restos de una descripcion envuelta:
#
#     9      FullyAssociative    Fully associative cache.  If
#                                0 is returned in this field, the cache is ...
#            ^^^^^^^^^^^^^^^^    ^
#            un campo de verdad   y esto se colaba como un campo llamado `0`
#
# Lo encontro el contraste contra una tercera fuente, no nuestras pruebas.
HAS_LETTER = re.compile(r"[A-Za-z]")

# Como AMD marca un hueco en la columna del nombre.  **No es una celda vacia**:
# escribe un doble guion.  Tratarlos como "sin nombre" mandaba a la basura la
# mayor parte de los reservados, que son documentacion -- dicen que bits no se
# tocan.
HOLE_MARKS = ("--", "---", "-")

# Puntuacion que el manual pega al nombre y no forma parte de el: `MOVBE:` es
# el campo `MOVBE`.
TRAILING = ":.,;"

# El reparto en ficheros.  Por hoja, que en AMD es como se consulta.
GROUPS = (
    ("ibs", "Instruction-Based Sampling", (0x8000001B,)),
    ("perf", "Monitorizacion de rendimiento y sus extensiones",
     (0x8000000A, 0x80000022)),
    ("topology", "Topologia: cache, hermanos SMT y nodo",
     (0x80000005, 0x80000006, 0x8000001D, 0x8000001E, 0x80000026)),
    ("power", "Energia, control termico y estados de rendimiento",
     (0x80000007, 0x80000008)),
    ("security", "Cifrado de memoria, mitigaciones y aislamiento",
     (0x8000001F, 0x80000020, 0x80000021, 0x80000023)),
    ("features", "Mapas de bits de caracteristicas",
     (0x80000001, 0x80000019, 0x8000001A, 0x8000001C)),
    ("ident", "Identificacion y cadena de marca",
     (0x80000000, 0x80000002, 0x80000003, 0x80000004, 0x80000009)),
)

GROUP_TITLE = dict((n, t) for n, t, _ in GROUPS)
GROUP_TITLE["misc"] = "Hojas que no caen en ninguna familia"
GROUP_ORDER = [g[0] for g in GROUPS] + ["misc"]

LEAF_GROUP = {}
for _n, _t, _leaves in GROUPS:
    for _l in _leaves:
        LEAF_GROUP[_l] = _n


def group_of(leaf):
    """A que fichero va una hoja."""
    return LEAF_GROUP.get(leaf, "misc")


def load_pages():
    """Devuelve el volcado partido en paginas."""
    if not os.path.exists(TABLE_DUMP):
        raise IOError(
            "falta %s.\nGeneralo con:\n"
            "    pdftotext -table <amd64-vol3.pdf> %s" % (TABLE_DUMP, TABLE_DUMP))
    with io.open(TABLE_DUMP, encoding=ENCODING) as f:
        return f.read().split("\f")


def collect(pages_text):
    """Recorre el volcado y devuelve los campos y lo descartado.

    Se cuenta lo que se descarta y por que.  En la tabla de Intel eso caza ocho
    registros que se perdian en silencio, y aqui es todavia mas necesario porque
    el formato es menos regular.
    """
    fields = []
    skipped = {"sin_hoja": 0, "sin_nombre": 0, "rango_malo": 0}
    unnamed = []
    badrange = []   # rangos de verdad malformados: los limites al reves
    notrow = []     # lo que no era una fila de bits: paginas, otras tablas
    leaf = None
    reg = None
    name_col = None
    bits_col = None
    desc_col = None
    reg_col = None
    sub = ""
    # Reservados que una cabecera multi-registro da por buenos MIENTRAS no
    # aparezca una tabla que los desmienta.  Ver donde se llenan.
    pending = []

    for page in pages_text:
        for line in page.splitlines():
            m = LEAF.match(line)
            if m:
                # Lo que quedara pendiente de la hoja anterior ya no puede ser
                # desmentido por ninguna tabla: se da por bueno.
                fields.extend(pending)
                pending = []

                leaf = int(m.group(1) + m.group(2), 16)
                suffix = m.group(3)
                reg = None
                name_col = None
                bits_col = None
                desc_col = None
                reg_col = None
                sub = ""

                s = SUF_MULTI.match(suffix)
                if s:
                    # `_E[D,C,B]X`: la cabecera nombra VARIOS registros a la
                    # vez.  Casi siempre es para decir que estan reservados,
                    # pero no siempre -- `_E[A, B, C, D]X_x11` encabeza una
                    # tabla de verdad --, y la descripcion no basta para
                    # distinguirlo: `_E[D,C]X_x2` se titula "Processor Extended
                    # State Enumeration" y el reservado lo dice la PROSA de
                    # debajo.
                    #
                    # Asi que el reservado se deja PENDIENTE.  Si detras viene
                    # una tabla con columna de registro, manda la tabla y el
                    # pendiente se tira; si no viene ninguna, el reservado vale.
                    # Decidirlo por la descripcion inventaba un dato FALSO:
                    # `Fn0000_000D_x11_EAX` no esta reservado, es `CetUserSize`.
                    rest = suffix[s.end():]
                    t = SUF_SUB.match(rest)
                    if t and t.group(1):
                        sub = "_x%s" % t.group(1)
                    # Y el hueco se anota CON su subhoja.  Sin ella declaraba
                    # reservado el registro entero de la hoja, pisando los
                    # campos reales de las demas subhojas.
                    pending = [{
                        "leaf": leaf, "reg": "E%sX%s" % (r, sub),
                        "hi": 31, "lo": 0,
                        "name": "RESERVED_31_0", "hole": 1,
                    } for r in re.findall(r"[A-D]", s.group(1))]
                    continue

                s = SUF_REG.match(suffix)
                if s:
                    reg = "E%sX" % s.group(1)
                    # Puede venir con subhoja detras: `_EAX_x0`, `_EAX_x[N:0]`.
                    # La parametrica NO se numera -- inventarle un numero seria
                    # inventar una subhoja --, pero la explicita si.
                    rest = suffix[s.end():]
                    t = SUF_SUB.match(rest)
                    if t and t.group(1):
                        sub = "_x%s" % t.group(1)
                        reg = reg + sub
                    continue

                # Sufijo de SOLO subhoja: `_x0`, `_x1`.  Es la forma que usa el
                # manual cuando la tabla trae columna de registro, asi que aqui
                # no viene ningun registro: lo dira cada fila.  Contarlo como
                # desconocido perdia las seis subhojas de `Fn8000_0020` y las
                # de calidad de servicio.
                t = SUF_SUB.match(suffix)
                if t:
                    if t.group(1):
                        sub = "_x%s" % t.group(1)
                    continue

                # Sufijo que no se reconoce.  Se cuenta: si esto sube, es que
                # el manual cambio de forma.
                if suffix:
                    skipped["sufijo"] = skipped.get("sufijo", 0) + 1
                continue

            m = BITS_HEADER.match(line)
            if m:
                # De la cabecera salen las columnas.  La del nombre es lo unico
                # que permite ver que una celda esta VACIA: partir por espacios
                # no deja ninguna marca de la celda que falta.
                bits_col = m.start(3)
                name_col = m.start(4)
                desc_col = m.start(5)
                reg_col = m.start(2) if m.group(2) else None
                if reg_col is not None:
                    # Hay tabla de verdad: el reservado que dejo pendiente la
                    # cabecera de la hoja queda desmentido.
                    pending = []
                    reg = None
                continue

            if name_col is None or (reg is None and reg_col is None):
                continue

            # Con columna de registro, la fila empieza por el REGISTRO y no por
            # el rango de bits, asi que hay que apartar esa celda antes de
            # leerlo.  Una fila de continuacion la deja vacia, y eso significa
            # "el mismo registro que la de arriba".
            row = line
            if reg_col is not None:
                cell = line[reg_col:bits_col].strip() \
                    if len(line) > reg_col else ""
                if cell:
                    r = REG_CELL.match(cell)
                    if not r:
                        continue
                    reg = "E%sX%s" % (r.group(1), sub)
                if reg is None:
                    continue
                # Se blanquea la celda del registro conservando las columnas:
                # todo lo que viene despues se lee por POSICION.
                row = " " * bits_col + line[bits_col:]

            m = BITS.match(row)
            if not m:
                continue

            # La celda de bits vive A LA IZQUIERDA de la del nombre.  Un numero
            # que aparece mas a la derecha no es una celda de bits: es texto de
            # una descripcion envuelta que empieza por cifra.
            if m.start(1) >= name_col:
                skipped["no_es_fila"] = skipped.get("no_es_fila", 0) + 1
                continue

            hi = int(m.group(1))
            lo = int(m.group(2)) if m.group(2) else hi

            # La celda del nombre: el PRIMER TOKEN que sigue al rango de bits.
            #
            # Antes se leia desde la columna que fijaba la cabecera, y eso falla
            # cuando `pdftotext` no alinea la fila con su encabezado: el nombre
            # salia PARTIDO POR LA MITAD.  `L1DTlb2and4MAssoc` se quedaba en
            # `b2and4MAssoc`, y como a `L1ITlb2and4MAssoc` le pasaba lo mismo
            # los dos acababan con el mismo nombre y rangos distintos, asi que
            # el desempate los descartaba A LOS DOS.  Un nombre cortado es peor
            # que un campo perdido: parece un campo.
            #
            # La columna sigue haciendo falta para lo unico que necesita
            # posicion: distinguir una celda de nombre VACIA, que no deja
            # ninguna marca en el texto.  Se decide por CERCANIA -- un token que
            # cae mas cerca de la columna de la descripcion que de la del nombre
            # es descripcion, y entonces el nombre estaba vacio --, que tolera
            # el desalineado sin dejar de ver el hueco.
            after = m.end(2) if m.group(2) else m.end(1)
            nxt = re.compile(r"\S").search(row, after)
            token = ""
            if nxt and abs(nxt.start() - name_col) <= abs(nxt.start() - desc_col):
                token = row[nxt.start():].split()[0]
            token = token.rstrip(TRAILING)
            # El manual a veces repite el rango de bits DENTRO del nombre --
            # `XFeatureSupportedMask[31:0]` --.  Es el mismo dato que ya trae la
            # columna de bits, asi que sobra en el nombre.
            token = re.sub(r"\[[^\]]*\]$", "", token)
            # Todo lo que sigue al rango de bits.  Solo se consulta cuando la
            # celda del nombre esta vacia, para ver si la descripcion dice que
            # el hueco es reservado.
            tail = row[after:].strip()

            # PRIMERO: ¿es esto siquiera una fila de bits?  Un numero de pagina
            # con el encabezado corriente detras tiene la MISMA forma -- cifra,
            # mucho espacio, texto:
            #
            #   31:20                            Reserved        <- fila de bits
            #   628                              Obtaining ...   <- numero de pagina
            #
            # Los dos dejan la columna de nombre vacia.  La diferencia es que el
            # reservado **lo dice en la descripcion**.
            #
            # Esta comprobacion va ANTES que la del rango a proposito: fiarlo a
            # que el numero de pagina sea mayor que 63 funcionaba de casualidad
            # -- una pagina 12 habria entrado como un campo reservado inventado
            # --, y ademas etiquetaba mal lo descartado, que es peor: el
            # contador decia "rango imposible" donde la causa era otra.
            # Un indice de bit NO PUEDE pasar de 63.  Asi que eso, por
            # definicion, no es una fila de bits -- es un numero de pagina, o
            # una fila de una tabla de codificacion.  Va en la misma categoria
            # que la celda de nombre vacia sin "Reserved", porque la causa es la
            # misma: se esta leyendo algo que no es la tabla que se cree.
            #
            # Tenerlo como "rango malo" aparte etiquetaba mal el descarte, y un
            # contador que miente sobre POR QUE descarta no sirve para lo unico
            # que hace falta: enterarse de que la deteccion se ha roto.
            if hi > 63 or (not token
                           and not re.search(r"\breserved\b", tail, re.I)):
                skipped["no_es_fila"] = skipped.get("no_es_fila", 0) + 1
                notrow.append((leaf, reg, hi, lo, line.strip()[:60]))
                continue

            # Y esto si es un rango de verdad malformado: los limites al reves
            # dentro de lo que si es una tabla de bits.
            if hi < lo:
                skipped["rango_malo"] += 1
                badrange.append((leaf, reg, hi, lo, line.strip()[:60]))
                continue

            if (not token or token in HOLE_MARKS
                    or token.lower().startswith("reserved")):
                fields.append({
                    "leaf": leaf, "reg": reg, "hi": hi, "lo": lo,
                    "name": "RESERVED_%d_%d" % (hi, lo), "hole": 1,
                })
                continue
            # Nombres con guion -- `SEV-SNP`, `SEV-ES` -- o con barra -- `U/S`,
            # el bit de usuario/supervisor de CET -- son campos reales, y ni el
            # guion ni la barra valen en un identificador de C.  Se sustituyen
            # por subrayado y **se conserva el original**: el nombre del manual
            # es lo que se busca en el manual, asi que perderlo seria perder la
            # forma de encontrarlo.
            original = token
            token = token.replace("-", "_").replace("/", "_")

            if not IDENT.match(token) or not HAS_LETTER.search(token):
                skipped["sin_nombre"] += 1
                unnamed.append((leaf, reg, hi, lo, original))
                continue

            fields.append({
                "leaf": leaf, "reg": reg, "hi": hi, "lo": lo,
                "name": token, "hole": 0,
                "manual_name": original if original != token else None,
            })

    # La ultima hoja del manual no tiene ninguna detras que cierre su pendiente.
    fields.extend(pending)

    return fields, skipped, unnamed, badrange, notrow


def macro_of(f):
    """El nombre de la macro: fabricante, hoja, registro y campo."""
    return "AMD_CPUID_%08X_%s_%s" % (f["leaf"], f["reg"], f["name"])


def dedupe(fields):
    """Une repeticiones y separa lo que choca."""
    by_macro = {}
    for f in fields:
        by_macro.setdefault(macro_of(f), []).append(f)

    unique, conflicting = [], []
    for macro, group in sorted(by_macro.items()):
        shapes = {(g["hi"], g["lo"]) for g in group}
        if len(shapes) == 1:
            unique.append(group[0])
        else:
            conflicting.append((macro, sorted(shapes)))
    return unique, conflicting


HEADER = """\
/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file %(file)s
 * @brief %(title)s
 *
 * FICHERO GENERADO.  No se edita a mano: se regenera con
 *
 *     python tools/gen_amd_cpuid_index.py
 *
 * a partir del manual de AMD, que no viaja en el repositorio.
 *
 * ALCANCE: **SOLO AMD**.  Los de Intel estan en `common/cpuid/intel/`, y no se
 * mezclan porque el rango extendido lo reparten los dos: Intel llega a
 * `80000008H` y de ahi en adelante manda AMD.
 *
 * Los nombres son los del manual de AMD -- CamelCase, no MAYUSCULAS --, para
 * que se pueda buscar en el la misma cadena que hay aqui.  El prefijo `AMD_`
 * dice de que manual salio cada macro.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef %(guard)s
#define %(guard)s

"""

FOOTER = """
#endif /* %(guard)s */
"""


def emit_group(name, items, dry_run):
    """Escribe el fichero de una familia de hojas."""
    path = os.path.join(OUT_DIR, "cpuid_amd_%s.h" % name)
    guard = "VXP_COMMON_CPUID_AMD_%s_H" % name.upper()
    out = [HEADER % {"file": "cpuid_amd_%s.h" % name,
                     "title": GROUP_TITLE[name], "guard": guard}]

    last = None
    for f in sorted(items, key=lambda x: (x["leaf"], x["reg"], -x["hi"])):
        if (f["leaf"], f["reg"]) != last:
            out.append("/* ---- Fn%08X, %s ---- */\n\n" % (f["leaf"], f["reg"]))
            last = (f["leaf"], f["reg"])

        mark = " -- RESERVADO, no se toca" if f["hole"] else ""
        if f.get("manual_name"):
            # El nombre tal cual lo escribe AMD, para poder buscarlo en el
            # manual: la macro lleva subrayado donde el manual lleva guion.
            mark += " -- el manual lo llama `%s`" % f["manual_name"]
        macro = macro_of(f)
        if f["hi"] == f["lo"]:
            out.append("/** %s bit %d%s */\n" % (f["reg"], f["hi"], mark))
            out.append("#define %s %du\n\n" % (macro, f["hi"]))
        else:
            width = f["hi"] - f["lo"] + 1
            out.append("/** %s bits %d:%d%s */\n"
                       % (f["reg"], f["hi"], f["lo"], mark))
            out.append("#define %s_SHIFT %du\n" % (macro, f["lo"]))
            out.append("#define %s_MASK 0x%Xu\n\n"
                       % (macro, (1 << width) - 1))

    out.append(FOOTER % {"guard": guard})
    text = "".join(out)
    if not dry_run:
        with io.open(path, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return path, len(items), text


def emit_index(names, conflicting, dry_run):
    """Escribe el `index.h` de AMD."""
    path = os.path.join(OUT_DIR, "index.h")
    guard = "VXP_COMMON_CPUID_AMD_INDEX_H"
    out = [HEADER % {"file": "index.h",
                     "title": "Todos los campos de CPUID de AMD, por familia.",
                     "guard": guard}]
    for n in names:
        out.append("#include \"cpuid_amd_%s.h\"\n" % n)
    out.append("\n")

    if conflicting:
        out.append(
            "/*\n"
            " * CAMPOS QUE APARECEN CON DOS FORMAS, y por eso NO se definen.\n"
            " *\n"
            " * Elegir una seria preguntar por el bit equivocado en la mitad de\n"
            " * las piezas, que es un fallo callado: sale cero y parece que la\n"
            " * capacidad no esta.\n"
            " *\n")
        for macro, shapes in conflicting:
            forms = ", ".join("[%d:%d]" % (hi, lo) for hi, lo in shapes)
            out.append(" *   %-52s %s\n" % (macro, forms))
        out.append(" */\n")

    out.append(FOOTER % {"guard": guard})
    text = "".join(out)
    if not dry_run:
        with io.open(path, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return path, text


def differs(path, text):
    """¿Lo que hay en el arbol no es lo que saldria ahora?"""
    if not os.path.exists(path):
        return True
    with io.open(path, encoding="ascii", newline="") as f:
        return f.read().replace("\r\n", "\n") != text


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--dry-run", action="store_true", help="cuenta y no escribe")
    ap.add_argument("--check", action="store_true",
                    help="no escribe: falla si el arbol esta viejo")
    ap.add_argument("--report", action="store_true",
                    help="el detalle de lo descartado")
    args = ap.parse_args()
    if args.check:
        args.dry_run = True

    try:
        pages_text = load_pages()
    except IOError as exc:
        print("gen_amd_cpuid_index: %s" % exc, file=sys.stderr)
        return 1

    fields, skipped, unnamed, badrange, notrow = collect(pages_text)
    unique, conflicting = dedupe(fields)

    grouped = {}
    for f in unique:
        grouped.setdefault(group_of(f["leaf"]), []).append(f)

    if not args.dry_run and not os.path.isdir(OUT_DIR):
        os.makedirs(OUT_DIR)

    leaves = sorted({f["leaf"] for f in unique})
    holes = len([f for f in unique if f["hole"]])
    print("paginas leidas:          %d" % len(pages_text))
    print("filas de campo:          %d" % len(fields))
    print("hojas:                   %d" % len(leaves))
    print("campos definidos:        %d  (de ellos %d reservados)"
          % (len(unique), holes))
    print("con dos formas:          %d  (no se definen; se listan)"
          % len(conflicting))
    print("descartados:             sin_nombre=%d sin_hoja=%d rango_malo=%d"
          % (skipped["sin_nombre"], skipped["sin_hoja"], skipped["rango_malo"]))
    print("no eran filas de bits:   %d  (numeros de pagina y encabezados)"
          % skipped.get("no_es_fila", 0))
    print()

    stale = []
    emitted = []
    for name in GROUP_ORDER:
        if name not in grouped:
            continue
        path, n, text = emit_group(name, grouped[name], args.dry_run)
        if args.check and differs(path, text):
            stale.append(path)
        emitted.append(name)
        print("  %-28s %4d" % (os.path.basename(path), n))
    path, text = emit_index(emitted, conflicting, args.dry_run)
    if args.check and differs(path, text):
        stale.append(path)
    print("  %-28s" % "index.h")

    if args.report:
        print()
        print("=== hojas cubiertas ===")
        print("  " + " ".join("Fn%08X" % l for l in leaves))
        print()
        print("=== filas con bits pero sin nombre utilizable ===")
        for leaf, reg, hi, lo, tok in unnamed[:30]:
            print("  Fn%08X %s[%d:%d]  %r" % (leaf, reg, hi, lo, tok))
        print("  (%d en total)" % len(unnamed))
        print()
        print("=== lo que no era una fila de bits ===")
        print("  (numeros de pagina con el encabezado corriente detras, y")
        print("   filas de tablas de codificacion que empiezan por cifras)")
        for leaf, reg, hi, lo, txt in notrow[:20]:
            print("  Fn%08X %-8s %-6d %s" % (leaf, reg, hi, txt))
        print("  (%d en total)" % len(notrow))
        print()
        print("=== rangos de verdad malformados (limites al reves) ===")
        for leaf, reg, hi, lo, txt in badrange[:20]:
            print("  Fn%08X %-8s hi=%-3d lo=%-3d %s" % (leaf, reg, hi, lo, txt))
        print("  (%d en total)" % len(badrange))

    if args.check:
        if stale:
            print()
            print("DESACTUALIZADO.  Regeneralo con:")
            print("    python tools/gen_amd_cpuid_index.py")
            for p in stale:
                print("   %s" % os.path.relpath(p, ROOT))
            return 1
        print("\n(--check: el arbol esta al dia)")
        return 0

    if args.dry_run:
        print("\n(--dry-run: no se escribio nada)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
