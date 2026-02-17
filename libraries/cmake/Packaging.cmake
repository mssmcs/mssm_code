# General target installation settings
set(_mssm_runtime_dep_args "")
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(APPEND _mssm_runtime_dep_args RUNTIME_DEPENDENCY_SET "${PROJECT_NAME}_runtime_deps")
endif()

install(TARGETS ${PROJECT_NAME}
  COMPONENT ${PROJECT_NAME}
  ${_mssm_runtime_dep_args}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Register each app so the top-level apps/CMakeLists.txt can create
# one CPack target per app (instead of one monolithic installer).
get_target_property(_mssm_packaged_output_name ${PROJECT_NAME} OUTPUT_NAME)
if(NOT _mssm_packaged_output_name OR _mssm_packaged_output_name STREQUAL "_mssm_packaged_output_name-NOTFOUND")
  set(_mssm_packaged_output_name "${PROJECT_NAME}")
endif()

set_property(
  GLOBAL
  APPEND
  PROPERTY MSSM_PACKAGED_APPS
  "${PROJECT_NAME}|${_mssm_packaged_output_name}|${PROJECT_SOURCE_DIR}|${PROJECT_VERSION}|${PROJECT_VERSION_MAJOR}|${PROJECT_VERSION_MINOR}|${PROJECT_VERSION_PATCH}|${PROJECT_COMPANY_NAME}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # using Clang
  target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  #target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()


# Settings for packaging per platform
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  include(${CMAKE_SOURCE_DIR}/../libraries/cmake/packaging/Windows.cmake)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  include(${CMAKE_SOURCE_DIR}/../libraries/cmake/packaging/Linux.cmake)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  include(${CMAKE_SOURCE_DIR}/../libraries/cmake/packaging/Darwin.cmake)
endif()

