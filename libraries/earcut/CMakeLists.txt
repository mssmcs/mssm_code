set(NAME "earcut")

CPMAddPackage(
    NAME  earcut
    GIT_REPOSITORY  https://github.com/mapbox/earcut.hpp
    GIT_TAG v2.2.4
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY TRUE
)

add_library(earcut INTERFACE
    ${earcut_SOURCE_DIR}/include/mapbox/earcut.hpp
)

target_include_directories(
    earcut
    INTERFACE
    $<BUILD_INTERFACE:${earcut_SOURCE_DIR}/include/mapbox>
    $<INSTALL_INTERFACE:${earcut_SOURCE_DIR}/include/mapbox>
)

