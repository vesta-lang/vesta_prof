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

## Que pierde respecto al integrado

La atribucion a nodos del intermedio, que necesita lo que sabe el compilador.
Fichero y linea si los tiene, porque salen de DWARF y DWARF es autonomo. Ver
`user/README.md`.
