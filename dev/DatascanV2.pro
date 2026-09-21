# ============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : APP.pro
# Author      : Thierry Lechevallier
# Date        : 13/05/2020
#
# ============================================================================
## -- VERSION
DEF_VERSION = rev_2.0.4-FR
//DEF_VERSION = rev_2.0.4-ENG
//DEF_VERSION = rev_2.0.4-ES
//DEF_VERSION = rev_2.0.4-EDF-LIMULE-BUGGY
CONFIG(debug, debug|release) {
    DEF_VERSION = $$DEF_VERSION"-DEV"
}
VERSION_STR = $$replace(DEF_VERSION, -, _)
DEFINES += "APP_VERSION=\\\"$$VERSION_STR\\\""

## -- DIRECTORIES
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR     = $$DESTDIR/moc
RCC_DIR     = $$DESTDIR/qrc
UI_DIR      = $$DESTDIR/ui
QMLCACHE_DIR = $$DESTDIR/qml

## -- APPLICATION
include(src/app.pri)

## -- FRAMEWORK
## -- FRAMEWORK
BASEDIR_FW_COMMON = $$PWD/ext/fw_common/dev/src
include($$BASEDIR_FW_COMMON/fw_common.pri)
BASEDIR_FW_UI = $$PWD/ext/fw_ui/dev/src
include($$BASEDIR_FW_UI/fw_ui.pri)

## -- COPY FILES
CONFIG += file_copies
COPIES += CopyRes CopySDL2 CopyJKQT CopyVLCQt CopyVLCQtBin

## Copy resources
CopyRes.files = $$PWD/src/res
CopyRes.path  = build

## Copy SDL2
CopySDL2.files = $$PWD/lib/SDL2/bin/SDL2.dll
CopySDL2.path  = $$DESTDIR

## Copy JKQTPlotter DLLs
CONFIG(debug, debug|release) {
    CopyJKQT.files = \
        $$PWD/lib/jkqtplotter/bin/libJKQTCommon6_Debug.dll \
        $$PWD/lib/jkqtplotter/bin/libJKQTMath6_Debug.dll \
        $$PWD/lib/jkqtplotter/bin/libJKQTMathText6_Debug.dll \
        $$PWD/lib/jkqtplotter/bin/libJKQTPlotter6_Debug.dll
} else {
    CopyJKQT.files = \
        $$PWD/lib/jkqtplotter/bin/libJKQTCommon6_Release.dll \
        $$PWD/lib/jkqtplotter/bin/libJKQTMath6_Release.dll \
        $$PWD/lib/jkqtplotter/bin/libJKQTMathText6_Release.dll \
        $$PWD/lib/jkqtplotter/bin/libJKQTPlotter6_Release.dll
}
CopyJKQT.path = $$DESTDIR

## Copy VLCQt DLLs
CopyVLCPlugins.files = $$PWD/lib/vlcqt/bin/plugins
CopyVLCPlugins.path  = $$DESTDIR
COPIES += CopyVLCPlugins
CONFIG(debug, debug|release) {
    CopyVLCQt.files = \
        $$PWD/lib/vlcqt/bin/libVLCQtCored.dll \
        $$PWD/lib/vlcqt/bin/libVLCQtWidgetsd.dll
} else {
    CopyVLCQt.files = \
        $$PWD/lib/vlcqt/bin/libVLCQtCore.dll \
        $$PWD/lib/vlcqt/bin/libVLCQtWidgets.dll
}
CopyVLCQt.path = $$DESTDIR

## Copy libvlc native DLLs
CopyVLCQtBin.files = \
    $$PWD/lib/vlcqt/bin/libvlc.dll \
    $$PWD/lib/vlcqt/bin/libvlccore.dll
CopyVLCQtBin.path = $$DESTDIR

## Copy Qt Multimedia plugins
COPIES += CopyMultimedia
CopyMultimedia.files = \
    $$[QT_INSTALL_PLUGINS]/multimedia/windowsmediaplugin.dll \
    $$[QT_INSTALL_PLUGINS]/multimedia/ffmpegmediaplugin.dll
