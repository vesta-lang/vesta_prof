"""Acceso a los volcados de texto del manual de Intel.

El manual es un PDF de 5342 paginas. Se vuelca a texto UNA vez, en dos modos, y
todo lo demas trabaja sobre esos volcados:

    pdftotext -layout manual.pdf manual.txt
    pdftotext -table  manual.pdf manual.table.txt

Los dos hacen falta, y no es redundancia. El modo normal respeta los saltos de
parrafo y produce una prosa legible, pero **desincroniza las tablas**: la
columna de descripcion se desplaza una fila y `r/m8` sale como `r/m81` con la
llamada al pie pegada. El modo tabla alinea las columnas correctamente, pero
mete una linea en blanco entre cada dos y estropea la prosa.

De ahi la regla del importador: **la prosa sale del volcado normal y las tablas
del volcado en modo tabla**. Mezclarlo al reves publica datos falsos que no
parecen falsos, que es la peor clase de error.

Los volcados NO viajan en el repositorio: son 35 MB derivados de un PDF que
tampoco se publica. Se buscan en `manual/`, que esta en las exclusiones
locales, o donde diga la variable de entorno `VESTA_SDM_DIR`.
"""

import io
import os

from . import text

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# Directorio de los volcados. La variable de entorno permite tenerlos fuera del
# arbol del proyecto, que es lo razonable cuando se comparten entre repos.
MANUAL_DIR = os.environ.get("VESTA_SDM_DIR") or os.path.join(ROOT, "manual")

LAYOUT_DUMP = os.path.join(MANUAL_DIR, "sdm.txt")
TABLE_DUMP = os.path.join(MANUAL_DIR, "sdm.table.txt")

# `pdftotext` emite Latin-1, no UTF-8: leerlo como UTF-8 revienta en el primer
# simbolo de marca registrada. La normalizacion a ASCII se hace despues.
ENCODING = "latin-1"

# Separador de pagina que emite `pdftotext`.
PAGE_BREAK = "\f"


class DumpMissing(Exception):
    """El volcado no existe. Lleva la orden que hay que ejecutar."""


def _load(path, flag):
    if not os.path.exists(path):
        raise DumpMissing(
            "falta %s.\nGeneralo con:\n    pdftotext %s <manual.pdf> %s"
            % (path, flag, path)
        )
    with io.open(path, encoding=ENCODING) as f:
        # La normalizacion a ASCII se hace AQUI, en el unico punto por el que
        # pasa todo. Hacerla al escribir obligaria a acordarse en cada salida,
        # y ademas las expresiones regulares del resto del paquete trabajan
        # sobre texto ASCII, que es lo que esperan.
        return text.normalize(f.read()).split(PAGE_BREAK)


def load_layout():
    """Devuelve las paginas del volcado normal, para la prosa."""
    return _load(LAYOUT_DUMP, "-layout")


def load_tables():
    """Devuelve las paginas del volcado en modo tabla, para las tablas."""
    return _load(TABLE_DUMP, "-table")


# Limites de la referencia de instrucciones dentro del manual combinado.
#
# Se localizan por el encabezado de capitulo y no se fijan a mano: el manual se
# reedita y las paginas se desplazan. Si una edicion nueva mueve los capitulos,
# esto los vuelve a encontrar en lugar de leer el sitio equivocado en silencio.
CHAPTER_MARKS = (
    "INSTRUCTION SET REFERENCE, A-L",
    "INSTRUCTION SET REFERENCE, M-U",
    "INSTRUCTION SET REFERENCE, V",
    "SAFER MODE EXTENSIONS REFERENCE",
)

# Lo que viene despues de la referencia y NO se importa.
END_MARK = "OPCODE MAP"


def reference_range(pages):
    """Devuelve `(primera, ultima)` de la referencia de instrucciones.

    @param pages Paginas de cualquiera de los dos volcados.
    @returns Tupla de indices, con la ultima excluida.
    @raises ValueError Si no se reconocen los limites.
    """
    def first_with(mark):
        for i, page in enumerate(pages):
            # Solo la cabecera de la pagina: el mismo texto aparece dentro del
            # indice general, cientos de paginas antes.
            if mark in page[:400]:
                return i
        return None

    start = None
    for mark in CHAPTER_MARKS:
        found = first_with(mark)
        if found is not None:
            start = found if start is None else min(start, found)

    end = first_with(END_MARK)

    if start is None or end is None or end <= start:
        raise ValueError(
            "no se reconocen los limites de la referencia de instrucciones; "
            "el manual pudo cambiar de estructura"
        )
    return start, end
