set(NAME "static_mesh")
# DEPENDS_ON: color half_edge

add_library(${NAME} STATIC
    staticmesh.h
    staticmesh.cpp
    vertex3duv.h
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC
    half_edge
    mssm_color
)
