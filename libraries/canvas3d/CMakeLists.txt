set(NAME "canvas3d")
# DEPENDS_ON: canvas2d static_mesh

#include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
canvas3d.h
vertextypes3d.h
canvas3d.cpp
ITriWriter.h
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC
canvas2d
static_mesh
)

# file(GLOB_RECURSE GLSL_SOURCE_FILES "${PROJECT_SOURCE_DIR}/assets/vulkan/*.glsl")


# # message("Assets/${NEW_FILE} FILE ${FILE}")
# # Make sure it shows up in the IDE Assets folder
# source_group("Assets/${NEW_FILE_PATH}" FILES "${FILE}")



# target_glsl_shaders(${NAME}
#     PRIVATE ${GLSL_SOURCE_FILES}
#     COMPILE_OPTIONS --target-env vulkan1.1)
