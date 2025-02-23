cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_C_STANDARD_REQUIRED ON)

# add subdirectories that should be excluded from the build
set(EXCLUDE_SUBDIRS
mlspring25-logistic_regression-ml_app
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


# Find and add subdirectories
file(GLOB SUBDIRECTORIES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */)

# message("Subdirectories: ${SUBDIRECTORIES}")

message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(ALL_LIBRARIES "")
set(PROJECT_FOLDERS "")

# first scan subdirectories (assumed to be projects) for their dependencies
foreach(subdir ${SUBDIRECTORIES})

    if ("${subdir}" IN_LIST EXCLUDE_SUBDIRS)
        message("skipping project (because it appears in EXCLUDE_SUBDIRS): ${subdir}")
        continue()
    endif()

    set(FULLSUBDIR "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    if(IS_DIRECTORY ${FULLSUBDIR})
        # Extract the name of the directory
        get_filename_component(dirname ${FULLSUBDIR} NAME)

        if (dirname STREQUAL "build" OR dirname STREQUAL "cmake" OR dirname STREQUAL "visualstudiosln")
            continue()
        endif()

        # Construct the full path to the CMakeLists.txt file
        set(cmake_file "${FULLSUBDIR}/CMakeLists.txt")

        # message("Looking for cmake file: ${cmake_file}")

        if(NOT EXISTS "${cmake_file}")
            # message("No CMakeLists.txt file found in: ${FULLSUBDIR}")
            continue()
        endif()

        check_supports_os("${cmake_file}" SUPPORTS_OS)
        if (NOT SUPPORTS_OS)
            message("OS not supported, skipping project: ${subdir}")
            continue()
        endif()

        if (NOT QT_FOUND) 
           check_uses_qt("${cmake_file}" USES_QT)
           if (USES_QT)
                message("QT not found, skipping QT project: ${subdir}")
                continue()
            endif()
        endif()


        
        message(STATUS "Getting referenced libraries: ${subdir}")            
        get_library_list("${FULLSUBDIR}" LIBRARY_LIST)
        message("Library list: ${LIBRARY_LIST}")



        list(APPEND ALL_LIBRARIES ${LIBRARY_LIST})

        list(APPEND PROJECT_FOLDERS ${subdir})
    else()
        # message("Not a directory: ${FULLSUBDIR}")
    endif()
endforeach()

message("All libraries: ${ALL_LIBRARIES}")

set(LIBRARY_RELATIVE_PATH ../libraries/)

get_filename_component(LIBRARY_ABSOLUTE_PATH ${LIBRARY_RELATIVE_PATH} ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

message("Library absolute path: ${LIBRARY_ABSOLUTE_PATH}")

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
    add_subdirectory(${subdir})
endforeach()


