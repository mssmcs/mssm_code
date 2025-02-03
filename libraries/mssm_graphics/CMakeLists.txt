set(NAME "mssm_graphics")
# DEPENDS_ON: window paths array sound rand canvas2d

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
graphics.h graphics.cpp
graphicsbackend.h
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
mssm_window
mssm_paths
mssm_array
mssm_sound
mssm_rand
canvas2d
)

