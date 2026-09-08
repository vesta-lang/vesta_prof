# include/

Lo **publico**, y solo dos cosas:

1. **El contrato compartido kernel-usuario**: la disposicion del anillo, el
   formato de una muestra y los codigos de control.
2. **La API de la biblioteca de usuario**, que es lo que consumen el compilador
   y el binario suelto.

Las dos cruzan una frontera de verdad. Lo que no la cruce va al lado de su
fuente.

Como funciona el contrato -- las tres capas con las que se describe a si mismo,
la forma del anillo y de la muestra, y las reglas que lo hacen sobrevivir a un
cambio de version -- esta en [`doc/contract.md`](../doc/contract.md).

## Y por eso este directorio es MIT

Es la unica parte del proyecto que no lleva la GPLv2. La frontera de la licencia
es exactamente esta: **`include/` es MIT, todo lo demas es GPLv2** -- ver
`NOTICE` en la raiz.

El motivo sale de lo que se acaba de decir. El contrato se describe a si mismo
para que **herramientas de terceros** puedan leer lo que el perfilador produce.
Bajo copyleft sin excepcion, cualquiera que incluyera `vxp_abi.h` para escribir
un analizador tendria que licenciar su herramienta como GPL: se habria
construido un formato abierto que legalmente nadie puede consumir sin cambiar la
licencia de su producto, y el trabajo de hacerlo autodescriptivo no serviria de
nada.

Tiene una consecuencia practica al decidir donde va una cabecera, y conviene
tenerla presente: **poner algo aqui es publicarlo bajo MIT**. Si una cabecera no
cruza la frontera kernel-usuario ni forma parte de la API de la biblioteca, va
al lado de su fuente -- no por orden, sino porque aqui se regala.
