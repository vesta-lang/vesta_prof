# profiler

El perfilador: el codigo que corre en **kernel** para muestrear con el PMU, y el
que corre en **usuario** para vaciar el anillo, llevar la sesion y atribuir las
muestras.

El diseno completo -- que se muestrea, como se comunica con el driver, que tiene
que llevar el registro de una corrida -- vive en `plan-vtune.md`. Este fichero
solo responde a **donde vive cada cosa y por que**, que es lo que hay que tener
decidido antes de escribir el primer mecanismo.

## Por que no se llama `driver/`

Porque en este arbol **"driver" ya significa otra cosa**: el driver AOT, que es
como los comentarios de `src/toolchain/aot_build.cpp` llaman a la parte que
orquesta la construccion -- "el driver obtiene un `NativeBackend`", "el driver
los serializa en la seccion `.vxgc_smap`". No tiene nada que ver con un driver
del sistema operativo.

No es una precaucion teorica: la confusion ya ocurrio en una conversacion de
diseno, y hubo que parar a aclarar de cual de los dos se estaba hablando. Un
directorio llamado `driver/` la habria vuelto permanente.

## Disposicion

```text
profiler/
  include/        el contrato COMPARTIDO y la API del lado de usuario
  common/         logica PORTABLE: no incluye una sola cabecera del sistema
  windows/        el .sys           (WDK)
  linux/          el modulo         (kbuild)
  user/           anillo, sesion, atribucion
  cli/            el ejecutable suelto
  tests/          las pruebas, sobre todo de `common/`
  bench/          cuanto cuesta el propio perfilador
  tools/          leer y presentar un registro de corrida
```

Las tres ultimas siguen la disposicion del asignador, que ya resolvio este mismo
problema.

**`tests/`** es donde se cobra lo de `common/`: como no incluye cabeceras del
sistema, sus pruebas corren en espacio de usuario como cualquier otra, con un
area DS falsa y un anillo de mentira. Lo que quede fuera de `common/` solo se
puede probar cargando el modulo.

**`bench/`** no es opcional aqui. Un perfilador que cuesta mas de lo que mide no
sirve, y el coste no se estima: se mide. Lo que hay que vigilar esta escrito en
el plan -- una lectura con `RDPMC` deberia rondar 20-30 ciclos, y el precio del
muestreo es **una interrupcion por desbordamiento**, no por instruccion --, y
son numeros que hay que poder comprobar en cada maquina. Con su `baseline/`
fechado y etiquetado por maquina, igual que en el asignador: una medida sin
maquina y sin fecha no se puede comparar con nada.

**`tools/`** para leer un registro de corrida y presentarlo, que es el papel que
`alloc_tree` hace para el asignador. Va aparte del `cli/` a proposito: el `cli/`
toma medidas y las herramientas las interpretan, y mezclarlo obligaria a
reconstruir el perfilador para cambiar como se pinta un informe.

Todo junto, y no el lado de usuario en `src/`, por como se distribuye: **el
driver viaja con el instalador del compilador**, asi que las dos mitades se
publican a la vez. Separarlas obligaria a mantener sincronizados dos arboles que
solo tienen sentido juntos.

## `common/`: la linea no es Windows contra Linux

La separacion util no es por sistema operativo. Es entre **lo que es del
procesador** y **lo que es del sistema**, y casi todo lo dificil cae del primer
lado -- que es el mismo en los dos.

```text
del PROCESADOR, portable            del SISTEMA, por fuerza aparte
----------------------------        ------------------------------
que MSR y con que codificacion      pedir memoria
la disposicion del area DS          mapearla a espacio de usuario
como se decodifica un registro      registrar el manejador de la PMI
  de PEBS y uno de LBR              cerrojos e inhibicion de interrupcion
detectar la microarquitectura       el dispositivo y sus IOCTL
la lista blanca y su validacion     temporizadores y trabajo diferido
los indices del anillo              la instruccion de leer un MSR *
```

`IA32_PERFEVTSELx`, `MSR_PEBS_DATA_CFG` o el formato de un registro de PEBS no
tienen nada de Windows ni de Linux: son de Intel. Escribir eso dos veces seria
mantener dos copias de la misma tabla y descubrir sus divergencias en
produccion.

