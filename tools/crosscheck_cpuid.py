#!/usr/bin/env python3
"""Contrasta nuestras tablas de CPUID contra una tercera fuente independiente.

POR QUE UNA TERCERA FUENTE.  Las nuestras salen de los manuales de Intel y de
AMD, extraidas con lectores que escribimos nosotros.  Eso deja dos modos de
fallar que nada de lo que tenemos puede detectar: que el extractor lea mal una
tabla, y que el manual se equivoque.  Los dos ya han pasado en esta sesion --
ocho registros perdidos por una forma de direccion no contemplada, y setenta y
dos filas del manual con la columna decimal mal.

Una base de datos hecha por otra gente, a partir de los mismos manuales pero con
otro criterio, es el unico modo de cazar eso.  No la sustituye: la CONTRASTA.

QUE COMPARA, Y QUE NO.  **La posicion y la anchura de cada campo**, no el
nombre.  Los nombres difieren por fuerza -- donde el manual escribe `VERSION`,
la base escribe `pmu_version` -- y exigir que coincidan produciria miles de
falsos desacuerdos que esconderian los de verdad.

QUE INFORMA:

    CONFLICTO         el mismo bit de arranque con OTRA anchura.  Uno de los
                      dos esta mal, y hay que mirarlo a mano
    SOLO EN LA BASE   campo que ellos tienen y nosotros no: un hueco nuestro
    SOLO NUESTRO      al reves.  Suele ser un reservado, que ellos no listan

DE DONDE SALEN LOS DATOS.  De la base `x86-cpuid-db`, cuyo XML se lee para sacar
los HECHOS -- hoja, subhoja, registro, bit, anchura --, que es lo mismo que se
hace con los manuales: una posicion de bit no es de nadie.  No se copia su
prosa ni su estructura.

Uso:
    python tools/crosscheck_cpuid.py           el resumen
    python tools/crosscheck_cpuid.py --all     ademas, todo el detalle
"""

import argparse
import io
import os
import re
import sys
import xml.etree.ElementTree as ET

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# La base tampoco viaja en el repositorio.  Se busca en `manual/`, que esta en
# las exclusiones locales, o donde diga la variable de entorno.
DB_DIR = os.environ.get("VESTA_CPUID_DB") or os.path.join(
    ROOT, "manual", "x86-cpuid-db", "db", "xml")

OURS = (
    os.path.join(ROOT, "common", "cpuid", "intel"),
    os.path.join(ROOT, "common", "cpuid", "amd"),
)

REGS = ("eax", "ebx", "ecx", "edx")

# Nuestras macros, de vuelta.  Se leen del FICHERO EMITIDO y no de la logica del
# generador a proposito: lo que hay que contrastar es lo que ve el compilador,
# no lo que el generador creia estar escribiendo.
#
#   CPUID_01_EDX_DS 21u
#   CPUID_07_00_EDX_HYBRID 15u
#   CPUID_0A_EAX_VERSION_SHIFT 0u
#   AMD_CPUID_8000001B_EAX_FetchSam 0u
MACRO_BIT = re.compile(
    r"^#define\s+(?:AMD_)?CPUID_([0-9A-F]{2,8})(?:_([0-9A-F]{2}))?"
    r"_(EAX|EBX|ECX|EDX)(?:_x([0-9A-F]+))?_(\S+?)\s+(\d+)u\s*$")
MACRO_SHIFT = re.compile(
    r"^#define\s+(?:AMD_)?CPUID_([0-9A-F]{2,8})(?:_([0-9A-F]{2}))?"
    r"_(EAX|EBX|ECX|EDX)(?:_x([0-9A-F]+))?_(\S+?)_SHIFT\s+(\d+)u\s*$")
MACRO_MASK = re.compile(
    r"^#define\s+(?:AMD_)?CPUID_([0-9A-F]{2,8})(?:_([0-9A-F]{2}))?"
    r"_(EAX|EBX|ECX|EDX)(?:_x([0-9A-F]+))?_(\S+?)_MASK\s+0x([0-9A-F]+)u\s*$")


