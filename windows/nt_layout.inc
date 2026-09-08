/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/*
 * La disposicion que tienen que tener las estructuras declaradas en `nt.h`.
 *
 * NO ES UNA CABECERA: no tiene guardas y se incluye a proposito mas de una vez,
 * con las macros definidas de forma distinta cada vez.  Asi la MISMA lista
 * sirve para dos cosas:
 *
 *   1. comprobar, al compilar, que nuestras estructuras miden y colocan lo que
 *      dice esta tabla;
 *   2. comprobar la tabla CONTRA UNA CABECERA DE REFERENCIA, cuando la hay.
 *
 * Lo segundo es lo que hace que esto valga algo.  Si los numeros solo se
 * contrastaran contra nuestras propias estructuras, un error al teclearlos
 * pasaria desapercibido: la tabla estaria mal, la estructura estaria mal
 * IGUAL, y las dos coincidirian.  Con la referencia, ese error no compila.
 *
 * ANADIR UNA ESTRUCTURA es anadir una linea por campo que se use.  No hace
 * falta enumerarlos todos: basta con los que se escriben o se leen, mas el
 * tamano total, que es lo que caza un campo olvidado en medio.
 */

/* Cadenas del kernel. */
NT_LAYOUT_SIZE(UNICODE_STRING, 16)
NT_LAYOUT_OFF(UNICODE_STRING, Length, 0)
NT_LAYOUT_OFF(UNICODE_STRING, MaximumLength, 2)
NT_LAYOUT_OFF(UNICODE_STRING, Buffer, 8)

/* El objeto de driver.  `DriverUnload` es el unico que se escribe, pero se
 * comprueban tambien sus vecinos y el tamano total: un campo olvidado antes de
 * el no cambiaria su nombre, cambiaria su desplazamiento. */
NT_LAYOUT_SIZE(DRIVER_OBJECT, 336)
NT_LAYOUT_OFF(DRIVER_OBJECT, DriverExtension, 48)
NT_LAYOUT_OFF(DRIVER_OBJECT, DriverName, 56)
NT_LAYOUT_OFF(DRIVER_OBJECT, DriverUnload, 104)
NT_LAYOUT_OFF(DRIVER_OBJECT, MajorFunction, 112)

/* Afinidad por grupo: se le pasa al kernel por valor, asi que si el tamano no
 * coincide se lee basura como mascara -- y una mascara equivocada no falla,
 * mide OTRO nucleo. */
NT_LAYOUT_SIZE(GROUP_AFFINITY, 16)
NT_LAYOUT_OFF(GROUP_AFFINITY, Mask, 0)
NT_LAYOUT_OFF(GROUP_AFFINITY, Group, 8)

/* El resultado de una operacion de E/S.
 *
 * NO se comprueba el desplazamiento del primer campo, que es una union: en las
 * cabeceras de referencia se llama `DUMMYUNIONNAME` -- una macro que segun la
 * configuracion se expande a un nombre o a nada --, asi que nombrarlo aqui
 * romperia la mitad que contrasta contra ellas.  Con el tamano total y el
 * desplazamiento de `Information` queda igual de sujeto: si la union midiera
 * otra cosa, `Information` no caeria en 8. */
NT_LAYOUT_SIZE(IO_STATUS_BLOCK, 16)
NT_LAYOUT_OFF(IO_STATUS_BLOCK, Information, 8)

/* Como se nombra un objeto del kernel.  `Length` es el propio tamano de la
 * estructura y el kernel lo VALIDA, asi que un tamano equivocado aqui se
 * convierte en un rechazo al abrir el fichero, no en corrupcion. */
NT_LAYOUT_SIZE(OBJECT_ATTRIBUTES, 48)
NT_LAYOUT_OFF(OBJECT_ATTRIBUTES, Length, 0)
NT_LAYOUT_OFF(OBJECT_ATTRIBUTES, RootDirectory, 8)
NT_LAYOUT_OFF(OBJECT_ATTRIBUTES, ObjectName, 16)
NT_LAYOUT_OFF(OBJECT_ATTRIBUTES, Attributes, 24)
NT_LAYOUT_OFF(OBJECT_ATTRIBUTES, SecurityDescriptor, 32)
NT_LAYOUT_OFF(OBJECT_ATTRIBUTES, SecurityQualityOfService, 40)
