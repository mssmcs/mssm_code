set(NAME "mssm_graphics_vulk")
# DEPENDS_ON: vulkan window vulk_canvas paths array sound rand vulkan_glfw mssm_graphics canvas3d

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
vulkbackendwindow.h vulkbackendwindow.cpp
vulkgraphicsbackend.h vulkgraphicsbackend.cpp
graphics3d.h graphics3d.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
vulk
vulk_canvas
vulk_glfw
mssm_graphics
canvas3d
)

