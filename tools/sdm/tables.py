"""Lectura de las tablas alineadas por columnas del manual.

`pdftotext -table` alinea las columnas con espacios, de modo que una tabla del
manual llega asi:

    Opcode          Instruction        Op/    64-bit    Compat/    Description
                                       En     Mode      Leg Mode

    14 ib           ADC AL, imm8       I      Valid     Valid      Add with carry imm8 to AL.
    REX.W + 15 id   ADC RAX, imm32     I      Valid     N.E.       Add with carry imm32 sign extended to 64-
                                                                   bits to RAX.

Leerla es cuestion de saber DONDE empieza cada columna, y eso lo dice la fila
de encabezados. Partir por espacios no vale: `REX.W + 15 id` y `ADC AL, imm8`
llevan espacios dentro, y una celda vacia no deja ninguna marca.

Tres detalles que hay que tratar, y que son los que hacen que esto no sea un
`split`:

- **El encabezado ocupa dos lineas.** `Op/ En`, `64-bit Mode` y `Compat/ Leg
  Mode` se parten por falta de anchura, asi que la segunda linea continua la
  primera en las mismas columnas.
- **Las filas se envuelven.** Una descripcion larga sigue en la linea de abajo
  con las demas columnas vacias; esa linea pertenece a la fila anterior.
- **Las llamadas al pie se pegan al valor.** `r/m8` con una nota numero 1
  queda como `r/m81`, y eso convierte un operando de 8 bits en uno de 81.
"""

import re

# Separacion minima entre dos columnas. Con menos, dos palabras de la misma
# celda se tomarian por celdas distintas.
GAP = 2

# Una llamada al pie: uno o dos digitos pegados al final de un token que
# termina en letra. `r/m81` es `r/m8` con la nota 1; `imm32` no es nada de eso,
# porque el 32 va precedido de letra pero forma parte del nombre.
#
# No se puede distinguir por la forma sola, asi que se usa la lista de notas
# que la tabla declara al final: si la tabla no tiene nota 1, no se quita nada.
FOOTNOTE_MARK = re.compile(r"^(NOTES?|\*)\s*:?\s*$", re.I)

# Pie de pagina del manual: `Vol. 2A  3-264`. Cuando una tabla cruza un salto
# de pagina, el volcado lo deja pegado al final de la ultima celda de la fila,
# o suelto como una fila entera. En los dos casos es palabreria de impresion y
# no dato.
PAGE_FOOTER = re.compile(r"\s*Vol\.\s*\d[A-D]?\s+\d+-\d+\s*$")


# Cuantas lineas del principio se consideran candidatas a definir las columnas.
HEADER_SEARCH = 4


def starts_of(line):
    """Devuelve las columnas en que empieza cada celda de una linea."""
    return sorted({
        m.start()
        for m in re.finditer(r"\S+(?:\s\S+)*?(?=\s{%d,}|$)" % GAP, line)
    })


def widest(lines):
    """Elige, entre las primeras lineas, la que MAS columnas define.

    Es lo que necesitan las tablas matriciales del manual -- las de resultados
    de x87 -- cuya primera linea es un rotulo que abarca todo el ancho, como la
    palabra `DEST` centrada, y define una sola columna.

    @param lines Lineas de la tabla, sin las vacias.
    @returns Indice de la linea de encabezado.
    """
    best, index = [], 0
    for i, line in enumerate(lines[:HEADER_SEARCH]):
        starts = starts_of(line)
        if len(starts) > len(best):
            best, index = starts, i
    return index


def first_row(lines):
    """Elige la primera linea que define dos columnas o mas.

    Es lo que necesitan las tablas de codificaciones, donde una fila de datos
    suele partirse en mas trozos que el encabezado porque su descripcion lleva
    espacios anchos dentro. Con `widest`, el encabezado entero pasaba a
    contarse como datos y la palabra `Instruction` se leia como notacion de
    opcode seiscientas veces.

    @param lines Lineas de la tabla, sin las vacias.
    @returns Indice de la linea de encabezado.
    """
    for i, line in enumerate(lines[:HEADER_SEARCH]):
        if len(starts_of(line)) >= 2:
            return i
    return 0


def starts_at(lines, prefix):
    """Devuelve un selector que busca el encabezado por su primera palabra.

    Es el mas fiable cuando se conoce la tabla: la de codificaciones siempre
    empieza por `Opcode`, asi que no hay nada que deducir. Se recurre a el
    desde el modulo que conoce el dominio, en lugar de adivinar aqui.

    @param lines Lineas de la tabla, sin las vacias.
    @param prefix Texto con que empieza el encabezado, en minusculas.
    @returns Indice de la linea de encabezado, o el de `first_row`.
    """
    for i, line in enumerate(lines[:HEADER_SEARCH]):
        if line.strip().lower().startswith(prefix):
            return i
    return first_row(lines)


