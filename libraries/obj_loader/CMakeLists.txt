set(NAME "obj_loader")

# LIBRARY_EXAMPLE: single header library stored locally (not pulled from github)

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} INTERFACE
tiny_obj_loader.h
)

target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)