(*) La instruccion es la misma; solo cambia como se escribe -- `__readmsr`
frente a `rdmsrl`. Es de las pocas cosas que `common/` necesita del sistema, y
las recibe por una interfaz pequena que cada lado implementa. Esa interfaz es
toda la dependencia que se le permite: **`common/` no incluye una sola cabecera
del sistema operativo**.

### Y esto es lo que hace que el driver se pueda probar

Es la razon de peso, por encima de no duplicar codigo. Codigo de kernel que solo
corre dentro del kernel se depura cargandolo en una maquina y viendo si se
cuelga. Codigo que no incluye cabeceras del sistema **se compila y se prueba en
espacio de usuario**, con el mismo arnes de `tests/` que todo lo demas: se le da
un area DS falsa y se comprueba que decodifica lo que tiene que decodificar; se
le llena el anillo y se comprueba que la politica de desbordamiento hace lo que
dice.

Dicho de otro modo: cuanto mas se lleve a `common/`, menos codigo queda que solo
se pueda probar arriesgando un equipo.

## Que pasa cuando el driver falla

Primero la mala noticia, porque condiciona todo lo demas:

> **En modo kernel no hay aislamiento.** Un fallo en el driver es un pantallazo
> en Windows y un panico en Linux. No existe "capturar la excepcion para no
> afectar a los demas": el driver **es** el kernel.

Cualquier diseno que prometa lo contrario esta mintiendo. La seguridad sale de
cuatro sitios, y estan ordenados por lo que compran de verdad.

### 1. Que no pueda fallar (el 95%)

Casi todo ya esta decidido y no se toma como medida de seguridad, pero lo es:

- **sin reservas despues del arranque** -- no hay camino de fallo de reserva en
  el manejador;
- **sin cerrojos en el manejador** -- no hay interbloqueo posible;
- **arrays de tamano fijo con indices validados** -- ni bucles sin cota ni
  despacho dinamico;
- **`common/` probado en espacio de usuario** antes de pisar un kernel, que es
  la diferencia entre depurar con un arnes y depurar reiniciando un equipo;
- **lista blanca de eventos** en vez de acceso generico a MSR, y
  **`METHOD_BUFFERED`** en vez de punteros de usuario crudos. Las dos ya estan
  en `plan-vtune.md`, con el motivo: es el patron que acaba en la lista de
  drivers vulnerables de Microsoft.

### 2. Los dos sitios donde capturar SI es posible

Y son dos, no mas:

| | Windows | Linux |
| --- | --- | --- |
| tocar memoria de usuario | `ProbeForRead`/`ProbeForWrite` dentro de `__try`/`__except` | `copy_from_user`/`copy_to_user`, que devuelven error |
| leer un MSR que quiza no exista | no hay variante segura: comprobar CPUID antes, o envolver en `__try` | `rdmsrl_safe` / `wrmsrl_safe` |

Windows si tiene SEH en kernel, y captura violaciones de acceso y #GP. **No es
una red de seguridad general**: usarlo para tapar un desreferenciado nulo propio
es enviar el fallo escondido en vez de arreglarlo.

De esa asimetria sale una decision de API que hay que tomar **antes** de escribir
la interfaz de `common/`, no despues:

> La primitiva de leer y escribir MSR tiene que **poder fallar**: devuelve un
> codigo, no `void`.

Si fuera `void`, el lado de Linux no podria usar `rdmsrl_safe` y el de Windows
no tendria como informar de un #GP capturado.

### 3. Fallar cerrado, y descargar en el orden correcto

- **Al cargar**: validar que la microarquitectura esta soportada, que el PMU
  existe y que los MSR son los que se esperan. Si algo no cuadra, **no cargar**.
  Un driver a medias es peor que uno ausente.
- **Al descargar**, el orden importa y es la causa numero uno de caidas en esta
  clase de driver: se paran los contadores, se desregistra el manejador, se
  sincroniza, y **solo entonces** se libera la memoria. Al reves, la siguiente
  interrupcion escribe en memoria ya liberada -- un manejador huerfano es un
  pantallazo garantizado.

