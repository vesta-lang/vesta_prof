"""Troceo de una instruccion del manual en sus secciones.

Una entrada del manual ocupa entre una y veinticuatro paginas, y cada pagina
lleva encima o debajo la cabecera del capitulo y el pie con el mnemonico y el
numero de pagina. Al concatenarlas, esa palabreria queda incrustada en mitad
de la prosa: sin quitarla, la descripcion de cualquier instruccion aparece
partida por un `ADC--Add With Carry   Vol. 2A 3-9` cada cuarenta lineas.

Despues, el cuerpo se parte por sus encabezados. Los del manual son constantes
y van solos en su linea, lo que los hace faciles de reconocer; lo que no es
constante es CUALES aparecen, porque una instruccion sin banderas no trae
"Flags Affected" y una de x87 trae secciones que no tiene ninguna otra.

De ahi que el troceo no busque una lista fija: reconoce cualquier encabezado y
clasifica despues. Lo que no encaja se conserva con su titulo, porque el manual
tiene paginas -- `CPUID` es el caso extremo -- cuyo contenido no se parece a
nada.
"""

import re

# Cabecera de capitulo que se repite en la parte superior de cada pagina.
CHAPTER_HEADER = re.compile(
    r"^\s*(INSTRUCTION SET REFERENCE|SAFER MODE EXTENSIONS REFERENCE|"
    r"INSTRUCTION FORMAT|OPCODE MAP)\b.*$"
)

# Pie con el mnemonico y la referencia de volumen y pagina.
PAGE_FOOTER = re.compile(r"Vol\.\s*2[A-D]\s+\d+-\d+\s*$")

# Numero de pagina suelto.
BARE_NUMBER = re.compile(r"^\s*\d+-\d+\s*$")

# Un encabezado de seccion: linea corta, sin punto final, que empieza por
# mayuscula. La lista de los conocidos sirve para clasificar, no para
# reconocer.
KNOWN_HEADINGS = (
    "Description",
    "Operation",
    "Flags Affected",
    "Intel C/C++ Compiler Intrinsic Equivalent",
    "Intel C/C++ Compiler Intrinsic Equivalents",
    "Instruction Operand Encoding",
    "IA-32 Architecture Compatibility",
    "IA-32 Architecture Legacy Compatibility",
    "Numeric Exceptions",
    "Floating-Point Exceptions",
    "SIMD Floating-Point Exceptions",
    "Other Exceptions",
    "Exceptions (All Operating Modes)",
    "Protected Mode Exceptions",
    "Real-Address Mode Exceptions",
    "Virtual-8086 Mode Exceptions",
    "Compatibility Mode Exceptions",
    "64-Bit Mode Exceptions",
    "Compatibility and 64-Bit Mode Exceptions",
)

HEADING_SET = {h.lower() for h in KNOWN_HEADINGS}

# Identificador estable de cada seccion. No depende del idioma ni del titulo
# exacto del manual, de modo que si una edicion retoca el rotulo la seccion
# sigue siendo la misma.
SECTION_IDS = {
    "description": "description",
    "operation": "operation",
    "flags affected": "flags",
    "intel c/c++ compiler intrinsic equivalent": "intrinsics",
    "intel c/c++ compiler intrinsic equivalents": "intrinsics",
    "instruction operand encoding": "operand-encoding",
    "ia-32 architecture compatibility": "compatibility",
    "ia-32 architecture legacy compatibility": "compatibility",
}

# Modos de operacion con nombre neutro, para las secciones de excepciones. Se
# usan nombres que un lector de otro juego de instrucciones puede mapear a los
# suyos, en lugar de los titulos del manual.
EXCEPTION_MODES = {
    "protected mode exceptions": ["protected"],
    "real-address mode exceptions": ["real"],
    "virtual-8086 mode exceptions": ["virtual8086"],
    "compatibility mode exceptions": ["compat"],
    "64-bit mode exceptions": ["long"],
    "compatibility and 64-bit mode exceptions": ["compat", "long"],
    "exceptions (all operating modes)": ["real", "protected", "virtual8086",
                                        "compat", "long"],
}


