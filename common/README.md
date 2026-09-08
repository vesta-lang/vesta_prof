# common/

Lo que es **del procesador** y no del sistema operativo. Es decir, casi todo lo
dificil.

```text
aqui                                aqui NO
--------------------------------    ------------------------------
que MSR y con que codificacion      pedir memoria
la disposicion del area DS          mapearla a espacio de usuario
decodificar un registro de PEBS     registrar el manejador de la PMI
  y uno de LBR                      cerrojos e inhibicion de interrupcion
detectar la microarquitectura       el dispositivo y sus IOCTL
la lista blanca y su validacion     temporizadores y trabajo diferido
los indices del anillo
```

`IA32_PERFEVTSELx`, `MSR_PEBS_DATA_CFG` o el formato de un registro de PEBS no
tienen nada de Windows ni de Linux: son de Intel. Escribirlos dos veces seria
mantener dos copias de la misma tabla y descubrir que divergieron en produccion.

## La regla

> **No se incluye ni una sola cabecera del sistema operativo.**

Lo poco que hace falta del sistema -- leer un MSR es la misma instruccion, solo
cambia como se escribe: `__readmsr` frente a `rdmsrl` -- entra por una interfaz
pequena que implementa cada lado. Esa interfaz es toda la dependencia que se
permite.

Y una condicion de esa interfaz, que hay que respetar al escribirla:

> **Leer y escribir un MSR tiene que poder FALLAR.** Devuelve un codigo, no
> `void`.

Porque los dos lados fallan distinto y los dos tienen que poder decirlo. Linux
tiene `rdmsrl_safe`/`wrmsrl_safe`, que devuelven error en vez de provocar una
excepcion de proteccion general. Windows no tiene variante segura: comprueba
CPUID antes, o envuelve en `__try`/`__except`.

Con una firma `void` ninguno de los dos podria informar, y un MSR que no existe
en esa microarquitectura **pasaria por un valor leido** -- no un fallo, un dato
falso.

## Y por eso el driver se puede probar

Es la razon de peso, por encima de no duplicar codigo.

Codigo de kernel que solo corre dentro del kernel se depura cargandolo en una
maquina y viendo si se cuelga. Codigo sin cabeceras del sistema **se compila y
se prueba en espacio de usuario**, con el mismo arnes que todo lo demas: se le
da un area DS falsa y se comprueba que decodifica lo que debe; se le llena el
anillo y se comprueba que la politica de desbordamiento hace lo que dice.

Cuanto mas se lleve aqui, menos codigo queda que solo se pueda probar
arriesgando un equipo.

## Aqui no se reserva memoria. Nunca

> `common/` recibe la memoria ya reservada. No pide, no libera, no guarda un
> asignador.

Es una obligacion de este directorio, no una recomendacion. El manejador de la
PMI corre a IRQL alta (contexto de NMI en Linux) y ahi reservar puede paginar,
tomar un cerrojo o bloquearse contra si mismo. Todo lo que este directorio hace
esta pensado para trabajar sobre memoria que ya existe.

El efecto lateral es el que hace posible probarlo: en espacio de usuario, las
pruebas le pasan un bufer de `malloc` y se comporta igual que con un
`ExAllocatePool2` o un `vmalloc` detras.

El motivo completo, y **el procedimiento a seguir cuando parezca que hace falta
reservar** -- que no es reservar igualmente --, en
[`doc/memory.md`](../doc/memory.md).

## Lenguaje: C, pero C que compile como C++

**C** porque este directorio se comparte con el modulo de Linux, y el kernel de
Linux no admite C++.

**Que compile como C++** porque las pruebas de aqui son C++ y van a incluir
estas cabeceras. En la practica: guardas `#ifndef` en las macros, nada de
structs anonimos sin marcar, y `extern "C"`.

No es una precaucion teorica. `CreateELF.h`, en el arbol del compilador que
reutiliza este proyecto, redefinia macros que ya traia su libreria hermana y
usaba structs anonimos; el resultado fue que el trozo de codigo mas facil de
equivocar -- aritmetica de direcciones, sin contexto para saber si el numero que
sale es el bueno -- era justo el unico que no se podia probar, porque su
cabecera no compilaba como C++.

## Las tablas de los manuales tienen DOS caras

Lo que se saca de los manuales de Intel y AMD se emite dos veces, en la misma
pasada del generador:

| | Para que |
| --- | --- |
| `msr_index.h`, `cpuid_index.h` | **macros**. Preguntar por un campo que ya se conoce al escribir el codigo |
| `*/table.c` | **datos**. Recorrerlos todos sin saber de antemano cuales hay |

No es duplicar. Una macro no existe en ejecucion: no se puede iterar sobre ella
ni imprimir su nombre, que es justo lo que necesita un volcado. Salen del mismo
sitio en la misma pasada para que no puedan discrepar; generarlas por separado
es como se acaba con una que dice una cosa y otra que dice otra.

**Cada tabla es su propia unidad de traduccion**, y eso es lo que permite que
esten en el arbol sin que el driver las pague: quien no las referencie no las
enlaza. El nombre de cada fila es un **desplazamiento** dentro de un bloque de
cadenas, no un puntero -- un puntero por fila serian ocho bytes y una
reubicacion que el cargador resuelve al arrancar, una por fila --.

Y el bloque es un **array de bytes**, no un literal de cadena: los literales
adyacentes se concatenan en uno solo, C99 garantiza 4.095 caracteres y MSVC
corta en 65.535, que es un limite duro y los nombres de MSR ya rondan los
treinta mil.

## Cabeceras

Al lado de sus fuentes, no en `include/`. Una cabecera de aqui incluida desde
codigo de usuario corriente es un error esperando a ocurrir: esta escrita para
tres entornos y da por hechas primitivas que quien la incluye tiene que
proporcionar. Que estuviera en `include/` la anunciaria como algo utilizable.

## Y para escribir aqui dentro

- **Como se habla con el procesador y como se escriben los modulos** --
  intrinsecos y nunca asm en linea, tipos opacos con almacenamiento del
  llamante, nada de punteros opcionales: [`doc/style.md`](../doc/style.md).
- **Las tres politicas del muestreo** -- que hace el anillo cuando se llena, que
  eventos se pueden pedir y cada cuanto se muestrea:
  [`doc/sampling.md`](../doc/sampling.md).
