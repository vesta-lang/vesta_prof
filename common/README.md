# common/

Lo que es **del procesador** y no del sistema operativo. Es decir, casi todo lo
dificil.

```text
aqui                                aqui NO
--------------------------------    ------------------------------
que MSR y con que codificacion      pedir memoria
la disposicion del area DS          mapearla a espacio de usuario
decodificar un registro de PEBS     registrar el manejador de la PMI
  y uno de LBR                      cerrojos e inhibicion de interrupcion
detectar la microarquitectura       el dispositivo y sus IOCTL
la lista blanca y su validacion     temporizadores y trabajo diferido
los indices del anillo
```

`IA32_PERFEVTSELx`, `MSR_PEBS_DATA_CFG` o el formato de un registro de PEBS no
tienen nada de Windows ni de Linux: son de Intel. Escribirlos dos veces seria
mantener dos copias de la misma tabla y descubrir que divergieron en produccion.

## La regla

> **No se incluye ni una sola cabecera del sistema operativo.**

Lo poco que hace falta del sistema -- leer un MSR es la misma instruccion, solo
cambia como se escribe: `__readmsr` frente a `rdmsrl` -- entra por una interfaz
pequena que implementa cada lado. Esa interfaz es toda la dependencia que se
permite.

Y una condicion de esa interfaz, que hay que respetar al escribirla:

> **Leer y escribir un MSR tiene que poder FALLAR.** Devuelve un codigo, no
> `void`.

Porque los dos lados fallan distinto y los dos tienen que poder decirlo. Linux
tiene `rdmsrl_safe`/`wrmsrl_safe`, que devuelven error en vez de provocar una
excepcion de proteccion general. Windows no tiene variante segura: comprueba
CPUID antes, o envuelve en `__try`/`__except`.

Con una firma `void` ninguno de los dos podria informar, y un MSR que no existe
en esa microarquitectura **pasaria por un valor leido** -- no un fallo, un dato
falso.

## Y por eso el driver se puede probar

Es la razon de peso, por encima de no duplicar codigo.

Codigo de kernel que solo corre dentro del kernel se depura cargandolo en una
maquina y viendo si se cuelga. Codigo sin cabeceras del sistema **se compila y
se prueba en espacio de usuario**, con el mismo arnes que todo lo demas: se le
da un area DS falsa y se comprueba que decodifica lo que debe; se le llena el
anillo y se comprueba que la politica de desbordamiento hace lo que dice.

Cuanto mas se lleve aqui, menos codigo queda que solo se pueda probar
arriesgando un equipo.

## Ensamblador: intrinsecos, y solo eso

Casi todo lo que este directorio necesita del procesador -- `rdpmc`, `rdmsr`,
`wrmsr`, `cpuid`, `rdtsc`, las barreras -- **tiene intrinseco** en las dos
familias de compilador. Y usarlo no es una concesion: sale mejor codigo.

### Por que NO asm en linea

Medido con `gcc -O2`, leyendo un contador:

```text
asm en linea                intrinseco
------------                ----------
rdpmc                       rdpmc
salq $32, %rdx              salq $32, %rdx
movl %eax, %eax   <--       orq  %rdx, %rax
orq  %rdx, %rax             ret
ret
```

Una instruccion mas. Ese `movl %eax, %eax` esta porque al compilador se le dijo
que la salida era `unsigned` y tiene que sanear la parte alta; con el intrinseco
conoce la forma exacta del resultado y se lo ahorra.

Para operaciones de una instruccion, el asm en linea **no es igual de rapido,
suele ser peor**: es una barrera de optimizacion, y el compilador no puede
programar alrededor ni suponer nada de lo que toca salvo que las restricciones
esten perfectas -- que es donde viven los fallos.

Ademas, en el lado de Linux no se podria escribir en Intel aunque se quisiera:
`-masm=intel` es por unidad de traduccion, y ahi se incluyen cabeceras del
kernel llenas de asm en linea en AT&T (`rdmsrl` es una).

### Por que NO asm en fichero aparte, para lo caliente

Un simbolo definido en un `.S` es **opaco**. Medido, la misma lectura:

```text
asm en fichero aparte (17 instr)      intrinseco (11 instr)
--------------------------------      ---------------------
push r12 / push rsi / push rbx        rdpmc
sub  rsp, 32       <- shadow space    ...
call lee_pmc_asm                      rdpmc
call lee_pmc_asm                      ...
add  rsp, 32                          ret
pop  rbx / pop rsi / pop r12
```

