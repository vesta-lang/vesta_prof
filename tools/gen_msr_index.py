#!/usr/bin/env python3
"""Genera la tabla de direcciones de MSR a partir del manual de Intel.

QUE PRODUCE
-----------
Un fichero por familia funcional en `common/msr/`, mas `common/msr_index.h` que
los incluye todos.  Solo DIRECCIONES: los campos de bits de los registros que el
perfilador decodifica se escriben a mano en `common/msr.h`, porque llevan el
porque de cada uno y eso no sale de una tabla.

COMO SE PREPARA
---------------
El manual es un PDF y no viaja en el repositorio.  Se vuelca a texto una vez, en
dos modos, y la variable `VESTA_SDM_DIR` dice donde estan:

    pdftotext -layout <manual.pdf> sdm.txt
    pdftotext -table  <manual.pdf> sdm.table.txt

Aqui solo se usa el de modo tabla: lo que se extrae son tablas, y el volcado
normal desincroniza sus columnas.  La regla es del importador de la web, de
donde viene el paquete `sdm/`, y mezclarlo al reves publica datos falsos que no
parecen falsos.

LO QUE NO SE COPIA
------------------
Direcciones, nombres y disposiciones de bits son HECHOS y se extraen.  La prosa
descriptiva del manual es texto de Intel y **no se copia**: lo que sale al
fichero generado es el nombre, la direccion, la tabla de la que viene y la
condicion de existencia, que son datos.

LO QUE EL MANUAL SE EQUIVOCA
----------------------------
Cada cabecera trae la direccion dos veces, en hexadecimal y en decimal, y en 72
casos **no coinciden**.  No es el volcado: los dos volcados dicen lo mismo, y
`IA32_PMC_FX1_CFG_C` llega a aparecer con 6532 y con 6535 para la misma 1987H
(que es 6535).  De modo que **manda el hexadecimal**, el decimal vale como
comprobacion, y los desacuerdos se MARCAN en el fichero generado en vez de
resolverse en silencio eligiendo uno.

Uso:
    python tools/gen_msr_index.py            genera
    python tools/gen_msr_index.py --dry-run  cuenta y no escribe
    python tools/gen_msr_index.py --report   ademas, el detalle de lo raro
"""

import argparse
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from sdm import pages  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# Los de Intel van en su propio subdirectorio, y los de AMD en el suyo.  No es
# orden: los dos espacios de direcciones **se solapan sin coincidir**.
# `0xC0000080` es `IA32_EFER` para Intel y `EFER` para AMD -- el mismo registro
# con dos nombres y dos fuentes --, mientras que `0xC0011030` solo existe en
# AMD.  Mezclarlos seria perder de quien es cada cosa.
OUT_DIR = os.path.join(ROOT, "common", "msr", "intel")
INDEX = os.path.join(OUT_DIR, "index.h")

# La cabecera de cada registro dentro de las tablas de MSR.  Cuatro formas, y
# las cuatro son legitimas -- tratarlas como una sola es de donde salen las
# entradas inventadas:
#
#   Register Address: 1BH, 27                 IA32_APIC_BASE (APIC_BASE)
#   Register Address: 180H-185H, 384-389      N/A                    <- rango
#   Register Address: C90H+n, 3216+n          IA32_L3_MASK_n         <- indexada
#   Register Address:                         IA32_PMC_GPn_CFG_C     <- sin direccion
SINGLE = re.compile(r"^Register Address:\s*([0-9A-F]+)H,\s*([0-9]+)\s+(\S.*?)\s*$")
RANGE = re.compile(
    r"^Register Address:\s*([0-9A-F]+)H-([0-9A-F]+)H,\s*[0-9]+-[0-9]+\s+(\S.*?)\s*$")
INDEXED = re.compile(
    r"^Register Address:\s*([0-9A-F]+)H\+n,\s*[0-9]+\+n\s+(\S.*?)\s*$")
