set(NAME "half_edge")
# DEPENDS_ON: obj_loader vec geometry

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
halfedgeloader.h
halfedgeloader.cpp
meshloader.h
meshloader.cpp 
mesh.h
loopiterable.h
)

target_link_libraries(${NAME} PUBLIC obj_loader mssm_vec geometry)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