El compilador no ve dentro, asi que supone lo peor: salva registros preservados,
reserva los 32 bytes de sombra que Win64 exige en cada llamada, y no programa
nada a traves de la frontera. **Y LTO no lo arregla**: trabaja sobre el
intermedio, y un objeto ensamblado no tiene.

Aqui eso no es academico. El plan presupuesta `RDPMC` en 20-30 ciclos; envolverlo
en un `.S` le anade la llamada y los volcados encima, y falsea justo la medida
que `bench/` existe para tomar.

### La regla del ensamblador

```text
caliente y corto            ->  intrinseco.  Nunca fichero aparte.
frio, o exige control que
  el compilador no da       ->  fichero .S aparte, asumiendo que es una llamada
                                (stub de entrada, guardar/restaurar contexto)
asm en linea                ->  nunca
```

Los intrinsecos difieren entre las dos familias, y esa envoltura es exactamente
la interfaz pequena que este directorio ya necesita para leer un MSR. No hace
falta mecanismo nuevo.

Cuando haya que escribir un `.S`, en **sintaxis Intel** -- que es la de los
bloques `asm {}` del propio Vesta (`asm { mov rax, gs:[0x60] }`) -- con
`.intel_syntax noprefix` en GAS o NASM directamente.

## Aqui no se reserva memoria. Nunca

> `common/` recibe la memoria ya reservada. No pide, no libera, no guarda un
> asignador.

Es una obligacion de este directorio, no una recomendacion. El motivo esta en
`profiler/README.md`, seccion "Memoria", junto con **el procedimiento a seguir
cuando parezca que hace falta reservar** -- que no es reservar igualmente.

En resumen: el manejador de la PMI corre a IRQL alta (contexto de NMI en Linux)
y ahi reservar puede paginar, tomar un cerrojo o bloquearse contra si mismo.
Todo lo que este directorio hace esta pensado para trabajar sobre memoria que
ya existe.

El efecto lateral es el que hace posible probarlo: en espacio de usuario, las
pruebas le pasan un bufer de `malloc` y se comporta exactamente igual que con un
`ExAllocatePool2` o un `vmalloc` detras.

## Estilo: modulos con dueno, sin llamadas indirectas

Se escribe con orientacion a objetos **sin polimorfismo y sin punteros a
funcion**. Todo lo que sigue cuesta **cero en ejecucion** y es donde esta casi
todo el valor: encapsulacion, invariantes con dueno y modulos que se leen
solos.

- **Tipo opaco.** `struct ring;` en la cabecera, definido en el `.c`. Quien lo
  usa no puede tocar los campos, asi que **no puede romper el invariante desde
  fuera**.
- **Almacenamiento del llamante.** Lo normal es que un tipo opaco obligue al
  modulo a reservar; aqui no puede (ver "Aqui no se reserva memoria"), asi que
  se expone `ring_bytes_needed(n)` y el bufer lo aporta quien llama.
  Encapsulacion sin reserva.
- **El prefijo es el espacio de nombres**: `ring_init`, `ring_push`,
  `ring_drain`. El primer parametro es el `this`.
- **`static` para todo lo que no este en la cabecera.** El fichero es la
  frontera del modulo.
- **Forma fija**: `X_init(X *, args) -> estado`, `X_fini(X *)`. Siempre igual,
  para saber que hace sin leerlo.
- **`const` en los observadores**, que es documentacion que el compilador
  comprueba.

Y es lo que hace este directorio **probable**: un modulo con tipo opaco,
almacenamiento del llamante y API con prefijo se ejercita desde una prueba en
C++ sin montar nada.

### La receta

Dos ficheros. La cabecera publica **el tamano, no la forma**:

```c
/* contador.h */
#include "vxp_object.h"

OPAQUE_STORAGE(contador, 32, 8);

MUST_CHECK status contador_init(contador *c, u32 limit);
MUST_CHECK status contador_add(contador *c, u32 n);
MUST_CHECK status contador_value(const contador *c, u32 *out);
void contador_fini(contador *c);
```

Y el `.c` tiene la forma real, que no sale de ahi:

```c
struct contador_impl { u32 value; u32 limit; u32 magic; };
#define CONTADOR_MAGIC 0x434E5452u

OPAQUE_FITS(struct contador_impl, contador);   /* ¿cabe?  lo dice el compilador */

static struct contador_impl *impl(contador *c) {
    return (struct contador_impl *)(void *)c->opaque;
}

status contador_add(contador *c, u32 n) {
    if (c == 0) return ERR_INVALID;                  /* la frontera */
    struct contador_impl *s = impl(c);
    if (s->magic != CONTADOR_MAGIC) return ERR_STATE; /* usado sin init */
    if (n > s->limit - s->value) return ERR_NOSPACE;
    s->value += n;
    return OK;
}
```

