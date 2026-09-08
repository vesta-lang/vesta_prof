/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file dump.h
 * @brief Volcado de CPUID y del catalogo de MSR, campo a campo.
 *
 * QUE ES.  `pmu_caps` responde a preguntas concretas -- ¿hay PEBS?, ¿cuantos
 * contadores? -- y para eso mira los campos que sabe de antemano que le
 * importan.  Esto es lo otro: recorrer TODO lo que los manuales documentan y
 * decir que vale en esta maquina, sin decidir de antemano que es interesante.
 *
 * Sirve para lo que no se puede anticipar: mandar un volcado con un informe de
 * fallo, comparar dos piezas, o enterarse de que una capacidad existe antes de
 * que a nadie se le ocurra preguntar por ella.
 *
 * DE DONDE SALE LO QUE IMPRIME.  De `cpuid_table.h` y `msr_table.h`, que son
 * las tablas generadas de los manuales.  Aqui no hay ni un nombre ni un numero
 * de bit escrito a mano: si el volcado dice `PDCM`, es porque el manual lo
 * llama asi.
 *
 * LOS DOS LADOS NO SON SIMETRICOS, y no por descuido:
 *
 *   CPUID  se ejecuta sin privilegios y la tabla trae la posicion y la anchura
 *          de cada campo, asi que sale desglosado bit a bit.
 *   MSR    solo se lee en anillo cero, y la tabla trae la direccion, el nombre
 *          y la puerta -- pero NO los campos de dentro, que el manual da como
 *          prosa --.  Asi que sale el valor de sesenta y cuatro bits entero,
 *          sin desglosar.
 *
 * SALE EN CSV, Y NO ALINEADO PARA LEER.  La regla del proyecto es que quien
 * mide no interpreta -- ver `cli/README.md` --, y un volcado alineado a
 * columnas es interpretacion: fija como se ve, y cambiarlo obliga a reconstruir
 * el perfilador.  En CSV, comparar dos maquinas es un `diff` exacto en vez de
 * mirar columnas, y quien quiera verlo alineado lo tiene con `column -t -s,`.
 *
 * No hay preambulo, y no es un olvido: la hoja maxima y el fabricante ya son
 * FILAS del propio volcado (`MAX_LEAF` es CPUID.0:EAX[31:0], los `VENDOR_ID_*`
 * son EBX/ECX/EDX de la misma hoja).  Un preambulo, ademas, romperia el formato
 * para todo lo que lea CSV.
 *
 * NINGUNA CELDA NECESITA COMILLAS.  Los nombres son identificadores por
 * construccion: el emisor de las tablas rechaza cualquiera que no lo sea, asi
 * que no puede aparecer una coma.  Es una garantia de la generacion, no una
 * suposicion de aqui.
 *
 * Los esquemas:
 *
 *   CPUID  cpu,leaf,subleaf,reg,lo,width,name,reserved,value
 *   MSR    cpu,addr,name,indexed,gate_leaf,gate_subleaf,gate_reg,gate_lo,
 *          gate_width,state,value
 *
 * Las hojas, direcciones y valores en hexadecimal; las posiciones de bit en
 * decimal.  No es una eleccion nuestra: es como los escribe el manual, y asi lo
 * que sale del volcado se busca alli tal cual.  Una celda vacia es "no aplica"
 * -- sin subhoja, o sin valor --, que no es lo mismo que un cero.
 *
 * LA COLUMNA `cpu` ES LO QUE HACE EL FICHERO REUTILIZABLE.  Una pieza hibrida
 * no responde lo mismo en un nucleo P que en uno E, asi que un volcado sin
 * decir de que procesador viene cada fila no se puede ni interpretar ni unir:
 * veinticuatro recorridos concatenados serian veinticuatro filas con la misma
 * clave y valores distintos.  Con la columna, el fichero es UNA tabla, la clave
 * es `(cpu, leaf, subleaf, reg, lo)` y se puede agrupar, comparar entre nucleos
 * o unir con el volcado de otra maquina.
 *
 * Y el esquema es el MISMO lo escriba el driver o lo escriba el programa de
 * usuario.  Esa es la otra mitad de que sea reutilizable: los dos ficheros se
 * concatenan, se comparan y se unen sin convertir nada.
 */

