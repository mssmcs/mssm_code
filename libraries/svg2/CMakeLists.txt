set(NAME "svg2")
# DEPENDS_ON: vec tinyxml2

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC

# zen/fixed_list.h
# zen/scope_guard.h
# zen/stl_tools.h
# zen/string_tools.h
# zen/string_traits.h
# zen/type_tools.h
# zen/type_traits.h
# zen/utf.h

# zenxml/bind.h
# zenxml/cvrt_struc.h
# zenxml/cvrt_text.h
# zenxml/dom.h
# zenxml/error.h
# zenxml/io.h
# zenxml/parser.h
# zenxml/static_unit_test.cpp
# zenxml/xml.h

parseinput.cpp 
parseinput.h
svg.cpp 
svg.h
svgelement.cpp 
svgelement.h 
svgpath.cpp 
svgpath.h
svgshape.cpp 
svgshape.h 
svgstructelement.cpp 
svgstructelement.h
)

if(WIN32)
    add_compile_definitions(ZEN_WIN)
elseif(UNIX AND NOT APPLE)
    add_compile_definitions(ZEN_LINUX)
elseif(APPLE)
    add_compile_definitions(ZEN_MAC)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

target_link_libraries(${NAME} PUBLIC mssm_vec tinyxml2)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
