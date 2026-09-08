# windows/

El `.sys`: lo que solo sabe hacer Windows.

Pedir memoria no paginada, mapearla a espacio de usuario con una MDL, registrar
el manejador de la PMI, el dispositivo y sus IOCTL, los cerrojos y el trabajo
diferido. Todo lo que sea del **procesador** y no del sistema va a `common/`.

## No convive con `linux/`

El `.sys` se construye en Windows y el `.ko` en Linux, **nunca los dos desde la
misma configuracion**. Un modulo de kernel se compila con las herramientas y las
cabeceras del kernel al que va a cargarse; eso no se arregla con banderas de
cross-compilacion.

## Aqui NO se depende del WDK

> Lo que haga falta del kernel **vive en este directorio**. Nadie tiene que
> instalar nada aparte para construir el driver.

Es la misma postura que el resto del proyecto: los emisores de PE y de ELF
tampoco dependen de la cadena de nadie.

Del WDK solo salen tres cosas, y las tres se pueden traer:

| lo que da el WDK | como se resuelve aqui |
| --- | --- |
| declaraciones del kernel | las escribimos, en `nt.h` |
| `ntoskrnl.lib` para enlazar | se genera de un `.def` nuestro |
| las banderas del enlace | `--subsystem native`, `--entry DriverEntry`, `-nostdlib`, `-mno-red-zone` |

**Comprobado, no supuesto.** Con esas tres piezas sale un `.sys` de verdad:
`Subsystem 1 (NT native)`, importando de `ntoskrnl.exe`, **sin incluir ni una
cabecera ajena y sin la biblioteca de importacion de nadie**.

### Declarar estructuras internas: se hace, con una comprobacion

Para escribir `DRIVER_OBJECT.DriverUnload` hay que saber donde cae ese campo, o
sea que hay que declarar la estructura entera -- aunque no se usen los demas
campos, porque lo que importa es el DESPLAZAMIENTO.

Eso es exactamente donde una declaracion a mano se equivoca y el driver
corrompe memoria sin avisar. Asi que **la disposicion se contrasta**: un
programa de usuario que imprime `sizeof` y `offsetof` de nuestra version, y otro
que hace lo mismo con una cabecera de referencia, y se comparan.

La primera vez que se hizo salio esto, y los seis valores coincidieron:

```text
sizeof(DRIVER_OBJECT)     336
offsetof(DriverUnload)    104
offsetof(MajorFunction)   112
offsetof(DriverName)       56
offsetof(DriverExtension)  48
sizeof(UNICODE_STRING)     16
```

La regla que queda: **toda estructura del kernel que declaremos lleva su prueba
de disposicion**. Es barata -- corre en espacio de usuario, con el resto de la
bateria -- y es la unica forma de que un desplazamiento equivocado se vea al
probar y no al cargar.

Para las funciones no hace falta: una firma mal da un error de enlace, que es
ruidoso. Lo silencioso son los desplazamientos.

## Construccion

Objetivo de CMake normal. No busca el WDK ni lo necesita.

## Firma

Un driver no carga sin firma. Para desarrollo:

    profiler/tools/dev_setup.cmd      monta la maquina (pide administrador)
    profiler/tools/dev_teardown.cmd   la devuelve a su estado

Eso genera un certificado propio, lo confia y activa el modo de pruebas. **No es
un camino de publicacion**: lo que se distribuye va firmado por atestacion con
un certificado EV.

## Sin cabeceras publicas

Un driver no tiene API: nadie incluye sus interioridades. Lo que necesite
compartir con el lado de usuario va al contrato de `include/`.