#ifndef VXP_COMMON_DUMP_H
#define VXP_COMMON_DUMP_H

#include "pmu_caps.h"
#include "vxp_base.h"

/**
 * @brief Como leer un MSR, que solo se puede en anillo cero.
 *
 * Entra por puntero a funcion para que el volcado no dependa de donde corre:
 * el driver pasa su lectura de verdad y un programa de usuario pasa cero, y el
 * mismo codigo sirve para los dos.
 *
 * @param addr la direccion del registro
 * @param ctx  lo que el llamante quiera arrastrar
 */
typedef msr_value (*msr_read_fn)(u32 addr, void *ctx);

/**
 * @brief Vuelca todos los campos de CPUID que el manual documenta.
 *
 * Elige la tabla por el fabricante de la maquina.  Una hoja por encima del
 * maximo que la pieza anuncia NO se consulta: CPUID no falla ahi, devuelve
 * otra cosa, y volcar eso seria inventarse datos con toda la apariencia de
 * ser buenos.
 *
 * @param cpu     el procesador logico desde el que se esta preguntando; va como
 *                primera columna.  Quien llama es el unico que lo sabe: este
 *                fichero no puede preguntarselo al sistema
 * @param header  distinto de cero para emitir la fila de cabecera.  Se apaga al
 *                encadenar procesadores en un mismo fichero: una cabecera
 *                repetida en medio convierte la tabla en algo que ningun lector
 *                de CSV puede procesar de una pasada
 * @param buf     donde escribir
 * @param cap     cuanto cabe
 * @param written cuanto se escribio
 * @return `OK`, o `ERR_NOSPACE` si no cupo -- y entonces lo escrito es un
 *         prefijo valido, nunca basura.
 */
status cpuid_dump(u32 cpu, int header, char *buf, usize cap, usize *written);

/**
 * @brief Vuelca el catalogo de MSR: direccion, nombre, puerta y valor.
 *
 * El VALOR solo aparece si se cumplen las dos condiciones: que `read` no sea
 * nulo, y que el manual documente una puerta de CPUID que esta maquina
 * satisfaga.  Los demas salen igual, con el nombre y la direccion, y la columna
 * `state` dice cual de los cuatro motivos:
 *
 *   no_gate      el manual no documenta la condicion al lado.  NO dice que el
 *                registro no exista: dice que por esta via no se sabe
 *   unsatisfied  hay condicion, y esta pieza no la cumple
 *   no_reader    la cumple, pero quien vuelca no puede leer -- modo usuario
 *   ok           leido, y el valor esta en la ultima columna
 *   faulted      se intento y el procesador la rechazo
 *
 * Que sea una COLUMNA y no una frase es lo que permite filtrar.
 *
 * Y el reparto importa: **de los mil quinientos ochenta y ocho MSR de Intel,
 * solo trescientos quince traen la puerta adyacente en el manual.**  Leer los
 * otros a ciegas es una excepcion de proteccion general, que en anillo cero se
 * lleva la maquina por delante.  Asi que no se leen, y se dice que no se
 * leyeron.  Un volcado que se saltara esos registros en silencio pareceria
 * decir que no existen.
 *
 * @param cpu     el procesador logico desde el que se lee; primera columna
 * @param header  distinto de cero para emitir la fila de cabecera
 * @param guarded distinto de cero si `read` sobrevive a leer un registro que no
 *                existe.  Es lo que decide si se miran o no los MSR SIN puerta
 *                documentada, que son la gran mayoria: con guarda se leen y el
 *                que no exista sale como `faulted`, que es un dato; sin ella ni
 *                se intentan, porque un `#GP` en anillo cero se lleva la
 *                maquina por delante
 * @param read    como leer un MSR, o cero para volcar solo el catalogo
 * @param ctx     se le pasa tal cual a `read`
 * @param buf     donde escribir
 * @param cap     cuanto cabe
 * @param written cuanto se escribio
 * @return `OK`, o `ERR_NOSPACE` si no cupo
 */
status msr_dump(u32 cpu, int header, int guarded, msr_read_fn read, void *ctx,
                char *buf, usize cap, usize *written);

#endif /* VXP_COMMON_DUMP_H */
