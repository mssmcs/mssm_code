set(NAME "sdl")

# Add SDL2 via CPM
CPMAddPackage(
    NAME SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.8
    GIT_SHALLOW TRUE
    EXCLUDE_FROM_ALL TRUE
)

# Interface library for SDL2
add_library(${NAME} INTERFACE)
target_include_directories(
    ${NAME}
    INTERFACE
    $<BUILD_INTERFACE:${SDL2_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${SDL2_SOURCE_DIR}/include>
)
target_link_libraries(
    ${NAME}
    INTERFACE
    SDL2::SDL2main
    SDL2::SDL2
)

# 

# add_library(${NAME} STATIC
# soundengine.h
# soundengine.cpp
# )

# target_link_libraries(${NAME} PUBLIC miniaudio)
# set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

# target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
