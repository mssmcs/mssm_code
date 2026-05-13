set(NAME "mssm_graphics_dawn")
# DEPENDS_ON: mssm_graphics mssm_webgpu

add_library(${NAME} STATIC
    dawngraphicsbackend.h
    dawngraphicsbackend.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC
    mssm_graphics
    mssm_webgpu
)

