

set(NAME "glew")

# https://github.com/MattGuerrette/Metal.git

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
src/glew.c
include/GL/glew.h
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(${NAME} PUBLIC GLEW_STATIC )

find_package(OpenGL REQUIRED)

# Define set of OS specific files to include
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${NAME} PUBLIC  ${OPENGL_LIBRARY})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(${NAME}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework OpenGL")
    target_compile_definitions(${NAME} PUBLIC _GLFW_COCOA GL_SILENCE_DEPRECATION)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${NAME} PUBLIC  ${OPENGL_LIBRARY})
endif ()

