#!/usr/bin/env python3
"""Precalcula lo que un desensamblador dice de cada secuencia de bytes.

POR QUE HACE FALTA UNA TERCERA FUENTE, Y AQUI MAS QUE EN NINGUN SITIO.  El
barrido de la ISA mide el procesador: pone bytes en una pagina, los ejecuta y
mira que pasa.  Eso da la verdad de campo, y no tiene con que compararse -- un
oraculo que mida mal no se distingue de una maquina que decodifique raro.

Un desensamblador escrito por otra gente, a partir de los mismos manuales pero
con otro criterio, es lo unico que caza eso.  Y la comparacion va en los dos
sentidos, que es donde estan los hallazgos:

    CONFLICTO         los dos decodifican y dan LONGITUD DISTINTA.  Uno de los
                      dos esta mal, y para un desensamblador eso es grave: una
                      longitud equivocada desalinea todo lo que viene detras
    SOLO EL SILICIO   el procesador la ejecuta y el desensamblador no la conoce.
                      Es el hallazgo que este arbol existe para encontrar
    SOLO LA TABLA     al reves: el desensamblador la conoce y esta pieza no.
                      Suele ser una extension que la maquina no tiene, y eso
                      TAMBIEN es un dato -- AVX-512 esta fusionado en Alder Lake
                      porque los nucleos pequenos no lo tienen

QUE GENERA.  Una longitud por cada secuencia posible de `profundidad` bytes,
indexada por esos bytes leidos como un numero en orden grande -- que es
exactamente como los numera el recorrido, asi que el lado de C no traduce nada.
Un cero quiere decir "no decodifica".

POR QUE PRECALCULADO Y NO EN EL MOMENTO.  El lado del procesador es lo caro: un
intento cuesta una excepcion, y la mitad de los subarboles se llevan el proceso
por delante.  El lado del desensamblador es barato y se puede tener entero de
antemano, asi que el barrido compara mientras corre sin pagar nada y sin
arrastrar una dependencia a un arbol que no tiene ninguna.

EL RELLENO ES PARTE DE LA PREGUNTA.  Mas alla de `profundidad` los bytes valen
cero, igual que en la pagina del barrido.  Preguntarle al desensamblador con
otro relleno daria otra respuesta y las dos corridas no serian comparables.

    python tools/gen_ref_x86.py 3 ref/x86-64-d3.bin
"""

import struct
import sys
import os

MAGIC = b"VXPREF1\n"
HEADER = 24


def build(depth):
    """Devuelve un bytearray con una longitud por candidata."""
    try:
        from capstone import Cs, CS_ARCH_X86, CS_MODE_64
    except ImportError:
        sys.exit("hace falta capstone: pip install capstone")

    md = Cs(CS_ARCH_X86, CS_MODE_64)
    total = 1 << (8 * depth)
    out = bytearray(total)

    # El relleno se prepara una vez: lo que hay mas alla de los bytes enumerados
    # es cero, igual que en la pagina donde el barrido pone la candidata.
    relleno = bytes(15 - depth)
    for n in range(total):
        cabeza = n.to_bytes(depth, "big")
        for ins in md.disasm(cabeza + relleno, 0):
            out[n] = ins.size
            break
    return out


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__.strip().splitlines()[-1].strip())
    depth = int(sys.argv[1])
    if depth < 1 or depth > 4:
        # Mas de cuatro no cabe en memoria de forma razonable: son 4 GB de tabla.
        sys.exit("la profundidad va de 1 a 4")
    destino = sys.argv[2]

    datos = build(depth)
    carpeta = os.path.dirname(destino)
    if carpeta:
        os.makedirs(carpeta, exist_ok=True)
    with open(destino, "wb") as f:
        f.write(MAGIC)
        f.write(struct.pack("<II", depth, 0))
        f.write(struct.pack("<Q", len(datos)))
        f.write(datos)

    decodifican = sum(1 for v in datos if v)
    print("profundidad %d: %d candidatas, %d decodifican (%.1f%%)"
          % (depth, len(datos), decodifican, 100.0 * decodifican / len(datos)))
    reparto = {}
    for v in datos:
        if v:
            reparto[v] = reparto.get(v, 0) + 1
    for k in sorted(reparto):
        print("  %2d bytes %10d" % (k, reparto[k]))
    print("escrito en %s (%d bytes)" % (destino, HEADER + len(datos)))


if __name__ == "__main__":
    main()
