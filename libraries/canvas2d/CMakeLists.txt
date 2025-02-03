set(NAME "canvas2d")
# DEPENDS_ON: array color vec font linmath

#include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} INTERFACE
canvas2d.h
)

target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} INTERFACE 
mssm_array
mssm_color
mssm_vec
mssm_font
mssm_linmath
)

# file(GLOB_RECURSE GLSL_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/assets/vulkan/*.glsl")


# # message("Assets/${NEW_FILE} FILE ${FILE}")
# # Make sure it shows up in the IDE Assets folder
# source_group("Assets/${NEW_FILE_PATH}" FILES "${FILE}")



# target_glsl_shaders(${NAME}
#     PRIVATE ${GLSL_SOURCE_FILES}
#     COMPILE_OPTIONS --target-env vulkan1.1)
