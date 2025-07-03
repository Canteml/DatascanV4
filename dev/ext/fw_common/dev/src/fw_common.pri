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

INCLUDEPATH += $${BASEDIR_FW_COMMON}

## -- Standard sources

SOURCES += $${BASEDIR_FW_COMMON}/core/Core.cpp \
           $$PWD/log/Loggable.cpp \
           $$PWD/log/Logger.cpp \
           $$PWD/log/StatisticManager.cpp \
           $$PWD/xml/XmlSettings.cpp \
           $$PWD/xml/XmlProject.cpp \
           $$PWD/xml/XmlHelper.cpp \
           $$PWD/xml/XmlSettingsLoggable.cpp

HEADERS += $${BASEDIR_FW_COMMON}/core/Core.h \
           $$PWD/log/Loggable.h \
           $$PWD/log/Logger.h \
           $$PWD/log/StatisticManager.h \
           $$PWD/xml/XmlSettings.h \
           $$PWD/xml/XmlProject.h \
           $$PWD/xml/XmlHelper.h \
           $$PWD/xml/XmlSettingsLoggable.h

## -- SCAN sources

defined(USE_FW_COMMON_SCAN, var) {
    message("fw_common : use module scan.")
    SOURCES += $$files($$PWD/scan/*.cpp) \
               $$files($$PWD/scan/zone/*.cpp) \
               $$files($$PWD/scan/environment/*.cpp) \
               $$files($$PWD/scan/measure/*.cpp) \
               $$files($$PWD/scan/equipment/*.cpp) \
               $$files($$PWD/scan/probe/*.cpp) \
               $$files($$PWD/scan/report/*.cpp) \
               $$files($$PWD/scan/structure/*.cpp) \
               $$files($$PWD/scan/vector/*.cpp) \
               $$files($$PWD/scan/exam/*.cpp) \
               $$files($$PWD/scan/check/*.cpp) \
               $$files($$PWD/scan/type/*.cpp)

    HEADERS += $$files($$PWD/scan/*.h) \
               $$files($$PWD/scan/zone/*.h) \
               $$files($$PWD/scan/environment/*.h) \
               $$files($$PWD/scan/measure/*.h) \
               $$files($$PWD/scan/equipment/*.h) \
               $$files($$PWD/scan/probe/*.h) \
               $$files($$PWD/scan/report/*.h) \
               $$files($$PWD/scan/structure/*.h) \
               $$files($$PWD/scan/vector/*.h) \
               $$files($$PWD/scan/exam/*.h) \
               $$files($$PWD/scan/check/*.h) \
               $$files($$PWD/scan/type/*.h)
}

## -- V2 sources

defined(USE_FW_COMMON_V2, var) {
    message("fw_common : use module V2.")

    SOURCES += $$files($$PWD/v2/com/*.cpp) \
               $$files($$PWD/v2/prot/*.cpp) \
               $$files($$PWD/v2/vector/*.cpp) \
               $$files($$PWD/v2/scan/*.cpp) \
               $$files($$PWD/v2/exam/*.cpp) \
               $$files($$PWD/v2/camera/*.cpp)

    HEADERS += $$files($$PWD/v2/com/*.h) \
               $$files($$PWD/v2/prot/*.h) \
               $$files($$PWD/v2/vector/*.h) \
               $$files($$PWD/v2/scan/*.h) \
               $$files($$PWD/v2/exam/*.h) \
               $$files($$PWD/v2/camera/*.h)
}