# Y una cuarta, la del rango alto: sin columna decimal y con el guion bajo de
# agrupacion.  Es la de `IA32_EFER`, las de llamada al sistema y `IA32_TSC_AUX`,
# que es la que `rdtscp` devuelve como identificador de nucleo.  Se descubrio
# contando lo descartado: sin esta forma se perdian ocho registros
# arquitectonicos EN SILENCIO.
#
#   Register Address: C000_0080H                     IA32_EFER
NODEC = re.compile(r"^Register Address:\s*([0-9A-F_]+)H\s+(\S.*?)\s*$")

# Y la forma con la LETRA O donde iba el digito CERO: `7BOH, 1968`.  Es un
# defecto del manual (o de su capa de texto), y aqui es donde la columna decimal
# deja de ser redundante y se gana el sueldo: 1968 es 0x7B0, asi que **el
# decimal reconstruye el hexadecimal corrupto**.
#
# Matiza la regla: el hexadecimal manda cuando los dos son legibles; cuando uno
# se corrompe, el otro lo recupera.  Y se marca, porque un dato reconstruido no
# es lo mismo que uno leido.
TYPO = re.compile(r"^Register Address:\s*([0-9A-FO_]+)H,\s*([0-9]+)\s+(\S.*?)\s*$")

BARE = re.compile(r"^Register Address:\s*(\S.*?)\s*$")

# LA PUERTA: la condicion de CPUID bajo la que el registro existe.
#
# Es el dato que decide si un driver puede leerlo, y por tanto no es un adorno
# del registro: **es documentacion en si mismo**.  Leer un MSR que no existe
# provoca una excepcion de proteccion general, y en kernel eso es un pantallazo.
#
# Se extrae ESTRUCTURADA -- hoja, subhoja, registro y bit -- y no como texto,
# porque asi el codigo puede comprobarla en vez de que alguien la lea y la
# transcriba.  Transcribir es donde se cuelan los errores.
#
# El manual la escribe de cuatro formas y esta expresion cubre las cuatro:
#
#   CPUID.01H:ECX[15]                       la comun
#   CPUID.0AH:EAX[7:0]                      un rango en vez de un bit
#   CPUID.07H.00H:EBX[1]                    con subhoja
#   CPUID.10H.01H:EDX.CAT_MAX_CLOS[15:0]    con el nombre del campo por medio
GATE = re.compile(
    r"CPUID\.([0-9A-F]+)H(?:\.([0-9A-F]+)H)?:(E[A-D]X)"
    r"(?:\.[A-Za-z0-9_]+)?\[(\d+)(?::(\d+))?\]")

# El indice de registro que se emite, para que la puerta sea comprobable sin
# tener que interpretar una cadena.
REG_INDEX = {"EAX": 0, "EBX": 1, "ECX": 2, "EDX": 3}

# El titulo de tabla que precede a un bloque de registros.  Da el CONTEXTO, que
# es lo que distingue dos registros con el mismo nombre y distinta direccion --
# los bancos de comprobacion de maquina se rebasan por familia, y 266 nombres
# estan en ese caso.
TABLE_TITLE = re.compile(r"^\s*Table\s+([0-9]+-[0-9]+)\.\s+(\S.*?)\s*(?:\.\.+.*)?$")

# El nombre suele traer el antiguo entre parentesis: `IA32_APIC_BASE (APIC_BASE)`.
FORMER = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)\s*\(([^)]*)\)\s*$")

# Un nombre utilizable como macro de C.
IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")

# Lo que NO es un registro aunque aparezca en la columna del nombre.
NOT_A_NAME = ("N/A", "Reserved", "Architectural MSR Name", "Hex,")


