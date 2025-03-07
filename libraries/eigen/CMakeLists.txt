set(NAME "eigen")

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

CPMAddPackage(
    NAME  eigenSrc
    GIT_REPOSITORY  https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    SOURCE_SUBDIR Eigen
    OPTIONS "BUILD_TESTING OFF" "BUILD_SMOKE_TESTING OFF"
)

add_library(eigen INTERFACE)
add_library(Eigen::Eigen ALIAS eigen)
target_include_directories(eigen INTERFACE ${eigenSrc_SOURCE_DIR})

# target_compile_definitions(eigen INTERFACE EIGEN_FAST_MATH EIGEN_DEFAULT_TO_ROW_MAJOR
#     $<$<CONFIG:Release>:EIGEN_NO_DEBUG>)


