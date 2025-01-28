set(NAME "qt_docking")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)

CPMAddPackage(
    NAME advanced_docking
    GIT_REPOSITORY https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System
    GIT_TAG 4.3.1
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY   FALSE
    OPTIONS "BUILD_EXAMPLES OFF" "BUILD_STATIC ON"
)

add_library(${NAME} INTERFACE)

target_link_libraries(${NAME} INTERFACE qt${QT_VERSION_MAJOR}advanceddocking )
target_include_directories(
    ${NAME}
    INTERFACE
    $<BUILD_INTERFACE:${advanced_docking_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${advanced_docking_SOURCE_DIR}/src>
)
