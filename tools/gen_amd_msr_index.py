#!/usr/bin/env python3
"""Genera la tabla de direcciones de MSR de AMD, de su PPR.

POR QUE ES UN GENERADOR APARTE Y NO UN PARAMETRO DEL DE INTEL.  Porque la fuente
es otra y el formato tambien.  AMD declara un registro asi:

    MSRC001_1030 [IBS Fetch Control] (Core::X86::Msr::IBS_FETCH_CTL)

-- direccion, descripcion y nombre simbolico en una linea --, mientras que Intel
lo hace con una cabecera de dos columnas y la direccion repetida en decimal.
Meter las dos en un extractor con un `if` dentro seria una funcion que no
entiende ninguna de las dos del todo.

Y hay una razon de fondo, que es la que decide el reparto en directorios: **los
dos espacios de direcciones se solapan sin coincidir**.  `0xC0000080` es
`IA32_EFER` para Intel y `EFER` para AMD -- el mismo registro, dos nombres y dos
fuentes --, mientras que `0xC0011030` solo existe en AMD.  Mezclarlos en un
fichero seria perder de quien es cada cosa.

QUE PRODUCE
-----------
Un fichero por familia en `common/msr/amd/`, mas su `index.h`.  Solo
DIRECCIONES, igual que el de Intel: los campos de bits de lo que se decodifique
se escriben a mano, porque llevan el porque de cada uno.

DE DONDE SALE
-------------
Del Processor Programming Reference de AMD, que es donde documenta sus MSR.  Es
un PDF y no viaja en el repositorio; se vuelca a texto y `VESTA_AMD_DIR` dice
donde esta:

    pdftotext -table <ppr.pdf> amd_ppr.table.txt

LO QUE TRAE Y NO TENIAMOS
-------------------------
El juego de registros de IBS al completo.  IBS es el mecanismo de muestreo
preciso de AMD y el plan lo pone al mismo nivel que PEBS -- no como degradacion
--, asi que sin estas direcciones la mitad AMD del perfilador se construiria
adivinando.

Uso:
    python tools/gen_amd_msr_index.py            genera
    python tools/gen_amd_msr_index.py --dry-run  cuenta y no escribe
    python tools/gen_amd_msr_index.py --check    falla si el arbol esta viejo
    python tools/gen_amd_msr_index.py --report   el detalle de lo descartado
"""

import argparse
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR = os.path.join(ROOT, "common", "msr", "amd")

# El volcado del PPR.  Fuera del arbol, como el de Intel: son megas derivados de
# un PDF que tampoco se publica.  Se busca en `manual/`, que esta en las
# exclusiones locales, o donde diga la variable de entorno -- la misma
# convencion que usa el lector de `tools/sdm/`.
MANUAL_DIR = os.environ.get("VESTA_AMD_DIR") or os.path.join(ROOT, "manual")
TABLE_DUMP = os.path.join(MANUAL_DIR, "amd_ppr.table.txt")

# `pdftotext` emite Latin-1.  Leerlo como UTF-8 revienta en el primer simbolo
# raro, y el PPR esta lleno de ellos.
ENCODING = "latin-1"

# La declaracion de un registro.  Una linea, tres datos:
#
#   MSRC001_1030 [IBS Fetch Control] (Core::X86::Msr::IBS_FETCH_CTL)
#
# El guion bajo agrupa cifras para leerlas; no es parte del numero.
DECL = re.compile(
    r"^MSR([0-9A-F]{4})_([0-9A-F]{4})\s+\[([^\]]+)\]\s+"
    r"\(Core::X86::Msr::([A-Za-z0-9_]+)\)\s*$")

# Una declaracion sin el nombre entre parentesis.  Se cuenta aparte: no se puede
# definir una macro sin nombre, pero saber cuantas hay dice si el patron se esta
# quedando corto.
DECL_NONAME = re.compile(r"^MSR([0-9A-F]{4})_([0-9A-F]{4})\s+\[([^\]]+)\]\s*$")

