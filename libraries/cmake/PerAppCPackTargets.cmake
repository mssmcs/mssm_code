get_property(_mssm_packaged_apps GLOBAL PROPERTY MSSM_PACKAGED_APPS)
if(NOT _mssm_packaged_apps)
  message(STATUS "No packaged apps were registered.")
  return()
endif()

if(NOT CMAKE_CPACK_COMMAND)
  find_program(CMAKE_CPACK_COMMAND cpack REQUIRED)
endif()

get_filename_component(_mssm_code_root "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
set(_mssm_shared_packaging_dir "${_mssm_code_root}/shared/packaging")
set(_mssm_default_icons_dir "${_mssm_code_root}/assets/icons")
set(_mssm_cpack_dir "${CMAKE_BINARY_DIR}/cpack")
file(MAKE_DIRECTORY "${_mssm_cpack_dir}")

set(_mssm_package_targets "")

foreach(_mssm_entry IN LISTS _mssm_packaged_apps)
  string(REPLACE "|" ";" _mssm_fields "${_mssm_entry}")
  list(LENGTH _mssm_fields _mssm_field_count)
  if(_mssm_field_count LESS 8)
    message(WARNING "Skipping malformed packaged app entry: ${_mssm_entry}")
    continue()
  endif()

  list(GET _mssm_fields 0 _mssm_cmake_project_name)
  list(GET _mssm_fields 1 _mssm_executable_name)
  list(GET _mssm_fields 2 _mssm_project_source_dir)
  list(GET _mssm_fields 3 _mssm_version)
  list(GET _mssm_fields 4 _mssm_version_major)
  list(GET _mssm_fields 5 _mssm_version_minor)
  list(GET _mssm_fields 6 _mssm_version_patch)
  list(GET _mssm_fields 7 _mssm_company_name)

  if(_mssm_company_name STREQUAL "")
    set(_mssm_company_name "Humanity")
  endif()

  set(_mssm_icons_dir "${_mssm_default_icons_dir}")
  if(EXISTS "${_mssm_project_source_dir}/assets/icons/icon.ico")
    set(_mssm_icons_dir "${_mssm_project_source_dir}/assets/icons")
  endif()

  set(_mssm_cfg "${_mssm_cpack_dir}/${_mssm_cmake_project_name}-CPackConfig.cmake")
  set(_mssm_package_name "${_mssm_executable_name}")
  set(_mssm_package_dir "${CMAKE_BINARY_DIR}/distribution/${_mssm_package_name}")
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_mssm_cpack_generator "ZIP;NSIS")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(_mssm_cpack_generator "TGZ;DragNDrop")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_mssm_cpack_generator "TGZ;DEB")
  else()
    set(_mssm_cpack_generator "TGZ")
  endif()

  set(_mssm_cfg_content [=[
set(CPACK_PACKAGE_NAME "@_mssm_package_name@")
set(CPACK_PACKAGE_VENDOR "@_mssm_company_name@")
set(CPACK_PACKAGE_VERSION "@_mssm_version@")
set(CPACK_PACKAGE_VERSION_MAJOR "@_mssm_version_major@")
set(CPACK_PACKAGE_VERSION_MINOR "@_mssm_version_minor@")
set(CPACK_PACKAGE_VERSION_PATCH "@_mssm_version_patch@")
set(CPACK_PACKAGE_FILE_NAME "@_mssm_package_name@-@_mssm_version@")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "@_mssm_package_name@")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "@_mssm_package_name@ @_mssm_version@")
set(CPACK_PACKAGE_DIRECTORY "@_mssm_package_dir@")
set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_CMAKE_GENERATOR "@CMAKE_GENERATOR@")
set(CPACK_INSTALL_CMAKE_PROJECTS "@CMAKE_BINARY_DIR@;@CMAKE_PROJECT_NAME@;@_mssm_cmake_project_name@;/")
set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_SET_DESTDIR OFF)

set(CPACK_RESOURCE_FILE_WELCOME "@_mssm_shared_packaging_dir@/Welcome.txt")
set(CPACK_PACKAGE_DESCRIPTION_FILE "@_mssm_shared_packaging_dir@/Description.txt")
set(CPACK_RESOURCE_FILE_README "@_mssm_shared_packaging_dir@/Readme.txt")
set(CPACK_RESOURCE_FILE_LICENSE "@_mssm_shared_packaging_dir@/License.txt")

set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE OFF)
set(CPACK_DMG_BACKGROUND_IMAGE "@_mssm_shared_packaging_dir@/dmg/AppDMGBackground.tiff")
set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "@_mssm_shared_packaging_dir@/dmg/AppDMGSetup.scpt")
set(CPACK_DMG_VOLUME_NAME "@_mssm_package_name@")

set(CPACK_NSIS_DISPLAY_NAME "@_mssm_package_name@")
set(CPACK_NSIS_PACKAGE_NAME "@_mssm_package_name@")
set(CPACK_NSIS_MANIFEST_DPI_AWARE true)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL YES)
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_NSIS_MUI_ICON "@_mssm_icons_dir@/icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "@_mssm_icons_dir@/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "@_mssm_shared_packaging_dir@/nsis\\\\uninstall_icon.ico")
set(CPACK_NSIS_MUI_HEADERIMAGE "@_mssm_shared_packaging_dir@/nsis\\\\nsis_header.bmp")
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "@_mssm_shared_packaging_dir@/nsis\\\\nsis_install_welcome.bmp")
set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "@_mssm_shared_packaging_dir@/nsis\\\\nsis_uninstall_welcome.bmp")
set(CPACK_NSIS_CREATE_ICONS_EXTRA
  "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\@_mssm_package_name@.lnk' '$INSTDIR\\\\bin\\\\@_mssm_executable_name@.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
  "Delete '$SMPROGRAMS\\\\$START_MENU\\\\@_mssm_package_name@.lnk'")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SECTION Miscellaneous)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "@_mssm_company_name@")
set(CPACK_GENERATOR "@_mssm_cpack_generator@")
]=])

  string(CONFIGURE "${_mssm_cfg_content}" _mssm_cfg_content @ONLY)
  file(WRITE "${_mssm_cfg}" "${_mssm_cfg_content}")

  string(MAKE_C_IDENTIFIER "${_mssm_package_name}" _mssm_pkg_target_suffix)
  set(_mssm_pkg_target "package_${_mssm_pkg_target_suffix}")

  add_custom_target(
    ${_mssm_pkg_target}
    COMMAND ${CMAKE_CPACK_COMMAND} --config "${_mssm_cfg}"
    DEPENDS ${_mssm_cmake_project_name}
    COMMENT "Packaging ${_mssm_package_name}"
    VERBATIM
  )

  list(APPEND _mssm_package_targets ${_mssm_pkg_target})
endforeach()

if(_mssm_package_targets)
  add_custom_target(package_apps DEPENDS ${_mssm_package_targets})
endif()
