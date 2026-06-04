set(NAME "cli11")
# MSSM_LIB_DESC: CLI11 command-line parser (header-oriented; exposes INTERFACE target cli11)

if(NOT TARGET CLI11::CLI11)
    CPMAddPackage(
        NAME CLI11
        VERSION 2.4.2
        GITHUB_REPOSITORY CLIUtils/CLI11
        GIT_TAG v2.4.2
        OPTIONS "CLI11_BUILD_TESTS OFF" "CLI11_BUILD_EXAMPLES OFF"
    )
endif()

add_library(cli11 INTERFACE)
target_link_libraries(cli11 INTERFACE CLI11::CLI11)
