# bench/baseline/

Las medidas de referencia. Se versionan, al contrario que el resto de
mediciones.

## El nombre lleva con que compararse

Como en el asignador:

    <que>_<sistema>_<compilador>_<microarquitectura>_<fecha-hora>.txt

Por ejemplo:

    rdpmc_cost_windows_gcc-10.3_raptorlake_20260908-031500.txt

No es burocracia. Una medida de perfilado sin sistema, compilador,
microarquitectura y fecha **no se puede comparar con nada**: ni con otra
maquina, porque el hardware manda -- el coste de `RDPMC` y la latencia de la
interrupcion cambian entre generaciones --, ni consigo misma un mes despues,
porque no se sabria que cambio en medio.

Un fichero mal nombrado aqui no molesta hoy; deja de servir dentro de seis
meses, que es justo cuando hace falta.
