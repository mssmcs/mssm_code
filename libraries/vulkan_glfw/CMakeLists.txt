set(NAME "vulk_glfw")
# DEPENDS_ON: vulkan window

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
vulkGlfw.cpp
vulkanwindowglfw.h
vulkanwindowglfw.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
vulk
mssm_window
)

