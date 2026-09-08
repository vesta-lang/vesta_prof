# Que pasa cuando el driver falla

Primero la mala noticia, porque condiciona todo lo demas:

> **En modo kernel no hay aislamiento.** Un fallo en el driver es un pantallazo
> en Windows y un panico en Linux. No existe "capturar la excepcion para no
> afectar a los demas": el driver **es** el kernel.

Cualquier diseno que prometa lo contrario esta mintiendo. La seguridad sale de
cuatro sitios, y estan ordenados por lo que compran de verdad.

## 1. Que no pueda fallar (el 95%)

Casi todo ya esta decidido y no se toma como medida de seguridad, pero lo es:

- **sin reservas despues del arranque** -- no hay camino de fallo de reserva en
  el manejador;
- **sin cerrojos en el manejador** -- no hay interbloqueo posible;
- **arrays de tamano fijo con indices validados** -- ni bucles sin cota ni
  despacho dinamico;
- **`common/` probado en espacio de usuario** antes de pisar un kernel, que es
  la diferencia entre depurar con un arnes y depurar reiniciando un equipo;
- **lista blanca de eventos** en vez de acceso generico a MSR, y
  **`METHOD_BUFFERED`** en vez de punteros de usuario crudos. Las dos estan
  decididas, y el motivo es el mismo: es el patron que acaba en la lista de
  drivers vulnerables de Microsoft.

## 2. Los dos sitios donde capturar SI es posible

Y son dos, no mas:

| | Windows | Linux |
| --- | --- | --- |
| tocar memoria de usuario | `ProbeForRead`/`ProbeForWrite` dentro de `__try`/`__except` | `copy_from_user`/`copy_to_user`, que devuelven error |
| leer un MSR que quiza no exista | no hay variante segura: comprobar CPUID antes, o envolver en `__try` | `rdmsrl_safe` / `wrmsrl_safe` |

Windows si tiene SEH en kernel, y captura violaciones de acceso y #GP. **No es
una red de seguridad general**: usarlo para tapar un desreferenciado nulo propio
es enviar el fallo escondido en vez de arreglarlo.

De esa asimetria sale una decision de API que hay que tomar **antes** de escribir
la interfaz de `common/`, no despues:

> La primitiva de leer y escribir MSR tiene que **poder fallar**: devuelve un
> codigo, no `void`.

Si fuera `void`, el lado de Linux no podria usar `rdmsrl_safe` y el de Windows
no tendria como informar de un #GP capturado.

Y mientras esa captura no este escrita, la unica proteccion es **no pedirlo**:
se leen solo los MSR cuya existencia garantiza un bit de CPUID ya comprobado.
Menos ambicioso, y no arriesga el equipo.

## 3. Fallar cerrado, y descargar en el orden correcto

- **Al cargar**: validar que la microarquitectura esta soportada, que el PMU
  existe y que los MSR son los que se esperan. Si algo no cuadra, **no cargar**.
  Un driver a medias es peor que uno ausente.
- **Al descargar**, el orden importa y es la causa numero uno de caidas en esta
  clase de driver: se paran los contadores, se desregistra el manejador, se
  sincroniza, y **solo entonces** se libera la memoria. Al reves, la siguiente
  interrupcion escribe en memoria ya liberada -- un manejador huerfano es un
  pantallazo garantizado.

## 4. El anillo es entrada NO confiable

Lo comparte el espacio de usuario, que puede escribir en el. De ahi la regla: el
driver **trata la region como no confiable para decidir nada**.

En concreto: los indices que ve usuario no valen para una decision de limites.
El driver lleva su propia copia y comprueba contra ella. Confiar en un indice
compartido es la version en memoria compartida del clasico "comprobar y luego
usar".

### Y en el anillo no va ni un puntero

> Todo lo que cruza son **indices y desplazamientos**. Ninguna direccion.

Por dos motivos, y el segundo es el grave:

1. **No sirve.** La region se mapea en espacio de usuario en otra direccion, asi
   que un puntero de kernel ahi no significa nada del otro lado.
2. **Es una fuga.** Publicar direcciones de kernel en memoria que lee el espacio
   de usuario es como se saltan las protecciones de aleatorizacion del espacio
   de direcciones. Es una clase de vulnerabilidad conocida, no una hipotesis.

Ademas sale gratis: el anillo y el estado por nucleo ya son arrays, y un indice
con centinela **se puede comprobar** -- `i < n` -- mientras que un puntero
corrupto no.

## 5. Herramientas, y donde se desarrolla

Driver Verifier en Windows; KASAN, lockdep y kmemleak en Linux. Y el desarrollo
va en una **maquina virtual con instantanea**: no por miedo, sino porque cada
ciclo de prueba incluye un reinicio y conviene que sea barato.
