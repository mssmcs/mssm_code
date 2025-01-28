# Set a default build type if none was specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif ()

find_program(CCACHE ccache)
if (CCACHE)
  message(STATUS "Using ccache")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else ()
  message(STATUS "Ccache not found")
endif ()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DEACTIVATE_LOGGING "Disable logging" OFF)
if (DEACTIVATE_LOGGING)
  add_compile_definitions(APP_DEACTIVATE_LOGGING)
endif ()

option(DEBUG "Enable debug statements and asserts" OFF)
if (DEBUG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(DEBUG APP_PROFILE)
endif ()

if (NOT CMAKE_CXX_COMPILER_ID)
  message(FATAL_ERROR "Unknown compiler (maybe project() has not been called?)")
endif ()


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#  string(APPEND CMAKE_CXX_FLAGS " -Wall -Werror -pedantic-errors -Wextra -Wno-unused-function")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
#  string(APPEND CMAKE_CXX_FLAGS " -Wall -Werror -pedantic-errors -Wextra -Wno-unused-function")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
#
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
string(APPEND CMAKE_CXX_FLAGS " /Zc:preprocessor /std:c++latest")
# add_compile_options(Zc:preprocessor Zc:__cplusplus std:clatest std:c++latest)
#  string(APPEND CMAKE_CXX_FLAGS " /WX /W4 /wd4505 /permissive- /analyze:external- /external:anglebrackets /external:W0 /wd4251 /Zc:preprocessor /Zc:__cplusplus /Zc:char8_t Yes /Zc:rvalueCast")
#  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
#  string(APPEND CMAKE_CXX_FLAGS_RELEASE " /fp:fast /GL /MP /Gw /GR- /Oy /Ot /Oi")
#  string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " /fp:fast /GL /MP /Gw /GR- /Oy /Ot /Oi")
#  string(APPEND CMAKE_CXX_FLAGS_MINSIZEREL " /fp:fast /GL /MP /Gw /GR- /Oy /Ot /Oi")
endif()

