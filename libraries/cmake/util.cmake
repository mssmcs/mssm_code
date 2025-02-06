cmake_minimum_required(VERSION 3.22)

# Filter values through regex
#   filter_regex({INCLUDE | EXCLUDE} <regex> <listname> [items...])
#   Element will included into result list if
#     INCLUDE is specified and it matches with regex or
#     EXCLUDE is specified and it doesn't match with regex.
# Example:
#   filter_regex(INCLUDE "(a|c)" LISTOUT a b c d) => a c
#   filter_regex(EXCLUDE "(a|c)" LISTOUT a b c d) => b d
function(filter_regex _action _regex _listname)
    # check an action
    if("${_action}" STREQUAL "INCLUDE")
        set(has_include TRUE)
    elseif("${_action}" STREQUAL "EXCLUDE")
        set(has_include FALSE)
    else()
        message(FATAL_ERROR "Incorrect value for ACTION: ${_action}")
    endif()

    set(${_listname})
    foreach(element ${ARGN})
        string(REGEX MATCH ${_regex} result ${element})
        if(result)
            if(has_include)
                list(APPEND ${_listname} ${element})
            endif()
        else()
            if(NOT has_include)
                list(APPEND ${_listname} ${element})
            endif()
        endif()
    endforeach()

    # put result in parent scope variable
    set(${_listname} ${${_listname}} PARENT_SCOPE)
endfunction()

function(get_library_target cmake_filename output_variable)

    # looks for the NAME variable being set in the CMakeLists.txt file, and returns the value
    # set(NAME "mssm_paths")

    file(STRINGS ${cmake_filename}/CMakeLists.txt THIS_FILE)

    while(THIS_FILE)
        list(POP_FRONT THIS_FILE LINE)
        #message("CHECKING LINE: ${LINE}")
        if (LINE MATCHES ".*set\\(NAME[ \t\r\n]+\"(.+)\".*")
            set(${output_variable} ${CMAKE_MATCH_1} PARENT_SCOPE)
            break()
        endif()
    endwhile()
endfunction()

function(get_library_dependencies cmake_filename output_variable)

    # message("Getting Library Dependencies: ${cmake_filename}")

    if(NOT EXISTS "${cmake_filename}/CMakeLists.txt")
        message(FATAL_ERROR "Library not found!!: ${cmake_filename}")
        return()        
    endif()

    file(STRINGS "${cmake_filename}/CMakeLists.txt" THIS_FILE)

    # message("CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")

    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        # Windows-specific libraries
        set(OS_DEP_LIBRARIES "WINDOWS_DEPENDS_ON")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        # Linux-specific libraries
        set(OS_DEP_LIBRARIES "LINUX_DEPENDS_ON")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        # macOS-specific libraries
        set(OS_DEP_LIBRARIES "MAC_DEPENDS_ON")
    else()
        set(OS_DEP_LIBRARIES "UNKOS_LIBRARIES")
    endif()
    

    # message("OS DEPENDENCIES: ${OS_DEP_LIBRARIES}")

    set(TMP_DEP_LIST "")

    while(THIS_FILE)
        list(POP_FRONT THIS_FILE LINE)
        # message("CHECKING LINE: ${LINE}")
        # search for lines of the form:  # DEPENDS_ON: lib_one lib_two lib_three
        if (LINE MATCHES "[ \\t]*#[ \\t]*DEPENDS_ON:?(.+)")
            string(REGEX MATCHALL "[a-zA-Z_0-9]+\ |[a-zA-Z_0-9]+$" DEPS_LIST "${CMAKE_MATCH_1}")
            # string(CONCAT TMP_DEP_LIST ${TMP_DEP_LIST} " " ${DEPS_LIST})
            list(APPEND TMP_DEP_LIST ${DEPS_LIST})
        endif()

        if (LINE MATCHES "[ \\t]*#[ \\t]*${OS_DEP_LIBRARIES}:?(.+)")
            string(REGEX MATCHALL "[a-zA-Z_0-9]+\ |[a-zA-Z_0-9]+$" DEPS_LIST "${CMAKE_MATCH_1}")
            # string(CONCAT TMP_DEP_LIST ${TMP_DEP_LIST} " " ${DEPS_LIST})
            list(APPEND TMP_DEP_LIST ${DEPS_LIST})
        endif()

    endwhile()

    set(${output_variable} ${TMP_DEP_LIST} PARENT_SCOPE)
endfunction()

