# ============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : t_core_log.pro
# Author      : Thierry Lechevallier
# Date        : 13/05/2020
#
# ============================================================================

QT -= gui

CONFIG += c++11
CONFIG -= app_bundle


DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        TestCore.cpp \
        main.cpp

HEADERS += TestCore.h

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

### FRAMEWORK -------------------------
BASEDIR_FW_COMMON = ../../dev/src
include($$BASEDIR_FW_COMMON/fw_common.pri)

