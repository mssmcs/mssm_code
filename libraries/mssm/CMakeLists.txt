set(NAME "mssm_utils")
# DEPENDS_ON: array rand paths vec
# WINDOWS_DEPENDS_ON: fmt

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
mssm.h
mssm.cpp
)

target_link_libraries(${NAME} PUBLIC mssm_array mssm_rand mssm_paths mssm_vec print_compat)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