def load_db():
    """Los hechos de la base: `(hoja, subhoja, registro, bit) -> (ancho, id)`.

    Solo posiciones, anchuras y nombres.  La descripcion no se toca.
    """
    out = {}
    if not os.path.isdir(DB_DIR):
        raise IOError("no encuentro la base en %s" % DB_DIR)

    for fn in sorted(os.listdir(DB_DIR)):
        if not fn.startswith("leaf_") or not fn.endswith(".xml"):
            continue
        try:
            root = ET.parse(os.path.join(DB_DIR, fn)).getroot()
        except ET.ParseError:
            continue
        leaf = int(root.get("id", "0"), 16)
        for sub in root.iter("subleaf"):
            sid = sub.get("id")
            subleaf = int(sid, 0) if sid is not None else None
            for reg in REGS:
                node = sub.find(reg)
                if node is None:
                    continue
                for child in node:
                    # Los campos son elementos `bitN`.
                    m = re.match(r"^bit(\d+)$", child.tag)
                    if not m:
                        continue
                    lo = int(m.group(1))
                    width = int(child.get("len", "1"))
                    out[(leaf, subleaf, reg.upper(), lo)] = (
                        width, child.get("id", ""))
    return out


def load_ours():
    """Nuestros campos, leidos de los ficheros generados."""
    out = {}
    pending_shift = {}

    for d in OURS:
        if not os.path.isdir(d):
            continue
        for fn in sorted(os.listdir(d)):
            if not fn.endswith(".h"):
                continue
            with io.open(os.path.join(d, fn), encoding="ascii") as f:
                for line in f:
                    line = line.rstrip("\n")

                    m = MACRO_SHIFT.match(line)
                    if m:
                        key = (int(m.group(1), 16),
                               int(m.group(2), 16) if m.group(2) else None,
                               m.group(3), m.group(5))
                        pending_shift[key] = int(m.group(6))
                        continue

                    m = MACRO_MASK.match(line)
                    if m:
                        key = (int(m.group(1), 16),
                               int(m.group(2), 16) if m.group(2) else None,
                               m.group(3), m.group(5))
                        lo = pending_shift.pop(key, None)
                        if lo is None:
                            continue
                        mask = int(m.group(6), 16)
                        width = mask.bit_length()
                        out[(key[0], key[1], key[2], lo)] = (width, key[3])
                        continue

                    m = MACRO_BIT.match(line)
                    if m:
                        lo = int(m.group(6))
                        out[(int(m.group(1), 16),
                             int(m.group(2), 16) if m.group(2) else None,
                             m.group(3), lo)] = (1, m.group(5))
    return out


