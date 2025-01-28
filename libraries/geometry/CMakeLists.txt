set(NAME "geometry")
# DEPENDS_ON: vec

# LIBRARY_EXAMPLE: single header library stored locally (not pulled from github)

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} INTERFACE
geometry.h
)

target_link_libraries(${NAME} INTERFACE mssm_vec)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


