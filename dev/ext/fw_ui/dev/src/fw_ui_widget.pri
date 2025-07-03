# ============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : fw_ui_widget.pri
# Author      : Thierry Lechevallier
# Date        : 30/07/2020
#
# ============================================================================

### FRAMEWORK -------------------------

isEmpty(BASEDIR_FW_UI) {
    BASEDIR_FW_UI = ext/fw_ui/dev/src
}

QT       += core gui

SOURCES += $$PWD/coreUi/CoreUiWidget.cpp \
           $$PWD/window/GMainWindow.cpp

HEADERS += $$PWD/coreUi/CoreUiWidget.h \
           $$PWD/window/GMainWindow.h

INCLUDEPATH += $${BASEDIR_FW_UI}
