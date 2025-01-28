
set(NAME "mssm_vec")
# DEPENDS_ON fmt

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} INTERFACE
vecUtil.h
vec2d.h
vec3d.h
vec4d.h
)

target_link_libraries(${NAME} INTERFACE fmt)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
