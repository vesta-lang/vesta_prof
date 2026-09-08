# windows/asm/

Ensamblador del lado de Windows: lo que depende de la ABI de Win64, del
ensamblador del WDK, o de estructuras del kernel de Windows.

## Que acaba aqui

Lo poco que no tiene intrinseco y no puede tenerlo, que es **codigo frio** por
naturaleza:

- el stub de entrada de la interrupcion de rendimiento (la PMI), que recibe el
  control con el estado que dejo el procesador y no una llamada de C;
- guardar y restaurar contexto extendido, cuando haya que tocarlo;
- cualquier secuencia donde importe la instruccion exacta y no valga lo que el
  compilador decida emitir.

Lo **caliente** no baja aqui. Leer un contador o un MSR se hace con intrinseco:
un simbolo definido en un `.asm` es opaco para el compilador, que entonces salva
registros preservados, reserva los 32 bytes de sombra en cada llamada y no
programa nada a traves de la frontera. La medida esta en
`profiler/common/README.md`, con los dos listados al lado.

## Ensamblador y sintaxis

MASM (`ml64`), que es lo que espera el WDK. Sintaxis Intel, que es la de MASM y
la misma que usan los bloques `asm {}` del propio Vesta.

Ojo con las directivas: MASM no entiende `.globl` ni `.text`; usa `PUBLIC`,
`.code`, `PROC` y `ENDP`. Es el motivo por el que un fichero no se puede
compartir tal cual con el lado de Linux aunque las instrucciones sean las
mismas.

## ABI

Win64: argumentos enteros en `rcx`, `rdx`, `r8`, `r9`; el **llamante** reserva
32 bytes de espacio de sombra encima de la direccion de retorno; no hay zona
roja -- nada por debajo de `rsp` sobrevive a una interrupcion.

Esa ultima parte importa el doble aqui: este codigo corre en contextos donde las
interrupciones son lo normal, no la excepcion.

## Disposicion

Por ISA, y el subdirectorio nace con su primer fichero:

```text
windows/asm/x86_64/
windows/asm/arm64/
```
