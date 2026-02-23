set(NAME "googletest")
# MSSM_LIB_DESC: GoogleTest framework for unit tests

if(NOT TARGET gtest AND NOT TARGET GTest::gtest)
    CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL TRUE
        OPTIONS
            "INSTALL_GTEST OFF"
            "BUILD_GMOCK OFF"
            "BUILD_GTEST ON"
            "gtest_force_shared_crt ON"
    )
endif()

if(NOT TARGET ${NAME})
    add_library(${NAME} INTERFACE)
endif()
if(TARGET GTest::gtest)
    target_link_libraries(${NAME} INTERFACE GTest::gtest)
elseif(TARGET gtest)
    target_link_libraries(${NAME} INTERFACE gtest)
endif()

if(NOT TARGET googletest_main)
    add_library(googletest_main INTERFACE)
endif()
if(TARGET GTest::gtest_main)
    target_link_libraries(googletest_main INTERFACE GTest::gtest_main)
elseif(TARGET gtest_main)
    target_link_libraries(googletest_main INTERFACE gtest_main)
endif()
