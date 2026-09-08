# Memoria

## De donde sale

No escribimos asignador: los dos kernels traen el suyo.

| | Windows | Linux |
| --- | --- | --- |
| memoria no paginada | `ExAllocatePool2(POOL_FLAG_NON_PAGED, n, tag)` | `kmalloc` / `kzalloc` |
| bloques grandes | el mismo, o una MDL | `vmalloc`, `alloc_pages` |
| mapear a usuario | MDL + `MmMapLockedPagesSpecifyCache` | `vmalloc_user` + `remap_vmalloc_range` |

`vesta_alloc` **no** sirve aqui, y no por falta de calidad: usa `VirtualAlloc` y
`mmap`, reemplaza `operator new` y es C++. Las tres cosas son imposibles en
kernel. Tampoco haria falta -- no se optimiza un camino que se recorre tres
veces.

En Windows, la **etiqueta de pool** no es opcional: es lo que permite ver desde
el depurador quien retiene memoria (`!poolused`). Sin ella una fuga del driver
es una cifra anonima.

## LA REGLA: en el kernel se reserva al arrancar, y nunca mas

> Todas las reservas ocurren en la inicializacion de la sesion. Despues, ni una.
> Y `common/` no reserva NUNCA: recibe la memoria ya reservada.

No es una preferencia de estilo ni una optimizacion. **El manejador de la PMI no
puede reservar**: corre a IRQL alta en Windows y en contexto de NMI en Linux,
donde reservar puede paginar, puede tomar un cerrojo y puede bloquearse contra
si mismo. Todo el diseno -- preasignar un anillo y escribir en el -- existe por
eso.

Y tiene un efecto lateral que conviene aprovechar: como `common/` no reserva,
sus pruebas en espacio de usuario le pasan un bufer de `malloc` y se comporta
igual.

## Que hacer cuando parezca que hace falta reservar

Esto es lo importante de la regla, porque el caso llegara. **No se resuelve
reservando igualmente.** Se responde en este orden:

1. **¿Es memoria por muestra?** Entonces va **en el anillo**, que ya esta
   reservado. Casi todos los casos son este y se acaban aqui.

2. **¿Hace falta dentro del manejador?** Entonces **no se puede**, y no es
   opinable: el contexto lo prohibe. Lo que haya que guardar se escribe en el
   anillo y lo procesa el lado de usuario, que si puede reservar.

3. **¿Hace falta al abrir la sesion?** Entonces **no rompe la regla**: eso es la
   inicializacion. Se reserva ahi y se le entrega a `common/`.

4. **¿Hace falta al reconfigurar en marcha?** Se para la sesion, se vuelve a
   reservar y se arranca. Nunca una reserva en el camino caliente.

Si un caso no encaja en ninguno de los cuatro, **el diseno esta mal y hay que
replantearlo**, no saltarse la regla. Y ese replanteo se escribe con su motivo:
una excepcion sin razon anotada se convierte en la norma la siguiente vez.

### Un caso resuelto, para que se vea como se aplica

Dimensionar el estado por nucleo. La regla dice que se reserva al arrancar, pero
en Linux una CPU puede conectarse **despues** de abrir la sesion, y en Windows se
pueden anadir procesadores en caliente. Una CPU que aparezca luego no tendria
sitio.

No se resuelve reservando cuando aparezca -- eso seria el caso 4 disfrazado --,
sino dimensionando por el **maximo posible** en vez de por el activo
(`KeQueryMaximumProcessorCountEx`, `nr_cpu_ids`). Es una constante de arranque, y
la regla se cumple tal cual esta escrita.

## Estructuras de datos: casi ninguna, y es la senal de que el diseno va bien

| | que es de verdad |
| --- | --- |
| el anillo | un array de tamano fijo con dos indices |
| estado por nucleo | un array indexado por numero de nucleo, dimensionado al arrancar |
| lista blanca de eventos | una tabla `static const`; ni se reserva |

Nada de eso necesita un contenedor, y no es casualidad: el diseno se eligio para
que fuera asi.

De ahi sale una **prueba util**: si en el driver aparece la necesidad de un mapa
o de una lista enlazada, es senal de que eso pertenece al lado de usuario. Los
dos kernels ofrecen estructuras -- `list_head`, `rbtree`, `xarray` en Linux;
`LIST_ENTRY`, `RTL_AVL_TABLE` en Windows -- pero usarlas rompe `common/`, porque
son cabeceras del sistema.

Si algun dia `common/` necesitara una de verdad, tendria que ser plana, escrita
en C y sobre un bufer que le den. No hay tercera via.
