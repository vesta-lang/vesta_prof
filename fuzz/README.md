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

Y un quinto que no esta en el cuadro y es el mas grave para un desensamblador: los
dos la decodifican y dan **longitud distinta**. Una longitud equivocada desalinea
todo lo que viene detras.

### Lo que ha encontrado

Contra Capstone, en el espacio de dos bytes: **23.410 de acuerdo, cero conflictos de
longitud, 72 que el silicio ejecuta y el desensamblador no conoce** y 45 al reves.

Los cero conflictos son tan importantes como los 72: si nuestro oraculo midiera mal
las longitudes, contra un decodificador independiente saldrian miles.

De los 72, **56 son los escapes de x87** (`DC`, `DD`, `DE`, `DF`). Los 16 de `DC` son
`DC D0`..`DC DF`, comprobados uno a uno -- corren y miden 2:

```text
    DC D0    FCOM  ST(0)     sin documentar; la forma documentada es D8 D0
    DC D8    FCOMP ST(0)     idem
```

Son alias no documentados: el mismo efecto por otra codificacion, que el silicio
acepta desde siempre y los desensambladores no listan.

Los 45 del otro cuadrante son formas que Capstone decodifica y **esta pieza** rechaza
-- `8E` es `mov Sreg, r/m16` con selecciones de registro de segmento que no existen.

## Dos modos, y se diferencian en lo que PROMETEN

No es que uno sea una version rapida del otro. Afirman cosas distintas sobre un
subarbol que mato a un trabajador:

| | que dice de un subarbol letal | coste medido |
| --- | --- | --- |
| **caracterizador** (por defecto) | lo MUESTREA: se probaron 16 valores del byte siguiente y mataron N. No puede decir "estas son todas" | el espacio de 3 bytes entero en **41 s** y **256 procesos** |
| **exhaustivo** (bajo peticion) | estrecha y NOMBRA cada candidata letal. Puede decir "estas son todas" | el subarbol de `C2` solo, unos **196.000 procesos** |

El exhaustivo lleva un prefijo porque su sitio es una region concreta -- normalmente
una que el caracterizador senalo. Con el prefijo vacio es el espacio entero, que es
lo que cuesta la verdad completa y se mide en horas.

**Por que los dos, y no es un apano.** La pregunta que le hace el compilador a esto
es *"que toca esta instruccion"*, y `ret imm16` destruye el puntero de pila se
responde UNA vez; 65.280 confirmaciones no le sirven a nadie. Pero el exhaustivo
tiene que existir, porque es el unico que puede encontrar lo que nada describe: una
instruccion indocumentada es por definicion una cuyo formato no se conoce, y
muestrear por lo conocido se salta justo la region donde se esconde.

O sea: **caracterizar para saber DONDE mirar, exhaustivo para mirar.**

## Como se usa

```bash
vxp_fuzz                                              # caracteriza el espacio entero
vxp_fuzz <prof> <trab> <ms> [ref]                     # los tres, y la referencia
vxp_fuzz todo <prefijo-hex> [prof] [trab] [ms] [ref]  # exhaustivo bajo ese prefijo
vxp_fuzz solo <prof> <lo> <hi> [ref]                  # un barrido en ESTE proceso
vxp_fuzz worker <prof> <fijos> <lo> <hi> <prefijo-hex> [ref]
```

La **referencia** es lo que hace que esto encuentre algo en vez de solo medir. Se
precalcula una vez con un desensamblador ajeno y el barrido compara mientras corre:

```bash
python tools/gen_ref_x86.py 2 ref/x86-64-d2.bin
vxp_fuzz 2 12 8000 ref/x86-64-d2.bin
```

La ruta viaja en la linea de ordenes y cada trabajador la carga. Las **sondas y los
muestreos no la reciben**: a una sonda se le pregunta una cosa -- cuanto mide esto -- y
comparar no es eso; a profundidad tres son la mayoria de los procesos y cargarian
dieciseis megabytes para nada.

Las dos ultimas formas existen para poder mirar a mano lo que el padre encuentra.
Un trabajador se ejecuta suelto con los mismos argumentos que le pasaria el padre,
y `solo` imprime el barrido en vez de devolverlo en crudo -- **muere con la primera
candidata letal, que es justo para lo que sirve**: comprobar a mano una candidata
que el padre nombro.

Una corrida caracterizadora acaba diciendo donde apuntar la otra:

```text
  ZONA      FE   hasta 6   matan 0 de 16 probados, y hay 65536 candidatas
  ...
  92 zonas caracterizadas en vez de listadas
  y en 70 de ellas el muestreo no encontro lo letal: piden `todo <prefijo>`
```

Una `ZONA` **no** es un hallazgo sobre esos bytes: es "algo de aqui dentro se llevo a
un trabajador, y esto es cuantos de los valores muestreados lo hicieron". Un perfil
de cero quiere decir que el muestreo no lo encontro -- que es una senal de donde
mirar, no una afirmacion sobre `FE`.

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
