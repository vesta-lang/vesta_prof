/*
 * VestaVM -- Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).  Junto a su fuente, no en `include/`.
 */

/**
 * @file probe/dump.h
 * @brief
 * \~english Dump of CPUID and of the MSR catalogue, field by field.
 * \~spanish Volcado de CPUID y del catalogo de MSR, campo a campo.
 * \~
 *
 * \~english
 * WHAT IT IS.  `pmu_caps` answers specific questions -- is there PEBS?  how many
 * counters? -- and for that it looks at the fields it knows in advance it cares
 * about.  This is the other thing: walking EVERYTHING the manuals document and
 * saying what it is worth on this machine, without deciding beforehand what is
 * interesting.
 *
 * It serves what cannot be anticipated: sending a dump with a bug report,
 * comparing two parts, or finding out a capability exists before it occurs to
 * anyone to ask for it.
 *
 * WHERE WHAT IT PRINTS COMES FROM.  From `cpuid/table.h` and `msr/table.h`,
 * which are the tables generated from the manuals.  There is not one name nor
 * one bit number written by hand here: if the dump says `PDCM`, it is because
 * the manual calls it that.
 *
 * THE TWO SIDES ARE NOT SYMMETRIC, and not by oversight:
 *
 *   CPUID  runs without privileges and the table brings each field's position
 *          and width, so it comes out broken down bit by bit.
 *   MSR    is only readable in ring 0, and the table brings the address, the
 *          name and the gate -- but NOT the fields inside, which the manual
 *          gives as prose.  So the whole sixty-four bit value comes out,
 *          undivided.
 *
 * IT COMES OUT AS CSV, AND NOT ALIGNED FOR READING.  The project's rule is that
 * whoever measures does not interpret -- see `cli/README.md` -- and a
 * column-aligned dump is interpretation: it pins how it looks, and changing it
 * forces rebuilding the profiler.  In CSV, comparing two machines is an exact
 * `diff` instead of eyeballing columns, and whoever wants it aligned has it with
 * `column -t -s,`.
 *
 * There is no preamble, and that is not an oversight: the maximum leaf and the
 * vendor are already ROWS of the dump itself (`MAX_LEAF` is CPUID.0:EAX[31:0],
 * the `VENDOR_ID_*` are EBX/ECX/EDX of the same leaf).  A preamble would also
 * break the format for everything that reads CSV.
 *
 * NO CELL NEEDS QUOTING.  The names are identifiers by construction: the table
 * emitter rejects any that is not, so a comma cannot appear.  That is a
 * guarantee of the generation, not an assumption here.
 *
 * \~spanish
 * QUE ES.  `pmu_caps` responde a preguntas concretas -- ¿hay PEBS?, ¿cuantos
 * contadores? -- y para eso mira los campos que sabe de antemano que le
 * importan.  Esto es lo otro: recorrer TODO lo que los manuales documentan y
 * decir que vale en esta maquina, sin decidir de antemano que es interesante.
 *
 * Sirve para lo que no se puede anticipar: mandar un volcado con un informe de
 * fallo, comparar dos piezas, o enterarse de que una capacidad existe antes de
 * que a nadie se le ocurra preguntar por ella.
 *
 * DE DONDE SALE LO QUE IMPRIME.  De `cpuid/table.h` y `msr/table.h`, que son las
 * tablas generadas de los manuales.  Aqui no hay ni un nombre ni un numero de
 * bit escrito a mano: si el volcado dice `PDCM`, es porque el manual lo llama
 * asi.
 *
 * LOS DOS LADOS NO SON SIMETRICOS, y no por descuido:
 *
 *   CPUID  se ejecuta sin privilegios y la tabla trae la posicion y la anchura
 *          de cada campo, asi que sale desglosado bit a bit.
 *   MSR    solo se lee en anillo cero, y la tabla trae la direccion, el nombre y
 *          la puerta -- pero NO los campos de dentro, que el manual da como
 *          prosa --.  Asi que sale el valor de sesenta y cuatro bits entero, sin
 *          desglosar.
 *
 * SALE EN CSV, Y NO ALINEADO PARA LEER.  La regla del proyecto es que quien mide
 * no interpreta -- ver `cli/README.md` --, y un volcado alineado a columnas es
 * interpretacion: fija como se ve, y cambiarlo obliga a reconstruir el
 * perfilador.  En CSV, comparar dos maquinas es un `diff` exacto en vez de mirar
 * columnas, y quien quiera verlo alineado lo tiene con `column -t -s,`.
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
 * \~
 *
 *  \~english  The schemas:
 *  \~spanish  Los esquemas:
 *  \~
 *
 *    CPUID  cpu,leaf,subleaf,reg,lo,width,name,reserved,value
 *    MSR    cpu,addr,name,indexed,gate_leaf,gate_subleaf,gate_reg,gate_lo,
 *           gate_width,state,value
 *
 * \~english
 * Leaves, addresses and values in hexadecimal; bit positions in decimal.  That
 * is not a choice of ours: it is how the manual writes each, and that way what
 * comes out of the dump is looked up there verbatim.  An empty cell is "not
 * applicable" -- no subleaf, or no value -- which is not the same as a zero.
 *
 * THE `cpu` COLUMN IS WHAT MAKES THE FILE REUSABLE.  A hybrid part does not
 * answer the same on a P core as on an E core, so a dump that does not say which
 * processor each row comes from can neither be interpreted nor joined:
 * twenty-four concatenated walks would be twenty-four rows with the same key and
 * different values.  With the column, the file is ONE table, the key is
 * `(cpu, leaf, subleaf, reg, lo)` and it can be grouped, compared across cores
 * or joined with another machine's dump.
 *
 * And the schema is the SAME whether the driver writes it or the user-space
 * program does.  That is the other half of being reusable: the two files
 * concatenate, compare and join without converting anything.
 *
 * \~spanish
 * Las hojas, direcciones y valores en hexadecimal; las posiciones de bit en
 * decimal.  No es una eleccion nuestra: es como los escribe el manual, y asi lo
 * que sale del volcado se busca alli tal cual.  Una celda vacia es "no aplica"
 * -- sin subhoja, o sin valor --, que no es lo mismo que un cero.
 *
 * LA COLUMNA `cpu` ES LO QUE HACE EL FICHERO REUTILIZABLE.  Una pieza hibrida no
 * responde lo mismo en un nucleo P que en uno E, asi que un volcado sin decir de
 * que procesador viene cada fila no se puede ni interpretar ni unir:
 * veinticuatro recorridos concatenados serian veinticuatro filas con la misma
 * clave y valores distintos.  Con la columna, el fichero es UNA tabla, la clave
 * es `(cpu, leaf, subleaf, reg, lo)` y se puede agrupar, comparar entre nucleos
 * o unir con el volcado de otra maquina.
 *
 * Y el esquema es el MISMO lo escriba el driver o lo escriba el programa de
 * usuario.  Esa es la otra mitad de que sea reutilizable: los dos ficheros se
 * concatenan, se comparan y se unen sin convertir nada.
 */