CopyMultimedia.path = $$DESTDIR/plugins/multimedia

## Copy Qt e MinGW DLLs
win32 {
    QT_BIN = $$[QT_INSTALL_BINS]
    MINGW_BIN = $$[QT_INSTALL_PREFIX]/../../Tools/mingw1310_64/bin

    CopyQtDlls.files = \
        $$QT_BIN/Qt6Core.dll \
        $$QT_BIN/Qt6Gui.dll \
        $$QT_BIN/Qt6Multimedia.dll \
        $$QT_BIN/Qt6Network.dll \
        $$QT_BIN/Qt6OpenGL.dll \
        $$QT_BIN/Qt6PrintSupport.dll \
        $$QT_BIN/Qt6Qml.dll \
        $$QT_BIN/Qt6QmlMeta.dll \
        $$QT_BIN/Qt6QmlModels.dll \
        $$QT_BIN/Qt6QmlWorkerScript.dll \
        $$QT_BIN/Qt6Quick.dll \
        $$QT_BIN/Qt6SerialPort.dll \
        $$QT_BIN/Qt6Svg.dll \
        $$QT_BIN/Qt6Widgets.dll \
        $$QT_BIN/Qt6Xml.dll \
        $$QT_BIN/avcodec-61.dll \
        $$QT_BIN/avformat-61.dll \
        $$QT_BIN/avutil-59.dll \
        $$QT_BIN/swresample-5.dll \
        $$QT_BIN/swscale-8.dll \
        $$MINGW_BIN/libgcc_s_seh-1.dll \
        $$MINGW_BIN/libstdc++-6.dll \
        $$MINGW_BIN/libwinpthread-1.dll
    CopyQtDlls.path = $$DESTDIR
    COPIES += CopyQtDlls

    CopyQmlModules.files = $$[QT_INSTALL_QML]
    CopyQmlModules.path  = $$DESTDIR
    COPIES += CopyQmlModules

    CopyQtPlugins.files = \
        $$[QT_INSTALL_PLUGINS]/platforms \
        $$[QT_INSTALL_PLUGINS]/imageformats \
        $$[QT_INSTALL_PLUGINS]/styles \
        $$[QT_INSTALL_PLUGINS]/tls \
        $$[QT_INSTALL_PLUGINS]/multimedia
    CopyQtPlugins.path = $$DESTDIR
    COPIES += CopyQtPlugins
}

## -- ICON
RC_ICONS = $$PWD/src/res/img/application.ico

## -- COMPILER
win32 {
    MINGW_DIR = $$[QT_INSTALL_PREFIX]/../../Tools/mingw1310_64/bin

    QMAKE_CC   = $$MINGW_DIR/gcc.exe
    QMAKE_CXX  = $$MINGW_DIR/g++.exe
    QMAKE_LINK = $$MINGW_DIR/g++.exe

    QMAKE_CXXFLAGS_WARN_ON -= -Werror
    QMAKE_CXXFLAGS -= -Werror
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -Wno-error=deprecated-declarations
    QMAKE_SHELL = powershell.exe
}

SOURCES +=
RESOURCES += \
    translations.qrc
DISTFILES += \
    ext/fw_common/dev/ext/.gitignore

QMAKE_CXXFLAGS += -Wno-error -Wno-deprecated-declarations

win32 {
    QMAKE_CXXFLAGS_WARN_ON =
    QMAKE_CXXFLAGS -= -Wall -Wextra
    INCLUDEPATH -= /include
    QMAKE_CXXFLAGS += -pipe
    MINGW_DIR = $$[QT_INSTALL_PREFIX]/../../Tools/mingw1310_64/bin

    QMAKE_CC   = $$MINGW_DIR/gcc.exe
    QMAKE_CXX  = $$MINGW_DIR/g++.exe
    QMAKE_LINK = $$MINGW_DIR/g++.exe

    QML_IMPORT_TRACE = 1
    QT_LOGGING_RULES = qml.debug=true
}

