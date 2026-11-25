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

QT       += core gui quick qml xml

SOURCES += $$PWD/coreUi/CoreUiQml.cpp

HEADERS += $$PWD/coreUi/CoreUiQml.h

INCLUDEPATH += $${BASEDIR_FW_UI}
