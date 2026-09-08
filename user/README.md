# user/

El lado de usuario: vaciar el anillo, llevar la sesion y atribuir las muestras.

Es la biblioteca que consumen las dos formas de usar el perfilador -- dentro del
compilador, y el binario suelto de `cli/`.

## La regla que sostiene el modo suelto

> **No depende de nada del compilador.** Se puede sacar de aqui y llevarselo.

Es la misma que `vesta_alloc` declara en la cabecera de su `CMakeLists.txt`, y
por el mismo motivo: si dependiera, el binario suelto no existiria -- o
existiria arrastrando el compilador entero, que es lo mismo.

En la practica: **no enlaza contra `vmcore` ni contra `vx_lib`**. Lo que
necesite de ellos entra por una interfaz que el modo suelto pueda dejar sin
implementar.

## Donde se nota: la atribucion

Resolver una direccion a fichero y linea sale de DWARF, que es autonomo.
Atribuirla a un **nodo del intermedio** necesita lo que sabe el compilador.

El suelto tiene la primera y no la segunda. Eso es una diferencia **deliberada**
del producto, no una carencia que haya que tapar: quien perfila un binario sin
tener sus fuentes delante tampoco podria usar la segunda.

## El anillo

Los indices se calculan con la cabecera en linea que comparte con el productor.
No se reimplementan aqui: si las dos cuentas divergen, el anillo no da error --
devuelve muestras que no son.
