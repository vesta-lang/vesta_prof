@echo off
rem ===========================================================================
rem  Deja esta maquina lista para cargar el driver del perfilador en desarrollo.
rem
rem  Se hace doble clic y ya.  Lo unico que pide es el permiso de administrador,
rem  porque los dos pasos de fondo -- confiar un certificado propio y permitir
rem  drivers de prueba -- cambian la politica del EQUIPO, no la del proyecto.
rem
rem  Aqui no hay logica: solo eleva y llama a la herramienta.  Escribir el
rem  montaje en batch seria mantener una segunda version de algo que ya esta
rem  escrito, y las dos se separarian.
rem
rem  Para deshacerlo: dev_teardown.cmd
rem ===========================================================================
setlocal

rem Elevar si no lo estamos ya.  `net session` falla sin administrador y es la
rem comprobacion que funciona en todas las versiones; ir por otras vias da
rem falsos negativos segun la configuracion del equipo.
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Hacen falta permisos de administrador.  Pidiendolos...
    powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b 0
)

rem Python: primero el lanzador `py`, que es lo que instala Python en Windows;
rem si no esta, `python` a secas.
where py >nul 2>&1
if %errorlevel% equ 0 (
    set "PY=py -3"
) else (
    set "PY=python"
)

%PY% "%~dp0make_dev_cert.py" --install
set "RC=%errorlevel%"

echo.
if "%RC%"=="0" (
    echo ---------------------------------------------------------------
    echo  Falta REINICIAR para que el modo de pruebas entre en vigor.
    echo ---------------------------------------------------------------
) else (
    echo Algo no salio bien; mira las lineas de arriba.
)

rem Sin esto la ventana se cierra sola al terminar y nadie llega a leer nada,
rem que es justo lo que pasa cuando se lanza con doble clic.
echo.
pause
exit /b %RC%