# El reparto en ficheros, por FUNCION.  Sale de mirar los nombres que usa AMD,
# que no se parecen a los de Intel: donde Intel escribe `IA32_PERF_GLOBAL_CTRL`,
# AMD escribe `PerfCntrGlobalCtl`.  Escribir el mapa de memoria habria dejado
# media tabla en `misc`.
#
# El orden importa: gana la primera que encaja, asi que lo especifico va antes.
FAMILIES = (
    ("ibs", "Instruction-Based Sampling: el muestreo preciso de AMD",
     r"IBS|BP_IBSTGT|IC_IBS"),
    ("perf", "Contadores de rendimiento, ramas y depuracion",
     r"PERF|PMC|PerfCntr|PerformanceCounter|IRPerfCount|APerfReadOnly"
     r"|MPerfReadOnly|BR_FROM|BR_TO|LastBranch|LastExcp|DBG_CTL|DebugExtnCtl"
     r"|QM_CTR|QM_EVTSEL|CTL[0-9]$|CTR[0-9]$"),
    ("mcheck", "Comprobacion de maquina",
     r"^MC[0-9_]|MCA|MCG|McExcepRedir|McaIntrCfg"),
    ("apic", "APIC, interrupciones y temporizadores locales",
     r"APIC|Apic|EOI|ESR|ErrorLvtEntry|InterruptCommand|IntPend|^LDR$"
     r"|ProcessorPriority|SelfIPI|SpecificEndOfInterrupt|^SVR$|Timer"
     r"|ThermalLvtEntry|^TPR$|ArbitrationPriority|SecureAVIC|AvicDoorbell"
     r"|InterruptEnable"),
    ("power", "Estados de rendimiento, energia y control termico",
     r"Cppc|PState|CState|PWR|THERM|HWCR|RAPL|ENERGY|LIMIT"),
    ("memory", "Memoria: MTRR, tipos de pagina y ventanas",
     r"MTRR|Mtrr|TOM|TOP_MEM|SYS_CFG|TSEG|^PAT$|MmioCfgBaseAddr"
     r"|PrefetchControl"),
    ("virt", "Virtualizacion y modo de gestion del sistema",
     r"SVM|VM_|NPT|HSAVE|GUEST_TSC|TscRateMsr|SMM|SMI_ON_IO|SmiTrigIo"),
    ("security", "Mitigaciones, pila de sombra e identificacion de pieza",
     r"SPEC_CTRL|PRED_CMD|FLUSH_CMD|_CET$|Ssp|PPIN|OSVW"),
)

FAMILY_TITLE = dict((n, t) for n, t, _ in FAMILIES)
FAMILY_TITLE["platform"] = "Plataforma: llamada al sistema, bases y marca de tiempo"
FAMILY_ORDER = [f[0] for f in FAMILIES] + ["platform"]

_COMPILED = [(n, re.compile(p)) for n, _, p in FAMILIES]


def family_of(name):
    """A que fichero va un registro."""
    for fam, pat in _COMPILED:
        if pat.search(name):
            return fam
    # Lo que queda es la plataforma: EFER, las de llamada al sistema, las bases
    # de segmento, el contador de marca de tiempo.  No es un cajon de sastre --
    # es una familia de verdad, y por eso tiene nombre propio en vez de `misc`.
    return "platform"


def load_pages():
    """Devuelve el volcado del PPR partido en paginas."""
    if not os.path.exists(TABLE_DUMP):
        raise IOError(
            "falta %s.\nGeneralo con:\n"
            "    pdftotext -table <ppr.pdf> %s" % (TABLE_DUMP, TABLE_DUMP))
    with io.open(TABLE_DUMP, encoding=ENCODING) as f:
        return f.read().split("\f")


def collect(pages_text):
    """Recorre el volcado y devuelve las declaraciones y lo descartado.

    Se cuenta lo que se descarta.  Es lo que caza un patron que se queda corto:
    en la tabla de Intel aparecieron ocho registros perdidos en silencio, y solo
    se vieron porque el contador de descartes no cuadraba.
    """
    entries = []
    skipped = {"sin_nombre": 0}
    noname = []

    for page in pages_text:
        for line in page.splitlines():
            line = line.rstrip()
            m = DECL.match(line)
            if m:
                addr = int(m.group(1) + m.group(2), 16)
                entries.append({
                    "addr": addr,
                    "desc": m.group(3).strip(),
                    "name": m.group(4),
                })
                continue
            m = DECL_NONAME.match(line)
            if m:
                skipped["sin_nombre"] += 1
                noname.append((int(m.group(1) + m.group(2), 16),
                               m.group(3).strip()))

    return entries, skipped, noname


