@echo off
setlocal
title Compilazione MSHV 2.76.7 + Doppler tracking (IC8TEM)
cd /d "%~dp0"

rem ======================================================================
rem  Compila MSHV per Windows 64 bit SENZA Qt preinstallato.
rem  La prima volta scarica e installa MSYS2 (compilatore + Qt5) in C:\msys64
rem  (circa 1,5 GB, 15-30 minuti). Le volte successive compila e basta.
rem  Risultato: cartella  bin\  con MSHV_WIN64.exe e le DLL necessarie.
rem ======================================================================

set "MSYSDIR=C:\msys64"
set "BASH=%MSYSDIR%\usr\bin\bash.exe"

if exist "%BASH%" goto have_msys
echo.
echo [1/4] MSYS2 non trovato: lo scarico da GitHub ...
powershell -NoProfile -ExecutionPolicy Bypass -Command "[Net.ServicePointManager]::SecurityProtocol='Tls12'; Invoke-WebRequest -UseBasicParsing -Uri 'https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe' -OutFile \"$env:TEMP\msys2-base.sfx.exe\""
if errorlevel 1 goto err
echo        Estraggo MSYS2 in C:\msys64 ...
"%TEMP%\msys2-base.sfx.exe" -y -oC:\
if not exist "%BASH%" goto err
"%BASH%" -lc "exit"
echo        Aggiorno MSYS2 (due passaggi) ...
"%BASH%" -lc "pacman -Syu --noconfirm"
"%BASH%" -lc "pacman -Syu --noconfirm"

:have_msys
set MSYSTEM=MINGW64
set CHERE_INVOKING=1
echo.
echo [2/4] Installo compilatore e Qt5 (solo se mancano) ...
"%BASH%" -lc "pacman -S --needed --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-qt5-base mingw-w64-x86_64-qt5-activeqt mingw-w64-x86_64-qt5-websockets mingw-w64-x86_64-qt5-tools"
if errorlevel 1 goto err

echo.
echo [3/4] Compilo MSHV ...
"%BASH%" -lc "./build_msys.sh"
if errorlevel 1 goto err

echo.
echo ==================  FATTO  ==================
echo  Avvia:  %~dp0bin\MSHV_WIN64.exe
echo =============================================
pause
exit /b 0

:err
echo.
echo *** ERRORE: compilazione non riuscita. Copia le ultime righe sopra e mandamele. ***
pause
exit /b 1
