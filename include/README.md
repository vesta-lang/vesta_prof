# include/

Lo **publico**, y solo dos cosas:

1. **El contrato compartido kernel-usuario**: la disposicion del anillo, el
   formato de una muestra y los codigos de control.
2. **La API de la biblioteca de usuario**, que es lo que consumen el compilador
   y el binario suelto.

Las dos cruzan una frontera de verdad. Lo que no la cruce va al lado de su
fuente.

## Y por eso este directorio es MIT

Es la unica parte del proyecto que no lleva la GPLv2. La frontera de la licencia
es exactamente esta: **`include/` es MIT, todo lo demas es GPLv2** -- ver
`NOTICE` en la raiz.

El motivo sale de lo que se acaba de decir. Toda la seccion siguiente trata de
que el contrato se describa a si mismo para que **herramientas de terceros**
puedan leer lo que el perfilador produce. Bajo copyleft sin excepcion, cualquiera
que incluyera `vxp_abi.h` para escribir un analizador tendria que licenciar su
herramienta como GPL: se habria construido un formato abierto que legalmente
nadie puede consumir sin cambiar la licencia de su producto, y el trabajo de
hacerlo autodescriptivo no serviria de nada.

Tiene una consecuencia practica al decidir donde va una cabecera, y conviene
tenerla presente: **poner algo aqui es publicarlo bajo MIT**. Si una cabecera no
cruza la frontera kernel-usuario ni forma parte de la API de la biblioteca, va
al lado de su fuente -- no por orden, sino porque aqui se regala.

## Lo que exige el contrato compartido

Tiene que compilar en **tres** entornos: kernel de Windows, kernel de Linux y
espacio de usuario. Eso impone dos cosas:

- **C plano, y ademas que compile como C++.** Sin STL, sin biblioteca estandar,
  sin ninguna cabecera nuestra -- pero con `extern "C"`, guardas `#ifndef` en
  las macros y ningun struct anonimo sin marcar, porque quien lo incluye por
  arriba es C++.
- **Sin `<stdint.h>`.** El kernel de Linux no lo tiene: usa `u8`/`u32`/`u64`.
  Una cabecera que lo incluya compila en los otros dos entornos y falla solo en
  ese, que es el que menos veces se prueba.

Los tipos se resuelven dentro de la propia cabecera segun donde se incluya, y
esa es **la unica logica condicional que se le permite**. Todo lo demas tiene
que ser identico byte a byte a los tres lados: si una estructura mide distinto
en un extremo, el anillo no da error -- devuelve muestras que no son.

## El contrato se describe a si mismo

El driver y el lado de usuario son **binarios distintos que se versionan por
separado**. Un equipo puede acabar con un driver viejo y un consumidor nuevo, o
al reves. Si el formato de una muestra cambia y nadie lo comprueba, el consumidor
no falla: lee campos corridos y publica numeros que parecen razonables.

Asi que el contrato **publica su propia disposicion**, en tres capas, y cada una
esta donde se puede pagar:

### 1. Cada registro del anillo lleva `{tipo, longitud}`

Cuatro bytes por registro, y compran la propiedad que importa: un consumidor
puede **saltarse un registro que no entiende** y seguir leyendo. Sin eso, un
tipo de muestra nuevo no degrada la lectura, la detiene.

Tambien tolera que un registro sea mas LARGO de lo que el consumidor conoce, que
es lo que permite anadir campos al final sin romper a nadie.

Es lo mas caro que se puede permitir aqui: esto lo escribe el manejador de la
PMI, millones de veces.

### 2. Las estructuras del control llevan su propio tamano

Las que van por IOCTL empiezan por su tamano, al estilo de `cbSize`. Se llaman
una vez por sesion, asi que el coste da igual, y permite lo mismo: campos nuevos
al final, y un extremo antiguo lee lo que conoce.

### 3. Y se puede PREGUNTAR por la disposicion

Un IOCTL que devuelve, por cada estructura publicada, su tamano y el
desplazamiento de sus campos. El consumidor lo consulta al conectar y compara
con lo que el trae compilado.

**Para VALIDAR, no para ACCEDER.** Es la precision que evita que esto salga caro:
se consulta una vez al abrir sesion y despues se usan los desplazamientos
compilados, como siempre. Un analizador que resolviera cada campo por el
descriptor seria correcto y lentisimo, y estariamos midiendo el perfilador en
vez del programa.

El propio descriptor tiene que ser lo mas simple posible -- entradas de tamano
fijo, `{id, desplazamiento, tamano}` -- porque **su formato es el unico que
nunca puede cambiar**. Es el problema del huevo y la gallina: si cambia como se
describe la disposicion, no queda forma de describirlo.

### La regla que hace que todo esto funcione

> Los campos se **anaden al final**. No se reordenan, no se quitan, y sobre todo
> **no se reutilizan para otra cosa**.

Un descriptor de disposicion salva de un campo movido. No salva de un campo que
cambio de SIGNIFICADO conservando su nombre, su sitio y su tamano: ahi los dos
extremos se entienden perfectamente y dicen cosas distintas.

### De donde sale, sin duplicar nada

La misma tabla que declara una estructura sirve para las tres cosas -- declarar,
afirmar la disposicion al compilar y describirla en ejecucion --, igual que
`windows/nt_layout.inc` ya hace las dos primeras.

Una sola fuente. Si el descriptor de ejecucion se escribiera aparte, seria una
segunda copia de la disposicion, y la que se separa siempre es la que nadie
mira.
