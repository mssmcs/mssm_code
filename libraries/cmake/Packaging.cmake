# General target installation settings
install(TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

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
endif ()


