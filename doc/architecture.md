# Arquitectura: donde se corta y por que

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

Es la misma linea que ya usa el emisor AOT del compilador que reutiliza este
proyecto, cuya cabecera interna lo dice igual de claro: *"se compila SOLO desde
los .c de emision (todos C); el lado C++ del compilador no lo ve"*.

### El C que ve C++ tiene que compilar como C++

Y solo ese. La regla cuesta poco desde el principio y carisimo despues, asi que
conviene tenerla clara:

- `include/` -- **si**: el contrato lo incluye el lado de usuario, que es C++.
- `common/` -- **si**: sus pruebas son C++ y van a incluir sus cabeceras.
- `windows/` y `linux/` por dentro -- **no**. Nadie las incluye desde C++, y sus
  interioridades no se pueden probar en usuario de todas formas.

En la practica, para lo que si: guardas `#ifndef` en las macros, nada de structs
anonimos sin marcar, y `extern "C"` en las cabeceras.

No es teorico. En el arbol del compilador que reutiliza este proyecto,
`CreateELF.h` redefinia macros que ya traia su libreria hermana y usaba structs
anonimos; el resultado fue que **el trozo de codigo mas facil de equivocar era
el unico que no se podia probar**, porque la cabecera que lo contenia no
compilaba como C++.

## Donde va cada cabecera

La regla es la del compilador que reutiliza este proyecto, que tiene 408
cabeceras en `include/` y **cinco** en `src/` -- `aot_emit_internal.h`,
`lowering_internal.h`, `codec_internal.h` y compania:

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
