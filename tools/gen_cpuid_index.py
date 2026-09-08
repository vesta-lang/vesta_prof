#!/usr/bin/env python3
"""Genera la tabla de campos de CPUID a partir del manual de Intel.

POR QUE EXISTE, Y NO ES SIMETRIA CON EL DE MSR.  Un numero de bit de CPUID
escrito de memoria falla exactamente igual de callado que una direccion de MSR:
se pregunta por el bit equivocado, sale cero, y el perfilador concluye que la
maquina no tiene algo que si tiene.  No da un error -- da una capacidad perdida.

Ya paso una vez con `IA32_PERF_CAPABILITIES.PEBS_FMT`, y la unica diferencia
entre aquello y esto es que el manual no lo desmintio a tiempo.

QUE PRODUCE
-----------
`common/cpuid_index.h` y los ficheros por rango en `common/cpuid/`.  Cada campo
sale como su posicion de bit -- si es un bit suelto -- o como desplazamiento y
mascara -- si es un rango --, con el nombre que le da el manual, para que se
pueda buscar en el la misma cadena que hay en el codigo.

DE DONDE SALE
-------------
Del capitulo de enumeracion de CPUID, que tabula asi:

    CPUID.06H -- Thermal and Power Management            <- la hoja
    Register   Field Name        Description   Domain    <- cabecera
    ECX[0]     HW_FEEDBACK_CAP   If 1, ...     Platform  <- el campo

El manual es un PDF y no viaja en el repositorio.  Se vuelca a texto una vez y
`VESTA_SDM_DIR` dice donde esta:

    pdftotext -table <manual.pdf> sdm.table.txt

LO QUE NO SE COPIA
------------------
Nombres, hojas y posiciones de bit son HECHOS y se extraen.  La descripcion es
prosa de Intel y **no se copia**.

Uso:
    python tools/gen_cpuid_index.py            genera
    python tools/gen_cpuid_index.py --dry-run  cuenta y no escribe
    python tools/gen_cpuid_index.py --check    falla si el arbol esta viejo
    python tools/gen_cpuid_index.py --report   el detalle de lo descartado
"""

import argparse
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from sdm import pages  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# Los de Intel en su subdirectorio y los de AMD en el suyo, por el mismo motivo
# que los MSR: los dos fabricantes definen hojas propias en el mismo rango
# extendido, y `Fn8000_001B` solo existe en AMD.
OUT_DIR = os.path.join(ROOT, "common", "cpuid", "intel")
INDEX = os.path.join(OUT_DIR, "index.h")

# La cabecera que abre una hoja, y a veces una subhoja:
#   CPUID.06H -- Thermal and Power Management
#   CPUID.07H.01H -- Structured Extended Feature Enumeration
LEAF = re.compile(
    r"^CPUID\.([0-9A-F]+)H(?:\.([0-9A-F]+)H)?\s*--\s*(\S.*?)\s*$")

# El titulo de tabla dice la hoja tambien.  Sirve de confirmacion cuando la
# cabecera de arriba quedo en otra pagina, que pasa cuando una tabla se parte.
LEAF_TABLE = re.compile(r"^\s*Table\s+[0-9]+-[0-9]+\.\s+Leaf\s+([0-9A-F]+)H")

# Una fila de campo: registro, rango de bits, nombre, y lo demas.
#   EAX[31:0]  MAX_LEAF      Maximum input value ...   Platform
#   ECX[15]    PERF_CAPABILITIES  If 1, ...            Platform
FIELD = re.compile(
    r"^(EAX|EBX|ECX|EDX)\[(\d+)(?::(\d+))?\]\s+(\S+)(?:\s+(.*))?$")

