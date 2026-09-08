# El contrato compartido

Lo unico que ven los **dos** mundos: la disposicion del anillo, el formato de una
muestra y los codigos de control. Vive en `include/`, que por eso mismo es la
unica parte del proyecto bajo MIT -- ver `NOTICE`.

## Lo que exige

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

## Se describe a si mismo

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

## El anillo y la muestra

Tres cosas que **no se pueden anadir despues**, y que por eso nacen ya en el
contrato: reservar el sitio antes de necesitarlo es barato, y el descriptor de
disposicion salva de un campo movido pero no de un campo que nunca existio.

### La cabecera del anillo lleva su propio tamano

Es la unica estructura que no puede tomar la medicina del `cbSize` mas tarde,
porque **la parsean los dos lados desde el primer arranque**. Y ahi van a querer
aparecer cosas: el descriptor del evento del anillo, la clase de nucleo, las
anclas TSC-QPC, el estado de estrangulamiento, un contador de generacion.

De modo que nace con su tamano por delante y con relleno **reservado**. Los dos
indices, ademas, en lineas de cache distintas -- y el tamano de linea es una
constante de construccion, no un 64 escrito a mano: en las Apple M son 128, y
equivocarse ahi no falla, solo va lento y nadie sabe por que.

> Y el driver **nunca** usa el indice del consumidor para una decision de
> limites. Lleva su copia en el bloque opaco del productor. Sin esa separacion,
> la regla de que el anillo es entrada no confiable no se puede cumplir aunque
> se quiera. Ver [`style.md`](style.md), "dos formas de objeto".

### El techo por registro es 64 KiB, y se dice

`length` es de 16 bits. No sube a 32, y el motivo es que la cabecera se escribe
**millones de veces desde el manejador de la PMI**: doblarla a 8 bytes es
precisamente el coste que esta capa no se puede permitir.

Con eso caben de sobra los casos previstos -- un registro PEBS adaptativo con
todos sus grupos ronda 1,3 KiB, y una copia de pila de refuerzo tampoco se
acerca --, pero es un TECHO y conviene tenerlo escrito antes de encontrarselo.

La via de escape, nombrada de antemano en vez de improvisada: una carga que no
quepa se parte en varios registros encadenados con un `kind` de continuacion.

### Como se parsea una muestra: un mapa de bits por sesion

Que campos lleva cada muestra se declara **una vez por sesion**, no registro a
registro. Encaja con lo que ya hay en vez de competir:

```text
kind          dice QUE es el registro
length        permite SALTARLO sin entenderlo
sample_type   declarado una vez, dice como se parsea la carga
```

Asi lo que viene despues -- el periodo efectivo en cada muestra, la pila de LBR,
la informacion de memoria, la cadena de llamadas, los grupos de PEBS -- es
**un bit mas**, y no una estructura nueva cada vez.

Con una regla que lo hace tolerante a versiones:

> Los campos se emiten en **orden canonico por numero de bit**.

Sin ella, un bit que el consumidor no conoce lo ciega para el registro ENTERO,
porque los campos son posicionales. Con ella, parsea hasta ese punto y salta el
resto, que es la misma propiedad que compra `length` un nivel mas arriba.

### Se pregunta tambien el CATALOGO, no solo la disposicion

La capa 3 de arriba responde "¿como es esta estructura?". Falta la hermana:
**"¿que eventos soportas?"**, preguntada una vez al abrir sesion.

Sin ella, un consumidor nuevo contra un driver viejo tiene que **probar y
fallar evento por evento** para averiguar que hay. Es el mismo problema que
resuelve el descriptor de disposicion, aplicado al catalogo, y por eso vive
aqui: se responde con entradas de tamano fijo y el consumidor compara con lo
que trae compilado.
