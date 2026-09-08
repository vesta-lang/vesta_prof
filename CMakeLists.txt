# =============================================================================
#  profiler -- el perfilador de Vesta: driver, lado de usuario y herramientas
# =============================================================================
#
# Se construye de dos maneras, igual que `vesta_alloc`:
#
#   - COMO PARTE de otro proyecto (`add_subdirectory`): el compilador enlaza
#     contra sus objetivos y las pruebas propias se apagan, para no duplicar
#     binarios ni tiempo de construccion.
#   - SOLO, como cualquier proyecto:
#
#         cmake -S . -B build && cmake --build build && ctest --test-dir build
#
# EL LADO DE KERNEL NO SE PROMETE.  El `.sys` necesita el WDK y el `.ko` las
# cabeceras del kernel en ejecucion, y ninguna de las dos esta en todas las
# maquinas.  Se detectan, y si faltan se omite DICIENDOLO: un desarrollador
# tiene que poder construir el compilador sin nada de eso instalado; lo que no
# puede es creerse que construyo el driver cuando no lo hizo.
#
# Ver README.md para las decisiones de estructura -- que va en cada directorio,
# que lenguaje, de donde sale la memoria.
cmake_minimum_required(VERSION 3.12)

# Detectar si se construye suelto o dentro de otro proyecto.  C ademas de C++:
# de la frontera para abajo todo es C, y las pruebas del cimiento se compilan
# EN LOS DOS lenguajes a proposito.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(vesta_profiler VERSION 0.1.0 LANGUAGES C CXX)
    set(VXP_STANDALONE ON)
else ()
    set(VXP_STANDALONE OFF)
endif ()

# Las pruebas se activan cuando se construye suelto y se apagan al integrarse.
option(VXP_BUILD_TESTS "Compilar las pruebas del perfilador" ${VXP_STANDALONE})

# -----------------------------------------------------------------------------
#  LTO: encendido por defecto, y no como una optimizacion mas.
# -----------------------------------------------------------------------------
#
# El estilo de `common/` -- cabecera opaca, implementacion en su `.c`,
# comprobaciones en la frontera -- pone una llamada real entre el consumidor y
# el modulo en cuanto viven en unidades de traduccion distintas, que es siempre.
#
# Medido sobre las pruebas del cimiento: sin LTO quedan 3 llamadas al modulo en
# el binario; con LTO, ninguna, y desaparece hasta la comprobacion de la marca
# porque el compilador prueba que `init` acababa de ponerla.  Misma salida.
#
# Es decir: sin esto el estilo se paga entero, y con esto sale gratis.  Por eso
# no es una bandera de rendimiento que se pueda olvidar -- construir sin LTO da
# otro perfil de coste, y en un perfilador eso significa medirse a uno mismo.
# Ver README.md, "LTO no es una optimizacion mas".
option(VXP_LTO "Optimizacion en tiempo de enlazado (LTO/IPO)" ON)

if (VXP_LTO)
    include(CheckIPOSupported)
    # Se PREGUNTA en vez de darlo por hecho: no todas las cadenas de
    # herramientas lo traen, y encenderlo a ciegas rompe la configuracion en la
    # maquina de otro en vez de degradarse.
    check_ipo_supported(RESULT VXP_IPO_OK OUTPUT VXP_IPO_WHY)
    if (VXP_IPO_OK)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
        # Menos en Debug, y a proposito.  La razon de existir de ese modo es que
        # la pila que se ve en el depurador corresponda al codigo escrito, y el
        # LTO deshace justamente eso.  En un driver, donde el volcado es muchas
        # veces la unica evidencia, no es un detalle.
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
        message(STATUS "profiler: LTO activado (apagado en Debug, para que el volcado se lea)")
    else ()
        # No se calla: sin LTO el binario tiene otro coste, y quien lo construya
        # tiene que saberlo.
        message(WARNING "profiler: LTO NO disponible -- ${VXP_IPO_WHY}\n"
                "  El binario tendra el coste de las llamadas entre modulos.  Ver README.md.")
    endif ()
endif ()