# Un nombre de campo de verdad.
#
# No basta con "empieza por letra": las filas sin columna de nombre traen prosa
# ahi -- `EAX[31:0]  Version information: Type, Family, ...` --, y `Version` es
# un identificador perfectamente valido.
#
# El discriminador que si funciona: **o lleva guion bajo, o esta todo en
# mayusculas**.  Los nombres del manual cumplen uno de los dos (`MAX_LEAF`,
# `DS`, `VENDOR_BRAND_STRING_BYTES_8_to_11`) y las palabras sueltas de la prosa
# no cumplen ninguno (`Version`, `Feature`, `Extended`, `as`, `holds`).
#
# Exigir mayusculas a secas, que era lo primero que probe, tiraba los campos de
# la cadena de marca por llevar `to` en minuscula.  Otra perdida silenciosa que
# solo aparecio al mirar lo descartado.
IDENT = re.compile(r"^[A-Za-z][A-Za-z0-9_]*$")


def is_field_name(tok):
    """¿Ese token es el nombre de un campo, o es prosa?"""
    if not IDENT.match(tok):
        return False
    return "_" in tok or tok.isupper()


# Los que documentan el hueco en vez de un campo.  **No se omiten**: un rango
# reservado dice que bits no se pueden tocar y cuales pueden convertirse en algo
# manana, y sin el la disposicion del registro queda con agujeros.  Se emiten
# con el rango en el nombre, que es lo que los hace unicos.
HOLE_NAMES = ("RESERVED", "N/A")

# El reparto en ficheros.  Por FUNCION y no por rango de hoja, por el mismo
# motivo que en los MSR: quien busca un campo lo busca por lo que hace, no por
# donde cayo su numero.  Un solo fichero con ochocientas entradas es lo mismo
# que no repartirlo.
#
# Cada grupo lleva las hojas que le tocan.  Una hoja que no este en ninguno cae
# en `misc`, y eso es una senal: si `misc` crece, es que hay una familia nueva
# que merece su sitio.
GROUPS = (
    ("perf", "Rendimiento: PMU, Intel PT, LBR y la extension del perfmon",
     (0x0A, 0x14, 0x1C, 0x23)),
    ("topology", "Topologia: caches, TLB, hilos hermanos y clase de nucleo",
     (0x04, 0x0B, 0x18, 0x1A, 0x1F)),
    ("power", "Frecuencia, energia, control termico y base de tiempo",
     (0x05, 0x06, 0x15, 0x16)),
    ("features", "Los mapas de bits de caracteristicas, y el estado extendido",
     (0x01, 0x07, 0x0D)),
    ("ident", "Identificacion: fabricante, marca y descriptores",
     (0x00, 0x02, 0x03, 0x17)),
    ("security", "Enclaves, cerrojo de claves y configuracion de plataforma",
     (0x12, 0x19, 0x1B, 0x20, 0x24)),
    ("rdt", "Reparto y vigilancia de recursos compartidos",
     (0x0F, 0x10)),
    ("amx", "Mosaicos de matrices",
     (0x1D, 0x1E)),
)

# Los dos rangos que se consultan aparte de todo lo demas.
HYPERVISOR_LO, HYPERVISOR_HI = 0x40000000, 0x4FFFFFFF
EXTENDED_LO = 0x80000000

GROUP_TITLE = dict((name, title) for name, title, _ in GROUPS)
GROUP_TITLE["misc"] = "Hojas basicas que no caen en ninguna familia"
GROUP_TITLE["hypervisor"] = "Rango de hipervisor (4000_0000H)"
GROUP_TITLE["extended"] = "Rango extendido (8000_0000H)"

# Se invierte una vez, en vez de recorrer los grupos por cada campo.
LEAF_GROUP = {}
for _name, _title, _leaves in GROUPS:
    for _leaf in _leaves:
        LEAF_GROUP[_leaf] = _name

# El orden en que se emiten y se listan.  `perf` primero porque es la razon de
# ser de este proyecto.
GROUP_ORDER = [g[0] for g in GROUPS] + ["misc", "hypervisor", "extended"]


def group_of(leaf):
    """A que fichero va una hoja."""
    if leaf >= EXTENDED_LO:
        return "extended"
    if HYPERVISOR_LO <= leaf <= HYPERVISOR_HI:
        return "hypervisor"
    return LEAF_GROUP.get(leaf, "misc")


