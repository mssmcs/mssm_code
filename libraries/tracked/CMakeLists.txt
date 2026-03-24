
set(NAME "tracked")
# DEPENDS_ON 

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} INTERFACE
tracked.h
)

set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