def clean_pages(pages, first, last):
    """Concatena las paginas de una instruccion quitando cabeceras y pies.

    @param pages Paginas de un volcado.
    @param first Primera pagina de la instruccion.
    @param last Pagina siguiente a la ultima.
    @returns Lista de lineas, ya sin la palabreria de pagina.
    """
    out = []
    for index in range(first, min(last, len(pages))):
        for line in pages[index].split("\n"):
            stripped = line.rstrip()
            if not stripped.strip():
                out.append("")
                continue
            if CHAPTER_HEADER.match(stripped):
                continue

            # El pie se quita de la linea, no se tira la linea con el pie
            # dentro. Cuando un encabezado de seccion cae en la primera linea
            # de la pagina, comparte fila con el pie y se lo llevaba por
            # delante: `DIV` perdia asi su `Operation`, y con el la seccion
            # entera, que acababa pegada a la descripcion. Eran 35 entradas.
            if PAGE_FOOTER.search(stripped):
                stripped = PAGE_FOOTER.sub("", stripped).rstrip()
                if not stripped.strip():
                    continue

            if BARE_NUMBER.match(stripped):
                continue
            out.extend(_detach_heading(stripped))
    return out


# Encabezado con algo pegado detras, separado por un hueco ancho. El hueco es
# lo que lo distingue de un titulo largo de verdad: `Operation in a
# Uni-Processor Platform` lleva espacios simples y no es dos cosas, es una.
TRAILING_ASIDE = re.compile(r"^(\S[^\s]*(?:\s\S+)*?)\s{3,}(\S.*)$")


def _detach_heading(line):
    """Separa el encabezado del texto que quedo en su misma linea.

    El volcado en columnas junta lo que en la pagina esta a distinta altura.
    `VFPCLASSSH` trae `Operation` con el comentario `// see VFPCLASSPH` a la
    derecha, y con el pegado la linea deja de reconocerse como encabezado: la
    seccion entera se pierde dentro de la anterior.

    @param line Linea ya limpia de cabeceras y pies.
    @returns Una lista de una o dos lineas.
    """
    match = TRAILING_ASIDE.match(line)
    if not match or not is_heading(match.group(1)):
        return [line]
    return [match.group(1), match.group(2)]


def is_heading(line):
    """Indica si una linea es un encabezado de seccion del manual."""
    text = line.strip()
    if not text or len(text) > 60:
        return False
    return text.lower() in HEADING_SET


def split(lines):
    """Parte el cuerpo de una instruccion en secciones.

    Lo que precede al primer encabezado es la cabecera de la entrada: el
    titulo y la tabla de codificaciones. Se devuelve como seccion `preamble`
    porque contiene la tabla, que es lo unico que el volcado en modo tabla
    aporta y el normal estropea.

    @param lines Salida de `clean_pages`.
    @returns Lista de `{id, title, lines}`, en orden de aparicion.
    """
    out = [{"id": "preamble", "title": None, "lines": []}]

    for line in lines:
        if is_heading(line):
            title = line.strip()
            out.append({
                "id": SECTION_IDS.get(title.lower(), section_id(title)),
                "title": title,
                "lines": [],
            })
            continue
        out[-1]["lines"].append(line)

    # Una seccion sin nada dentro no aporta y ensucia el recuento.
    return [s for s in out if any(l.strip() for l in s["lines"])]


def section_id(title):
    """Devuelve el identificador estable de una seccion no catalogada."""
    low = title.lower().strip()

    modes = EXCEPTION_MODES.get(low)
    if modes:
        return "exceptions." + "-".join(modes)

    if low.endswith("exceptions"):
        stem = re.sub(r"\s*exceptions$", "", low)
        return "exceptions." + slugify(stem or "other")

    return "extra." + slugify(low)


def slugify(text):
    """Convierte un titulo en un identificador con guiones."""
    return re.sub(r"^-+|-+$", "", re.sub(r"[^a-z0-9]+", "-", text.lower()))


def modes_of(section):
    """Devuelve los modos de operacion que cubre una seccion de excepciones.

    @param section Seccion devuelta por `split`.
    @returns Lista de modos neutros, o `None` si no es de excepciones.
    """
    if not section["title"]:
        return None
    return EXCEPTION_MODES.get(section["title"].lower())