def family_of(name, title):
    """A que fichero va un registro.

    El reparto es FUNCIONAL y no por rango de direcciones, porque quien busca un
    registro lo busca por lo que hace.  El orden importa: la primera que encaja
    gana, asi que lo especifico va antes que lo general.
    """
    n = name.upper()
    t = (title or "").upper()

    # Rendimiento primero: es la razon de ser de este proyecto y no quiero que
    # un registro del PMU acabe en `misc` por una coincidencia de nombre.
    if re.search(r"PERFEVTSEL|PERF_GLOBAL|PERF_CAPABILITIES|PERF_METRICS"
                 r"|_PMC|PMC_|FIXED_CTR|PEBS|DS_AREA|DEBUGCTL|LBR|LER_"
                 r"|RTIT|LASTBRANCH|LASTINT|BR_SELECT|PERF_FIXED", n):
        return "perf"
    if re.search(r"MC[0-9]+_|MCG_|MCI_|MACHINE.CHECK", n) or "MACHINE-CHECK" in t:
        return "mcheck"
    if re.search(r"VMX|SMBASE|SMRR|SMM_", n):
        return "virt"
    if re.search(r"SGX|CET_|ARCH_CAPABILITIES|CORE_CAPABILITIES|SPEC_CTRL"
                 r"|PRED_CMD|FLUSH_CMD|TSX|MKTME|TME_|PASID|U_CET|S_CET", n):
        return "security"
    if re.search(r"MTRR|PAT\b|SMRR|CR_PAT|L3_MASK|L2_MASK|QOS|PQR|RMID"
                 r"|CACHE|MISC_PACKAGE_CTLS", n):
        return "memory"
    if re.search(r"THERM|POWER|ENERGY|PERF_CTL|PERF_STATUS|APERF|MPERF|PPERF"
                 r"|HWP|PM_ENABLE|RAPL|PKG_|DRAM_|PL1|PL2|EPB|FREQ|TURBO"
                 r"|CLOCK_MODULATION|HW_FEEDBACK", n):
        return "power"
    if re.search(r"APIC|PLATFORM_ID|PLATFORM_INFO|BIOS_|UCODE|MICROCODE"
                 r"|FEATURE_CONTROL|TSC|TIME_STAMP|EFER|STAR|FMASK|FS_BASE"
                 r"|GS_BASE|SYSENTER|MISC_ENABLE|BARRIER|MONITOR_FILTER", n):
        return "platform"
    if n.startswith("IA32_"):
        return "misc"
    # Lo que no es arquitectonico y no encajo en ninguna familia: son los
    # nombres antiguos de P5/P6 y los model-specific de familias retiradas.
    return "legacy"


FAMILY_TITLE = {
    "perf": "Rendimiento: contadores, PEBS, LBR, Intel PT y Debug Store",
    "power": "Frecuencia, energia y control termico",
    "memory": "Memoria: MTRR, PAT, cifrado y reparto de cache",
    "virt": "Virtualizacion y modo de gestion del sistema",
    "security": "Capacidades y mitigaciones",
    "mcheck": "Comprobacion de maquina",
    "platform": "Plataforma, APIC, microcodigo y llamada al sistema",
    "misc": "Arquitectonicos que no caen en ninguna familia",
    "legacy": "Model-specific antiguos y nombres retirados",
}


def clean_name(raw):
    """Separa el nombre del antiguo, y dice si sirve como macro.

    Devuelve `(nombre, antiguo)` o `(None, None)` si eso no era un nombre de
    registro -- las cabeceras de tabla repetidas en cada pagina y las filas
    `N/A` de los rangos reservados caen aqui.
    """
    raw = raw.strip()
    for bad in NOT_A_NAME:
        if raw.startswith(bad):
            return None, None
    m = FORMER.match(raw)
    former = None
    if m:
        raw, former = m.group(1), m.group(2).strip()
    raw = raw.strip()
    if not IDENT.match(raw):
        return None, None
    return raw, former


def gate_after(lines, idx, decl_at):
    """La condicion de existencia del registro declarado en `lines[idx]`.

    Devuelve `{leaf, sub, reg, hi, lo}` o `None` si el manual no la dice ahi.

    Se busca SOLO dentro del bloque del registro -- hasta la siguiente
    declaracion --, y ese limite no es una optimizacion: sin el se le
    atribuiria la condicion del registro de abajo.  Ese fallo no da un error,
    da una puerta que parece razonable y no lo es, que es exactamente la clase
    de dato falso que este generador existe para no producir.
    """
    end = len(lines)
    for j in decl_at:
        if j > idx:
            end = j
            break
    m = GATE.search(" ".join(lines[idx:end]))
    if not m:
        return None
    hi = int(m.group(4))
    lo = int(m.group(5)) if m.group(5) else hi
    return {
        "leaf": int(m.group(1), 16),
        "sub": int(m.group(2), 16) if m.group(2) else None,
        "reg": m.group(3),
        "hi": hi,
        "lo": lo,
    }


