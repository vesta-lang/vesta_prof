# calib/

Cuanto cuestan las instrucciones de **esta** maquina.

No hay que confundirlo con `bench/`, que mide lo contrario: alli se mide lo que
cuesta el propio perfilador; aqui, lo que cuesta la maquina que el perfilador
observa.

## Para que sirve

Un perfilador que solo dice donde se va el tiempo es un listado. Para decir si
eso es mucho o poco hace falta lo que el codigo **deberia** haber costado, y eso
sale de una tabla de latencias y puertos por microarquitectura.

```text
    residuo  =  ciclos medidos      -  coste previsto del bloque
                (el PMU)               (la tabla)
```

Este directorio mide lo mismo que la tabla afirma, sobre la pieza que tienes
delante, **para comprobarla**. No para sustituirla: construir tablas de coste es
el trabajo de uops.info, se hace con automatizacion y con mucho hardware, y no se
rehace aqui.

Comprobarla no es un lujo, porque entre la tabla y la maquina hay siempre una
suposicion. La tabla dice `intel-alderlake-p`; la maquina puede ser un Raptor
Lake, bajo un hipervisor, con menos contadores de los que la pieza tiene. Que esa
tabla la describa se da por hecho hasta que alguien lo mide.

Y importa porque el residuo se usa para **ordenar**. Ante una lista de "ciclos
inexplicados", la primera pregunta razonable es *"¿como sabes que la parte
explicada esta bien?"*. Tener por respuesta una medida en la maquina de quien
pregunta no es lo mismo que decir que lo pone en una tabla.

## Necesita el driver cargado

Lee los contadores con `rdpmc`, que solo es legal en modo usuario si algo ha
puesto `CR4.PCE`. Sin eso el proceso muere con una excepcion de instruccion
privilegiada -- ruidosa, inmediata y senalando la causa, que es la forma correcta
de fallar.

```bash
vxp_calib          # el procesador logico 0
vxp_calib 20       # uno concreto
```

Se fija a **un** procesador porque los contadores son por nucleo: dos lecturas
desde procesadores distintos son dos cuentas de dos sitios y restarlas no
significa nada. En una pieza hibrida hay que correrlo en las dos clases de
nucleo, porque no responden lo mismo -- ni de lejos.

## Como mide

Dos preguntas distintas, dos formas de encadenar lo mismo:

```text
   LATENCIA                        THROUGHPUT
   cada una necesita la anterior   todas independientes

     add %rcx, %rax                  add %r8, %rax
     add %rcx, %rax   <- espera      add %r8, %rcx   <- no se esperan
     add %rcx, %rax   <- espera      add %r8, %rdx
     ...                             ...
   -> cuanto tarda UNA de            -> cuantas caben por ciclo
      principio a fin
```

Y tres reglas que sostienen el resultado:

- **Se toma el MINIMO, no la media.** Lo que se cruce -- una expropiacion, una
  interrupcion, otro hilo en el nucleo -- solo puede **sumar** ciclos. El minimo
  converge al coste real y la dispersion por encima es el ruido, que se imprime
  en vez de esconderse.
- **Las lecturas van entre vallas.** `rdpmc` no serializa: sin `lfence` el
  procesador solapa la lectura con el codigo medido y la resta sale menor que lo
  que de verdad corrio, sin que falle nada.
- **Se cuentan las instrucciones retiradas y se comparan con las que el nucleo
  ejecuta.** Es la comprobacion que no depende de ninguna expectativa: si no
  cuadra, el banco esta dividiendo por el numero equivocado y todas las cifras
  estan mal por un factor constante.

Ademas cada resultado se contrasta con una banda de cordura -- lo que es cierto
de **cualquier** pieza x86-64: una suma cuesta alrededor de un ciclo, una
multiplicacion varios, un acierto en la primera cache un punado --. Si algo se
sale, lo sospechoso es el banco, y el programa lo dice y sale con codigo distinto
de cero.

## Los nucleos medidos estan en ensamblador, y es el unico sitio donde toca

Lo que se mide **es** la secuencia de instrucciones. Un optimizador que quita un
`add` muerto, que reordena dos o que convierte sesenta en un desplazamiento no
esta mejorando el codigo: esta borrando el experimento. No hay C que fije eso.

Viven en `common/asm/x86_64/`, que es donde la politica de ese directorio los
manda: no mencionan nada que defina el sistema operativo. Los de latencia usan
solo registros volatiles en las dos convenciones de llamada; los de throughput
necesitan mas cadenas independientes, asi que toman prestados los preservados y
los devuelven.

Por eso `vxp_calib` es lo unico del arbol que **no** construyen los tres
compiladores: la sintaxis es la de GNU y MSVC quiere MASM. Escribir los nucleos
dos veces serian dos versiones de una medida, que es peor que una medida que no
todos puedan construir.
