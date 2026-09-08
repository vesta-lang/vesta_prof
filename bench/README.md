# bench/

Cuanto cuesta el **propio perfilador**.

No es opcional en este proyecto. Un perfilador que cuesta mas de lo que mide no
sirve, y ademas falsea justo lo que estaba midiendo. El coste no se estima: se
mide, y se vuelve a medir en cada maquina.

## Los numeros a vigilar

Salen del plan y hay que poder comprobarlos:

- una lectura con `RDPMC` deberia rondar los **20-30 ciclos**;
- el precio del muestreo es **una interrupcion por desbordamiento**, no por
  instruccion. Con un periodo de 2.000.000 de ciclos son unos 1000 desbordes por
  segundo a 2 GHz;
- vaciar el anillo desde usuario no debe provocar transiciones al kernel: para
  eso esta mapeado.

## `baseline/`

Las medidas de referencia, **fechadas y etiquetadas con la maquina**, igual que
en el asignador. Una medida sin maquina y sin fecha no se puede comparar con
nada: ni con otra maquina, porque el hardware manda, ni consigo misma un mes
despues, porque no se sabria que cambio.

Es la excepcion a la regla del `.gitignore` de no versionar mediciones. Estas si
van al repositorio, precisamente porque llevan con que compararse.