def macro_name(leaf, sub, reg, field):
    """El nombre de la macro.

    Lleva la hoja, la subhoja cuando la hay, el registro y el nombre del campo.
    Los cuatro hacen falta: el mismo nombre de campo aparece en hojas distintas,
    y el mismo bit significa cosas distintas en cada registro.
    """
    if sub is None:
        return "CPUID_%02X_%s_%s" % (leaf, reg, field)
    return "CPUID_%02X_%02X_%s_%s" % (leaf, sub, reg, field)


def collect(table_pages):
    """Recorre las paginas y devuelve los campos y lo que no se entendio.

    Se cuenta lo descartado y por que.  Es lo que caza una forma que no se
    trataba: en la tabla de MSR aparecieron ocho registros perdidos en silencio
    por no contar los descartes.
    """
    fields = []
    skipped = {"sin_hoja": 0, "referencia": 0, "reservado": 0, "rango_malo": 0}
    unnamed = []
    leaf_title = {}   # (hoja, subhoja) -> el titulo con que el manual la abre
    leaf = None
    sub = None

    for page in table_pages:
        for line in page.splitlines():
            m = LEAF.match(line)
            if m:
                leaf = int(m.group(1), 16)
                sub = int(m.group(2), 16) if m.group(2) else None
                # El titulo de la hoja es la CITA: dice de que seccion del
                # manual sale, que es un dato.  La prosa descriptiva no se
                # copia -- es texto de Intel --, pero con esto se llega a ella
                # en un paso.
                leaf_title[(leaf, sub)] = m.group(3)
                continue
            m = LEAF_TABLE.match(line)
            if m:
                # Solo confirma la hoja; la subhoja no aparece en el titulo, asi
                # que no se toca -- suponerla seria inventarla.
                leaf = int(m.group(1), 16)
                continue

            m = FIELD.match(line)
            if not m:
                continue
            if leaf is None:
                # Un campo antes de cualquier cabecera de hoja no se puede
                # colocar.  Se cuenta: si esto sube, es que la deteccion de
                # hoja se rompio.
                skipped["sin_hoja"] += 1
                continue

            reg, hi, lo, name = m.group(1), int(m.group(2)), m.group(3), m.group(4)
            lo = int(lo) if lo is not None else hi
            if hi < lo or hi > 31:
                skipped["rango_malo"] += 1
                continue
            # Un hueco documentado -- reservado, o no aplicable -- NO se omite.
            # Se emite con el rango en el nombre, que ademas es lo que lo hace
            # unico: un registro tiene varios reservados y todos se llaman
            # igual en el manual.
            if name.upper() in HOLE_NAMES:
                tag = "RESERVED" if name.upper() == "RESERVED" else "NA"
                hole = "%s_%d_%d" % (tag, hi, lo)
                fields.append({
                    "leaf": leaf, "sub": sub, "reg": reg,
                    "hi": hi, "lo": lo, "name": hole, "hole": 1,
                    "macro": macro_name(leaf, sub, reg, hole),
                })
                continue

            if not is_field_name(name):
                # Referencias cruzadas a otra tabla, del estilo
                # `EBX[31:0]  Feature information (see ...)`, y restos de filas
                # envueltas.  No son campos, pero **se anotan** en el fichero
                # para que la disposicion no tenga huecos invisibles.
                skipped["referencia"] += 1
                unnamed.append((leaf, sub, reg, hi, lo, name))
                continue

            fields.append({
                "leaf": leaf, "sub": sub, "reg": reg,
                "hi": hi, "lo": lo, "name": name, "hole": 0,
                "macro": macro_name(leaf, sub, reg, name),
            })

    return fields, skipped, unnamed, leaf_title


