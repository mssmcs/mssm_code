

# Link this "library" to use the warnings specified in CompilerWarnings.cmake.
# add_library(project_warnings INTERFACE)
include(${CMAKE_SOURCE_DIR}/../libraries/cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME})

# message("Calling processProjectLibraries.cmake")

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/processProjectLibraries.cmake)

include( InstallRequiredSystemLibraries )

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/AppAssets.cmake)
include(${CMAKE_SOURCE_DIR}/../libraries/cmake/Packaging.cmake)

if (MSVC) 
  target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:RELWITHDEBINFO>:/PROFILE>)
endif()