function(get_library_list cmake_filename output_variable)
    # Ensure the input is a valid file or directory
    if(IS_DIRECTORY ${cmake_filename})
        set(cmake_filename "${cmake_filename}/CMakeLists.txt")
    endif()

    # Read the contents of the CMakeLists.txt file
    if(EXISTS ${cmake_filename})
        file(STRINGS ${cmake_filename} THIS_FILE)
    else()
        message(FATAL_ERROR "CMakeLists.txt file not found: ${cmake_filename}")
        return()
    endif()

    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        # Windows-specific libraries
        set(OS_LIBRARIES "WINDOWS_LIBRARIES")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        # Linux-specific libraries
        set(OS_LIBRARIES "LINUX_LIBRARIES")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        # macOS-specific libraries
        set(OS_LIBRARIES "MAC_LIBRARIES")
    else()
        set(OS_LIBRARIES "UNKOS_LIBRARIES")
    endif()

    set(TMP_DEP_LIST "")

    # Parse the file line by line to find the LIBRARIES definition
    foreach(LINE IN LISTS THIS_FILE)
        # Trim leading whitespace
        string(STRIP "${LINE}" LINE)

        # Skip lines that are comments (either starting with # or //)
        if(LINE MATCHES "^#.*" OR LINE MATCHES "^//.*")
            continue()
        endif()


        if(LINE MATCHES "set\\((LIBRARIES|${OS_LIBRARIES}) ([^\\)]+)\\)")
            # Split the matched libraries into a list
            string(REPLACE " " ";" DEPS_LIST "${CMAKE_MATCH_2}")

            if("${CMAKE_MATCH_1}" STREQUAL "LIBRARIES")
                message("Concatenating: ${DEPS_LIST}")
            else()
                message("Concatenating OS Dependent: ${DEPS_LIST}")
            endif()

            # Append to the list
            list(APPEND TMP_DEP_LIST ${DEPS_LIST})
        endif()

    endforeach()

     message("RETURNING: ${TMP_DEP_LIST}")
    set(${output_variable} ${TMP_DEP_LIST} PARENT_SCOPE)

endfunction()



function(resolve_library_dependencies orig_list output_list)

    message("Resolving Library Dependencies")
    message("Original: ${orig_list}")

    set(SCANNED_LIBS "")
    set(LIBS_TO_SCAN ${orig_list})

    while(LIBS_TO_SCAN)
        list(GET LIBS_TO_SCAN 0 LIBRARY)
        string(STRIP "${LIBRARY}" LIBRARY)
        list(REMOVE_AT LIBS_TO_SCAN 0)

        if (${LIBRARY} IN_LIST SCANNED_LIBS)
            # message("Already Processed: ${LIBRARY}")
            continue()
        endif()

        list(APPEND SCANNED_LIBS ${LIBRARY})

        # message("Popped item: ${LIBRARY}")
        get_library_dependencies("${LIBRARY_ABSOLUTE_PATH}/${LIBRARY}" LIB_DEPENDENCIES)

        foreach (DEP ${LIB_DEPENDENCIES})
            string(STRIP "${DEP}" DEP)
            if (NOT ${DEP} IN_LIST SCANNED_LIBS)
                # message("Adding to scan list: ${DEP}")
                list(APPEND LIBS_TO_SCAN ${DEP})
            else()
                # message("Already scanned: ${DEP}")
            endif()
        endforeach()

    endwhile()

    message("After Scanning: ${SCANNED_LIBS}")

    set(${output_list} ${SCANNED_LIBS} PARENT_SCOPE)
endfunction()

function(check_uses_qt CMAKE_FILE RESULT)
    # Ensure the input file exists
    if (NOT EXISTS "${CMAKE_FILE}")
        message(FATAL_ERROR "File ${CMAKE_FILE} does not exist.")
    endif()

    # Read the file content
    file(READ "${CMAKE_FILE}" CMAKE_CONTENT)

    # Check for Qt-related keywords
    string(REGEX MATCH "find_package\\(Qt[^\)]*\\)|target_link_libraries\\([^\\)]*Qt[^\)]*\\)" MATCHED "${CMAKE_CONTENT}")

    # Set the result
    if (MATCHED)
        set(${RESULT} TRUE PARENT_SCOPE)
    else()
        set(${RESULT} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(check_supports_os CMAKE_FILE RESULT)
    # Ensure the input file exists
    if (NOT EXISTS "${CMAKE_FILE}")
        message(FATAL_ERROR "File ${CMAKE_FILE} does not exist.")
    endif()

    # Read the file content
    file(READ "${CMAKE_FILE}" CMAKE_CONTENT)

    # Check for Qt-related keywords
    string(REGEX MATCH "SUPPORTS_OS_${CMAKE_HOST_SYSTEM_NAME}" MATCHED "${CMAKE_CONTENT}")

    # Set the result
    if (MATCHED)
        set(${RESULT} TRUE PARENT_SCOPE)
    else()
        set(${RESULT} FALSE PARENT_SCOPE)
    endif()
endfunction()


