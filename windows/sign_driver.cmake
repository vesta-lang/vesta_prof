# =============================================================================
#  Firmar el driver, en tiempo de CONSTRUCCION.
# =============================================================================
#
# Se invoca con `cmake -P`, no desde el CMakeLists, y hay un motivo: la
# contrasena del `.pfx` esta en un fichero, y leerla al CONFIGURAR la dejaria
# escrita en los ficheros generados del directorio de construccion.  Leyendola
# aqui, en cada firma, no sale del sitio donde ya estaba.
#
# Uso:
#   cmake -DSIGNTOOL=... -DPFX=... -DPASS_FILE=... -DTARGET=... -P sign_driver.cmake
#
# Un driver sin firmar no carga.  Por eso esto no es un extra: es la diferencia
# entre un `.sys` y un fichero con extension `.sys`.

if (NOT SIGNTOOL OR NOT PFX OR NOT PASS_FILE OR NOT TARGET)
    message(FATAL_ERROR "sign_driver: faltan SIGNTOOL, PFX, PASS_FILE o TARGET")
endif ()

if (NOT EXISTS "${PASS_FILE}")
    message(FATAL_ERROR "sign_driver: no encuentro ${PASS_FILE}")
endif ()

file(READ "${PASS_FILE}" VXP_PFX_PASS)
string(STRIP "${VXP_PFX_PASS}" VXP_PFX_PASS)

execute_process(
        COMMAND "${SIGNTOOL}" sign /fd SHA256 /f "${PFX}" /p "${VXP_PFX_PASS}"
                "${TARGET}"
        RESULT_VARIABLE VXP_SIGN_RC
        OUTPUT_VARIABLE VXP_SIGN_OUT
        ERROR_VARIABLE VXP_SIGN_ERR)

if (NOT VXP_SIGN_RC EQUAL 0)
    # Se GRITA.  Un driver que se construye y no se firma tiene el mismo aspecto
    # que uno firmado hasta que alguien intenta cargarlo, y entonces el error
    # habla de la firma y no de la construccion, que es donde estaba el fallo.
    message(FATAL_ERROR
            "sign_driver: la firma fallo (${VXP_SIGN_RC})\n"
            "${VXP_SIGN_OUT}\n${VXP_SIGN_ERR}")
endif ()

message(STATUS "firmado: ${TARGET}")
