set(NAME "layout")
# DEPENDS_ON: vec canvas2d fontinfo svg2 window paths

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
recti.h recti.cpp
sizebound.h sizebound.cpp
layouthelper.h layouthelper.cpp
layoutcore.h layoutcore.cpp
layoutcontainers.h layoutcontainers.cpp
layoutwidgets.h layoutwidgets.cpp
texteditline.h texteditline.cpp
textgeometry.h textgeometry.cpp
texteditbox.h texteditbox.cpp
rectdraw.h rectdraw.cpp
layoutmanager.h layoutmanager.cpp
layoutcontext.h layoutcontext.cpp
layoutcalcs.h layoutcalcs.cpp
layoutmenu.h layoutmenu.cpp
layouttabs.h layouttabs.cpp
layoutflyout.h layoutflyout.cpp
layoutexpander.h layoutexpander.cpp
propertybag.h propertybag.cpp
icondrawer.h icondrawer.cpp
)

target_link_libraries(${NAME} PUBLIC  mssm_paths mssm_window mssm_vec canvas2d mssm_fontinfo svg2)

# target_link_libraries(${NAME} PUBLIC mssm_array mssm_rand mssm_paths mssm_vec print_compat)

set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
