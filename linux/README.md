# linux/

El modulo `.ko`: lo que solo sabe hacer Linux.

Reservar memoria, mapearla a espacio de usuario, registrar el manejador de la
NMI, el dispositivo de caracteres y su `ioctl`, la inhibicion de interrupcion y
el trabajo diferido. Todo lo que sea del **procesador** y no del sistema va a
`common/`.

## No se construye como el de Windows, y no es un detalle

El `.sys` es un objetivo de CMake normal. **Este no.** Los modulos de Linux se
construyen con `kbuild`, que es el sistema de construccion del propio kernel, y
lo unico que puede hacer CMake es invocarlo.

Escribir el `.ko` como si fuera una biblioteca de CMake produciria algo que
enlaza y que el kernel no carga -- otra vez el patron de "no falla, simplemente
no sirve".

Consecuencia practica: **kbuild construye DENTRO del arbol**. Deja sus
intermedios al lado del fuente (`*.mod.c`, `.*.cmd`, `Module.symvers`), y por eso
el `.gitignore` del proyecto los cubre.

## No convive con `windows/`

Nunca los dos desde la misma configuracion: un modulo se compila contra las
cabeceras del kernel al que va a cargarse.

Si faltan las cabeceras del kernel en ejecucion, no se construye y **se dice por
la salida de configuracion**.

## Firma

Solo hace falta con Arranque Seguro, o si el kernel se compilo con
`CONFIG_MODULE_SIG_FORCE`. Para generar el material:

    python profiler/tools/make_dev_cert.py --platform linux

La herramienta imprime despues los dos pasos que quedan: firmar con el
`scripts/sign-file` del propio kernel, e inscribir el certificado con `mokutil`
si hay Arranque Seguro. La inscripcion se confirma **en el arranque siguiente**,
en el menu azul del firmware; si se pasa ese menu, no queda inscrita y el modulo
seguira sin cargar.

## Sin cabeceras publicas

Un driver no tiene API. Lo que necesite compartir con el lado de usuario va al
contrato de `include/`.
