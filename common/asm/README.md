# common/asm/

Ensamblador que **no depende del sistema operativo**: solo de la ISA.

## Este directorio va a estar casi vacio, y eso es lo correcto

Conviene decirlo aqui para que nadie lo tome por un hueco que rellenar. En
x86-64 lo verdaderamente compartible es muy poco, y no por el motivo que
parece.

Lo que separa dos ficheros de ensamblador no es el sistema operativo. Son tres
ejes, y el que menos se ve es el que mas separa:

```text
ISA          x86-64 / arm64    instrucciones distintas
ABI          Win64 / SysV      MISMA ISA, otros registros de argumento,
                               32 bytes de espacio de sombra frente a zona
                               roja, otro conjunto de registros preservados
ENSAMBLADOR  MASM / GAS        mismas instrucciones, otras directivas:
                               .globl/.text frente a PUBLIC/PROC/ENDP
```

**Cualquier rutina con interfaz en C depende de la ABI**, y en x86-64 la ABI la
decide el sistema. Asi que casi todo lo que se llama desde C ya esta separado
aunque las instrucciones sean identicas.

Y aunque no lo estuviera, el tercer eje lo remata: un fichero compartido de
verdad tendria que ensamblarse con lo mismo en los dos sitios, y no se puede --
kbuild ensambla con GAS y el WDK espera MASM.

En **arm64 se comparte mas**, no menos: Windows en ARM sigue AAPCS64 igual que
Linux, asi que ahi el eje de la ABI casi desaparece.

## La prueba que tiene que pasar un fichero para estar aqui

> ¿Menciona algo que defina el sistema operativo?

- Solo registros e instrucciones -> aqui.
- Espacio de sombra, disposicion de un marco de interrupcion, desplazamiento de
  una estructura del kernel -> al directorio de su sistema.

Si dudas, va al del sistema. Un fichero de mas ahi no molesta a nadie; uno con
ABI de Win64 metido aqui rompe el lado de Linux, y lo hace en tiempo de
ejecucion.

## Disposicion

Por ISA, y el subdirectorio nace con su primer fichero:

```text
common/asm/x86_64/
common/asm/arm64/
```
