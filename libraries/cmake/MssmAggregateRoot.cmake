# Shared aggregate build for mssm_code project bundles (apps, examples, tests, ...).
# Each bundle's CMakeLists.txt sets optional MSSM_AGGREGATE_EXCLUDE_SUBDIRS, then includes this file.

if(NOT DEFINED MSSM_AGGREGATE_EXCLUDE_SUBDIRS)
    if(DEFINED EXCLUDE_SUBDIRS)
        set(MSSM_AGGREGATE_EXCLUDE_SUBDIRS ${EXCLUDE_SUBDIRS})
    else()
        set(MSSM_AGGREGATE_EXCLUDE_SUBDIRS "")
    endif()
endif()

if(NOT DEFINED FETCHCONTENT_QUIET)
    set(FETCHCONTENT_QUIET ON CACHE BOOL "Show FetchContent progress output")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include("${CMAKE_SOURCE_DIR}/../libraries/cmake/get_cpm.cmake")
include("${CMAKE_SOURCE_DIR}/../libraries/cmake/StaticAnalyzers.cmake")

# project() must be called in each bundle's top-level CMakeLists.txt (before this
# include) so Qt Creator shows apps/examples/tests instead of the default "Project".

include("${CMAKE_SOURCE_DIR}/../libraries/cmake/StandardProjectSettings.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../libraries/cmake/AppleBuild.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../libraries/cmake/util.cmake")

find_package(QT NAMES Qt6 Qt5 QUIET COMPONENTS Widgets)

if(QT_FOUND)
    message(STATUS "QT was found")
else()
    message(STATUS "QT not found")
endif()

option(MSSM_BUILD_TESTS "Build library unit tests (GoogleTest)" OFF)

if(NOT MSSM_TEST_LIBRARIES)
    set(MSSM_TEST_LIBRARIES layout CACHE STRING "Libraries with tests/ subdirectories")
endif()

file(GLOB TOP_LEVEL_DIRS RELATIVE ${CMAKE_SOURCE_DIR} */)

set(PROJECT_FOLDERS "")
set(ALL_LIBRARIES "")

foreach(topdir ${TOP_LEVEL_DIRS})
    if("${topdir}" IN_LIST MSSM_AGGREGATE_EXCLUDE_SUBDIRS)
        message(STATUS "Skipping excluded dir: ${topdir}")
        continue()
    endif()

    set(TOPDIR_PATH "${CMAKE_SOURCE_DIR}/${topdir}")
    if(NOT IS_DIRECTORY "${TOPDIR_PATH}")
        continue()
    endif()

    if(EXISTS "${TOPDIR_PATH}/CMakeLists.txt")
        set(PROJECT_DIRS "${TOPDIR_PATH}")
    else()
        file(GLOB SECOND_LEVEL_DIRS RELATIVE "${TOPDIR_PATH}" "${TOPDIR_PATH}/*/")
        set(PROJECT_DIRS "")
        foreach(subdir ${SECOND_LEVEL_DIRS})
            set(FULLSUBDIR "${TOPDIR_PATH}/${subdir}")
            if(EXISTS "${FULLSUBDIR}/CMakeLists.txt")
                list(APPEND PROJECT_DIRS "${FULLSUBDIR}")
            endif()
        endforeach()
    endif()

    foreach(PROJ_PATH ${PROJECT_DIRS})
        get_filename_component(dirname "${PROJ_PATH}" NAME)
        if(dirname STREQUAL "build" OR dirname STREQUAL "cmake")
            continue()
        endif()

        set(cmake_file "${PROJ_PATH}/CMakeLists.txt")

        check_supports_os("${cmake_file}" SUPPORTS_OS)
        if(NOT SUPPORTS_OS)
            message(STATUS "OS not supported, skipping: ${PROJ_PATH}")
            continue()
        endif()

        if(NOT QT_FOUND)
            check_uses_qt("${cmake_file}" USES_QT)
            if(USES_QT)
                message(STATUS "QT not found, skipping QT project: ${PROJ_PATH}")
                continue()
            endif()
        endif()

        message(STATUS "Discovered project: ${PROJ_PATH}")

        get_library_list("${PROJ_PATH}" LIBRARY_LIST)
        list(APPEND ALL_LIBRARIES ${LIBRARY_LIST})

        file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}" "${PROJ_PATH}")
        list(APPEND PROJECT_FOLDERS "${REL_PATH}")
    endforeach()
