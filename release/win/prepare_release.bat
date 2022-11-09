@echo off
cls
echo.

rem variables to set function to user installation
SET PATH_QT=C:\Qt\5.15.2
SET APPLICATION_NAME=DatascanV2

echo Variables :
echo PATH_QT = %PATH_QT%
echo APPLICATION_NAME = %APPLICATION_NAME%
echo.

rem set environnment
if not exist %PATH_QT%\mingw81_32\bin (
	echo Dossier Qt introuvable : %PATH_QT%\mingw81_32\bin
	goto :lbEnd
)
call %PATH_QT%\mingw81_32\bin\qtenv2.bat
 
rem remove all files
rmdir /Q /S "%~dp0\app"
rmdir /Q /S "%~dp0\tmp"

rem make dir
mkdir "%~dp0\app"
mkdir "%~dp0\app\bin"

rem copy binary
if not exist "%~dp0\..\..\dev\build\release\%APPLICATION_NAME%.exe" (
	echo Fichier introuvable : "%~dp0\..\..\dev\build\release\%APPLICATION_NAME%.exe"
	goto :lbEnd
)
xcopy "%~dp0\..\..\dev\build\release\%APPLICATION_NAME%.exe" "%~dp0\app\bin"

rem copy libraries
rem JKQTPLOTTER :
xcopy "%~dp0\..\..\dev\lib\jkqtplotter\bin\jkqtplotterlib.dll" "%~dp0\app\bin"
xcopy "%~dp0\..\..\dev\lib\jkqtplotter\bin\jkqtpcommonlib.dll" "%~dp0\app\bin"
rem SDL2 :
xcopy "%~dp0\..\..\dev\lib\SDL2\bin\SDL2.dll" "%~dp0\app\bin"

xcopy "%~dp0\..\..\dev\lib\vlcqt\bin\libvlc.dll" "%~dp0\app\bin"
xcopy "%~dp0\..\..\dev\lib\vlcqt\bin\libvlccore.dll" "%~dp0\app\bin"
xcopy "%~dp0\..\..\dev\lib\vlcqt\bin\libVLCQtCore.dll" "%~dp0\app\bin"
xcopy "%~dp0\..\..\dev\lib\vlcqt\bin\libVLCQtQml.dll" "%~dp0\app\bin"
xcopy "%~dp0\..\..\dev\lib\vlcqt\bin\libVLCQtWidgets.dll" "%~dp0\app\bin"

rem copy EquipmentFile
xcopy "%~dp0\EquipmentFile.txt" "%~dp0\app\bin"

rem build depencendies
call %PATH_QT%\mingw81_32\bin\windeployqt.exe --qmldir "%~dp0\..\..\dev\src\qml"  "%~dp0\app\bin\%APPLICATION_NAME%.exe" --opengl --printsupport
rem copy app resources
xcopy /E /H /C /R /Q /Y "%~dp0\..\..\dev\src\res" "%~dp0\app\res\"
xcopy /E /H /C /R /Q /Y /S "%~dp0\..\..\dev\src\qml" "%~dp0\app\res\qml\"

rem prepare tmp
mkdir "%~dp0\tmp"

rem copy extra resources to tmp
xcopy "%~dp0\..\..\res" "%~dp0\tmp" /E
xcopy "%~dp0\res" "%~dp0\tmp" /E

echo.
COLOR 02
echo OK
echo.
pause
exit

:lbEnd
echo.
COLOR 04
echo ERREUR :
echo Verifier les variables au debut du script
echo.
pause

