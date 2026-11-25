# Include directory
target_include_directories(APP PRIVATE ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src)

# Sources & headers
file(GLOB_RECURSE FW_UI_SOURCES
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/coreUi/*.cpp
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/window/*.cpp
)
file(GLOB_RECURSE FW_UI_HEADERS
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/coreUi/*.h
    ${CMAKE_SOURCE_DIR}/ext/fw_ui/dev/src/window/*.h
)
target_sources(APP PRIVATE ${FW_UI_SOURCES} ${FW_UI_HEADERS})