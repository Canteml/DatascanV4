#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "VLCQt::Widgets" for configuration "Release"
set_property(TARGET VLCQt::Widgets APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(VLCQt::Widgets PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libVLCQtWidgets.dll.a"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "VLCQt::Core;Qt6::Widgets"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libVLCQtWidgets.dll"
  )

list(APPEND _cmake_import_check_targets VLCQt::Widgets )
list(APPEND _cmake_import_check_files_for_VLCQt::Widgets "${_IMPORT_PREFIX}/lib/libVLCQtWidgets.dll.a" "${_IMPORT_PREFIX}/bin/libVLCQtWidgets.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
