# cli/

El perfilador como **binario suelto**, para usarlo sin tener el compilador
delante.

Es una capa fina sobre `user/`: abrir sesion, configurar eventos y periodo,
arrancar, vaciar el anillo y escribir el registro de corrida. Todo lo que sea
logica va en `user/`, porque esa misma logica la usa el compilador cuando el
perfilador va integrado.

## Toma medidas; no las interpreta

Leer un registro de corrida y presentarlo es trabajo de `tools/`, y esta
separado a proposito: si el `cli/` pintara informes, cambiar como se ve uno
obligaria a reconstruir el perfilador.

## `vxp_dump`, que ya esta

El segundo binario de aqui, y el unico que hoy funciona. Vuelca lo que los
manuales de Intel y AMD documentan y lo que vale en **esta** maquina:

```text
vxp_dump cpuid      todos los campos de CPUID, desglosados bit a bit
vxp_dump msr        el catalogo de MSR: direccion, nombre y puerta
```

No hay un modo que saque los dos: son **esquemas distintos**, y concatenarlos
produce algo que ningun lector de CSV puede leer.

Sirve para comparar dos piezas, adjuntarlo a un informe de fallo, o enterarse
de que una capacidad existe antes de que a nadie se le ocurra preguntar por
ella.

**Sale en CSV**, que es la regla de arriba aplicada: alinear columnas seria
interpretar, y entonces cambiar como se ve obligaria a reconstruir el
perfilador. Quien lo quiera alineado lo tiene en un comando:

```text
$ vxp_dump cpuid | head -4
leaf,subleaf,reg,lo,width,name,reserved,value
0x00000000,,EAX,0,32,MAX_LEAF,0,0x0000001F
0x00000001,,EAX,8,4,FAMILY_ID,0,0x6
0x00000001,,EDX,4,1,TSC,0,0x1

$ vxp_dump cpuid | column -t -s,      # alineado
$ diff <(vxp_dump cpuid) otra.csv     # comparar dos maquinas, exacto
```

No lleva preambulo, y no es un olvido: la hoja maxima y el fabricante ya son
filas (`MAX_LEAF`, `VENDOR_ID_*`). Un preambulo romperia el formato.

Aqui solo se elige que se vuelca y se escribe. Todo lo que decide vive en
`common/dump.c`, que es el mismo fuente que puede correr dentro del driver.

**No pide privilegios**, y de ahi que los dos lados no salgan igual. CPUID se
ejecuta en cualquier anillo, asi que sale entero. Un MSR solo se lee en anillo
cero, asi que sale el catalogo sin valores -- los valores son cosa del driver,
que entra por el puntero a funcion de `msr_dump`.

Y no se leen todos ni con privilegios: **de los 1.588 MSR que Intel documenta,
solo 315 traen al lado la condicion de CPUID que dice cuando existen**. Leer los
otros a ciegas es un `#GP`, que en anillo cero se lleva la maquina por delante.
El volcado los lista igual y la columna `state` dice por que no se leyeron;
saltarselos en silencio pareceria decir que no existen.

```text
$ vxp_dump msr | tail -n +2 | cut -d, -f9 | sort | uniq -c
   1273 no_gate        el manual no da la condicion al lado
    171 no_reader      la cumple, pero en modo usuario no se puede leer
    144 unsatisfied    hay condicion y esta pieza no la cumple
```

## Que pierde respecto al integrado

La atribucion a nodos del intermedio, que necesita lo que sabe el compilador.
Fichero y linea si los tiene, porque salen de DWARF y DWARF es autonomo. Ver
`user/README.md`.
