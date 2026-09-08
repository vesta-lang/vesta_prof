@echo off
rem ===========================================================================
rem  Devuelve la maquina a su estado normal: vuelve a exigir firma de
rem  produccion y deja de confiar el certificado de desarrollo.
rem
rem  Existe porque un guion de montaje sin su desmontaje es como se pudren las
rem  maquinas: se prueba algo una tarde y el equipo se queda para siempre
rem  admitiendo drivers sin firmar, sin que nadie recuerde por que.
rem
rem  El material de firma NO se borra por defecto.  Borrarlo obliga a reinstalar
rem  el certificado en todas partes la proxima vez, asi que se pide a proposito:
rem
rem      make_dev_cert.py --uninstall --purge
rem ===========================================================================
setlocal

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Hacen falta permisos de administrador.  Pidiendolos...
    powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b 0
)

where py >nul 2>&1
if %errorlevel% equ 0 (
    set "PY=py -3"
) else (
    set "PY=python"
)

%PY% "%~dp0make_dev_cert.py" --uninstall
set "RC=%errorlevel%"

echo.
if "%RC%"=="0" (
    echo ---------------------------------------------------------------
    echo  Falta REINICIAR para volver a exigir la firma de produccion.
    echo ---------------------------------------------------------------
)

echo.
pause
exit /b %RC%
