set(NAME "stbi")

set(COMMIT_HASH "5c205738c191bcb0abc65c4febfa9bd25ff35234")

CPMAddPackage(
    NAME  stb
    URL  https://github.com/nothings/stb/archive/${COMMIT_HASH}.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

add_library(stb INTERFACE)

target_include_directories(
    stb
    INTERFACE
    $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${stb_SOURCE_DIR}>
)

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
stbi_impl.cpp
)

target_link_libraries(${NAME} PUBLIC stb)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

