set(NAME "mssm_font")
# DEPENDS_ON: color fontstash fontinfo

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
vfontrenderer.h 
vfontrenderer.cpp
)

target_link_libraries(${NAME} PUBLIC mssm_color fontstash vulk mssm_fontinfo)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

