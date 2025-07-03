# ============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : fw_ui.pri
# Author      : Thierry Lechevallier
# Date        : 03/06/2020
#
# ============================================================================

### FRAMEWORK -------------------------

isEmpty(BASEDIR_FW_UI) {
    BASEDIR_FW_UI = ext/fw_ui/dev/src
}

QT       += core gui quick qml

SOURCES += $$PWD/coreUi/CoreUiQml.cpp  \
           $$PWD/window/WinLog.cpp

HEADERS += $$PWD/coreUi/CoreUiQml.h  \
           $$PWD/window/WinLog.h

INCLUDEPATH += $${BASEDIR_FW_UI}

FORMS += \
           $$PWD/window/WinLog.ui