### 4. El anillo es entrada NO confiable

Lo comparte el espacio de usuario, que puede escribir en el. El plan ya lo dice:
el driver **trata la region como no confiable para decidir nada**.

En concreto: los indices que ve usuario no valen para una decision de limites.
El driver lleva su propia copia y comprueba contra ella. Confiar en un indice
compartido es la version en memoria compartida del clasico "comprobar y luego
usar".

### Y en el anillo no va ni un puntero

> Todo lo que cruza son **indices y desplazamientos**. Ninguna direccion.

Por dos motivos, y el segundo es el grave:

1. **No sirve.** La region se mapea en espacio de usuario en otra direccion, asi
   que un puntero de kernel ahi no significa nada del otro lado.
2. **Es una fuga.** Publicar direcciones de kernel en memoria que lee el espacio
   de usuario es como se saltan las protecciones de aleatorizacion del espacio
   de direcciones. Es una clase de vulnerabilidad conocida, no una hipotesis.

Ademas sale gratis: el anillo y el estado por nucleo ya son arrays, y un indice
con centinela **se puede comprobar** -- `i < n` -- mientras que un puntero
corrupto no.

### 5. Herramientas, y donde se desarrolla

Driver Verifier en Windows; KASAN, lockdep y kmemleak en Linux. Y el desarrollo
va en una **maquina virtual con instantanea**: no por miedo, sino porque cada
ciclo de prueba incluye un reinicio y conviene que sea barato.

## Memoria

### De donde sale

No escribimos asignador: los dos kernels traen el suyo.

| | Windows | Linux |
| --- | --- | --- |
| memoria no paginada | `ExAllocatePool2(POOL_FLAG_NON_PAGED, n, tag)` | `kmalloc` / `kzalloc` |
| bloques grandes | el mismo, o una MDL | `vmalloc`, `alloc_pages` |
| mapear a usuario | MDL + `MmMapLockedPagesSpecifyCache` | `vmalloc_user` + `remap_vmalloc_range` |

`vesta_alloc` **no** sirve aqui, y no por falta de calidad: usa `VirtualAlloc` y
`mmap`, reemplaza `operator new` y es C++. Las tres cosas son imposibles en
kernel. Tampoco haria falta -- no se optimiza un camino que se recorre tres
veces.

En Windows, la **etiqueta de pool** no es opcional: es lo que permite ver desde
el depurador quien retiene memoria (`!poolused`). Sin ella una fuga del driver
es una cifra anonima.

### LA REGLA: en el kernel se reserva al arrancar, y nunca mas

> Todas las reservas ocurren en la inicializacion de la sesion. Despues, ni una.
> Y `common/` no reserva NUNCA: recibe la memoria ya reservada.

No es una preferencia de estilo ni una optimizacion. **El manejador de la PMI no
puede reservar**: corre a IRQL alta en Windows y en contexto de NMI en Linux,
donde reservar puede paginar, puede tomar un cerrojo y puede bloquearse contra
si mismo. Todo el diseno -- preasignar un anillo y escribir en el -- existe por
eso.

Y tiene un efecto lateral que conviene aprovechar: como `common/` no reserva,
sus pruebas en espacio de usuario le pasan un bufer de `malloc` y se comporta
igual.

### Que hacer cuando parezca que hace falta reservar

Esto es lo importante de la regla, porque el caso llegara. **No se resuelve
reservando igualmente.** Se responde en este orden:

1. **¿Es memoria por muestra?** Entonces va **en el anillo**, que ya esta
   reservado. Casi todos los casos son este y se acaban aqui.

2. **¿Hace falta dentro del manejador?** Entonces **no se puede**, y no es
   opinable: el contexto lo prohibe. Lo que haya que guardar se escribe en el
   anillo y lo procesa el lado de usuario, que si puede reservar.

3. **¿Hace falta al abrir la sesion?** Entonces **no rompe la regla**: eso es la
   inicializacion. Se reserva ahi y se le entrega a `common/`.

4. **¿Hace falta al reconfigurar en marcha?** Se para la sesion, se vuelve a
   reservar y se arranca. Nunca una reserva en el camino caliente.