def _looks_like_data(cell):
    """Indica si una celda parece un dato y no un rotulo de encabezado.

    El byte hexadecimal tiene que ir seguido de algo que no sea letra ni
    digito. Sin esa condicion `Description` empieza por `De`, que son dos
    digitos hexadecimales validos, y el encabezado se toma por datos.
    """
    text = cell.strip()
    if re.match(r"^[0-9A-F]{2}(?![0-9A-Za-z])", text, re.I):
        return True
    # La lista tiene que cubrir los mismos prefijos que reconoce el lector de
    # codificaciones. Cuando se separaron, `NFx 66 0F AE /7` dejo de parecer un
    # dato: la fila se absorbio como encabezado y `CLFLUSHOPT` se publico con
    # su sintaxis en el sitio del opcode y sin un solo byte.
    return text.upper().startswith((
        "REX", "VEX.", "EVEX.", "XOP.", "MVEX.", "NP ", "NFX", "F2 ", "F3 ", "66 ",
    ))


def slice_row(line, starts):
    """Corta una linea en celdas segun las posiciones de las columnas."""
    cells = []
    for i, start in enumerate(starts):
        stop = starts[i + 1] if i + 1 < len(starts) else len(line)
        cells.append(line[start:stop].strip())
    return cells


def splits_a_word(line, starts):
    """Indica si cortar por las columnas partiria una palabra.

    Es lo que distingue una fila de la tabla de una linea de prosa que viene
    justo debajo. Una fila respeta las columnas: en cada frontera hay un
    espacio. Una linea de prosa no sabe nada de ellas, asi que la frontera cae
    en mitad de una palabra.

    Sin esta comprobacion la tabla no terminaba nunca y se tragaba los parrafos
    siguientes convertidos en filas: `| A vendor id | entification string is |`.
    """
    for start in starts[1:]:
        if 0 < start < len(line) and line[start - 1].isalnum() and line[start].isalnum():
            return True
    return False


def is_continuation(cells):
    """Indica si una fila continua la anterior.

    Que la primera celda este vacia no basta. En las tablas matriciales del
    manual -- las de resultados de x87, `SRC` contra `DEST` -- la primera
    columna es una etiqueta que abarca varias filas, de modo que esta vacia en
    casi todas: dandolas por continuaciones se fusionaba la tabla entera en una
    sola fila y se descartaba por vacia.

    Lo que distingue de verdad a una continuacion es que trae poco: es el resto
    de un texto que no cupo, y ocupa una celda o dos. Una fila de matriz trae
    valor en casi todas.
    """
    if not cells or cells[0]:
        return False

    filled = sum(1 for c in cells[1:] if c)
    return 0 < filled <= 2 and filled * 2 < len(cells)


def merge(previous, cells):
    """Anade a una fila el contenido de su continuacion."""
    for i, value in enumerate(cells):
        if not value:
            continue
        if i < len(previous) and previous[i]:
            previous[i] += " " + value
        elif i < len(previous):
            previous[i] = value
    return previous


def footnotes(lines):
    """Devuelve los numeros de nota que la tabla declara al final.

    Sin esta lista no se puede saber si el `1` de `r/m81` es una llamada o
    parte del nombre, y quitarlo a ojo estropearia `imm32` o `xmm1`.
    """
    numbers = set()
    seen_mark = False
    for line in lines:
        text = line.strip()
        if FOOTNOTE_MARK.match(text):
            seen_mark = True
            continue
        if not seen_mark:
            continue
        m = re.match(r"^(\d+)\.\s+\S", text)
        if m:
            numbers.add(m.group(1))
    return numbers