def collect(table_pages):
    """Recorre las paginas y devuelve las entradas y lo que no se entendio.

    Se cuenta lo que se descarta y por que.  Un lector sobre una fuente que no
    controlamos falla en silencio si no se le pide cuentas, y esta tiene 5.343
    paginas.
    """
    entries = []
    skipped = {"sin_nombre": 0, "sin_direccion": 0, "rango": 0,
               "hex_ilegible": 0}
    mismatched = []
    unreadable = []   # hexadecimal corrupto que el decimal NO confirma
    formula = set()   # nombre presente, direccion definida por formula aparte
    title = None

    for page in table_pages:
        lines = page.splitlines()
        # Donde empieza cada declaracion.  Hace falta para ACOTAR el bloque de
        # cada registro: la puerta se busca solo dentro del suyo.  Sin ese
        # limite se le atribuiria la condicion del registro siguiente, y ese
        # error no falla -- da una puerta plausible y equivocada, que es peor.
        decl_at = [i for i, l in enumerate(lines)
                   if l.lstrip().startswith("Register Address:")]
        for idx, line in enumerate(lines):
            t = TABLE_TITLE.match(line)
            if t:
                # Se guarda el NUMERO de tabla ademas del titulo: es lo que
                # convierte el comentario en una cita con la que se llega al
                # sitio exacto del manual en un paso.  El "(Contd.)" que el
                # manual repite en cada pagina se quita: es paginacion, no dato.
                num = t.group(1)
                name = re.sub(r"\s*\(Contd\.?\)\s*$", "", t.group(2).strip())
                title = "Table %s. %s" % (num, name)
                continue
            if not line.lstrip().startswith("Register Address:"):
                continue
            line = line.strip()

            m = RANGE.match(line)
            if m:
                # Un rango reservado no es un registro.  Se cuenta y se deja:
                # expandirlo seria inventar entradas que el manual no nombra.
                skipped["rango"] += 1
                continue

            m = INDEXED.match(line)
            if m:
                name, former = clean_name(m.group(2))
                if name is None:
                    skipped["sin_nombre"] += 1
                    continue
                entries.append({
                    "name": name, "former": former, "base": int(m.group(1), 16),
                    "kind": "indexed", "title": title, "dec_ok": True,
                    "gate": gate_after(lines, idx, decl_at),
                })
                continue

            m = SINGLE.match(line)
            if m:
                name, former = clean_name(m.group(3))
                if name is None:
                    skipped["sin_nombre"] += 1
                    continue
                addr = int(m.group(1), 16)
                dec = int(m.group(2))
                ok = (addr == dec)
                if not ok:
                    mismatched.append((name, addr, dec))
                entries.append({
                    "name": name, "former": former, "base": addr,
                    "kind": "single", "title": title, "dec_ok": ok, "dec": dec,
                    "gate": gate_after(lines, idx, decl_at),
                })
                continue

            m = NODEC.match(line)
            if m:
                name, former = clean_name(m.group(2))
                if name is None:
                    skipped["sin_nombre"] += 1
                    continue
                # El guion bajo solo agrupa cifras para leerlas; no es parte
                # del numero.
                entries.append({
                    "name": name, "former": former,
                    "base": int(m.group(1).replace("_", ""), 16),
                    "kind": "single", "title": title, "dec_ok": True,
                    "gate": gate_after(lines, idx, decl_at),
                })
                continue

            m = TYPO.match(line)
            if m:
                name, former = clean_name(m.group(3))
                if name is None:
                    skipped["sin_nombre"] += 1
                    continue
                dec = int(m.group(2))
                fixed = int(m.group(1).replace("_", "").replace("O", "0"), 16)
                if fixed != dec:
                    # Se corrompio y el decimal NO lo confirma: no hay dos
                    # fuentes de acuerdo, asi que no se inventa una direccion.
                    skipped["hex_ilegible"] += 1
                    unreadable.append((name, m.group(1), dec))
                    continue
                entries.append({
                    "name": name, "former": former, "base": fixed,
                    "kind": "single", "title": title, "dec_ok": True,
                    "recovered": m.group(1),
                    "gate": gate_after(lines, idx, decl_at),
                })
                continue

            m = BARE.match(line)
            if m:
                # Registros cuyo nombre esta pero cuya direccion se define por
                # formula en otro sitio.  Se listan; omitirlos sin decirlo es
                # como se pierde un registro sin que nadie se entere.
                skipped["sin_direccion"] += 1
                nm, _ = clean_name(m.group(1))
                if nm:
                    formula.add(nm)

    return entries, skipped, mismatched, unreadable, sorted(formula)


