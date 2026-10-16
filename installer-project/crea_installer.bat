@echo off
rem Crea l'installer di DatascanV3: compila la release, prepara i file e lancia Qt IFW.
rem Doppio clic per avviarlo. Il setup finale viene scritto in installer-project\Output.

setlocal
cd /d "%~dp0"

for /f "tokens=3 delims=<>" %%v in ('findstr /c:"<Version>" config\config.xml') do set CUR_VERSION=%%v

echo.
echo  ===== Creazione installer DatascanV3 =====
echo.
echo  Versione attuale: %CUR_VERSION%
set NEW_VERSION=
set /p NEW_VERSION= Nuova versione (Invio per lasciare %CUR_VERSION%):
echo.

if "%NEW_VERSION%"=="" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_installer.ps1" -Build
) else (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_installer.ps1" -Build -Version %NEW_VERSION%
)

if errorlevel 1 (
    color 04
    echo.
    echo  ERRORE: installer NON creato, leggi i messaggi sopra.
    echo.
    pause
    exit /b 1
)

color 02
echo.
echo  Installer creato in: %~dp0Output
explorer "%~dp0Output"
pause
