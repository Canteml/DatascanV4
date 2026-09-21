#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "VLCQt::Widgets" for configuration "Debug"
set_property(TARGET VLCQt::Widgets APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(VLCQt::Widgets PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libVLCQtWidgetsd.dll.a"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "VLCQt::Core;Qt6::Widgets"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/libVLCQtWidgetsd.dll"
  )

list(APPEND _cmake_import_check_targets VLCQt::Widgets )
list(APPEND _cmake_import_check_files_for_VLCQt::Widgets "${_IMPORT_PREFIX}/lib/libVLCQtWidgetsd.dll.a" "${_IMPORT_PREFIX}/bin/libVLCQtWidgetsd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
