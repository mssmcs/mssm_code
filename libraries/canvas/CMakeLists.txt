set(NAME "vulk_canvas")
# DEPENDS_ON: vulkan color vec font linmath paths poly_partition

#include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
vulkcanvas.h 
vulkcanvas.cpp
canvas.h
vertextypes.h

vertextypes.cpp
triwriter.h
triwriter.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
vulk
mssm_color
mssm_vec
mssm_font
mssm_linmath
mssm_paths
poly_partition
)

# file(GLOB_RECURSE GLSL_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/assets/vulkan/*.glsl")


# # message("Assets/${NEW_FILE} FILE ${FILE}")
# # Make sure it shows up in the IDE Assets folder
# source_group("Assets/${NEW_FILE_PATH}" FILES "${FILE}")



# target_glsl_shaders(${NAME}
#     PRIVATE ${GLSL_SOURCE_FILES}
#     COMPILE_OPTIONS --target-env vulkan1.1)
