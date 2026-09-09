# profiler

El perfilador: el codigo que corre en **kernel** para muestrear con el PMU, y el
que corre en **usuario** para vaciar el anillo, llevar la sesion y atribuir las
muestras.

Herramienta de medida **por observacion**: muestrea un proceso desde fuera y
atribuye las muestras al codigo usando lo que el artefacto ya sabe de si mismo.
No instrumenta, no obliga a recompilar y no aparece dentro de su propia medida.

Este fichero responde a **donde vive cada cosa y como se construye**. El porque
de cada decision esta en [`doc/`](doc/), indexado al final.

## Como se construye

```bash
cmake -S . -B build && cmake --build build && ctest --test-dir build
```

Suelto o dentro de otro proyecto (`add_subdirectory`), como `vesta_alloc`.

Lo que puede faltar se detecta y **se dice por la salida de configuracion**, en
vez de omitirse en silencio:

| | hace falta | si no esta |
| --- | --- | --- |
| lado de usuario y pruebas | nada especial | se construye siempre |
| el `.sys` de Windows | GCC o Clang, y `dlltool` | se omite diciendolo |
| firmar el `.sys` | `signtool` y `.devcert/` | se construye sin firmar, y **no cargara** |
| el `.ko` de Linux | las cabeceras del kernel en ejecucion | se omite diciendolo |

**No hace falta el WDK.** Las declaraciones del kernel viven en `windows/nt.h`,
la biblioteca de importacion se genera de nuestro `windows/ntoskrnl.def`, y el
resto son banderas de enlace.

Para cargar el driver en desarrollo, `tools/dev_setup.cmd` monta la maquina
(certificado propio y modo de pruebas; pide administrador y un reinicio) y
`tools/dev_teardown.cmd` la devuelve a su estado.

## Disposicion

```text
profiler/
  include/        el contrato COMPARTIDO y la API del lado de usuario
  common/         logica PORTABLE: no incluye una sola cabecera del sistema
  windows/        el .sys           (sin WDK)
  linux/          el modulo         (kbuild)
  user/           anillo, sesion, atribucion
  cli/            el ejecutable suelto
  tests/          las pruebas, sobre todo de `common/`
  bench/          cuanto cuesta el propio perfilador
  tools/          leer y presentar un registro de corrida
  doc/            el porque de cada decision
```

**`tests/`** es donde se cobra lo de `common/`: como no incluye cabeceras del
sistema, sus pruebas corren en espacio de usuario como cualquier otra, con un
area DS falsa y un anillo de mentira. Lo que quede fuera de `common/` solo se
puede probar cargando el modulo.

**`bench/`** no es opcional aqui. Un perfilador que cuesta mas de lo que mide no
sirve, y el coste no se estima: se mide. Una lectura con `RDPMC` deberia rondar
20-30 ciclos, y el precio del muestreo es **una interrupcion por
desbordamiento**, no por instruccion -- numeros que hay que poder comprobar en
cada maquina. Con su `baseline/` fechado y etiquetado por maquina, igual que en
el asignador: una medida sin maquina y sin fecha no se puede comparar con nada.

**`tools/`** para leer un registro de corrida y presentarlo, que es el papel que
`alloc_tree` hace para el asignador. Va aparte del `cli/` a proposito: el `cli/`
toma medidas y las herramientas las interpretan, y mezclarlo obligaria a
reconstruir el perfilador para cambiar como se pinta un informe.

Todo junto, y no el lado de usuario en `src/`, por como se distribuye: **el
driver viaja con el instalador del compilador**, asi que las dos mitades se
publican a la vez. Separarlas obligaria a mantener sincronizados dos arboles que
solo tienen sentido juntos.

### Por que no se llama `driver/`

Porque en el arbol del compilador que reutiliza este proyecto **"driver" ya
significa otra cosa**: el driver AOT, la parte que orquesta la construccion --
"el driver obtiene un `NativeBackend`", "el driver los serializa en la seccion
`.vxgc_smap`". No tiene nada que ver con un driver del sistema operativo.

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

### Y por eso lleva dos licencias

Del mismo modo que `vesta_alloc` no lleva la GPLv2 de VestaVM, aqui la frontera
de la licencia es un directorio:

| | |
| --- | --- |
| `include/` | **MIT** (`LICENSE.MIT`) |
| todo lo demas | **GPLv2** (`LICENSE`) |

`include/` es el contrato publico, y se diseño para que herramientas de terceros
puedan leer las muestras. Bajo copyleft sin excepcion, quien incluyera
`vxp_abi.h` para escribir un analizador tendria que licenciar su herramienta como
GPL: un formato abierto que legalmente nadie puede consumir. El resto es GPLv2, y
para el `.ko` ademas no hay eleccion -- el subsistema de contadores de
rendimiento del nucleo esta marcado como solo-GPL, y sin `MODULE_LICENSE("GPL")`
el modulo no resuelve esos simbolos.

El razonamiento entero, y lo que NO se hereda de VestaVM, en `NOTICE`.

## La restriccion que sostiene el modo suelto

El perfilador se entrega de dos maneras: **dentro del compilador**, que es la
principal -- se consume desde el y, con el, desde el servidor LSP --, y **como
binario suelto**, para perfilar sin tenerlo delante.

De ahi la regla de capas mas importante del proyecto, que es literalmente la que
`vesta_alloc` declara en la cabecera de su `CMakeLists.txt`:

> **No depende de nada del compilador.** Se puede sacar de aqui y llevarselo.

Si dependiera, el modo suelto no existiria -- o existiria arrastrando el
compilador entero, que es lo mismo. En la practica: no enlaza contra `vmcore`
ni contra `vx_lib`, y lo que necesite de ellos entra por una interfaz que el
modo suelto pueda dejar sin implementar.

La atribucion es donde esto se nota. Resolver una direccion a fichero y linea
sale de DWARF, que es autonomo; atribuirla a un **nodo del intermedio** necesita
lo que sabe el compilador. El suelto tiene la primera y no la segunda, y eso es
una diferencia deliberada, no una carencia.

## El porque de cada decision

| | |
| --- | --- |
| [`doc/architecture.md`](doc/architecture.md) | donde se corta: `common/` frente al sistema, que lenguaje va donde, donde va cada cabecera |
| [`doc/safety.md`](doc/safety.md) | que pasa cuando el driver falla, y de donde sale la seguridad |
| [`doc/memory.md`](doc/memory.md) | de donde sale la memoria, la regla de reservar solo al arrancar, y que hacer cuando parezca que hace falta saltarsela |
| [`doc/contract.md`](doc/contract.md) | el contrato kernel-usuario: como se describe a si mismo, la forma del anillo y de la muestra |
| [`doc/sampling.md`](doc/sampling.md) | las tres politicas: desbordamiento del anillo, lista blanca de eventos, periodo |
| [`doc/style.md`](doc/style.md) | como se escribe `common/`: intrinsecos, modulos con dueno, nada de punteros opcionales |
| [`doc/build.md`](doc/build.md) | por que el `.sys` va aparte, por que LTO es parte del diseno, y por que la firma es del build |