#ifndef VXP_COMMON_PROBE_DUMP_H
#define VXP_COMMON_PROBE_DUMP_H

#include "pmu_caps.h"
#include "vxp_base.h"

/**
 * @brief
 * \~english How to read an MSR, which can only be done in ring 0.
 * \~spanish Como leer un MSR, que solo se puede en anillo cero.
 * \~
 *
 * \~english
 * It comes in as a function pointer so the dump does not depend on where it
 * runs: the driver passes its real read and a user-space program passes zero,
 * and the same code serves both.
 *
 * \~spanish
 * Entra por puntero a funcion para que el volcado no dependa de donde corre: el
 * driver pasa su lectura de verdad y un programa de usuario pasa cero, y el
 * mismo codigo sirve para los dos.
 * \~
 *
 * @param addr \~english the register's address \~spanish la direccion del registro \~
 * @param ctx  \~english whatever the caller wants to carry along \~spanish lo que el llamante quiera arrastrar \~
 */
typedef msr_value (*msr_read_fn)(u32 addr, void *ctx);

/**
 * @brief
 * \~english Dumps every CPUID field the manual documents.
 * \~spanish Vuelca todos los campos de CPUID que el manual documenta.
 * \~
 *
 * \~english
 * It picks the table by the machine's vendor signature.  A leaf above the
 * maximum the part announces is NOT queried: CPUID does not fail there, it
 * returns something else, and dumping that would be inventing data with every
 * appearance of being good.
 *
 * \~spanish
 * Elige la tabla por la firma del fabricante de la maquina.  Una hoja por encima
 * del maximo que la pieza anuncia NO se consulta: CPUID no falla ahi, devuelve
 * otra cosa, y volcar eso seria inventarse datos con toda la apariencia de ser
 * buenos.
 * \~
 *
 * @param cpu     \~english the logical processor being asked from; goes as the
 *                first column.  The caller is the only one who knows it: this
 *                file cannot ask the system
 *                \~spanish el procesador logico desde el que se esta
 *                preguntando; va como primera columna.  Quien llama es el unico
 *                que lo sabe: este fichero no puede preguntarselo al sistema \~
 * @param header  \~english non-zero to emit the header row.  Turned off when
 *                chaining processors into one file: a header repeated in the
 *                middle turns the table into something no CSV reader can process
 *                in one pass
 *                \~spanish distinto de cero para emitir la fila de cabecera.  Se
 *                apaga al encadenar procesadores en un mismo fichero: una
 *                cabecera repetida en medio convierte la tabla en algo que
 *                ningun lector de CSV puede procesar de una pasada \~
 * @param buf     \~english where to write \~spanish donde escribir \~
 * @param cap     \~english how much fits \~spanish cuanto cabe \~
 * @param written \~english how much was written \~spanish cuanto se escribio \~
 *
 * @return
 * \~english `OK`, or `ERR_NOSPACE` if it did not fit -- and then what was
 *           written is a valid prefix, never garbage.
 * \~spanish `OK`, o `ERR_NOSPACE` si no cupo -- y entonces lo escrito es un
 *           prefijo valido, nunca basura.
 */