Si un caso no encaja en ninguno de los cuatro, **el diseno esta mal y hay que
replantearlo**, no saltarse la regla. Y ese replanteo se escribe en
`plan-vtune.md` con su motivo: una excepcion sin razon anotada se convierte en
la norma la siguiente vez.

### Estructuras de datos: casi ninguna, y es la senal de que el diseno va bien

| | que es de verdad |
| --- | --- |
| el anillo | un array de tamano fijo con dos indices |
| estado por nucleo | un array indexado por numero de nucleo, dimensionado al arrancar |
| lista blanca de eventos | una tabla `static const`; ni se reserva |

Nada de eso necesita un contenedor, y no es casualidad: el diseno se eligio para
que fuera asi.

De ahi sale una **prueba util**: si en el driver aparece la necesidad de un mapa
o de una lista enlazada, es senal de que eso pertenece al lado de usuario. Los
dos kernels ofrecen estructuras -- `list_head`, `rbtree`, `xarray` en Linux;
`LIST_ENTRY`, `RTL_AVL_TABLE` en Windows -- pero usarlas rompe `common/`, porque
son cabeceras del sistema.

Si algun dia `common/` necesitara una de verdad, tendria que ser plana, escrita
en C y sobre un bufer que le den. No hay tercera via.

## Que lenguaje va donde

```text
include/   contrato compartido    C
common/    logica portable        C
linux/     el .ko                 C
windows/   el .sys                C
--------------------------------------- la frontera
user/      biblioteca             C++
cli/       binario suelto         C++
tests/     pruebas                C++
```

**Debajo de la frontera no hay eleccion que tomar.** El kernel de Linux no
admite C++: no es que este mal visto, es que kbuild no tiene compilador de C++
configurado, no hay excepciones, ni RTTI, ni biblioteca estandar. Y `common/`
existe para compartirse con ese modulo; en C++ no se podria, y el directorio
perderia su razon de ser.

El kernel de Windows si admite C++ con restricciones -- sin excepciones, sin
RTTI, sin STL, y con cuidado porque no hay CRT que ejecute los inicializadores
estaticos --, pero el `.sys` va en C igualmente. Tener las dos mitades del
kernel en el mismo lenguaje significa que quien revisa una entiende la otra, y
la alternativa solo compra asimetria.

Encima de la frontera, C++ sin dudarlo: la vida de la sesion y de los
descriptores pide RAII, los bufers de muestras piden contenedores, y ademas se
integra con el compilador, que es C++.

Es la misma linea que ya usa el emisor AOT del proyecto, cuya cabecera interna
lo dice igual de claro: *"se compila SOLO desde los .c de emision (todos C); el
lado C++ del compilador no lo ve"*.

### El C que ve C++ tiene que compilar como C++

Y solo ese. La regla cuesta poco desde el principio y carisimo despues, asi que
conviene tenerla clara:

- `include/` -- **si**: el contrato lo incluye el lado de usuario, que es C++.
- `common/` -- **si**: sus pruebas son C++ y van a incluir sus cabeceras.
- `windows/` y `linux/` por dentro -- **no**. Nadie las incluye desde C++, y sus
  interioridades no se pueden probar en usuario de todas formas.

En la practica, para lo que si: guardas `#ifndef` en las macros, nada de structs
anonimos sin marcar, y `extern "C"` en las cabeceras.

No es teorico. En este mismo arbol, `CreateELF.h` redefinia macros que ya traia
su libreria hermana y usaba structs anonimos; el resultado fue que **el trozo de
codigo mas facil de equivocar era el unico que no se podia probar**, porque la
cabecera que lo contenia no compilaba como C++.

## Donde va cada cabecera

La regla es la del resto del arbol, que tiene 408 cabeceras en `include/` y
**cinco** en `src/` -- `aot_emit_internal.h`, `lowering_internal.h`,
`codec_internal.h` y compania:

> `include/` es lo **publico**. Al lado del fuente va lo que incluir desde fuera
> seria un error, y se avisa en el nombre.

