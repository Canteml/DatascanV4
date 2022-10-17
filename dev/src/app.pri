# ============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : app.pri
# Author      : Thierry Lechevallier
# Date        : 13/05/2020
#
# ============================================================================

QT       += core gui quick qml widgets serialport network printsupport multimedia opengl

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# -- SOURCES
# MAIN
SOURCES += $$files($$PWD/*.cpp)
HEADERS += $$files($$PWD/*.h)

SOURCES += $$files($$PWD/scan/*.cpp)
HEADERS += $$files($$PWD/scan/*.h)

SOURCES += $$files($$PWD/control/*.cpp)
HEADERS += $$files($$PWD/control/*.h)

SOURCES += $$files($$PWD/camera/*.cpp)
HEADERS += $$files($$PWD/camera/*.h)

SOURCES += $$files($$PWD/exam/*.cpp)
HEADERS += $$files($$PWD/exam/*.h)

SOURCES += $$files($$PWD/mep/*.cpp)
HEADERS += $$files($$PWD/mep/*.h)

SOURCES += $$files($$PWD/adapter/*.cpp)
HEADERS += $$files($$PWD/adapter/*.h)

# JK PLOTTER COMPONENTS
SOURCES += $$files($$PWD/jkPlot/*.cpp)
HEADERS += $$files($$PWD/jkPlot/*.h)

## -- PATHS
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../ext/fw_common/dev/src/v2/com
INCLUDEPATH += $$PWD/../ext/fw_common/dev/src/v2/vector

## -- RESOURCES
TRANSLATIONS += $$PWD/res/languages/Datascan_en_GB.ts
RESOURCES += $$PWD/qml/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD/qml
QML_IMPORT_PATH += $$PWD/qml/imports

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Modules of Frawework common :
#USE_FW_COMMON_SCAN = yes
USE_FW_COMMON_V2 = yes

# QML Registering  :
CONFIG += qmltypes
QML_IMPORT_NAME = roboplanet.v2
QML_IMPORT_MAJOR_VERSION = 1
INCLUDEPATH += ext/fw_common/dev/src/v2/vector
INCLUDEPATH += $$PWD/mep

## -- LIBRARIES

## JKQTPLOTTER

INCLUDEPATH += lib\jkqtplotter\include
LIBS += -Llib/jkqtplotter/bin
LIBS += -ljkqtplotterlib -ljkqtpcommonlib

## SDL2
LIBS += -Llib/SDL2/bin
INCLUDEPATH += lib/SDL2/include

LIBS += -lSDL2
DEFINES += SDL_MAIN_HANDLED

## QXlsx : The MIT License - Copyright 2017 - https://github.com/j2doll/QXlsx
LIBS += -Llib/QXlsx/bin
INCLUDEPATH += lib/QXlsx/include

CONFIG(debug, debug|release) {
    LIBS += -lQXlsxd
} else {
    LIBS += -lQXlsx
}

LIBS += -Llib/vlcqt/bin

CONFIG(debug, debug|release) {
    LIBS += -llibVLCQtCored -llibVLCQtQmld -llibVLCQtWidgetsd
} else {
    LIBS += -llibVLCQtCore -llibVLCQtQml -llibVLCQtWidgets
}

INCLUDEPATH += lib/vlcqt/include
