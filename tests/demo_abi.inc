/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 + excepcion de runtime (ver LICENSE).
 */

/*
 * La disposicion de una estructura publicada de ejemplo.
 *
 * NO ES UNA CABECERA: sin guardas, y se incluye varias veces con las macros
 * definidas de forma distinta cada vez.  De esta unica lista salen las dos
 * cosas -- las aserciones de compilacion y la tabla de descripcion de
 * ejecucion --, que es el punto: escribir el descriptor aparte seria una
 * segunda copia de la disposicion.
 *
 * El ultimo numero es el ID del campo, y es lo que NO cambia nunca aunque el
 * campo se renombre o se mueva.  Es la identidad estable de la que depende todo
 * lo demas.
 */

ABI_SIZE(demo_sample, 24)
ABI_FIELD(demo_sample, head, 0, 1)
ABI_FIELD(demo_sample, ip, 8, 2)
ABI_FIELD(demo_sample, tid, 16, 3)
ABI_FIELD(demo_sample, cpu, 20, 4)