def is_hole(name):
    """¿Es un hueco nuestro, de los que la base no lista?"""
    return name.startswith("RESERVED") or name.startswith("NA_")


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--all", action="store_true", help="todo el detalle")
    args = ap.parse_args()

    try:
        db = load_db()
    except IOError as exc:
        print("crosscheck_cpuid: %s" % exc, file=sys.stderr)
        return 1
    ours = load_ours()

    # La subhoja se compara con holgura: el manual no siempre la numera y
    # nosotros la dejamos sin numerar cuando es parametrica.  Exigir que
    # coincida produciria desacuerdos que no son de dato, sino de notacion.
    def loose(key):
        leaf, sub, reg, lo = key
        return (leaf, reg, lo)

    db_loose = {}
    for k, v in db.items():
        db_loose.setdefault(loose(k), []).append(v)
    ours_loose = {}
    for k, v in ours.items():
        ours_loose.setdefault(loose(k), []).append(v)

    conflicts, only_db, only_ours, agree = [], [], [], 0

    for k in sorted(set(db_loose) | set(ours_loose)):
        d = db_loose.get(k)
        o = ours_loose.get(k)
        if d and o:
            dw = {w for w, _ in d}
            ow = {w for w, _ in o}
            if dw & ow:
                agree += 1
            else:
                # Se clasifica el desacuerdo, porque NO son la misma cosa y
                # meterlos en el mismo saco esconde el unico que importa:
                #
                #   hueco    nosotros tenemos ahi un RESERVADO y ellos un campo
                #            con nombre.  Suele ser una funcion que el manual
                #            marco reservada al retirarla y la base conserva.
                #            Los dos aciertan, en revisiones distintas
                #   real     los dos le dan nombre y no coinciden en la anchura.
                #            Ahi uno de los dos esta mal
                #   subdivide  la base parte en varios campos lo que el manual
                #              presenta entero.  `EAX[31:24] DESCRIPTOR_3` del
                #              manual son, para la base, `desc3`[30:24] mas
                #              `eax_invalid`[31].  Ninguna miente: estan a
                #              resoluciones distintas, y decir "conflicto"
                #              esconderia los que si lo son
                if all(is_hole(n) for _, n in o):
                    kind = "hueco"
                elif max(ow) > max(dw):
                    kind = "subdivide"
                else:
                    kind = "real"
                conflicts.append((k, sorted(dw), sorted(ow),
                                  d[0][1], o[0][1], kind))
        elif d:
            only_db.append((k, d[0][0], d[0][1]))
        else:
            if not all(is_hole(n) for _, n in o):
                only_ours.append((k, o[0][0], o[0][1]))

    print("=== contraste de CPUID contra x86-cpuid-db ===")
    print()
    print("campos en la base:        %d" % len(db))
    print("campos nuestros:          %d" % len(ours))
    print()
    print("coinciden en posicion:    %d" % agree)
    real = [c for c in conflicts if c[5] == "real"]
    hollow = [c for c in conflicts if c[5] == "hueco"]
    finer = [c for c in conflicts if c[5] == "subdivide"]
    print("CONFLICTO real:           %d  (los dos nombran, y no coinciden)"
          % len(real))
    print("  la base subdivide:      %d  (misma zona, mas resolucion)"
          % len(finer))
    print("  donde tenemos hueco:    %d  (nuestro reservado, su campo)"
          % len(hollow))
    print("solo en la base:          %d  (huecos nuestros)" % len(only_db))
    print("solo nuestros:            %d  (sin contar reservados)"
          % len(only_ours))

    if real:
        print()
        print("=== CONFLICTOS REALES: los dos nombran el campo y discrepan ===")
        print("   Uno de los dos esta mal.  El manual es el desempate.")
        for (leaf, reg, lo), dw, ow, dn, on, _ in real:
            print("  Fn%08X %s[%d]  base=%s (%s)  nuestro=%s (%s)"
                  % (leaf, reg, lo, dw, dn, ow, on))

    if hollow and args.all:
        print()
        print("=== donde nosotros tenemos RESERVADO y la base un campo ===")
        print("   Normalmente el manual retiro la funcion y la marco reservada,")
        print("   y la base la conserva.  No es un fallo: son dos revisiones.")
        for (leaf, reg, lo), dw, ow, dn, on, _ in hollow:
            print("  Fn%08X %s[%d]  base=%s (%s)  nuestro=%s"
                  % (leaf, reg, lo, dw, dn, ow))

    lim = None if args.all else 25
    if only_db:
        print()
        print("=== SOLO EN LA BASE: campos que nos faltan ===")
        for (leaf, reg, lo), w, n in only_db[:lim]:
            print("  Fn%08X %s[%d:%d]  %s" % (leaf, reg, lo + w - 1, lo, n))
        if lim and len(only_db) > lim:
            print("  ... y %d mas  (--all para verlos)" % (len(only_db) - lim))

    if only_ours:
        print()
        print("=== SOLO NUESTROS: campos que la base no trae ===")
        for (leaf, reg, lo), w, n in only_ours[:lim]:
            print("  Fn%08X %s[%d:%d]  %s" % (leaf, reg, lo + w - 1, lo, n))
        if lim and len(only_ours) > lim:
            print("  ... y %d mas  (--all para verlos)" % (len(only_ours) - lim))

    return 0


if __name__ == "__main__":
    sys.exit(main())
