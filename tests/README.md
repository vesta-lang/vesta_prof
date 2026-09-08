# tests/

Las pruebas del perfilador, y sobre todo **las de `common/`**.

Aqui es donde se cobra la regla de que `common/` no incluye cabeceras del
sistema: al no incluirlas, se compila y se prueba en espacio de usuario como
cualquier otra cosa del proyecto. No hace falta cargar nada en un kernel para
saber si decodifica bien un registro de PEBS.

## Que se puede probar sin kernel

- que la decodificacion de un registro de PEBS y de uno de LBR da lo que debe,
  a partir de un area DS **falsa** construida a mano;
- que la politica de desbordamiento del anillo hace lo que dice cuando se llena;
- que productor y consumidor calculan los mismos indices -- que es lo que evita
  que el anillo devuelva muestras que no son;
- que la lista blanca rechaza lo que no esta en ella;
- que la deteccion de microarquitectura acierta con una salida de CPUID dada.

## Que no

Todo lo que quede fuera de `common/`: pedir memoria, mapearla, registrar el
manejador de la interrupcion. Eso solo se prueba cargando el modulo, y por eso
la regla es llevar a `common/` todo lo que quepa.

## Los tres compiladores, y por que la prueba se parte en dos lenguajes

`vxp_base.h` tiene que valer con **TDM/MinGW, Clang y MSVC**, y como C y como
C++. Una prueba escrita en un solo lenguaje no comprobaria lo que dice: la
manera de que una cabecera diga cosas distintas a cada lado es justo que nadie
mire los dos.

Por eso `test_vxp_base.cpp` se reparte: `opaque_probe.c` se compila como **C** y
publica lo que ve -- tamanos, alineacion, el valor de `OK` --; el `.cpp` se
compila como **C++** y compara. Si los tipos midieran distinto, o si faltara
`extern "C"`, no enlazaria, y eso ya seria el fallo cazado.

Mientras no exista el `CMakeLists.txt`, se construye a mano:

```sh
# TDM-GCC
gcc  -std=c11   -Wall -Wextra -pedantic-errors -Iinclude -Itests -c tests/opaque_probe.c -o probe.o
g++  -std=gnu++17 -Wall -Wextra -pedantic-errors -Iinclude -Itests -o t.exe tests/test_vxp_base.cpp probe.o

# Clang: los mismos, con clang / clang++
# MSVC:  cl /std:c11 /W4 /Iinclude /Itests /c tests\opaque_probe.c
#        cl /std:c++17 /W4 /EHsc /Iinclude /Itests tests\test_vxp_base.cpp opaque_probe.obj
```

Ya salio un fallo real de hacerlo asi: **Clang define `_MSC_VER` en Windows**
para hacerse pasar por MSVC, y la cabecera tomaba la rama de los `__intN`, que
el mismo Clang rechaza como extension bajo `-pedantic-errors`. Con un solo
compilador eso no aparece.

## Las pruebas del propio driver cargado

Cuando existan, van aparte y **no** en la bateria normal: necesitan
administrador, una maquina en modo de pruebas y un reinicio de por medio. Un
rojo permanente por falta de esas condiciones se acaba ignorando, y con el se
ignoran los rojos de verdad.