def dedupe(entries):
    """Une las repeticiones y separa las que de verdad chocan.

    El mismo registro aparece en varias tablas -- una por familia de
    procesador --, asi que la mayoria de las repeticiones son identicas y se
    funden.  Las que traen OTRA direccion con el mismo nombre son las que
    importan: se conservan las dos y se anota, porque elegir una seria publicar
    una direccion equivocada para la mitad de las piezas.
    """
    by_name = {}
    for e in entries:
        by_name.setdefault(e["name"], []).append(e)

    unique, conflicting = [], []
    for name, group in sorted(by_name.items()):
        addrs = sorted({g["base"] for g in group})
        if len(addrs) == 1:
            best = group[0]
            # De las repeticiones se queda la que trae MAS informacion.  La
            # puerta pesa mas que el nombre antiguo: una es lo que decide si el
            # registro se puede leer, el otro es una nota historica.
            for g in group:
                if g.get("gate"):
                    best = g
                    break
            else:
                for g in group:
                    if g["former"]:
                        best = g
                        break
            best["seen"] = len(group)
            unique.append(best)
        else:
            conflicting.append((name, group))
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
 *     python tools/gen_msr_index.py
 *
 * a partir del volcado de texto del manual de Intel, que no viaja en el
 * repositorio.  Editarlo aqui se pierde en la siguiente regeneracion, y ademas
 * separa el fichero de su fuente, que es como una tabla empieza a mentir.
 *
 * Solo DIRECCIONES.  Los campos de bits de los registros que se decodifican
 * viven en `common/msr.h`, escritos a mano, porque llevan el porque de cada uno
 * y eso no sale de una tabla.
 *
 * Los nombres son los del manual, literales, para que se pueda buscar en el la
 * misma cadena que hay aqui.
 *
 * ALCANCE: **SOLO INTEL**, que no es lo mismo que "completo".  Los MSR de AMD
 * -- los de IBS entre ellos, que es su mecanismo de muestreo preciso -- se
 * documentan en el manual de AMD y NO estan aqui.  Que falten no significa que
 * no existan: significa que su fuente es otra, y que hay que tratarla igual que
 * esta en vez de escribirlos de memoria.
 */

#ifndef %(guard)s
#define %(guard)s

