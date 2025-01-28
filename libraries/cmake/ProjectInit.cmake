# much of this cmake setup copied from Martin Helmut Fieber's excellent sample

# set(CPM_USE_NAMED_CACHE_DIRECTORIES TRUE)

message(">>>>> Current Source Directory: ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_C_STANDARD_REQUIRED ON)

message("Compiling with: ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")

include("${CMAKE_SOURCE_DIR}/../libraries/cmake/get_cpm.cmake")
include("${CMAKE_SOURCE_DIR}/../libraries/cmake/AppleBuild.cmake")

if (NOT PROJECT_DESCRIPTION)
    set(PROJECT_DESCRIPTION "A C++ project")
endif()

if (NOT PROJECT_ID) 
    # Extract the project name from the current source directory
    get_filename_component(PROJECT_ID "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    # Replace spaces in the project name with underscores
    string(REPLACE " " "_" PROJECT_ID "${PROJECT_ID}")
endif()

# Ensure the project name is non-empty, fallback to a default if needed
if(NOT PROJECT_ID)
    set(PROJECT_ID "DefaultProject")
endif()

if (NOT PROJECT_VERSION)
    set(PROJECT_VERSION 0.1.0.0)
endif()

# Define the project
project(${PROJECT_ID}
    VERSION "${PROJECT_VERSION}"
    DESCRIPTION "${PROJECT_DESCRIPTION}"
    LANGUAGES CXX C
)

message("********* ${PROJECT_NAME} *********")

set(FULL_PROJECT_VERSION ${PROJECT_VERSION})
set(SHORT_PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

message("Project name: ${PROJECT_NAME}")
message("Description: ${PROJECT_DESCRIPTION}")
message("Version: ${FULL_PROJECT_VERSION}")
# message(${SHORT_PROJECT_VERSION})

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/StandardProjectSettings.cmake)
include(GNUInstallDirs)
include(${CMAKE_SOURCE_DIR}/../libraries/cmake/StaticAnalyzers.cmake)
