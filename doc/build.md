# Que se construye, cuando, y con que

El lado de usuario se construye **siempre**: es parte de lo que se instala, y no
necesita nada especial.

Los dos lados de kernel no se parecen en nada mas que en el nombre, y conviene
no tratarlos como si fueran el mismo con un `if` delante.

**No conviven.** El `.sys` se construye en Windows y el `.ko` en Linux, y nunca
los dos desde la misma configuracion: un modulo de kernel se compila con las
herramientas y las cabeceras del kernel al que va a cargarse, asi que
cross-compilar uno desde el otro no es algo que se resuelva con banderas.

**No se construyen igual.** El `.sys` es un objetivo de CMake normal -- pero en
su propio directorio, porque un driver necesita que **no** se le anadan las
bibliotecas estandar del enlace, y esa variable es de ambito de directorio. Un
driver que importara de `kernel32` no fallaria al construirse: fallaria al
cargarse, con un error que habla de un modulo que no se encuentra y no de la
linea de enlace, que es donde estaba el problema.

El `.ko` **no puede** ser un objetivo de CMake: los modulos de Linux se
construyen con `kbuild`, que es el sistema de construccion del propio kernel, y
lo unico que puede hacer CMake es invocarlo. Escribirlo como una biblioteca de
CMake produciria algo que enlaza y que el kernel no carga.

De ahi que lo que se detecte sea, en cada anfitrion, si estan las herramientas
de SU lado, y que si faltan se omita **diciendolo por la salida de
configuracion**. Un desarrollador tiene que poder construir sin nada de esto
instalado; lo que no puede es creerse que construyo el driver cuando no lo hizo,
que es como se acaba enviando un instalador sin `.sys` dentro.

## El `.sys` no necesita el WDK

Ni para construirlo ni para enlazarlo. Del WDK saldrian tres cosas y las tres se
traen: las declaraciones del kernel (`windows/nt.h`), la biblioteca de
importacion (generada con `dlltool` de `windows/ntoskrnl.def`) y las banderas
del enlace, que son eso, banderas.

Con MSVC no se construye, y se dice por la salida de configuracion en vez de
producir algo que no carga: sus banderas de driver son otras por completo
(`/DRIVER`, `/SUBSYSTEM:NATIVE`, `/ENTRY:DriverEntry`, la biblioteca de
importacion con `lib` en lugar de `dlltool`). Mantener dos recetas de las que
solo una se usa a diario es como una de ellas se pudre sin que nadie lo note.

## La firma es parte de la construccion, no un remate

Un driver sin firmar **no carga**. Asi que si el certificado de desarrollo esta,
se firma como paso posterior del propio objetivo, y si la firma falla el build
se detiene: un `.sys` sin firmar tiene exactamente el mismo aspecto que uno
firmado hasta que alguien intenta cargarlo, y entonces el error habla de la
firma y no del build, que es donde estaba el fallo.

La contrasena del certificado se lee **al construir** y no al configurar, para
que no acabe escrita en los ficheros generados del directorio de construccion.

## LTO no es una optimizacion mas: es parte del diseno

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

**Tambien en el `.sys`**, que era la duda razonable: un enlace con `-nostdlib` y
punto de entrada propio podria perder `DriverEntry` por ser un simbolo al que no
llama nadie. Comprobado que no ocurre -- el binario sale con `Subsystem 1 (NT
native)`, `DriverEntry` conservado, las mismas importaciones y ademas mas
pequeno --, porque es el simbolo de entrada del enlace.

Para el `.ko` no aplica nada de esto: lo construye `kbuild`, con las opciones del
kernel, y no es CMake quien decide.

## Suelto o integrado

Cuando se construye suelto, las pruebas y los ejemplos se activan por defecto y
al integrarse se apagan, igual que hacen el preprocesador y el asignador: dentro
de otro proyecto duplicarian binarios y tiempo de construccion sin aportar nada.
