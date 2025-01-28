set(NAME "mssm_array")
# DEPENDS_ON: fmt

#include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
array.h
array.cpp
)

target_link_libraries(${NAME} PUBLIC fmt)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${NAME} DESTINATION lib)
