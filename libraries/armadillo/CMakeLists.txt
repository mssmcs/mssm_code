set(NAME "armadillo")

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

CPMAddPackage(
    NAME  armadillo
    GIT_REPOSITORY  https://gitlab.com/conradsnicta/armadillo-code.git
    GIT_TAG 14.0.2
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

