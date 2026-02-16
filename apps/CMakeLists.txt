cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_C_STANDARD_REQUIRED ON)

# add subdirectories that should be excluded from the build
set(EXCLUDE_SUBDIRS
homework1-hamling
Quickhouse3
sample_project
mssm_cmd
stack-hamling
)

# Not your situation, but in case you have a different linker
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--verbose")
# Clang passes flags through to the linker (likely ld) by
# set(CMAKE_EXE_LINKER_FLAGS "-Xlinker -v")
# Clang passing flags to the linker (likely ld) AND using -v itself to show how it calls the linker
# set(CMAKE_EXE_LINKER_FLAGS "-Xlinker -v -v")




include("${CMAKE_SOURCE_DIR}/../libraries/cmake/get_cpm.cmake")

include("${CMAKE_SOURCE_DIR}/../libraries/cmake/StaticAnalyzers.cmake")



# Debugging: Display the current source directory
# message(">>>>> Current Source Directory: ${CMAKE_CURRENT_SOURCE_DIR}")

# Extract the project name from the current source directory
get_filename_component(TOP_LEVEL_PROJ_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

# Replace spaces in the project name with underscores
string(REPLACE " " "_" TOP_LEVEL_PROJ_NAME "${TOP_LEVEL_PROJ_NAME}")

# Define the project
project(${TOP_LEVEL_PROJ_NAME})

include("${CMAKE_SOURCE_DIR}/../libraries/cmake/StandardProjectSettings.cmake")


include("${CMAKE_SOURCE_DIR}/../libraries/cmake/AppleBuild.cmake")
include("${CMAKE_SOURCE_DIR}/../libraries/cmake/util.cmake")

# check whether qt is installed, if not, skip loading any qt projects
find_package(QT NAMES Qt6 Qt5 QUIET COMPONENTS Widgets)  

if(QT_FOUND)
    message("QT Was found") 
else()
    message("QT Not Found") 
endif()

# Top-level directories under apps/
file(GLOB TOP_LEVEL_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */)

set(PROJECT_FOLDERS "")
set(ALL_LIBRARIES "")

foreach(topdir ${TOP_LEVEL_DIRS})

    if ("${topdir}" IN_LIST EXCLUDE_SUBDIRS)
        message("Skipping excluded dir: ${topdir}")
        continue()
    endif()

    set(TOPDIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${topdir}")
    if(NOT IS_DIRECTORY "${TOPDIR_PATH}")
        continue()
    endif()

    # ------------------------------------------------------------
    # Case 1: apps/SomeProject/CMakeLists.txt
    # ------------------------------------------------------------
    if (EXISTS "${TOPDIR_PATH}/CMakeLists.txt")
        set(CANDIDATE_DIR "${TOPDIR_PATH}")

        set(PROJECT_DIRS "${CANDIDATE_DIR}")
    else()
        # ------------------------------------------------------------
        # Case 2: apps/assignment/student/CMakeLists.txt
        # ------------------------------------------------------------
        file(GLOB SECOND_LEVEL_DIRS RELATIVE "${TOPDIR_PATH}" "${TOPDIR_PATH}/*/")

        set(PROJECT_DIRS "")
        foreach(subdir ${SECOND_LEVEL_DIRS})
            set(FULLSUBDIR "${TOPDIR_PATH}/${subdir}")

            if (EXISTS "${FULLSUBDIR}/CMakeLists.txt")
                list(APPEND PROJECT_DIRS "${FULLSUBDIR}")
            endif()
        endforeach()
    endif()

    # ------------------------------------------------------------
    # Process discovered projects
    # ------------------------------------------------------------
    foreach(PROJ_PATH ${PROJECT_DIRS})

        get_filename_component(dirname "${PROJ_PATH}" NAME)
        if (dirname STREQUAL "build" OR dirname STREQUAL "cmake")
            continue()
        endif()

        set(cmake_file "${PROJ_PATH}/CMakeLists.txt")

        check_supports_os("${cmake_file}" SUPPORTS_OS)
        if (NOT SUPPORTS_OS)
            message("OS not supported, skipping: ${PROJ_PATH}")
            continue()
        endif()

        if (NOT QT_FOUND)
            check_uses_qt("${cmake_file}" USES_QT)
            if (USES_QT)
                message("QT not found, skipping QT project: ${PROJ_PATH}")
                continue()
            endif()
        endif()

        message(STATUS "Discovered project: ${PROJ_PATH}")

        get_library_list("${PROJ_PATH}" LIBRARY_LIST)
        list(APPEND ALL_LIBRARIES ${LIBRARY_LIST})

        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJ_PATH}")
        list(APPEND PROJECT_FOLDERS "${REL_PATH}")

    endforeach()
endforeach()


message("All libraries: ${ALL_LIBRARIES}")

# Ensure the library relative path is set correctly
# Update the library relative path to point to the correct location
set(LIBRARY_RELATIVE_PATH ${CMAKE_SOURCE_DIR}/../libraries)

# Debugging: Print the resolved library path
message("Resolved Library Relative Path: ${LIBRARY_RELATIVE_PATH}")

# Get the absolute path of the libraries directory and set it in the cache
get_filename_component(LIBRARY_ABSOLUTE_PATH_TEMP ${LIBRARY_RELATIVE_PATH} ABSOLUTE)
set(LIBRARY_ABSOLUTE_PATH ${LIBRARY_ABSOLUTE_PATH_TEMP} CACHE STRING "Absolute path to the libraries directory")

# Debugging: Print the resolved absolute path
message("Resolved Library Absolute Path: ${LIBRARY_ABSOLUTE_PATH}")

get_filename_component(APP_TEMPLATE_ABSOLUTE_PATH ../app_template ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

message("Application Template path: ${APP_TEMPLATE_ABSOLUTE_PATH}")

resolve_library_dependencies("${ALL_LIBRARIES}" ALL_LIBRARIES)


message("All libraries: ${ALL_LIBRARIES}")

# process subdirectories for all referenced libraries
foreach (LIBRARY ${ALL_LIBRARIES})
    string(STRIP "${LIBRARY}" LIBRARY)
    message("##################################################")
    message("Library: ${LIBRARY}")
    # get_library_target(${LIBRARY_ABSOLUTE_PATH}/${LIBRARY} LIB_TARG_NAME)
    # message("Library Target Name: ${LIB_TARG_NAME}")
    message("Library absolute path: ${LIBRARY_ABSOLUTE_PATH}/${LIBRARY}")
    add_subdirectory("${LIBRARY_ABSOLUTE_PATH}/${LIBRARY}" local_library/${LIBRARY})
    # target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_TARG_NAME} )
endforeach ()

message("*************** DONE LOADING LIBRARIES ****************")

# now add subdirectories for the projects
foreach(subdir ${PROJECT_FOLDERS})
    message("-------- Adding subdirectory: ${subdir} ----------------")
    # Create a short, stable binary directory per project
    get_filename_component(PROJ_NAME "${subdir}" NAME)
    string(REPLACE "-" "_" PROJ_NAME "${PROJ_NAME}")
    message("--------Using Projdir: ${PROJ_NAME} ----------------")

    add_subdirectory(
        "${subdir}"
        "students/${PROJ_NAME}"
    )
endforeach()


