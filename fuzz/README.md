# fuzz/

Que decodifica **de verdad** el procesador que tienes delante.

No se le pregunta a un desensamblador ni a un manual: se le pregunta al silicio,
ejecutando bytes y mirando que hace. La diferencia importa porque cuando el manual
y la pieza discrepan, el que no se equivoca es el que ejecuta el codigo.

## Para que sirve

El compilador **no trata un bloque `asm` como una caja negra**: lo lee y convierte
lo que hace en conocimiento -- que registros toca, que memoria lee y escribe, que
banderas deja --. Eso sale de una base de datos de instrucciones, y esa base de
datos tiene dos agujeros que solo se ven midiendo:

```text
    la tabla dice que existe      y el procesador no la conoce
    el procesador la ejecuta      y la tabla no la tiene
```

El segundo es el que no se puede cerrar leyendo: una instruccion indocumentada no
esta en ninguna tabla por definicion. Y el primero tampoco, porque una forma
puede existir en la arquitectura y no en **esta** pieza -- AVX-512 esta fusionado
en Alder Lake porque los nucleos pequenos no lo tienen.

De ahi salen cuatro cuadrantes, y dos son hallazgos:

```text
                         el procesador SI       el procesador NO
    la tabla SI          de acuerdo             la tabla sobra aqui
    la tabla NO          HALLAZGO               de acuerdo
```

## Como se usa

```bash
vxp_fuzz                        # profundidad 3, 8 trabajadores, plazo 20 s
vxp_fuzz <prof> <trab> <ms>     # los tres, a mano
vxp_fuzz solo <prof> <lo> <hi>  # un barrido en ESTE proceso, para mirarlo
vxp_fuzz worker <prof> <fijos> <lo> <hi> <prefijo-hex>
```

Las dos ultimas formas existen para poder mirar a mano lo que el padre encuentra.
Un trabajador se ejecuta suelto con los mismos argumentos que le pasaria el padre,
y `solo` imprime el barrido en vez de devolverlo en crudo -- **muere con la primera
candidata letal, que es justo para lo que sirve**: comprobar a mano una candidata
que el padre nombro.

## Los tres mecanismos, y como se valida cada uno

Misma disciplina que el banco de latencias: si el aparato no reproduce lo
conocido, lo sospechoso es el aparato.

| | | se valida con |
| --- | --- | --- |
| **el oraculo de longitud** | la candidata se pone al final de una pagina cuya siguiente no esta mapeada; si falla en la frontera, el decodificador queria mas bytes | longitudes que no son opinables: `nop` 1, `mov eax,imm32` 5, `ud2` 2, con prefijos hasta el tope 15 |
| **el clasificador** | que excepcion salio: no existe, privilegiada, faltaban bytes, toco memoria, ninguna | `wrmsr` en anillo tres tiene que dar privilegiada; un byte invalido, #UD |
| **el recorrido** | se incrementa el ultimo byte que el decodificador consumio de verdad, no el ultimo del bufer | contra decodificadores inventados, comprobando que ninguna candidata se repite ni se pierde |

El primero tiene una trampa que costo una manana y no se ve leyendo el codigo: una
instruccion completa que corre y se cae por el borde, y una truncada que no llego a
decodificar, salen del sistema con el **mismo** codigo de excepcion, el mismo tipo
y la misma direccion. Solo las separa el puntero de instruccion.

## Por que los trabajadores son procesos

Porque un trabajador **muere**, y no es un riesgo a reducir sino el caso normal.
Ejecutando bytes arbitrarios, dos cosas no las sobrevive el hilo que las ejecuta:

```text
    01 C4   add esp, eax    cambia el puntero de pila, asi que el desenrollador
                            calcula el marco anterior a partir de un rsp que ya
                            no es el del prologo, y encuentra basura
    73 FE   jae -2          salta a si misma.  Nada de dentro del proceso puede
                            interrumpirla, porque nada de dentro esta corriendo
```

Un hilo no se puede matar con seguridad y un hilo colgado no se recupera. Un
proceso se termina desde fuera y se lleva su destrozo con el.

Medido en el espacio de dos bytes: de 10.262 trabajadores arrancados, **8.836
murieron y 110 se colgaron**. Cambiar el puntero de pila -- aunque sea un `push`,
que solo lo mueve ocho bytes -- es el limite del aparato, y el barrido lo descubrio
por su cuenta: las 3.901 candidatas que nombra son exactamente las que lo tocan.

Por eso el trabajo se reparte con una **cola** y no con un reparto fijo: lo que un
trabajador pierde se vuelve a meter mas pequeno, se parte por la mitad, y al final
queda una sola candidata -- que es la culpable, con nombre. Una muerte no es un
hueco en el barrido, es un hallazgo con direccion.

## Lo que sostiene que no falte nada

Cada candidata del espacio acaba en exactamente uno de tres sitios: probada,
saltada por el tunneling, o nombrada como la que se llevo a un trabajador. Que las
tres sumen el espacio es la unica afirmacion capaz de cazar un agujero, y sale en
la ultima linea de cada corrida:

```text
    44040 probadas + 17595 saltadas + 3901 nombradas = 65536 = 256^2
```

Un barrido con un agujero informa de instrucciones como **inexistentes**, que es
la forma de equivocarse que mas cuesta ver: el total tiene buena cara.

## Anillo tres primero, y no es timidez

Aqui casi todo lo peligroso es privilegiado: levanta #GP y es inofensivo. Eso es
una **propiedad del anillo**, no una limitacion, y permite validar los tres
mecanismos sin arriesgar nada. El anillo cero va despues, conducido por el mismo
barrido a traves de otro oraculo -- que es lo que compra que el oraculo entre como
tabla de operaciones en vez de que el barrido sepa lo que es una pagina.
