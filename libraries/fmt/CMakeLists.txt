set(NAME "fmt")

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

CPMAddPackage(
    NAME  fmt
    GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

