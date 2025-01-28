set(NAME "image_base")

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
image.h
image.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)


