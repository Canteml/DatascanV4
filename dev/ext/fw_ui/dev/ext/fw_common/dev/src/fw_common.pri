# ============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : fw_common.pri
# Author      : Thierry Lechevallier
# Date        : 13/05/2020
#
# ============================================================================

### FRAMEWORK -------------------------

QT += xml

isEmpty(BASEDIR_FW_COMMON) {
    BASEDIR_FW_COMMON = ext/fw_common/dev/src
}

SOURCES += $${BASEDIR_FW_COMMON}/core/Core.cpp \
           $$PWD/log/Loggable.cpp \
           $$PWD/log/Logger.cpp \
           $$PWD/xml/XmlSettings.cpp \
           $$PWD/xml/XmlProject.cpp \
           $$PWD/xml/XmlHelper.cpp

HEADERS += $${BASEDIR_FW_COMMON}/core/Core.h \
           $$PWD/log/Loggable.h \
           $$PWD/log/Logger.h \
           $$PWD/xml/XmlSettings.h \
           $$PWD/xml/XmlProject.h \
           $$PWD/xml/XmlHelper.h

INCLUDEPATH += $${BASEDIR_FW_COMMON}