def dedupe(fields):
    """Une repeticiones y separa lo que de verdad choca.

    El mismo campo se repite cuando una tabla cruza un salto de pagina.  Lo que
    importa son los que traen el MISMO nombre de macro con OTROS bits: ahi elegir
    uno seria publicar una posicion equivocada, asi que no se define ninguno y se
    listan.
    """
    by_macro = {}
    for f in fields:
        by_macro.setdefault(f["macro"], []).append(f)

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
 *     python tools/gen_cpuid_index.py
 *
 * a partir del volcado de texto del manual de Intel, que no viaja en el
 * repositorio.
 *
 * POR QUE SE GENERA.  Un numero de bit escrito de memoria falla callado: se
 * pregunta por el bit equivocado, sale cero, y se concluye que la maquina no
 * tiene algo que si tiene.  No da un error -- da una capacidad perdida.
 *
 * Los nombres son los del manual, para que se pueda buscar en el la misma
 * cadena que hay aqui.  Un campo de un bit sale como su POSICION; uno de varios,
 * como DESPLAZAMIENTO y MASCARA.
 *
 * ALCANCE: **SOLO INTEL**, y conviene no confundirlo con "completo".  Sale del
 * manual de Intel, asi que describe lo que Intel documenta -- y el rango
 * extendido de Intel acaba en 80000008H.  Por encima de ahi hay hojas que
 * existen y que son de AMD:
 *
 *     8000001BH   IBS, el muestreo preciso de AMD
 *     8000001DH   topologia de cache
 *     8000001EH   extensiones de topologia: hermanos SMT, nodo
 *     80000021H   tamano del parche de microcodigo, predictor de retorno
 *
 * Que no esten aqui NO significa que no existan: significa que su fuente es
 * otra, el manual de AMD, y que hay que tratarla igual que esta -- no de
 * memoria.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef %(guard)s
#define %(guard)s

