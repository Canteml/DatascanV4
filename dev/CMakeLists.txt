cmake_minimum_required(VERSION 3.16)
project(APP VERSION 4.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Qt6 ---
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Quick Qml)

# --- Aggiungi target eseguibile ---
qt_add_executable(APP
    src/CoreApp.cpp
    src/CoreApp.h
)

# --- fw_common ---
target_include_directories(APP PRIVATE ${CMAKE_SOURCE_DIR}/ext/fw_common/dev/src)
file(GLOB_RECURSE FW_COMMON
    ${CMAKE_SOURCE_DIR}/ext/fw_common/dev/src/*.cpp
    ${CMAKE_SOURCE_DIR}/ext/fw_common/dev/src/*.h
)
target_sources(APP PRIVATE ${FW_COMMON})

# --- fw_ui ---
target_include_directories(APP PRIVATE ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src)
file(GLOB_RECURSE FW_UI
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/coreUi/*.cpp
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/window/*.cpp
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/coreUi/*.h
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/window/*.h
)
target_sources(APP PRIVATE ${FW_UI})

# --- Resources (.qrc) ---
qt_add_resources(RES_SOURCES
    src/res/translations.qrc
)
target_sources(APP PRIVATE ${RES_SOURCES])

# --- Defines ---
target_compile_definitions(APP PRIVATE APP_VERSION="rev_4.0.0-FR")