No es una preferencia de estilo. `aot_emit_internal.h` es privada porque
arrastra las cabeceras de LibPEparse y solo compila como C; de haber estado en
`include/`, alguien la habria incluido y se habria encontrado veinte errores de
macro redefinida sin entender por que.

Aqui se traduce asi:

- **`include/`**: dos cosas y solo dos -- el contrato compartido kernel-usuario
  y la API publica de la biblioteca de usuario. Las dos cruzan una frontera de
  verdad.
- **`common/`**: sus cabeceras al lado de sus fuentes. Una cabecera de `common/`
  incluida desde codigo de usuario corriente es un error esperando a ocurrir:
  esta escrita para tres entornos y da por hechas unas primitivas que quien la
  incluye tiene que proporcionar.
- **`windows/` y `linux/`**: sin cabeceras publicas. Un driver no tiene API.

### La excepcion: los indices del anillo van en linea

Y no por estilo, por dos motivos concretos:

- el productor corre en el manejador de la PMI, a IRQL alta, donde una llamada
  cuesta y no se puede paginar;
- productor y consumidor tienen que calcular **exactamente lo mismo**. Si
  divergen, el anillo no falla: se lee desde el otro extremo y devuelve muestras
  que no son. Que los dos compilen la misma definicion es la unica garantia.

## Es un proyecto aparte que el compilador reutiliza

El mismo trato que `vesta_alloc`, y por los mismos motivos:

- **Dentro de otro proyecto** (`add_subdirectory`): el compilador enlaza contra
  su objetivo y ya esta.
- **Solo**, como cualquier proyecto: `cmake -S . -B build`, y sale su `.a` con
  sus cabeceras instalables y sus pruebas.

Se detecta cual de los dos con `CMAKE_SOURCE_DIR STREQUAL
CMAKE_CURRENT_SOURCE_DIR`, que es como lo hace el asignador.

A diferencia del asignador, aqui el objetivo principal es un **archivo estatico**
y no uno de objetos. Alli tiene que ser de objetos porque reemplaza el
`operator new` global y de un archivo el enlazador solo saca lo que alguien
nombra -- y nadie nombra `operator new`. Aqui no hay nada que se reclame de
forma implicita: todo lo que ofrece se llama por su nombre, asi que un `.a`
normal se comporta como se espera.

## La restriccion que sostiene el modo suelto

El perfilador se entrega de dos maneras: **dentro del compilador**, que es la
principal -- se consume desde el y, con el, desde el servidor LSP --, y **como
binario suelto**, para perfilar sin tenerlo delante.

De ahi la regla de capas mas importante de este directorio, que es literalmente
la que `vesta_alloc` declara en la cabecera de su `CMakeLists.txt`:

> **No depende de nada del compilador.** Se puede sacar de aqui y llevarselo.

Si dependiera, el modo suelto no existiria -- o existiria arrastrando el
compilador entero, que es lo mismo. En la practica: no enlaza contra `vmcore`
ni contra `vx_lib`, y lo que necesite de ellos entra por una interfaz que el
modo suelto pueda dejar sin implementar.

La atribucion es donde esto se nota. Resolver una direccion a fichero y linea
sale de DWARF, que es autonomo; atribuirla a un **nodo del intermedio** necesita
lo que sabe el compilador. El suelto tiene la primera y no la segunda, y eso es
una diferencia deliberada, no una carencia.

## El contrato compartido

`include/` guarda el unico fichero que ven los **dos** mundos: la disposicion
del anillo, el formato de una muestra y los codigos de control.

Tiene que poder incluirse desde codigo de kernel, asi que es **C plano**: tipos
de ancho fijo, sin biblioteca estandar, sin STL y sin ninguna cabecera nuestra.
Un descuido ahi no da un error de compilacion claro en el lado del kernel, da
uno incomprensible o -- peor -- una estructura con distinto tamano a cada lado
del anillo.

## Que se construye y cuando

El lado de usuario se construye **siempre**: es parte de lo que se instala, y no
necesita nada especial.

Los dos lados de kernel no se parecen en nada mas que en el nombre, y conviene
no tratarlos como si fueran el mismo con un `if` delante.

