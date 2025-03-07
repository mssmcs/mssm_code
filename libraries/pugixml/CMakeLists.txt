set(NAME "pugixml")

# LIBRARY_EXAMPLE: header-only pulled from github with tag

CPMAddPackage(
    NAME  pugixml
    GIT_REPOSITORY  https://github.com/zeux/pugixml.git
    GIT_TAG v1.15
    GIT_SHALLOW     TRUE
    DOWNLOAD_ONLY   TRUE
    EXCLUDE_FROM_ALL TRUE
)

add_library(pugixml INTERFACE)

target_include_directories(
    pugixml
    INTERFACE
    $<BUILD_INTERFACE:${pugixml_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${pugixml_SOURCE_DIR}>
)

# 

# add_library(${NAME} STATIC
# soundengine.h
# soundengine.cpp
# )

# target_link_libraries(${NAME} PUBLIC miniaudio)
# set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

# target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