def strip_footnotes(value, numbers):
    """Quita de una celda las llamadas al pie que la tabla declara.

    Solo se tocan los tokens que terminan en LETRA seguida de uno de esos
    numeros: `r/m81` con la nota 1 vuelve a ser `r/m8`, y `imm32` no se toca
    porque su `32` va detras de `imm`, que ya termina en letra... y por eso la
    condicion es mas fina: el numero debe estar declarado como nota Y el token
    sin el debe seguir pareciendo un operando.
    """
    if not numbers or not value:
        return value

    def fix(token):
        # La celda es una lista de operandos, asi que casi todos los tokens
        # arrastran una coma. Se aparta antes de mirar el final y se devuelve
        # despues: sin esto la llamada al pie nunca queda la ultima y no se
        # reconoce ni una.
        core = token.rstrip(",;)")
        tail = token[len(core):]

        for number in numbers:
            if not core.endswith(number) or len(core) <= len(number):
                continue
            stem = core[: -len(number)]
            # `r/m8` acaba en digito: quitarle la nota deja algo coherente.
            # `imm32` sin el `2` daria `imm3`, que tambien lo parece, asi que
            # se exige ademas que el resultado sea un operando conocido.
            if re.fullmatch(r"(r|r/m|imm|rel|moffs|ptr|m)\d+", stem):
                return stem + tail
        return token

    return " ".join(fix(t) for t in value.split(" "))


def parse(lines, header=first_row, absorb=True, stop_on_prose=False):
    """Lee una tabla alineada por columnas.

    Cual es la linea de encabezado NO se adivina aqui: lo decide quien llama,
    que es quien sabe que tabla esta leyendo. Deducirlo con una heuristica
    unica no es posible, porque las dos familias de tablas del manual piden
    reglas opuestas: en una matriz el encabezado es la linea con mas columnas,
    y en una tabla de codificaciones es justo la que hay que evitar.

    @param lines Lineas de la tabla, incluido su encabezado.
    @param header Selector que devuelve el indice de la linea de encabezado.
    @param absorb Si el encabezado puede ocupar varias lineas. Las tablas de
        codificaciones lo hacen; las matrices, no, y absorberles filas se les
        come datos.
    @param stop_on_prose Si la tabla termina en cuanto una linea no respeta las
        columnas. Lo necesitan las tablas incrustadas en la prosa, donde el
        parrafo siguiente viene pegado; las de codificaciones, no, porque una
        celda ancha puede invadir la columna vecina sin dejar de ser una fila.
    @returns `{head, rows, span}` con las celdas ya unidas y limpias.
    """
    body = [l for l in lines if l.strip()]
    if not body:
        return {"head": [], "rows": [], "span": []}

    header_index = header(body)
    starts = starts_of(body[header_index])
    if len(starts) < 2:
        return {"head": [], "rows": [], "span": []}

    notes = footnotes(lines)

    span = [l.strip() for l in body[:header_index] if l.strip()]
    head = slice_row(body[header_index], starts)
    rows = []

    # El encabezado puede ocupar varias lineas. `Opcode/` sigue en `Instruction`
    # y `64/32 bit` en `Mode Support`, en la linea de abajo. Se absorben
    # mientras no haya empezado el cuerpo y traigan pocas celdas: una fila de
    # datos de estas tablas siempre rellena tambien el modo o la descripcion.
    #
    # Sin esto, `Instruction` entraba como fila y su texto acababa contandose
    # como notacion de opcode: 607 veces.
    cursor = header_index + 1
    while absorb and cursor < len(body) and cursor <= header_index + 2:
        cells = slice_row(body[cursor], starts)
        if not any(cells):
            break
        # El cuerpo empieza en cuanto una celda parece notacion de opcode. Es
        # lo unico que distingue de verdad al encabezado de la primera fila:
        # contar celdas no vale, porque `Instruction En Mode` trae tres.
        if any(_looks_like_data(c) for c in cells if c):
            break
        merge(head, cells)
        cursor += 1

    for line in body[cursor:]:
        text = line.strip()
        if FOOTNOTE_MARK.match(text) or re.match(r"^\d+\.\s+\S", text):
            # A partir de aqui empiezan las notas: la tabla termino.
            break

        # Una linea que al cortarla parte una palabra no pertenece a la tabla:
        # es la prosa que viene detras. La tabla termina ahi.
        if stop_on_prose and rows and splits_a_word(line, starts):
            break

        # Una fila que solo es el pie de pagina no es una fila.
        if PAGE_FOOTER.search(line) and len(PAGE_FOOTER.sub("", line).strip()) < 3:
            continue

        cells = [PAGE_FOOTER.sub("", c).strip() for c in slice_row(line, starts)]
        if not any(cells):
            continue

        if rows and is_continuation(cells):
            merge(rows[-1], cells)
        elif not rows and is_continuation(cells):
            # Continuacion del encabezado, que ocupa dos lineas.
            merge(head, cells)
        else:
            rows.append(cells)

    rows = [[strip_footnotes(c, notes) for c in row] for row in rows]
    return {"head": [h for h in head], "rows": rows, "span": span}
