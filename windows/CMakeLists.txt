# =============================================================================
#  El `.sys`: el driver de Windows del perfilador.
# =============================================================================
#
# POR QUE ESTO ES UN DIRECTORIO APARTE Y NO UNAS LINEAS EN EL CMakeLists RAIZ.
# No es orden: es que un driver necesita que NO se le anadan las bibliotecas
# estandar que CMake pone por defecto en cada enlace (`-lkernel32 -luser32 ...`),
# y `CMAKE_C_STANDARD_LIBRARIES` es de ambito de DIRECTORIO.  Vaciarla en la
# raiz afectaria a las pruebas; vaciarla aqui afecta solo al driver.
#
# Un driver que importara de `kernel32` no fallaria al construirse.  Fallaria al
# cargarse, con un error que habla de un modulo que no se encuentra y no de la
# linea de enlace, que es donde estaba el problema.
#
# SIN WDK.  Las declaraciones estan en `nt.h`, la biblioteca de importacion sale
# de `ntoskrnl.def` con `dlltool`, y el resto son banderas.  Ver windows/README.md.

# Aqui, y solo aqui, el enlace va desnudo.
set(CMAKE_C_STANDARD_LIBRARIES "")

# -----------------------------------------------------------------------------
#  La biblioteca de importacion, generada de NUESTRO `.def`.
# -----------------------------------------------------------------------------
find_program(VXP_DLLTOOL
        NAMES dlltool x86_64-w64-mingw32-dlltool llvm-dlltool
        HINTS "${VXP_TOOLCHAIN_BIN}")

if (NOT VXP_DLLTOOL)
    # No se calla: sin esto no hay driver, y hay que saber por que.
    message(STATUS "profiler: sin `dlltool` -- NO se construye el .sys")
    return ()
endif ()

set(VXP_NTOSKRNL_IMPORT "${CMAKE_CURRENT_BINARY_DIR}/libntoskrnl.a")
add_custom_command(
        OUTPUT "${VXP_NTOSKRNL_IMPORT}"
        COMMAND "${VXP_DLLTOOL}"
                -d "${CMAKE_CURRENT_SOURCE_DIR}/ntoskrnl.def"
                -l "${VXP_NTOSKRNL_IMPORT}"
                --dllname ntoskrnl.exe
                -m i386:x86-64
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ntoskrnl.def"
        COMMENT "profiler: biblioteca de importacion de ntoskrnl (sin WDK)"
        VERBATIM)
add_custom_target(vxp_ntoskrnl_import DEPENDS "${VXP_NTOSKRNL_IMPORT}")

# -----------------------------------------------------------------------------
#  El driver.
# -----------------------------------------------------------------------------
#
# `common/pmu_caps.c` se compila AQUI DENTRO y no se enlaza desde una biblioteca
# compartida con las pruebas, y es a proposito: el mismo fuente necesita otras
# banderas dentro del kernel (`-ffreestanding`, `-mno-red-zone`) que en espacio
# de usuario.  Un artefacto compilado compartido entre los dos seria mentira --
# uno de los dos lados estaria usando codigo generado para el otro.
add_library(vesta_prof MODULE
        driver.c
        msr_win.c
        ../common/pmu_caps.c)

add_dependencies(vesta_prof vxp_ntoskrnl_import)

set_target_properties(vesta_prof PROPERTIES
        PREFIX ""
        SUFFIX ".sys"
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        LINKER_LANGUAGE C)

target_include_directories(vesta_prof PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_compile_options(vesta_prof PRIVATE
        # Sin biblioteca estandar ni suposiciones sobre el entorno.
        -ffreestanding
        # La zona roja NO existe en el kernel: una interrupcion puede escribir
        # justo debajo del puntero de pila en cualquier momento.  Es el fallo
        # clasico de un driver que "casi siempre funciona".
        -mno-red-zone
        # El comprobador de pila llama a funciones del CRT que aqui no hay.
        -fno-stack-protector
        -fno-asynchronous-unwind-tables
        -Wall -Wextra)

target_link_options(vesta_prof PRIVATE
        -nostdlib
        # Un driver es un binario NATIVO: no hay subsistema de Windows debajo.
        "-Wl,--subsystem,native"
        "-Wl,--entry,DriverEntry"
        "-Wl,--image-base,0x140000000"
        "-Wl,--dynamicbase"
        "-Wl,--nxcompat")

target_link_libraries(vesta_prof PRIVATE "${VXP_NTOSKRNL_IMPORT}")

# LTO: se deja el del proyecto, y NO es un descuido.  Comprobado sobre este
# mismo objetivo -- el binario sale con `Subsystem 1 (NT native)`, `DriverEntry`
# conservado y las mismas 10 importaciones, y ademas mas pequeno (19.794 frente
# a 21.640 bytes).  La duda razonable era que el LTO se llevara por delante un
# punto de entrada al que no llama nadie; no ocurre, porque es el simbolo de
# entrada del enlace.

# -----------------------------------------------------------------------------
#  Firma.
# -----------------------------------------------------------------------------
#
# Un driver sin firmar NO CARGA.  Asi que firmar no es un remate opcional: sin
# esto, lo que sale del build no sirve para nada mas que mirarlo.
#
# Se firma con el certificado de desarrollo del repositorio, si esta.  Si no
# esta, se dice -- y se dice tambien COMO generarlo, porque el error de "no
# carga" que aparece dos pasos despues no lo explica.
find_program(VXP_SIGNTOOL
        NAMES signtool
        PATHS "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin"
        PATH_SUFFIXES x64
        DOC "signtool.exe del SDK de Windows")

set(VXP_DEV_PFX "${CMAKE_CURRENT_SOURCE_DIR}/../.devcert/dev_cert.pfx")
set(VXP_DEV_PASS "${VXP_DEV_PFX}.pass")

if (VXP_SIGNTOOL AND EXISTS "${VXP_DEV_PFX}" AND EXISTS "${VXP_DEV_PASS}")
    add_custom_command(TARGET vesta_prof POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
                    "-DSIGNTOOL=${VXP_SIGNTOOL}"
                    "-DPFX=${VXP_DEV_PFX}"
                    "-DPASS_FILE=${VXP_DEV_PASS}"
                    "-DTARGET=$<TARGET_FILE:vesta_prof>"
                    -P "${CMAKE_CURRENT_SOURCE_DIR}/sign_driver.cmake"
            COMMENT "profiler: firmando el driver con el certificado de desarrollo"
            VERBATIM)
    message(STATUS "profiler: el .sys se firmara con .devcert/dev_cert.pfx")
elseif (NOT VXP_SIGNTOOL)
    message(STATUS "profiler: sin `signtool` -- el .sys se construye SIN FIRMAR y no cargara")
else ()
    message(STATUS "profiler: sin certificado de desarrollo -- el .sys se construye SIN FIRMAR.\n"
            "  Generalo con: python tools/make_dev_cert.py")
endif ()
