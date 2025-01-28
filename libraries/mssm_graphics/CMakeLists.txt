set(NAME "mssm_graphics")
# DEPENDS_ON: vulkan window canvas paths array sound rand vulkan_glfw

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
graphics.h graphics.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
vulk
mssm_window
vulk_canvas
vulk_glfw
mssm_paths
mssm_array
mssm_sound
mssm_rand
)

