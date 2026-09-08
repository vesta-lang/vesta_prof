# linux/asm/

Ensamblador del lado de Linux: lo que depende de la ABI SysV, de GAS, o de
estructuras del kernel de Linux.

## Que acaba aqui

Lo mismo que en el lado de Windows y por el mismo motivo -- **codigo frio** que
no tiene intrinseco y no puede tenerlo:

- el stub de entrada de la NMI, que recibe el control con el estado que dejo el
  procesador y no una llamada de C;
- guardar y restaurar contexto extendido;
- secuencias donde importe la instruccion exacta.

Lo **caliente** no baja aqui: se hace con intrinsecos, por lo medido en
`profiler/common/README.md`.

## Ensamblador y sintaxis

GAS, porque es con lo que ensambla kbuild y pelearse con eso no compensa.

Sintaxis Intel de todos modos, con `.intel_syntax noprefix` al principio del
fichero: es la del resto del proyecto y la de los bloques `asm {}` de Vesta.

Que quede claro que esto vale para **nuestros ficheros `.S`** y no para codigo
en C: dentro de una unidad que incluya cabeceras del kernel no se puede activar
`-masm=intel`, porque esas cabeceras traen su propio asm en linea escrito en
AT&T. Otro motivo mas para que `common/` no las incluya nunca.

## ABI

SysV: argumentos enteros en `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`; sin espacio
de sombra; con zona roja de 128 bytes por debajo de `rsp` **en espacio de
usuario**.

La zona roja es la trampa de este directorio: **en el kernel no existe**. El
kernel de Linux se compila con `-mno-red-zone` precisamente porque una
interrupcion pisaria esos 128 bytes. Codigo escrito dando por hecha la zona roja
funciona en las pruebas de usuario y corrompe la pila al cargarlo.

## Disposicion

Por ISA, y el subdirectorio nace con su primer fichero:

```text
linux/asm/x86_64/
linux/asm/arm64/
```
