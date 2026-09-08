# timings/

Lo que cuesta cada instruccion en cada microarquitectura: un fichero `.vxarch`
por pieza, con throughput reciproco, latencia, microoperaciones y puertos.

Vienen del compilador, que los construye desde medidas publicadas por uops.info y
desde los modelos de planificacion de LLVM. Aqui son un **dato de entrada**, no un
intermedio: el perfilador los lee para saber lo que un bloque de codigo deberia
haber costado.

```text
    residuo  =  ciclos medidos      -  coste previsto del bloque
                (el PMU)               (estas tablas)
```

Sin la segunda mitad, un perfil dice donde se va el tiempo pero no si eso es
mucho o poco.

## Por que se versionan aqui y no alla

En el compilador son regenerables y estan excluidos: alli hay un pipeline entero
y las fuentes externas de las que salen. Aqui no hay pipeline, y un perfilador
que se pueda sacar de su repositorio y llevarselo no puede depender de que quien
lo use tenga 140 MB de XML.

Es la misma excepcion que `bench/baseline/`, y por la misma razon: **estos
ficheros llevan con que compararse**. La primera linea de cada uno dice de que
microarquitectura es, de que fuente salio, con que fecha y con el resumen de los
datos de origen.

Ese resumen no es decorativo. El numero que identifica una forma de instruccion
es un indice denso que asigna la construccion de la base, y no es estable entre
regeneraciones: quien apunte uno esta apuntando algo que puede pasar a
significar otra instruccion. Comparando el resumen se distingue una tabla que
sigue queriendo decir lo mismo de otra que no, y quien no cuadre se niega a
comparar en vez de dar desvios sin sentido.

## Lo que hay, y lo que falta

Solo estan las microarquitecturas para las que hace falta una tabla. Falta una
la primera vez que alguien perfila en una pieza que no esta, y eso **no es un
fallo que haya que tapar**: se dice, la columna del residuo queda sin valor -- que
no es lo mismo que cero -- y se puede medir con `vxp_calib`, que para eso existe.

Lo que nunca se hace es sustituir una tabla por la de otra pieza parecida. Los
numeros de un nucleo P no describen uno E: en la maquina de desarrollo, la misma
`imul` tarda 3 ciclos en uno y 5 en el otro, y la primera cache responde en 5 y
en 3 respectivamente. Rellenar con la tabla del vecino da un residuo que parece
una respuesta.

## El formato

Texto, con la version en la cabecera (`vxarch 1`). Lo describe en detalle el
`README.md` del importador en el compilador; el lector de aqui vive en
`user/vxarch.h` y rechaza una version que no conozca en vez de interpretar las
columnas que le cuadren.
