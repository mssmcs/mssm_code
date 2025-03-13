set(NAME "print_compat")

# Explicitly declare header files so they show up in IDEs
set(HEADER_FILES
    print_compat.h
)

# Check for C++23 print support
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
check_cxx_source_compiles("
    #include <print>
    int main() {
        std::print(\"Hello, {}\", \"World\");
        return 0;
    }
" HAS_CPP23_PRINT)

# Create the print_compat library with explicit headers
add_library(${NAME} INTERFACE ${HEADER_FILES})
target_include_directories(${NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set compiler flags
target_compile_features(${NAME} INTERFACE cxx_std_23)

# IMPORTANT: Pass the detected value to the code
if(HAS_CPP23_PRINT)
    message(STATUS "Using native C++23 print/println")
    target_compile_definitions(${NAME} INTERFACE HAS_CPP23_PRINT=1)
else()
    message(STATUS "C++23 print/println not available, using fmt library as fallback")
    target_compile_definitions(${NAME} INTERFACE HAS_CPP23_PRINT=0)

    # Add fmt dependency using CPM
    if(COMMAND CPMAddPackage)
        CPMAddPackage(
            NAME fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 11.1.3
            GIT_SHALLOW TRUE
            EXCLUDE_FROM_ALL TRUE
        )

        # Explicitly make sure fmt's include directories are propagated
        target_include_directories(${NAME} INTERFACE
            $<TARGET_PROPERTY:fmt::fmt,INTERFACE_INCLUDE_DIRECTORIES>
        )
    else()
        # If CPM is not available, try to find fmt on the system
        find_package(fmt REQUIRED)
    endif()

    # Link fmt with print_compat
    target_link_libraries(${NAME} INTERFACE fmt::fmt)
endif()

# Optional: Expose the HAS_CPP23_PRINT variable to parent scope
set(HAS_CPP23_PRINT ${HAS_CPP23_PRINT} PARENT_SCOPE)
