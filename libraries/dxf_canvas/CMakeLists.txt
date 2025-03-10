set(NAME "dxf_canvas")
# DEPENDS_ON: canvas2d image dime

add_library(${NAME} STATIC
dxfcanvas.h
dxfcanvas.cpp
)

target_link_libraries(${NAME} PUBLIC canvas2d image_base dime)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
