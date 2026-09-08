# Las tres politicas del muestreo

Que hace el anillo cuando se llena, que eventos se pueden pedir y cada cuanto se
muestrea.

Viven en `common/` por el mismo motivo que todo lo demas de ese directorio: son
del procesador y no del sistema, asi que se prueban en espacio de usuario -- se
llena un anillo de mentira y se comprueba que la politica hace lo que dice, en
vez de averiguarlo cargando un driver.

## El anillo: uno por nucleo, y se pierde lo NUEVO

**Uno por procesador logico.** Eso hace al productor **unico**, que es lo que
quita el CAS de la cabeza y evita que una linea de cache rebote entre todos los
nucleos en el sitio mas caliente que hay. Ademas la perdida se vuelve LOCAL:
"el nucleo 3 perdio el 40%" se puede investigar, "la maquina perdio el 4%" no.
Y no hay que renunciar al orden global, porque cada muestra ya lleva TSC e id de
nucleo y usuario los mezcla al vaciar.

No es una eleccion exotica: es lo que hace `perf` (un anillo mapeado por CPU),
lo que impone IBS en AMD -- la interrupcion llega en el nucleo donde se ejecuto
la operacion -- y lo que hacen PMUv3 y SPE en ARM.

**Al llenarse se tira lo NUEVO**, y las otras dos opciones se descartaron por
motivos distintos:

| | |
| --- | --- |
| perder lo viejo | con registros de longitud variable, avanzar la cola obliga a **parsear el anillo a IRQL alta** sobre registros que el consumidor puede estar leyendo, y le impone copiar-y-validar con un bucle que bajo presion sostenida puede no converger |
| frenar al productor | imposible tal cual: el productor es el manejador de la PMI. Esperar ahi a un consumidor de usuario que puede estar desalojado no es lentitud, es un cuelgue |

Perder lo nuevo tiene la propiedad que importa a IRQL alta: **nunca se reescribe
un byte ya publicado**, asi que el consumidor decodifica en sitio y el argumento
de correccion cabe en una frase.

Su defecto es real y se compensa, no se ignora: pierde durante la rafaga, o sea
justo cuando pasa lo interesante. De ahi las dos piezas que lo acompanan:

- **el `LOST` va SITUADO en el tiempo** (`{registros, bytes, tsc_primero,
  tsc_ultimo}`). Un total dice "perdiste el 4%"; uno situado dice "y todo cayo
  en 12 ms del nucleo 3", que es la diferencia entre un perfil que se puede leer
  y uno que no;
- **al cruzar una marca de agua se ESTRANGULA** -- se sube el periodo y se
  anota. Es la unica forma realizable de "frenar al productor", y es
  estrictamente mejor que perder: una ventana estrangulada tiene un factor de
  escala **conocido**, una perdida tiene un agujero.

Sobrescribir sigue teniendo un caso legitimo -- la grabadora de vuelo, "dejalo
correr y cuando reviente vuelca los ultimos N ms" --, asi que queda como bandera
**reservada** en la cabecera, sin implementar. Reservarla hoy cuesta nada;
anadirla despues es romper el contrato publico.

## La lista blanca: los 7 arquitectonicos, y es una FRONTERA

Existe por seguridad y no por comodidad. Un IOCTL que deje escribir un MSR
arbitrario es una primitiva de escalada de privilegios -- el patron que acaba en
la lista de drivers vulnerables de Microsoft --, y esto es lo que lo convierte en
"pideme uno de estos".

La primera version son los **siete eventos arquitectonicos**, tres de ellos
servibles desde un contador fijo (lo cual libera uno de proposito general, y
**decide el asignador, no quien llama**). Tres razones, en orden:

1. **no necesitan tabla por microarquitectura**, asi que la primera version no
   queda esperando al estudio de eventos precisos;
2. Windows hace el mismo corte en lo que expone por su via de trazas, lo que es
   una confirmacion independiente de donde esta el limite natural;
3. `reference cycles` parece redundante y no lo es: los ciclos de nucleo varian
   con la frecuencia, y en una pieza hibrida los P y los E no corren a la misma.
   Es lo unico que hace que comparar entre clases de nucleo signifique algo.

