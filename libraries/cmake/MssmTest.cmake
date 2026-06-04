# Helpers for GoogleTest targets in mssm_code library bundles.
# Requires googletest to be added to ALL_LIBRARIES before library subdirectories run.

include(GoogleTest)

function(mssm_add_gtest target)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs SOURCES LINK_LIBS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "mssm_add_gtest(${target}) requires SOURCES")
    endif()

    add_executable(${target} ${ARG_SOURCES})

    target_link_libraries(${target} PRIVATE googletest_main)

    if(ARG_LINK_LIBS)
        target_link_libraries(${target} PRIVATE ${ARG_LINK_LIBS})
    endif()

    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
    )

    gtest_discover_tests(${target})
endfunction()
