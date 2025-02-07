set(NAME "mssm_csv")

# DEPENDS_ON:

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
csvreader.cpp
csvreader.h
csvwriter.cpp
csvwriter.h
)

set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

