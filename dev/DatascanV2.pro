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

//DEF_VERSION = 1.0.4-ENG
DEF_VERSION = 1.0.6-BETA-FR
CONFIG(debug, debug|release) {
    DEF_VERSION = $$DEF_VERSION"-DEV"
}

DEFINES += APP_VERSION=\\\"$$DEF_VERSION\\\"

## -- DIRECTORIES

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
QMLCACHE_DIR = $$DESTDIR/.qml

## -- APPLICATION
include(src/app.pri)

## -- FRAMEWORK
BASEDIR_FW_COMMON = ext/fw_common/dev/src
include($$BASEDIR_FW_COMMON/fw_common.pri)

BASEDIR_FW_UI = ext/fw_ui/dev/src
include($$BASEDIR_FW_UI/fw_ui.pri)

## -- COPY FILES

CONFIG += file_copies
COPIES += CopyRes CopySDL2

## Copy resources
CopyRes.files = $$PWD/src/res
CopyRes.path = build

# Copy lib SDL2
CopySDL2.files = $$PWD/lib/SDL2/bin/SDL2.dll
CopySDL2.path = $$DESTDIR

## -- ICON
RC_ICONS = $$PWD/src/res/img/application.ico

SOURCES +=

RESOURCES += \
    translations.qrc

DISTFILES += \
   ext/fw_common/dev/ext/.gitignore
