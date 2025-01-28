set(NAME "mssm_paths")

CPMAddPackage(
    NAME  platform_folders
    GIT_REPOSITORY  https://github.com/sago007/PlatformFolders.git
    GIT_TAG 4.2.0
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
paths.h
paths.cpp
whereami.c
whereami.h
)

target_link_libraries(${NAME} PUBLIC platform_folders)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