# -----------------------------------------------------------------------------
#  El contrato y el cimiento: por ahora, solo cabeceras.
# -----------------------------------------------------------------------------
#
# INTERFACE y no STATIC porque todavia no hay ni un `.c` que compilar.  Cuando
# `common/` y `user/` tengan fuentes, esto pasa a ser una biblioteca de verdad
# y quien la consume no se entera: el nombre del objetivo no cambia.
add_library(vxp_base INTERFACE)
target_include_directories(vxp_base INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
add_library(vesta_profiler::base ALIAS vxp_base)

# -----------------------------------------------------------------------------
#  Deteccion del lado de kernel.
# -----------------------------------------------------------------------------
#
# Solo se mira el lado que corresponde a ESTA maquina: el `.sys` se construye en
# Windows y el `.ko` en Linux, nunca los dos desde la misma configuracion.  Un
# modulo se compila contra las cabeceras del kernel al que va a cargarse, y eso
# no se arregla con banderas de cross-compilacion.
set(VXP_KERNEL_SIDE OFF)

if (WIN32)
    # NO se busca el WDK, y no es que sea opcional: es que no se usa.
    #
    # Lo que hace falta del kernel vive en `windows/` -- las declaraciones en
    # `nt.h` y la biblioteca de importacion generada de un `.def` nuestro --, y
    # las banderas del enlace son eso, banderas.  Comprobado que sale un `.sys`
    # nativo con esas tres piezas y nada mas.  Ver windows/README.md.
    #
    # De modo que en Windows el lado de kernel se puede construir SIEMPRE, con
    # el mismo compilador que el resto del proyecto.
    set(VXP_KERNEL_SIDE ON)
    message(STATUS "profiler: lado de kernel autonomo (sin WDK)")
elseif (UNIX AND NOT APPLE)
    # kbuild necesita el arbol de construccion del kernel en ejecucion.
    execute_process(COMMAND uname -r
            OUTPUT_VARIABLE VXP_UNAME_R
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
    if (VXP_UNAME_R AND EXISTS "/lib/modules/${VXP_UNAME_R}/build")
        set(VXP_KERNEL_SIDE ON)
        message(STATUS "profiler: cabeceras del kernel ${VXP_UNAME_R}; se puede construir el .ko")
    else ()
        message(STATUS "profiler: sin cabeceras del kernel -- NO se construye el .ko.  El resto del perfilador si.")
    endif ()
else ()
    message(STATUS "profiler: plataforma sin lado de kernel; solo el lado de usuario")
endif ()

# -----------------------------------------------------------------------------
#  El lado de kernel, cuando toca.
# -----------------------------------------------------------------------------
#
# El `.sys` es un objetivo de CMake normal -- pero en SU directorio, porque un
# driver necesita que no se le anadan las bibliotecas estandar del enlace y esa
# variable es de ambito de directorio.  Ver windows/CMakeLists.txt.
#
# Con MSVC no se construye, y no por capricho: las banderas de enlace de un
# driver son otras por completo (`/DRIVER`, `/SUBSYSTEM:NATIVE`,
# `/ENTRY:DriverEntry`, y la biblioteca de importacion se genera con `lib` en
# vez de con `dlltool`).  Soportar los dos seria mantener dos recetas de las que
# solo una se usa a diario.  Se dice, en vez de producir algo que no carga.
if (VXP_KERNEL_SIDE AND WIN32)
    if (MSVC)
        message(STATUS "profiler: el .sys no se construye con MSVC -- usa GCC o Clang")
    else ()
        add_subdirectory(windows)
    endif ()
endif ()

# El `.ko` de Linux NO puede ser un objetivo de CMake: los modulos se construyen
# con `kbuild`, que es el sistema de construccion del propio kernel, y lo unico
# que puede hacer CMake es invocarlo.  Escribirlo como una biblioteca de CMake
# produciria algo que enlaza y que el kernel no carga.  Ver linux/README.md.

# -----------------------------------------------------------------------------
#  Pruebas.
# -----------------------------------------------------------------------------
if (VXP_BUILD_TESTS)
    enable_testing()

    # El cimiento se prueba EN LOS DOS LENGUAJES: `opaque_probe.c` se compila
    # como C y publica lo que ve; el `.cpp` compila como C++ y compara.  Si los
    # tipos midieran distinto, o faltara `extern "C"`, esto no enlazaria -- y
    # ese es justamente el fallo que se quiere cazar.
    add_executable(test_vxp_base
            tests/test_vxp_base.cpp
            tests/opaque_probe.c)
    target_link_libraries(test_vxp_base PRIVATE vxp_base)
    target_include_directories(test_vxp_base PRIVATE tests)

    set_target_properties(test_vxp_base PROPERTIES
            C_STANDARD 11 C_STANDARD_REQUIRED ON
            CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

    # Estrictos a proposito: esta cabecera tiene que valer en TDM/MinGW, Clang y
    # MSVC, y los avisos de uno son los errores del siguiente.
    if (MSVC)
        target_compile_options(test_vxp_base PRIVATE /W4)
    else ()
        target_compile_options(test_vxp_base PRIVATE
                -Wall -Wextra -pedantic-errors)
    endif ()

    add_test(NAME vxp_base COMMAND test_vxp_base)

    # Las tres capas con las que el contrato se describe a si mismo.  Como C,
    # que es el lenguaje del lado que las produce.
    add_executable(test_vxp_abi tests/test_vxp_abi.c)
    target_link_libraries(test_vxp_abi PRIVATE vxp_base)
    target_include_directories(test_vxp_abi PRIVATE tests)
    set_target_properties(test_vxp_abi PROPERTIES
            C_STANDARD 11 C_STANDARD_REQUIRED ON)
    if (MSVC)
        target_compile_options(test_vxp_abi PRIVATE /W4)
    else ()
        target_compile_options(test_vxp_abi PRIVATE -Wall -Wextra -pedantic-errors)
    endif ()
    add_test(NAME vxp_abi COMMAND test_vxp_abi)

    # La deteccion del PMU, en ESPACIO DE USUARIO.
    #
    # Esto es lo que se compra con la regla de que `common/` no incluya
    # cabeceras del sistema: el mismo fuente que corre dentro del kernel se
    # compila aqui y se ejercita con un `msr_read` de mentira que define el
    # propio test.  La sustitucion es en el ENLACE -- no hay tabla de punteros a
    # funcion, ver common/msr.h --, que es la misma seleccion que usan los dos
    # lados de kernel.
    add_executable(test_pmu_caps
            tests/test_pmu_caps.c
            common/pmu_caps.c)
    target_link_libraries(test_pmu_caps PRIVATE vxp_base)
    target_include_directories(test_pmu_caps PRIVATE common)
    set_target_properties(test_pmu_caps PROPERTIES
            C_STANDARD 11 C_STANDARD_REQUIRED ON)
    if (MSVC)
        target_compile_options(test_pmu_caps PRIVATE /W4)
    else ()
        target_compile_options(test_pmu_caps PRIVATE -Wall -Wextra -pedantic-errors)
    endif ()
    add_test(NAME pmu_caps COMMAND test_pmu_caps)

    # La tabla de direcciones de MSR, que se GENERA del manual de Intel.
    #
    # Generarla quita los errores de tecleo y mete otro riesgo, peor porque es
    # silencioso: una regeneracion puede mover una direccion sin que nadie mire,
    # y eso no da un error -- da un perfilador que programa OTRO registro.  Las
    # que el codigo usa se fijan con su numero y se comprueban al compilar.
    add_executable(test_msr_index tests/test_msr_index.c)
    target_link_libraries(test_msr_index PRIVATE vxp_base)
    target_include_directories(test_msr_index PRIVATE common)
    set_target_properties(test_msr_index PROPERTIES
            C_STANDARD 11 C_STANDARD_REQUIRED ON)
    if (MSVC)
        target_compile_options(test_msr_index PRIVATE /W4)
    else ()
        target_compile_options(test_msr_index PRIVATE -Wall -Wextra -pedantic-errors)
    endif ()
    add_test(NAME msr_index COMMAND test_msr_index)

    # El volcado: recorre las tablas generadas y dice que vale cada campo en
    # ESTA maquina.
    #
    # Las cuatro tablas son unidades de traduccion aparte a proposito, y aqui se
    # ve para que: este objetivo las enlaza y el driver no, asi que los sesenta
    # kilobytes de nombres no llegan al kernel por el mero hecho de estar en el
    # arbol.
    set(VXP_TABLES
            common/cpuid/intel/table.c
            common/cpuid/amd/table.c
            common/msr/intel/table.c
            common/msr/amd/table.c)

    add_executable(test_dump
            tests/test_dump.c
            common/dump.c
            common/cpu_tables.c
            ${VXP_TABLES})
    target_link_libraries(test_dump PRIVATE vxp_base)
    target_include_directories(test_dump PRIVATE common)
    set_target_properties(test_dump PROPERTIES
            C_STANDARD 11 C_STANDARD_REQUIRED ON)
    if (MSVC)
        target_compile_options(test_dump PRIVATE /W4)
    else ()
        target_compile_options(test_dump PRIVATE -Wall -Wextra -pedantic-errors)
    endif ()
    add_test(NAME dump COMMAND test_dump)

    # Y el volcado como binario, que es para lo que sirve: pegarlo en un informe
    # de fallo o comparar dos maquinas.  Sin privilegios, asi que los MSR salen
    # como catalogo -- nombre, direccion y puerta -- y sin valor.
    add_executable(vxp_dump
            cli/dump_main.c
            common/dump.c
            common/cpu_tables.c
            ${VXP_TABLES})
    target_link_libraries(vxp_dump PRIVATE vxp_base)
    target_include_directories(vxp_dump PRIVATE common)
    set_target_properties(vxp_dump PROPERTIES
            C_STANDARD 11 C_STANDARD_REQUIRED ON)
    if (MSVC)
        target_compile_options(vxp_dump PRIVATE /W4)
    else ()
        target_compile_options(vxp_dump PRIVATE -Wall -Wextra -pedantic-errors)
    endif ()

    if (WIN32)
        # La disposicion de las estructuras del kernel que declaramos nosotros.
        # Se compila como C, que es como se va a usar `nt.h`.
        add_executable(test_nt_layout tests/test_nt_layout.c)
        target_link_libraries(test_nt_layout PRIVATE vxp_base)
        target_include_directories(test_nt_layout PRIVATE windows)
        set_target_properties(test_nt_layout PROPERTIES
                C_STANDARD 11 C_STANDARD_REQUIRED ON)
        if (NOT MSVC)
            target_compile_options(test_nt_layout PRIVATE -Wall -Wextra)
        endif ()
        add_test(NAME nt_layout COMMAND test_nt_layout)

        # Y la otra mitad: la MISMA tabla contra una cabecera que no escribimos
        # nosotros.  Sin esto, un error al teclear un numero pasaria
        # desapercibido -- la tabla y la estructura estarian mal igual y
        # coincidirian.
        #
        # La referencia son las cabeceras `ddk/` de MinGW-w64, y es OPCIONAL: el
        # driver no las necesita para nada.  Si no estan, este objetivo no se
        # construye y se dice, para que nadie crea que se comprobo.
        find_path(VXP_DDK_REFERENCE
                NAMES ddk/ntddk.h
                PATHS
                "${CMAKE_C_COMPILER}/../../x86_64-w64-mingw32/include"
                "C:/TDM-GCC-64/x86_64-w64-mingw32/include"
                "C:/mingw64/x86_64-w64-mingw32/include"
                NO_DEFAULT_PATH)
        if (VXP_DDK_REFERENCE)
            add_executable(test_nt_layout_ref tests/test_nt_layout_ref.c)
            target_include_directories(test_nt_layout_ref PRIVATE
                    windows "${VXP_DDK_REFERENCE}" "${VXP_DDK_REFERENCE}/ddk")
            set_target_properties(test_nt_layout_ref PROPERTIES
                    C_STANDARD 11 C_STANDARD_REQUIRED ON)
            add_test(NAME nt_layout_ref COMMAND test_nt_layout_ref)
            message(STATUS "profiler: hay cabeceras de referencia; la tabla de disposicion se contrasta")
        else ()
            message(STATUS "profiler: sin cabeceras de referencia -- la tabla de disposicion NO se contrasta contra nada externo")
        endif ()
    endif ()
endif ()

# -----------------------------------------------------------------------------
#  Instalacion (solo cuando se construye suelto).
# -----------------------------------------------------------------------------
if (VXP_STANDALONE)
    install(DIRECTORY include/ DESTINATION include
            FILES_MATCHING PATTERN "*.h")
endif ()
