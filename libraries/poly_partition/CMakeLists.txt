set(NAME "poly_partition")

set(COMMIT_HASH "b000a4a2a72b46e1305fb6e95b080448d7c12049")

CPMAddPackage(
    NAME  poly_partition
    URL  https://github.com/ivanfratric/polypartition/archive/${COMMIT_HASH}.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY   TRUE
)

add_library(poly_partition STATIC
    ${poly_partition_SOURCE_DIR}/src/polypartition.cpp
    ${poly_partition_SOURCE_DIR}/src/polypartition.h
)

target_include_directories(
    poly_partition
    PUBLIC
    $<BUILD_INTERFACE:${poly_partition_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${poly_partition_SOURCE_DIR}/src>
)

