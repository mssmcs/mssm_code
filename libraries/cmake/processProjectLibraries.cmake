# LIBRARY_ABSOLUTE_PATH is now expected to be in the cache, set by the top-level apps/CMakeLists.txt
message("[DEBUG] Using LIBRARY_ABSOLUTE_PATH from cache: ${LIBRARY_ABSOLUTE_PATH}")

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/util.cmake)

# message("BEFORE RESOLVING DEPENDENCIES: ${LIBRARIES}")

# expand list of libraries to include all dependencies
# (determined by DEPENDS_ON: in the CMakeLists.txt file of each library)
resolve_library_dependencies("${LIBRARIES}" LIBRARIES)

# message("Setting libraries!: ${LIBRARIES}")

# add each library to the project and add them as link libraries to the main executable
foreach (LIBRARY ${LIBRARIES})
    string(STRIP "${LIBRARY}" LIBRARY)
    get_library_target(${LIBRARY_ABSOLUTE_PATH}/${LIBRARY} LIB_TARG_NAME)
    string(TOUPPER "HASLIB_${LIBRARY}" LIBCOMPDEF)
    add_compile_definitions(${LIBCOMPDEF})
    message(">>>> Library ${LIBRARY} Target Name: ${LIB_TARG_NAME}  #define: ${LIBCOMPDEF}")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_TARG_NAME} )
endforeach ()

