
# specify sub folders containing libraries to be loaded from LIBRARY_RELATIVE_PATH
set(LIBRARY_RELATIVE_PATH ../../libraries/)

# message("$$$$$$$$$$$$$$$$$ ${CMAKE_CURRENT_SOURCE_DIR} $$$$$$$$$$$$$$$$$")

get_filename_component(LIBRARY_ABSOLUTE_PATH ${LIBRARY_RELATIVE_PATH} ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/util.cmake)

# expand list of libraries to include all dependencies
# (determined by DEPENDS_ON: in the CMakeLists.txt file of each library)
resolve_library_dependencies("${LIBRARIES}" LIBRARIES)

# message("Setting libraries: ${LIBRARIES}")

# add each library to the project and add them as link libraries to the main executable
foreach (LIBRARY ${LIBRARIES})
    string(STRIP "${LIBRARY}" LIBRARY)
    get_library_target(${LIBRARY_ABSOLUTE_PATH}/${LIBRARY} LIB_TARG_NAME)
    string(TOUPPER "HASLIB_${LIBRARY}" LIBCOMPDEF)
    add_compile_definitions(${LIBCOMPDEF})
    message(">>>> Library ${LIBRARY} Target Name: ${LIB_TARG_NAME}  #define: ${LIBCOMPDEF}")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_TARG_NAME} )
endforeach ()