Seis reglas, y el porque de cada una:

| | por que |
| --- | --- |
| tipo opaco | los invariantes no se pueden romper desde fuera |
| el llamante coloca el objeto | el modulo **no reserva** |
| `nombre_` de prefijo, objeto primero | el prefijo es el espacio de nombres; el primer parametro es el `this` |
| marca (`magic`) | distingue "sin inicializar" y "ya terminado" de valido |
| comprobar **en la frontera** | dentro se confia en el contrato |
| devolver `status`, nunca puntero | con `MUST_CHECK`, olvidarse de mirar es un aviso |

**Como se elige el tamano publicado**: se escribe el `impl`, se compila, y
`OPAQUE_FITS` dice si no cabe. Con holgura, porque **subirlo despues no rompe a
nadie y bajarlo si**.

Hay dos ejemplos completos y probados en `profiler/tests/opaque_probe.c`: uno de
cada forma.

### Lo que cuesta el idioma, medido

La pregunta obvia de un patron asi es si se paga por el. Se comprobo mirando el
codigo maquina, no razonando: la misma operacion escrita como estructura normal
con campos publicos, y escrita con `OPAQUE_STORAGE` mas su accesor.

| Compilador | ABI | Resultado |
| --- | --- | --- |
| TDM-GCC 10.3 | Win64 | identico byte a byte |
| Clang 22.1 | Win64 | identico byte a byte |
| MSVC 14.51 | Win64 | identico byte a byte |
| GCC 15.2 | SysV | identico byte a byte |
| Clang 19.1 | SysV | identico byte a byte |

```
<a_add>:  8b 01  01 d0  89 01  c3     ; struct normal
<b_add>:  8b 01  01 d0  89 01  c3     ; tipo opaco + accesor
```

Tiene que salir asi, y el motivo conviene decirlo en voz alta porque es lo que
hace legitimo el patron: `opaque` esta en el desplazamiento **cero** del
almacenamiento, de modo que el accesor no es una conversion en ejecucion sino
aritmetica de cero. El puntero que entra y el que sale son el mismo valor y no
hay nada que emitir.

La prueba que importa mas que esa es la otra: un bucle escrito de las dos
maneras **se vectoriza igual** (`movdqu`/`paddd` en los dos). El accesor no
introduce una barrera de aliasing ni le impide al optimizador razonar sobre la
memoria, que es lo que de verdad se teme de un tipo opaco.

**Lo que si cuesta son las comprobaciones**, y eso es otra cosa. Para
`probe_add`, con las tres de frontera -- puntero nulo, marca, tope:

| | instrucciones | bytes | ramas |
| --- | --- | --- | --- |
| sin comprobar | 4 | 7 | 0 |
| con las tres | 13 | 35 | 3 |

Los tres saltos son "no tomado" en operacion normal, asi que en un nucleo fuera
de orden son unos pocos uops fusionados con prediccion perfecta. Despreciable en
la API; no en un bucle que corre millones de veces. De ahi la regla:

> El manejador de la PMI **no pasa por la API comprobada**. Escribe en el anillo
> por el camino directo, sobre punteros que se validaron una vez al abrir la
> sesion. Comprobar en la frontera significa en la frontera, no dentro.

### Dos formas de objeto, y cual toca en cada caso

`OPAQUE_STORAGE` publica un tamano **constante**, y eso cubre la mayoria de los
modulos. No cubre el caso en que el tamano depende de algo que se sabe en
ejecucion -- un anillo de N ranuras -- y ese caso llega seguro.

**Forma A, tamano fijo.** El almacenamiento se declara con `OPAQUE_STORAGE` y el
llamante lo coloca donde quiera:

```c
OPAQUE_STORAGE(sesion, 64, 8);
MUST_CHECK status sesion_init(sesion *s, ...);
```

**Forma B, tamano variable.** El objeto se parte en dos: un bloque de control de
tamano fijo -- opaco, forma A -- y una region de datos que se le entrega:

```c
MUST_CHECK status ring_init(ring *r, void *datos, usize bytes);
usize ring_bytes_needed(u32 ranuras);   /* cuanto pedir para `datos` */
```

