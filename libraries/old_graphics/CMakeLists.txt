set(NAME "mssm_graphics_old")
# DEPENDS_ON: paths array sound rand window

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
third_party/glfw/src/context.c
third_party/glfw/src/init.c
third_party/glfw/src/input.c
third_party/glfw/src/monitor.c
third_party/glfw/src/vulkan.c
third_party/glfw/src/window.c
third_party/glfw/src/platform.c
third_party/glfw/src/null_platform.h
third_party/glfw/src/null_init.c
third_party/glfw/src/null_window.c
third_party/glfw/src/null_monitor.c
third_party/glfw/src/null_joystick.c
third_party/nanovg/nanovg.c

third_party/whereami.c

third_party/glfw/include/GLFW/glfw3.h
third_party/glfw/include/GLFW/glfw3native.h
third_party/whereami.h
third_party/nanovg/fontstash.h
third_party/nanovg/nanovg.h
third_party/nanovg/nanovg_gl.h
third_party/nanovg/nanovg_gl_utils.h
third_party/nanovg/stb_image.h
third_party/stb_image_write.h
third_party/nanovg/stb_truetype.h

graphics/graphics.cpp
graphics/graphics.h

graphics/objcanvas.h
graphics/objcanvas.cpp

graphics/svgcanvas.h
graphics/svgcanvas.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/graphics )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
mssm_paths
mssm_array
mssm_sound
mssm_rand
)

