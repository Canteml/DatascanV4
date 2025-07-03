QT += quick

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        TestCoreUiQml.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

### -- COPY FILES

CONFIG += file_copies
# Add a variable that describes the copied files.
COPIES += CopyRes

CopyRes.files = $$PWD/res
CopyRes.path = build


### FRAMEWORK -------------------------
BASEDIR_FW_COMMON = ../../dev/ext/fw_common/dev/src
include($$BASEDIR_FW_COMMON/fw_common.pri)

BASEDIR_FW_UI = ../../dev/src
include($$BASEDIR_FW_UI/fw_ui.pri)

HEADERS += \
    TestCoreUiQml.h
