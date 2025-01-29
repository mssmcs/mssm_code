set(NAME "fast_matrix_market")

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

CPMAddPackage(
    NAME  ${NAME}
    GIT_REPOSITORY  https://github.com/alugowski/fast_matrix_market.git
    GIT_TAG v1.7.6
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