Lo que hace que sea una frontera y no una lista, y es la parte que se olvida:

> Quien llama fija **`USR` y `OS`, y nada mas**. `ANY` -- contar los dos
> hermanos SMT -- se fuerza a cero SIEMPRE: es un canal de observacion entre
> hilos y ademas hace imposible atribuir. Y no basta con darlo por obsoleto:
> medido en la pieza de desarrollo, el bit sigue vivo.

`EDGE`, `INV` y `CMASK` en cambio **si viven en la tabla**, porque son parte de
la DEFINICION de muchos eventos y no knobs sueltos -- el idioma `CMASK=1,
EDGE=1` convierte "cuantos ciclos atascado" en "cuantos episodios de atasco", y
los ficheros de eventos del fabricante traen decenas de entradas asi. Cerrarlos
en el contrato dejaria a la lista blanca sin poder expresar sus propias entradas
futuras.

Y la tabla esta **indexada por clase de nucleo desde el principio**, aunque hoy
las dos filas salgan iguales: el mismo id simbolico se codifica distinto en un P
que en un E y entre familias, y si nace plana, meter el segundo eje despues es
tocar la frontera de seguridad entera.

### El segundo eje: poder hacer PEBS es de EJECUCION

Que un evento sea preciso no es una propiedad del evento: es de la maquina tal
como esta configurada **ahora mismo**, y puede cambiar entre dos arranques del
mismo equipo. Asi que se consulta al abrir sesion, se devuelve en la respuesta y
se anota en el registro de la corrida.

No es teorico. En la maquina de desarrollo -- un i7-13700KF con Hyper-V activo --
CPUID dice que no hay Debug Store, y la lectura autorizada lo confirma:
`IA32_MISC_ENABLE` bit 12 puesto en los 24 procesadores logicos, con el formato
de registro PEBS a cero. Ahi la respuesta es "no", y eso es informacion, no un
fallo.

## El periodo: los dos modos, con suelo y sorteado

Se admiten **periodo fijo y tasa objetivo**, los dos. Y tres reglas que hacen
falta y no son obvias:

**El suelo se RECHAZA, no se recorta.** Cada evento lleva el suyo en la misma
tabla de la lista blanca. Por debajo se devuelve error diciendo cual es. No es
solo higiene: un periodo minusculo sobre un evento frecuente son millones de
interrupciones por segundo, o sea una denegacion de servicio pedida desde
usuario. Y recortar callando convierte "pediste algo imposible" en "aqui tienes
otra cosa", que es como se acaba publicando un numero que no corresponde a lo
que se pidio.

Con la tasa objetivo hay una interaccion que muerde: el lazo puede empujar el
periodo por debajo del suelo. Ahi se queda en el suelo y **lo dice** -- si no,
entrega menos muestras de las pedidas sin explicar por que, y eso se lee como
que el programa hizo menos trabajo.

**El periodo se sortea, y por defecto.** Uno estrictamente fijo **aliasa con el
comportamiento periodico del programa**: si el coste por iteracion de un bucle
divide al periodo, se muestrea siempre la misma instruccion y sale un perfil muy
confiado y completamente falso. Se sortea cada recarga en `P +- j%` con un PRNG
por (nucleo, evento). Como las recargas son independientes y de media `P`, el
estimador `eventos ~= muestras x P` **sigue siendo insesgado**.

Dos detalles del sorteo, los dos aprendidos de pensar en como falla:

- **no se realimenta al lazo** de la tasa objetivo. Si el lazo viera el ruido del
  sorteo lo perseguiria y oscilaria; trabaja sobre la media;
- **la semilla va al registro**, para que una corrida sorteada se pueda repetir
  exactamente. Asi se tiene el sorteo y la reproducibilidad, en vez de elegir.

**Una ventana estrangulada vale menos que una normal**, y se declara. No es
ruido: se estrangula cuando hace calor, o sea que se submuestrea precisamente la
parte caliente. Es un sesgo correlacionado y tiene que viajar con el hecho, como
todo lo demas.
