set(NAME "mssm_webgpu")
# DEPENDS_ON: dawn window

add_library(${NAME} STATIC
    webgpu_context.h
    webgpu_context.cpp
    webgpu_glfw_surface.h
    webgpu_glfw_surface.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC
    mssm_dawn
    mssm_window
)

