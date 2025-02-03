set(NAME "mssm_graphics_nanovg")
# DEPENDS_ON: mssm_graphics canvas2d

add_library(${NAME} STATIC

graphics/objcanvas.h
graphics/objcanvas.cpp

graphics/svgcanvas.h
graphics/svgcanvas.cpp

nanovggraphicsbackend.h nanovggraphicsbackend.cpp
nanovggraphics.h nanovggraphics.cpp

third_party/nanovg/nanovg.c
third_party/nanovg/fontstash.h
third_party/nanovg/nanovg.h
third_party/nanovg/nanovg_gl.h
third_party/nanovg/nanovg_gl_utils.h
third_party/nanovg/stb_image.h
third_party/stb_image_write.h
third_party/nanovg/stb_truetype.h
nanovgwindow.h nanovgwindow.cpp
)

target_include_directories(${NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/nanovg
    ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
mssm_graphics
canvas2d
)


if (WIN32)
    # add sources specific to Windows
# target_sources(${${NAME}}
#     PUBLIC
    # third_party/glfw/src/win32_init.c
    # third_party/glfw/src/win32_joystick.c
    # third_party/glfw/src/win32_monitor.c
    # third_party/glfw/src/win32_time.c
    # third_party/glfw/src/win32_thread.c
    # third_party/glfw/src/win32_window.c
    # third_party/glfw/src/wgl_context.c
    # third_party/glfw/src/egl_context.c
    # third_party/glfw/src/osmesa_context.c
    # third_party/glfw/src/win32_module.c
    # third_party/glew.c
    # third_party/glfw/src/win32_platform.h
    # third_party/glfw/src/win32_joystick.h
    # third_party/GL/eglew.h
    # third_party/GL/glxew.h
    # third_party/GL/wglew.h
    # third_party/GL/glew.h
# )
target_link_libraries(${NAME} PUBLIC gdi32 opengl32)
add_compile_definitions(_GLFW_WIN32 NANOVG_GLEW GLEW_STATIC PROVIDE_WINMAIN )
set_target_properties(${NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)
endif()