def dedupe(entries):
    """Une repeticiones y separa lo que choca.

    El PPR repite una declaracion cuando la tabla cruza un salto de pagina.  Lo
    que importa son los nombres que aparecen con OTRA direccion: ahi elegir una
    seria publicar la equivocada, asi que no se define ninguna y se listan.
    """
    by_name = {}
    for e in entries:
        by_name.setdefault(e["name"], []).append(e)

    unique, conflicting = [], []
    for name, group in sorted(by_name.items()):
        addrs = sorted({g["addr"] for g in group})
        if len(addrs) == 1:
            unique.append(group[0])
        else:
            conflicting.append((name, addrs))
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
 *     python tools/gen_amd_msr_index.py
 *
 * a partir del Processor Programming Reference de AMD, que no viaja en el
 * repositorio.
 *
 * ALCANCE: **SOLO AMD**.  Los MSR de Intel estan en `common/msr/intel/`, y no
 * se mezclan porque los dos espacios se solapan sin coincidir: `0xC0000080` es
 * `IA32_EFER` para Intel y `EFER` para AMD -- el mismo registro con dos nombres
 * y dos fuentes --, mientras que `0xC0011030` solo existe aqui.
 *
 * Los nombres son los del PPR, para que se pueda buscar en el la misma cadena
 * que hay aqui.  Solo direcciones: los campos de bits de lo que se decodifique
 * se escriben a mano.
 *
 * No incluye nada, a proposito: solo define macros.
 */

#ifndef %(guard)s
#define %(guard)s

"""

FOOTER = """
#endif /* %(guard)s */
"""


def emit_family(family, items, dry_run):
    """Escribe el fichero de una familia."""
    path = os.path.join(OUT_DIR, "msr_amd_%s.h" % family)
    guard = "VXP_COMMON_MSR_AMD_%s_H" % family.upper()
    out = [HEADER % {"file": "msr_amd_%s.h" % family,
                     "title": FAMILY_TITLE[family], "guard": guard}]

    for e in sorted(items, key=lambda x: (x["addr"], x["name"])):
        # La descripcion que da el PPR se usa como rotulo corto.  Es un nombre
        # de registro, no prosa: `IBS Fetch Control`.
        out.append("/** %s */\n" % e["desc"])
        out.append("#define AMD_%s 0x%Xu\n\n" % (e["name"], e["addr"]))

    out.append(FOOTER % {"guard": guard})
    text = "".join(out)
    if not dry_run:
        with io.open(path, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return path, len(items), text


def emit_index(families, conflicting, dry_run):
    """Escribe el `index.h` de AMD."""
    path = os.path.join(OUT_DIR, "index.h")
    guard = "VXP_COMMON_MSR_AMD_INDEX_H"
    out = [HEADER % {"file": "index.h",
                     "title": "Todos los MSR del PPR de AMD, por familia.",
                     "guard": guard}]
    for fam in families:
        out.append("#include \"msr_amd_%s.h\"\n" % fam)
    out.append("\n")

    if conflicting:
        out.append(
            "/*\n"
            " * NOMBRES CON MAS DE UNA DIRECCION, y por eso NO se definen.\n"
            " *\n"
            " * Suele ser un registro que cambio de sitio entre familias de\n"
            " * procesador.  Elegir una direccion seria publicar la equivocada\n"
            " * para la mitad de las piezas.\n"
            " *\n")
        for name, addrs in conflicting:
            out.append(" *   %-32s %s\n"
                       % (name, ", ".join("0x%X" % a for a in addrs)))
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
        print("gen_amd_msr_index: %s" % exc, file=sys.stderr)
        return 1

    entries, skipped, noname = collect(pages_text)
    unique, conflicting = dedupe(entries)

    grouped = {}
    for e in unique:
        grouped.setdefault(family_of(e["name"]), []).append(e)

    if not args.dry_run and not os.path.isdir(OUT_DIR):
        os.makedirs(OUT_DIR)

    print("paginas leidas:            %d" % len(pages_text))
    print("declaraciones:             %d" % len(entries))
    print("nombres unicos definidos:  %d" % len(unique))
    print("nombres en conflicto:      %d  (no se definen; se listan)"
          % len(conflicting))
    print("declaradas sin nombre:     %d" % skipped["sin_nombre"])
    print()

    stale = []
    emitted = []
    for fam in FAMILY_ORDER:
        if fam not in grouped:
            continue
        path, n, text = emit_family(fam, grouped[fam], args.dry_run)
        if args.check and differs(path, text):
            stale.append(path)
        emitted.append(fam)
        print("  %-28s %4d" % (os.path.basename(path), n))
    path, text = emit_index(emitted, conflicting, args.dry_run)
    if args.check and differs(path, text):
        stale.append(path)
    print("  %-28s" % "index.h")

    if args.report:
        print()
        print("=== declaradas sin nombre simbolico ===")
        for addr, desc in noname[:40]:
            print("  0x%-10X %s" % (addr, desc))
        print("  (%d en total)" % len(noname))

    if args.check:
        if stale:
            print()
            print("DESACTUALIZADO.  Regeneralo con:")
            print("    python tools/gen_amd_msr_index.py")
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
