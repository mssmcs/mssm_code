set(NAME "svg_canvas")

# DEPENDS_ON: canvas2d image

# set(COMMIT_HASH "ea6a6aca009422bba0dbad4c80df6e6ba0c82183")

# CPMAddPackage(
#     NAME  nanosvg
#     URL  https://github.com/memononen/nanosvg/archive/${COMMIT_HASH}.zip
#     # GIT_SHALLOW     TRUE
#     EXCLUDE_FROM_ALL TRUE
# )


# add_library(nanosvg1 INTERFACE)

# target_include_directories(
#     nanosvg1
#     INTERFACE
#     $<BUILD_INTERFACE:${nanosvg_SOURCE_DIR}/src>
#     $<INSTALL_INTERFACE:${nanosvg_SOURCE_DIR}/src>
# )

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
svg.hpp
svgcanvas.h
svgcanvas.cpp
)

target_link_libraries(${NAME} PUBLIC
canvas2d
image_base
)

set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
