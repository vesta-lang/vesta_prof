# tools/import/

Dos piezas del importador de la base de instrucciones de VestaVM, copiadas
literalmente desde `tools/import/` del compilador. La ruta coincide a proposito:
que el camino sea el mismo hace evidente que esto es un espejo y no un pariente.

```text
gen_cpp_db.py   emite el codigo C++ estatico desde una base ya construida
database.py     el lector de los formatos .vxisa y .vxarch
```

Que siguen siendo la misma cosa se comprueba comparando el resumen:

```bash
sha256sum tools/import/gen_cpp_db.py ../tools/import/gen_cpp_db.py
```

**Es un espejo, no una bifurcacion**: los cambios se hacen en el compilador y
bajan aqui. La documentacion del pipeline vive alli, en su propio
`tools/import/README.md`, y no se duplica.

## Que hace falta de aqui, y por que

El perfilador se puede usar **sin el compilador delante** -- es parte del motivo
de que sea un repositorio propio --, y para ordenar un perfil por lo inexplicado
hace falta saber lo que un bloque de codigo deberia costar:

```text
    residuo  =  ciclos medidos      -  coste previsto del bloque
                (el PMU)               (la base de coste)
```

Sin la segunda mitad, un perfil dice donde se va el tiempo pero no si eso es
mucho o poco.

## Estas dos piezas no construyen la base

`gen_cpp_db.py` va al final de la cadena: lee una base ya construida y la reemite
como tablas de C++. Construirla es otra cosa, y necesita el resto del importador
del compilador mas sus fuentes:

```text
    instructions.xml (uops.info)  --->  .vxisa (las formas, una)
                                        .vxarch (el coste, uno por microarq)
    modelos de scheduling de LLVM  --->  .vxarch de relleno
                                          |
                                          v
                                    gen_cpp_db.py  --->  tablas de C++
```

## Las fuentes son externas, y no se versionan

Ni el XML de uops.info -- unos 140 MB -- ni el volcado de los modelos de LLVM
entran en un repositorio. La base intermedia (`.vxisa`, `.vxarch`) tampoco: se
regenera desde ellos.

Las dos fuentes se complementan y donde se solapan se fusionan con prioridad
declarada, **medida por encima de modelada**: en x86 los puertos de coma flotante
de AMD salen de uops.info y los enteros de ALU y AGU de LLVM, que uops.info no
cubre. Cada `.vxarch` lleva su procedencia en la primera linea -- microarquitectura
de origen, fecha y resumen del XML --, que es lo que permite saber contra que se
contrasto cada fila.