endforeach()

if(MSSM_BUILD_TESTS)
    enable_testing()
    include("${CMAKE_SOURCE_DIR}/../libraries/cmake/MssmTest.cmake")
    list(APPEND ALL_LIBRARIES googletest)
    foreach(test_lib ${MSSM_TEST_LIBRARIES})
        list(APPEND ALL_LIBRARIES ${test_lib})
    endforeach()
endif()

set(LIBRARY_RELATIVE_PATH ${CMAKE_SOURCE_DIR}/../libraries)
get_filename_component(LIBRARY_ABSOLUTE_PATH_TEMP ${LIBRARY_RELATIVE_PATH} ABSOLUTE)
set(LIBRARY_ABSOLUTE_PATH ${LIBRARY_ABSOLUTE_PATH_TEMP} CACHE STRING "Absolute path to the libraries directory")

resolve_library_dependencies("${ALL_LIBRARIES}" ALL_LIBRARIES)

message(STATUS "All libraries: ${ALL_LIBRARIES}")

foreach(LIBRARY ${ALL_LIBRARIES})
    string(STRIP "${LIBRARY}" LIBRARY)
    message(STATUS "Library: ${LIBRARY} at ${LIBRARY_ABSOLUTE_PATH}/${LIBRARY}")
    add_subdirectory("${LIBRARY_ABSOLUTE_PATH}/${LIBRARY}" local_library/${LIBRARY})
endforeach()

message(STATUS "Done loading libraries")

foreach(subdir ${PROJECT_FOLDERS})
    message(STATUS "Adding project subdirectory: ${subdir}")

    string(MD5 fullhash "${subdir}")
    string(SUBSTRING "${fullhash}" 0 6 build_id)

    get_filename_component(base "${subdir}" NAME)
    string(REPLACE " " "_" base "${base}")
    string(MAKE_C_IDENTIFIER "${base}" base)

    string(SUBSTRING "${fullhash}" 0 4 shorttag)

    set(MAX_BASE_LEN 24)
    string(LENGTH "${base}" base_len)
    if(base_len GREATER MAX_BASE_LEN)
        string(SUBSTRING "${base}" 0 ${MAX_BASE_LEN} base)
    endif()

    get_filename_component(base "${subdir}" NAME)
    set(PROJECT_OUTPUT_NAME "${base}")
    set(PROJECT_ID "${base}_${shorttag}")

    message(STATUS "  Qt Project ID: ${PROJECT_ID}")
    message(STATUS "  Build folder : s/${build_id}")

    add_subdirectory("${subdir}" "s/${build_id}")
endforeach()

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/PerAppCPackTargets.cmake)

option(MSSM_ENABLE_AGGREGATE_CPACK "Enable one package containing all apps." OFF)
if(MSSM_ENABLE_AGGREGATE_CPACK)
    get_filename_component(MSSM_CODE_ROOT "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
    set(APPS_ICONS_SOURCE_FOLDER "${MSSM_CODE_ROOT}/assets/icons")

    set(SHARED_CPACK_CONFIG_FILE ${CMAKE_SOURCE_DIR}/../shared/packaging/CPackConfig.cmake)
    if(EXISTS ${SHARED_CPACK_CONFIG_FILE})
        set(SHARED_PACKAGING_DIR ${CMAKE_SOURCE_DIR}/../shared/packaging)
        include(${SHARED_CPACK_CONFIG_FILE})
    endif()
endif()
