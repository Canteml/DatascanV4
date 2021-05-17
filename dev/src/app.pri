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

QT       += core gui quick qml widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += $$files($$PWD/*.cpp)

HEADERS +=  $$files($$PWD/*.h)


FORMS += $$files($$PWD/ui/*.ui)

INCLUDEPATH += $$PWD

TRANSLATIONS += $$PWD/res/languages/App_en_GB.ts

RESOURCES += $$PWD/qml/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