status cpuid_dump(u32 cpu, int header, char *buf, usize cap, usize *written);

/**
 * @brief
 * \~english Dumps the MSR catalogue: address, name, gate and value.
 * \~spanish Vuelca el catalogo de MSR: direccion, nombre, puerta y valor.
 * \~
 *
 * \~english
 * The VALUE only appears if both conditions hold: that `read` is not null, and
 * that the manual documents a CPUID gate this machine satisfies.  The rest come
 * out all the same, with their name and address, and the `state` column says
 * which of the reasons:
 *
 *   no_gate      the manual does not document the condition next to it.  It does
 *                NOT say the register does not exist: it says that by this route
 *                we do not know
 *   unsatisfied  there is a condition, and this part does not meet it
 *   no_reader    it meets it, but whoever dumps cannot read -- user space
 *   ok           read, and the value is in the last column
 *   faulted      it was attempted and the processor refused it
 *
 * That it is a COLUMN and not a sentence is what allows filtering.
 *
 * And the split matters: **of the one thousand five hundred and eighty-eight
 * Intel MSRs, only three hundred and fifteen bring the gate adjacent in the
 * manual.**  Reading the others blind is a general protection fault, which in
 * ring 0 takes the machine down.  So they are not read, and it says they were
 * not.  A dump that silently skipped those registers would seem to say they do
 * not exist.
 *
 * \~spanish
 * El VALOR solo aparece si se cumplen las dos condiciones: que `read` no sea
 * nulo, y que el manual documente una puerta de CPUID que esta maquina
 * satisfaga.  Los demas salen igual, con el nombre y la direccion, y la columna
 * `state` dice cual de los motivos:
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
 * leyeron.  Un volcado que se saltara esos registros en silencio pareceria decir
 * que no existen.
 * \~
 *
 * @param cpu     \~english the logical processor being read from; first column \~spanish el procesador logico desde el que se lee; primera columna \~
 * @param header  \~english non-zero to emit the header row \~spanish distinto de cero para emitir la fila de cabecera \~
 * @param guarded \~english non-zero if `read` survives reading a register that
 *                does not exist.  It is what decides whether the MSRs WITHOUT a
 *                documented gate get looked at, which are the vast majority:
 *                with a guard they are read and one that does not exist comes
 *                out as `faulted`, which is data; without it they are not even
 *                attempted, because a `#GP` in ring 0 takes the machine down
 *                \~spanish distinto de cero si `read` sobrevive a leer un
 *                registro que no existe.  Es lo que decide si se miran o no los
 *                MSR SIN puerta documentada, que son la gran mayoria: con guarda
 *                se leen y el que no exista sale como `faulted`, que es un dato;
 *                sin ella ni se intentan, porque un `#GP` en anillo cero se
 *                lleva la maquina por delante \~
 * @param read    \~english how to read an MSR, or zero to dump only the catalogue \~spanish como leer un MSR, o cero para volcar solo el catalogo \~
 * @param ctx     \~english passed through to `read` \~spanish se le pasa tal cual a `read` \~
 * @param buf     \~english where to write \~spanish donde escribir \~
 * @param cap     \~english how much fits \~spanish cuanto cabe \~
 * @param written \~english how much was written \~spanish cuanto se escribio \~
 *
 * @return \~english `OK`, or `ERR_NOSPACE` if it did not fit \~spanish `OK`, o `ERR_NOSPACE` si no cupo \~
 */
status msr_dump(u32 cpu, int header, int guarded, msr_read_fn read, void *ctx,
                char *buf, usize cap, usize *written);

#endif /* VXP_COMMON_PROBE_DUMP_H */
