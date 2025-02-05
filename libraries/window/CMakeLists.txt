
set(NAME "mssm_window")
# DEPENDS_ON: glew

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

CPMAddPackage(
    NAME  glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

set(GLFW_INSTALL OFF)
#set(BUILD_SHARED_LIBS ON)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_COCOA OFF)
set(GLFW_BUILD_WAYLAND OFF)

add_library(${NAME} STATIC
glfwindow.h
glfwindow.cpp
windowbase.h
windowbase.cpp
windowevents.h windowevents.cpp
eventmanager.h eventmanager.cpp
)

target_link_libraries(${NAME} PUBLIC glew glfw)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