No hace falta maquinaria nueva para la B: es una funcion que devuelve un tamano.
Lo unico que hay que respetar es que **el modulo no reserva** -- pregunta cuanto
y el llamante aporta.

### Y esto resuelve lo del anillo, que parecia una contradiccion

Se dijo "opaco por defecto" y tambien "los tipos de la ABI son transparentes", y
el anillo es las dos cosas a la vez. No hay contradiccion porque **son dos
objetos**:

| | que es | como se declara |
| --- | --- | --- |
| la region compartida | el contrato: cabecera, ranuras, indices que ve usuario | **transparente**, en `include/`, porque la parsean los dos lados |
| el estado del productor | capacidad, indices de VERDAD, a donde apunta la region | **opaco**, forma A, y no sale del modulo |

La separacion no es estetica: es la que sostiene la regla de seguridad de que
**el anillo es entrada no confiable**. Los indices que el driver usa para decidir
limites viven en el bloque opaco, que usuario no puede tocar; los de la region
compartida son informativos. Si fueran el mismo campo, la regla no se podria
cumplir aunque se quisiera.

### La excepcion: los tipos de la ABI son transparentes

La disposicion del anillo y del registro de muestra **la parsean los dos lados**.
Ahi la transparencia es el contrato, no un descuido.

> Opaco por defecto; transparente solo lo que cruza la frontera, y por eso
> mismo.

### Tabla de funciones: solo si conviven dos implementaciones

> ¿Hay mas de una implementacion **presente a la vez** en el mismo binario?
> Si -> tabla de punteros. No -> declarar en la cabecera y enlazar la que toca.

- **La interfaz al sistema NO lleva tabla.** Una compilacion de Windows enlaza la
  de Windows y nunca ve la otra: eso es seleccion en tiempo de enlace, no
  polimorfismo. Una tabla ahi pondria una llamada indirecta en el camino mas
  caliente -- leer un contador --, impediria inlinear el intrinseco, y con las
  mitigaciones actuales (retpoline, IBT, kCFI, kCFG) las llamadas indirectas en
  kernel son ademas mas caras y objetivo de secuestro de flujo.
- **El backend de PMU SI la lleva.** Intel PEBS y AMD IBS se eligen en ejecucion
  segun CPUID y conviven en el mismo binario. Ese es polimorfismo de verdad y
  ahi la tabla es la herramienta correcta.

## Nada de punteros opcionales

En C no hay referencias ni `optional`, pero casi todo se puede:

**Devolver estado, no puntero.** En vez de `Thing *get(...)` que devuelve `NULL`,
`int get(..., Thing *out)`. Quien llama **no puede olvidarse** de mirar: el valor
sale por el parametro y el codigo por el retorno. Es la misma forma que exige la
lectura de MSR.

**Indices en vez de punteros.** El anillo, el estado por nucleo y la lista blanca
son arrays. Un `uint32_t` con centinela (`INVALID = UINT32_MAX`) **se puede
comprobar** -- `i < n` --; un puntero corrupto no.

**Que el cero sea un estado valido.** Estructuras en las que todo-ceros signifique
"vacio" y no "puntero basura": asi una reserva a cero da algo usable y una
inicializacion olvidada no da un desreferenciado.

**Las comprobaciones van en la FRONTERA**, no sembradas por todas partes: lo que
entra de usuario y lo que devuelve el sistema. Un `if (!p) return;` en cada
funcion interna no es rigor, es ruido que esconde el fallo real -- y convierte un
error de programacion en un retorno silencioso.

## Lenguaje: C, pero C que compile como C++

**C** porque este directorio se comparte con el modulo de Linux, y el kernel de
Linux no admite C++.

**Que compile como C++** porque las pruebas de aqui son C++ y van a incluir
estas cabeceras. En la practica: guardas `#ifndef` en las macros, nada de
structs anonimos sin marcar, y `extern "C"`.

No es una precaucion teorica. `CreateELF.h`, en este mismo arbol, redefinia
macros que ya traia su libreria hermana y usaba structs anonimos; el resultado
fue que el trozo de codigo mas facil de equivocar -- aritmetica de direcciones,
sin contexto para saber si el numero que sale es el bueno -- era justo el unico
que no se podia probar, porque su cabecera no compilaba como C++.

## Cabeceras

Al lado de sus fuentes, no en `include/`. Una cabecera de aqui incluida desde
codigo de usuario corriente es un error esperando a ocurrir: esta escrita para
tres entornos y da por hechas primitivas que quien la incluye tiene que
proporcionar. Que estuviera en `include/` la anunciaria como algo utilizable.