"""

FOOTER = """
#endif /* %(guard)s */
"""


def emit_range(name, title, items, notes, titles, dry_run):
    """Escribe el fichero de un rango de hojas."""
    path = os.path.join(OUT_DIR, "cpuid_%s.h" % name)
    guard = "VXP_COMMON_CPUID_%s_H" % name.upper()
    out = [HEADER % {"file": "cpuid_%s.h" % name, "title": title,
                     "guard": guard}]

    last = None
    for f in sorted(items, key=lambda x: (x["leaf"], x["sub"] if x["sub"]
                                          is not None else -1, x["reg"],
                                          -x["hi"])):
        key = (f["leaf"], f["sub"])
        if key != last:
            where = ("hoja %02XH" % f["leaf"]) if f["sub"] is None else \
                ("hoja %02XH, subhoja %02XH" % (f["leaf"], f["sub"]))
            # El titulo con que el manual abre la hoja va como CITA: dice donde
            # esta el texto, sin copiarlo.
            cite = titles.get(key)
            if cite:
                out.append("/* ---- %s -- %s ---- */\n\n" % (where, cite))
            else:
                out.append("/* ---- %s ---- */\n\n" % where)
            last = key

        # Un hueco se marca.  Se emite igual que un campo -- no se omite, que
        # dejaria la disposicion del registro con agujeros invisibles -- pero
        # quien lo lea tiene que ver que ahi no hay nada que preguntar, solo
        # bits que no se tocan.
        mark = " -- RESERVADO, no se toca" if f["hole"] else ""

        if f["hi"] == f["lo"]:
            out.append("/** %s bit %d%s */\n" % (f["reg"], f["hi"], mark))
            out.append("#define %s %du\n\n" % (f["macro"], f["hi"]))
        else:
            width = f["hi"] - f["lo"] + 1
            mask = (1 << width) - 1
            out.append("/** %s bits %d:%d%s */\n"
                       % (f["reg"], f["hi"], f["lo"], mark))
            out.append("#define %s_SHIFT %du\n" % (f["macro"], f["lo"]))
            out.append("#define %s_MASK 0x%Xu\n\n" % (f["macro"], mask))

    if notes:
        # Filas que el manual tiene y que no son un campo: remiten a otra tabla
        # que describe el registro entero.  No se pueden definir -- no hay un
        # numero que definir --, pero **se anotan**: omitirlas dejaria la
        # impresion de que ese registro no esta documentado.
        out.append("/*\n"
                   " * Filas del manual que REMITEN A OTRA TABLA en vez de\n"
                   " * definir un campo.  Se listan para que no parezca que\n"
                   " * esos registros no estan documentados.\n"
                   " *\n")
        for leaf, sub, reg, hi, lo, tok in notes:
            where = "%02XH" % leaf
            if sub is not None:
                where += ".%02XH" % sub
            out.append(" *   hoja %-9s %s[%d:%d]  -> %s...\n"
                       % (where, reg, hi, lo, tok))
        out.append(" */\n")

    out.append(FOOTER % {"guard": guard})
    text = "".join(out)
    if not dry_run:
        with io.open(path, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return path, len(items), text


def emit_index(names, conflicting, dry_run):
    """Escribe el fichero que los incluye todos."""
    # Distinta de la de `common/cpuid_index.h`, que es quien lo incluye.  Con
    # la misma, aquel la define primero y este se salta entero, sin error.
    guard = "VXP_COMMON_CPUID_INTEL_INDEX_H"
    out = [HEADER % {"file": "index.h",
                     "title": "Todos los campos de CPUID del manual, por rango.",
                     "guard": guard}]
    for n in names:
        out.append("#include \"cpuid_%s.h\"\n" % n)
    out.append("\n")

    if conflicting:
        out.append(
            "/*\n"
            " * CAMPOS QUE APARECEN CON DOS FORMAS DISTINTAS, y por eso NO se\n"
            " * definen.\n"
            " *\n"
            " * El mismo nombre en la misma hoja y el mismo registro, con otro\n"
            " * rango de bits.  Suele ser una tabla que cambio entre revisiones\n"
            " * del manual y quedaron las dos.  Elegir una seria preguntar por\n"
            " * el bit equivocado en la mitad de las piezas, que es un fallo\n"
            " * callado: sale cero y parece que la capacidad no esta.\n"
            " *\n")
        for macro, shapes in conflicting:
            forms = ", ".join("[%d:%d]" % (hi, lo) for hi, lo in shapes)
            out.append(" *   %-44s %s\n" % (macro, forms))
        out.append(" */\n")

    out.append(FOOTER % {"guard": guard})
    text = "".join(out)
    if not dry_run:
        with io.open(INDEX, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return INDEX, text


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
        table_pages = pages.load_tables()
    except pages.DumpMissing as exc:
        print("gen_cpuid_index: %s" % exc, file=sys.stderr)
        return 1

    fields, skipped, unnamed, leaf_titles = collect(table_pages)
    unique, conflicting = dedupe(fields)

    grouped = {}
    for f in unique:
        grouped.setdefault(group_of(f["leaf"]), []).append(f)

    # Las filas que remiten a otra tabla van al fichero de su rango, para que se
    # vean junto a las hojas de las que hablan.
    noted = {}
    for row in unnamed:
        noted.setdefault(group_of(row[0]), []).append(row)

    if not args.dry_run and not os.path.isdir(OUT_DIR):
        os.makedirs(OUT_DIR)

    leaves = {(f["leaf"], f["sub"]) for f in unique}
    print("paginas leidas:          %d" % len(table_pages))
    print("filas de campo:          %d" % len(fields))
    print("hojas y subhojas:        %d" % len(leaves))
    print("campos definidos:        %d" % len(unique))
    print("con dos formas:          %d  (no se definen; se listan)"
          % len(conflicting))
    print("descartados:             reservado=%d referencia=%d sin_hoja=%d "
          "rango_malo=%d"
          % (skipped["reservado"], skipped["referencia"], skipped["sin_hoja"],
             skipped["rango_malo"]))
    print()

    stale = []
    emitted = []
    for name in GROUP_ORDER:
        if name not in grouped:
            continue
        title = GROUP_TITLE[name]
        path, n, text = emit_range(name, title, grouped[name],
                                   sorted(noted.get(name, [])), leaf_titles,
                                   args.dry_run)
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
        print("=== filas que REMITEN a otra tabla en vez de definir un campo ===")
        for leaf, sub, reg, hi, lo, tok in unnamed[:40]:
            where = "%02XH" % leaf
            if sub is not None:
                where += ".%02XH" % sub
            print("  hoja %-9s %s[%d:%d]  primer token: %r"
                  % (where, reg, hi, lo, tok))
        print("  (%d en total; van anotadas en el fichero generado)"
              % len(unnamed))

    if args.check:
        if stale:
            print()
            print("DESACTUALIZADO.  Regeneralo con:")
            print("    python tools/gen_cpuid_index.py")
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