"""

FOOTER = """
#endif /* %(guard)s */
"""


def emit_family(family, items, dry_run):
    """Escribe el fichero de una familia."""
    path = os.path.join(OUT_DIR, "msr_%s.h" % family)
    guard = "VXP_COMMON_MSR_%s_H" % family.upper()
    out = [HEADER % {
        "file": "msr_%s.h" % family,
        "title": FAMILY_TITLE[family],
        "guard": guard,
    }]

    for e in sorted(items, key=lambda x: (x["base"], x["name"])):
        notes = []
        if e["former"]:
            notes.append("antes `%s`" % e["former"])
        if e["title"]:
            # La CITA, no la descripcion.  La prosa del manual es de Intel y no
            # se copia; el sitio del que sale es un dato, y con el se llega al
            # texto en un paso.  Ver la nota de alcance de la cabecera.
            notes.append("SDM %s" % e["title"])
        if e["kind"] == "indexed":
            notes.append("direccion INDEXADA: la base, y el indice se suma")
        if e.get("recovered"):
            # Un dato reconstruido no es lo mismo que uno leido, y se dice.
            notes.append("el manual escribe `%sH` con la letra O; la direccion "
                         "se reconstruyo desde su columna decimal, que si "
                         "cuadra" % e["recovered"])
        if not e["dec_ok"]:
            # No se corrige: se dice.  El manual trae las dos columnas y no
            # coinciden, y ocultar cual es cual es peor que ensenarlo.
            notes.append("OJO: el manual da %d en decimal, que no es 0x%X"
                         % (e["dec"], e["base"]))
        g = e.get("gate")
        if g:
            where = "CPUID.%02XH" % g["leaf"]
            if g["sub"] is not None:
                where += ".%02XH" % g["sub"]
            bits = ("[%d]" % g["hi"]) if g["hi"] == g["lo"] \
                else ("[%d:%d]" % (g["hi"], g["lo"]))
            notes.append("PUERTA: %s:%s%s" % (where, g["reg"], bits))

        if notes:
            out.append("/** %s */\n" % ".  ".join(notes))
        out.append("#define %s 0x%Xu\n" % (e["name"], e["base"]))

        if g:
            # La puerta, en piezas COMPROBABLES.  Emitirla solo como texto en
            # un comentario obligaria a que alguien la leyera y la
            # transcribiera, y transcribir es donde se cuelan los errores: el
            # codigo puede preguntar por ella tal cual.
            out.append("#define %s_GATE_LEAF 0x%Xu\n" % (e["name"], g["leaf"]))
            if g["sub"] is not None:
                out.append("#define %s_GATE_SUBLEAF 0x%Xu\n"
                           % (e["name"], g["sub"]))
            out.append("#define %s_GATE_REG %du /* %s */\n"
                       % (e["name"], REG_INDEX[g["reg"]], g["reg"]))
            if g["hi"] == g["lo"]:
                out.append("#define %s_GATE_BIT %du\n" % (e["name"], g["hi"]))
            else:
                width = g["hi"] - g["lo"] + 1
                out.append("#define %s_GATE_SHIFT %du\n" % (e["name"], g["lo"]))
                out.append("#define %s_GATE_MASK 0x%Xu\n"
                           % (e["name"], (1 << width) - 1))
        out.append("\n")

    out.append(FOOTER % {"guard": guard})
    text = "".join(out)
    if not dry_run:
        with io.open(path, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return path, len(items), text


def emit_index(families, conflicting, formula, dry_run):
    """Escribe el fichero que los incluye todos."""
    # OJO A LA GUARDA: tiene que ser DISTINTA de la de `common/msr_index.h`,
    # que es quien incluye este fichero.  Con la misma, el de arriba la define
    # primero y este se salta ENTERO -- y eso no da un error de compilacion,
    # da mil quinientas macros que desaparecen sin ruido.  Ya paso.
    guard = "VXP_COMMON_MSR_INTEL_INDEX_H"
    out = [HEADER % {
        "file": "index.h",
        "title": "Todas las direcciones de MSR del manual, por familia.",
        "guard": guard,
    }]
    for fam in sorted(families):
        out.append("#include \"msr_%s.h\"\n" % fam)
    out.append("\n")

    if conflicting:
        out.append(
            "/*\n"
            " * NOMBRES CON MAS DE UNA DIRECCION, y por eso NO se definen aqui.\n"
            " *\n"
            " * No es un descuido del manual: los bancos de comprobacion de\n"
            " * maquina y algunos contadores se rebasan por familia de\n"
            " * procesador, asi que el mismo nombre vale una cosa en una pieza y\n"
            " * otra en otra.  Elegir una direccion seria publicar la equivocada\n"
            " * para la mitad de las maquinas.\n"
            " *\n"
            " * Quien los necesite tiene que resolverlos POR FAMILIA, sabiendo\n"
            " * en cual esta.  Se listan para que conste que se vieron.\n"
            " *\n")
        for name, group in sorted(conflicting)[:400]:
            addrs = ", ".join("0x%X" % a
                              for a in sorted({g["base"] for g in group}))
            out.append(" *   %-34s %s\n" % (name, addrs))
        out.append(" */\n")

    if formula:
        out.append(
            "\n/*\n"
            " * NOMBRADOS EN EL MANUAL, pero con la direccion definida por\n"
            " * FORMULA en otro sitio -- de la forma `base + n`, donde la base\n"
            " * depende de lo que enumere CPUID.\n"
            " *\n"
            " * No se definen porque no hay un numero que definir.  Se listan\n"
            " * para que conste que se vieron: omitirlos sin decirlo es como se\n"
            " * pierde un registro sin que nadie se entere.\n"
            " *\n")
        for nm in formula:
            out.append(" *   %s\n" % nm)
        out.append(" */\n")

    out.append(FOOTER % {"guard": guard})
    text = "".join(out)
    if not dry_run:
        with io.open(INDEX, "w", encoding="ascii", newline="\n") as f:
            f.write(text)
    return INDEX, text


def differs(path, text):
    """¿Lo que hay en el arbol no es lo que saldria ahora?

    Es la comprobacion que hace que una tabla generada no envejezca en silencio:
    sin ella, alguien cambia el extractor, no regenera, y el fichero del arbol
    sigue diciendo lo de antes sin que nada falle.
    """
    if not os.path.exists(path):
        return True
    with io.open(path, encoding="ascii", newline="") as f:
        return f.read().replace("\r\n", "\n") != text


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--dry-run", action="store_true",
                    help="cuenta y no escribe")
    ap.add_argument("--report", action="store_true",
                    help="ademas, el detalle de lo raro")
    ap.add_argument("--check", action="store_true",
                    help="no escribe: falla si lo generado difiere de lo que "
                         "hay en el arbol")
    args = ap.parse_args()
    if args.check:
        # `--check` implica no escribir: se compara y se informa.
        args.dry_run = True

    try:
        table_pages = pages.load_tables()
    except pages.DumpMissing as exc:
        print("gen_msr_index: %s" % exc, file=sys.stderr)
        return 1

    entries, skipped, mismatched, unreadable, formula = collect(table_pages)
    unique, conflicting = dedupe(entries)

    families = {}
    for e in unique:
        families.setdefault(family_of(e["name"], e["title"]), []).append(e)

    if not args.dry_run and not os.path.isdir(OUT_DIR):
        os.makedirs(OUT_DIR)

    print("paginas leidas:            %d" % len(table_pages))
    print("cabeceras de registro:     %d" % len(entries))
    print("nombres unicos definidos:  %d" % len(unique))
    print("nombres en conflicto:      %d  (no se definen; se listan)"
          % len(conflicting))
    print("descartados:               rango=%d sin_nombre=%d sin_direccion=%d "
          "hex_ilegible=%d"
          % (skipped["rango"], skipped["sin_nombre"], skipped["sin_direccion"],
             skipped["hex_ilegible"]))
    print("hex/decimal discrepantes:  %d  (se marcan en el fichero)"
          % len(mismatched))
    print("direccion por formula:     %d  (se listan; no se inventan)"
          % len(formula))
    print()

    stale = []
    for fam in sorted(families):
        path, n, text = emit_family(fam, families[fam], args.dry_run)
        if args.check and differs(path, text):
            stale.append(path)
        print("  %-28s %4d" % (os.path.basename(path), n))
    path, text = emit_index(sorted(families), conflicting, formula, args.dry_run)
    if args.check and differs(path, text):
        stale.append(path)
    print("  %-28s" % "index.h")

    if args.report:
        print()
        print("=== hex/decimal discrepantes ===")
        for name, addr, dec in mismatched:
            print("  %-32s 0x%-6X manual dice %d, que es 0x%X"
                  % (name, addr, dec, dec))
        print()
        print("=== nombres con mas de una direccion ===")
        for name, group in sorted(conflicting):
            addrs = ", ".join("0x%X" % a
                              for a in sorted({g["base"] for g in group}))
            print("  %-32s %s" % (name, addrs))

    if args.check:
        if stale:
            print()
            print("DESACTUALIZADO: lo que hay en el arbol no es lo que saldria")
            print("ahora.  Regeneralo con:  python tools/gen_msr_index.py")
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
