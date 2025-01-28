set(NAME "mssm_sound")

# LIBRARY_EXAMPLE: header-only pulled from github with tag

CPMAddPackage(
    NAME  miniaudio
    GIT_REPOSITORY  https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.21
    GIT_SHALLOW     TRUE
    DOWNLOAD_ONLY   TRUE
    EXCLUDE_FROM_ALL TRUE
)

add_library(miniaudio INTERFACE)

target_include_directories(
    miniaudio
    INTERFACE
    $<BUILD_INTERFACE:${miniaudio_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${miniaudio_SOURCE_DIR}>
)

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
soundengine.h
soundengine.cpp
)

target_link_libraries(${NAME} PUBLIC miniaudio)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