**No conviven.** El `.sys` se construye en Windows y el `.ko` en Linux, y nunca
los dos desde la misma configuracion: un modulo de kernel se compila con las
herramientas y las cabeceras del kernel al que va a cargarse, asi que
cross-compilar uno desde el otro no es algo que se resuelva con banderas.

**No se construyen igual.** El `.sys` es un objetivo de CMake normal con las
banderas del WDK. El `.ko` **no**: los modulos de Linux se construyen con
`kbuild`, que es el sistema de construccion del propio kernel, y lo unico que
puede hacer CMake es invocarlo. Escribir el `.ko` como si fuera una biblioteca
de CMake produciria algo que enlaza y que el kernel no carga.

De ahi que lo que se detecte sea, en cada anfitrion, si estan las herramientas
de SU lado -- el WDK, o las cabeceras del kernel en ejecucion -- y que si faltan
se omita **diciendolo por la salida de configuracion**. Un desarrollador tiene
que poder construir el compilador sin nada de esto instalado; lo que no puede es
creerse que construyo el driver cuando no lo hizo, que es como se acaba enviando
un instalador sin `.sys` dentro.

### LTO no es una optimizacion mas: es parte del diseno

Va encendido **por defecto**, y apagarlo cambia lo que cuesta el codigo, no solo
lo rapido que va.

El motivo es el estilo que se eligio en `common/`: modulos con dueno, cabecera
opaca, implementacion en su `.c`, y comprobaciones en la frontera. Ese reparto
pone una llamada real entre el consumidor y el modulo **en cuanto viven en
unidades de traduccion distintas**, que es siempre. Medido sobre las pruebas del
cimiento, el mismo programa:

| | llamadas a `probe_*` en el binario |
| --- | --- |
| sin LTO | 3 |
| con LTO | **0** |

Los dos binarios dan el mismo resultado. Con LTO no solo desaparece la llamada:
desaparece tambien **la comprobacion de la marca**, porque el compilador ve que
`init` acababa de ponerla y prueba que la condicion es falsa. Lo que queda no es
lo que se escribio, y esa es exactamente la idea -- se escribe la version segura
y legible, y el enlazador se queda con la barata.

Sin LTO, el precio de este estilo se paga entero: una llamada indirecta al
modulo, mas sus comprobaciones, en cada operacion. Con el, el precio es cero y
lo unico que queda es la seguridad.

De modo que **no se trata de una bandera de rendimiento que se pueda olvidar**.
Si alguien construye sin LTO tiene otro perfil de coste, y en un perfilador eso
significa medirse a uno mismo.

La unica excepcion es `Debug`, donde va apagado a proposito: la razon de existir
del modo es que la pila que se ve en el depurador corresponda al codigo escrito,
y el LTO deshace justamente eso. En un driver, donde muchas veces el volcado es
la unica evidencia, no es un detalle.

Para el `.ko` no aplica nada de esto: lo construye `kbuild`, con las opciones del
kernel, y no es CMake quien decide.

### El contrato, visto desde los tres sitios

Esto obliga a que la cabecera compartida compile en **tres** entornos, no en
dos: kernel de Windows, kernel de Linux y espacio de usuario.

Y ahi hay una trampa concreta: **el kernel de Linux no tiene `<stdint.h>`**. Sus
tipos de ancho fijo son `u8`/`u32`/`u64` (o `__u8` en las cabeceras que ve el
espacio de usuario). Una cabecera que incluya `<stdint.h>` compila en los otros
dos entornos y falla solo en ese, que es justo el que menos veces se prueba.

Los tipos, por tanto, se resuelven en la propia cabecera segun donde se este
incluyendo, y esa es la unica logica condicional que se le permite: todo lo
demas -- disposicion del anillo, formato de la muestra, codigos de control --
tiene que ser identico byte a byte a los tres lados, o el anillo se lee mal
desde el otro extremo.

Cuando se construye suelto, las pruebas y los ejemplos se activan por defecto y
al integrarse se apagan, igual que hacen el preprocesador y el asignador: dentro
de otro proyecto duplicarian binarios y tiempo de construccion sin aportar nada.
