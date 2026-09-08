"""Normalizacion del texto que llega del manual.

El proyecto escribe su documentacion en ASCII, con una unica excepcion para la
ortografia castellana. El manual usa comillas tipograficas, rayas, flechas,
letras griegas en las formulas de x87 y simbolos de marca registrada, y todo
eso tiene que convertirse antes de guardarse.

Se hace **al cargar** y no al escribir. Es un solo punto por el que pasa todo,
de modo que ningun modulo posterior tiene que acordarse; y ademas las
expresiones regulares del resto del paquete trabajan sobre texto ASCII, que es
lo que esperan.

La lista sale de contar los caracteres no ASCII del corpus, no de imaginar
cuales podrian aparecer. `check()` esta para volver a contarlos cuando el
manual cambie de edicion.
"""

import re

# Sustituciones, en el orden en que se aplican. Cada una convierte un caracter
# que el proyecto no admite en su equivalente ASCII.
SUBSTITUTIONS = (
    # Puntuacion tipografica.
    ("—", "-"), ("–", "-"),
    ("’", "'"), ("‘", "'"), ("‚", ","),
    ("“", '"'), ("”", '"'),
    ("…", "..."), (" ", " "),
    # Flechas y relaciones, que el manual usa en el pseudocodigo.
    ("←", "<-"), ("→", "->"), ("↔", "<->"),
    ("≤", "<="), ("≥", ">="), ("≠", "!="),
    # Operadores.
    ("×", "x"), ("·", "*"), ("∗", "*"),
    ("−", "-"), ("⁄", "/"), ("±", "+/-"),
    ("∞", "infinito"), ("«", "<<"), ("•", "-"),
    # Marcas y unidades.
    ("®", "(R)"), ("™", "(TM)"), ("©", "(C)"),
    ("µ", "u"), ("°", " grados"),
    # Griegas de las formulas de x87. Borrarlas cambiaria lo que dice el
    # texto, asi que se transliteran.
    ("π", "pi"), ("σ", "sigma"), ("ε", "epsilon"),
    ("Σ", "Sigma"), ("Ι", "I"), ("μ", "u"),
    # Combinantes: la barra superpuesta tacha el operador anterior para
    # negarlo. Se pierde la forma y se conserva el sentido.
    ("̸", "/"), ("̌", ""),
    ("÷", "/"), ("¹", "1"), ("»", ">>"), ("½", "1/2"),
    # Guion suave: marca donde se PODRIA partir una palabra, y el volcado lo
    # conserva aunque no se haya partido ahi. Es invisible al leer y rompe
    # cualquier busqueda de texto, asi que se quita.
    ("­", ""),
    # El PDF trae bytes que no decodifican; llegan como caracter de reemplazo.
    ("�", ""),
)


def normalize(text):
    """Convierte a ASCII el texto que llega del manual."""
    for bad, good in SUBSTITUTIONS:
        text = text.replace(bad, good)
    return text


def check(text):
    """Devuelve los caracteres no ASCII que quedan, con su recuento.

    Sirve para revisar una edicion nueva del manual: si aparece algo que la
    tabla no cubre, esto lo saca a la luz en lugar de dejarlo llegar a un
    fichero publicado, donde solo lo veria el linter mucho despues.

    @param text Texto ya normalizado.
    @returns Diccionario de caracter a numero de apariciones.
    """
    out = {}
    for ch in text:
        if ord(ch) > 127:
            out[ch] = out.get(ch, 0) + 1
    return out


# Palabra partida por el guion de fin de linea del PDF. El volcado conserva el
# guion y el salto se vuelve un espacio, asi que llega `han- dle`.
HYPHEN_BREAK = re.compile(r"(?<=[A-Za-z])-\s+(?=[a-z])")

# Lo mismo cuando lo que precede es un numero. Ahi el guion SI forma parte de
# la palabra -- `32- bit` es `32-bit` -- y solo sobra el espacio.
HYPHEN_KEPT = re.compile(r"(?<=[0-9])-\s+(?=[a-z])")


def dehyphenate(text):
    """Reune las palabras que el PDF partio al final de la linea.

    El manual justifica el texto y parte palabras con guion. El volcado
    conserva el guion y convierte el salto en un espacio, de modo que la prosa
    llega con `han- dle`, `proces- sors` e `Introduc- tion`. Publicado tal cual
    parece un error de escritura, y ademas rompe cualquier busqueda por esa
    palabra.

    Se distingue por lo que hay a la izquierda: tras una letra el guion es del
    salto y se quita entero; tras un numero es parte de la palabra
    (`32-bit`) y solo sobra el espacio.

    @param text Texto tal como llega del volcado.
    @returns El mismo texto con las palabras reunidas.
    """
    text = HYPHEN_KEPT.sub("-", text)
    return HYPHEN_BREAK.sub("", text)
