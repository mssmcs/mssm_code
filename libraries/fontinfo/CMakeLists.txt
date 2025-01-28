set(NAME "mssm_fontinfo")
# DEPENDS_ON:

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
textinfo.h
textinfo.cpp 
)

# target_link_libraries(${NAME} PUBLIC fontstash)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

