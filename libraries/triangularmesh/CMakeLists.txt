set(NAME "triangularmesh")
# DEPENDS_ON: half_edge

add_library(${NAME} INTERFACE
triangularmesh.h
)

target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${NAME} INTERFACE
    half_edge
)



