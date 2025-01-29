set(NAME "eigen")

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

CPMAddPackage(
    NAME  eigen
    GIT_REPOSITORY  https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    OPTIONS "BUILD_TESTING OFF" "BUILD_SMOKE_TESTING OFF"
)

