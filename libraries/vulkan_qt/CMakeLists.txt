set(NAME "vulk_qt")
# DEPENDS_ON: vulkan window vulk_canvas

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(${NAME} STATIC
corewindowqt.h
corewindowqt.cpp
vulkanwindowqt.h
vulkanwindowqt.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
vulk
mssm_window
vulk_canvas
)

target_link_libraries(${NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

