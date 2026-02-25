set(NAME "opencv")
# MSSM_LIB_DESC: OpenCV computer vision library (core/imgproc/imgcodecs/highgui/videoio)

# Allow projects to override the module set if they need less/more.
if(NOT DEFINED MSSM_OPENCV_COMPONENTS)
    set(MSSM_OPENCV_COMPONENTS core imgproc imgcodecs highgui videoio)
endif()

option(MSSM_OPENCV_ALLOW_CPM_ON_WINDOWS
    "Allow CPM source-build fallback for OpenCV on Windows toolchains"
    ON
)

# Prefer a preinstalled OpenCV (system package, vcpkg, etc.) for faster configure/build.
find_package(OpenCV QUIET COMPONENTS ${MSSM_OPENCV_COMPONENTS})

if(OpenCV_FOUND)
    message(STATUS "Using system OpenCV: ${OpenCV_VERSION}")
else()
    if(WIN32 AND NOT MSSM_OPENCV_ALLOW_CPM_ON_WINDOWS)
        message(FATAL_ERROR
            "OpenCV was not found on this Windows setup.\n"
            "Install OpenCV and/or set OpenCV_DIR so find_package(OpenCV ...) succeeds,\n"
            "or allow source-build fallback with:\n"
            "  -DMSSM_OPENCV_ALLOW_CPM_ON_WINDOWS=ON"
        )
    endif()

    message(STATUS "System OpenCV not found; fetching with CPM (this may take a while).")

    # Repo defaults to C23, but OpenCV's bundled third-party C code is more
    # broadly compatible with C11 on MinGW.
    set(_mssm_prev_c_standard "${CMAKE_C_STANDARD}")
    set(_mssm_prev_c_ext "${CMAKE_C_EXTENSIONS}")
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_EXTENSIONS ON)

    CPMAddPackage(
        NAME opencvSrc
        GITHUB_REPOSITORY opencv/opencv
        GIT_TAG 4.10.0
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL TRUE
        OPTIONS
            # Prefer static OpenCV in CPM fallback mode so apps "just work"
            # without runtime DLL path setup in IDE run environments.
            "BUILD_SHARED_LIBS OFF"
            "BUILD_LIST=core,imgproc,imgcodecs,highgui,videoio"
            "BUILD_TESTS OFF"
            "BUILD_PERF_TESTS OFF"
            "BUILD_EXAMPLES OFF"
            "BUILD_opencv_apps OFF"
            "BUILD_JAVA OFF"
            "BUILD_opencv_python OFF"
            "BUILD_opencv_java_bindings_generator OFF"
            "BUILD_opencv_js_bindings_generator OFF"
            "INSTALL_CREATE_DISTRIB OFF"
    )

    set(CMAKE_C_STANDARD "${_mssm_prev_c_standard}")
    set(CMAKE_C_EXTENSIONS "${_mssm_prev_c_ext}")
endif()

add_library(${NAME} INTERFACE)

if(OpenCV_FOUND)
    # OpenCV package exports imported targets (OpenCV::...) and compatibility vars.
    set(_opencv_targets "")
    foreach(_mod IN LISTS MSSM_OPENCV_COMPONENTS)
        if(TARGET OpenCV::${_mod})
            list(APPEND _opencv_targets OpenCV::${_mod})
        endif()
    endforeach()

    if(_opencv_targets)
        target_link_libraries(${NAME} INTERFACE ${_opencv_targets})
        if(OpenCV_INCLUDE_DIRS)
            target_include_directories(${NAME} INTERFACE ${OpenCV_INCLUDE_DIRS})
        endif()
    else()
        target_include_directories(${NAME} INTERFACE ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(${NAME} INTERFACE ${OpenCV_LIBS})
    endif()
else()
    # Ensure downstream targets always see OpenCV headers in CPM fallback mode.
    if(DEFINED opencvSrc_SOURCE_DIR)
        target_include_directories(${NAME} INTERFACE "${opencvSrc_SOURCE_DIR}/include")
        # OpenCV source layout keeps many public headers under modules/<name>/include.
        target_include_directories(${NAME} INTERFACE "${opencvSrc_SOURCE_DIR}/modules/core/include")
        foreach(_mod IN LISTS MSSM_OPENCV_COMPONENTS)
            if(EXISTS "${opencvSrc_SOURCE_DIR}/modules/${_mod}/include")
                target_include_directories(${NAME} INTERFACE "${opencvSrc_SOURCE_DIR}/modules/${_mod}/include")
            endif()
        endforeach()
    endif()
    if(DEFINED opencvSrc_BINARY_DIR)
        target_include_directories(${NAME} INTERFACE "${opencvSrc_BINARY_DIR}")
    endif()
    # OpenCV generates opencv2/opencv_modules.hpp in the top-level app build dir.
    target_include_directories(${NAME} INTERFACE "${CMAKE_BINARY_DIR}")

    # CPM OpenCV build exposes opencv_<module> targets.
    foreach(_mod IN LISTS MSSM_OPENCV_COMPONENTS)
        if(TARGET opencv_${_mod})
            target_link_libraries(${NAME} INTERFACE opencv_${_mod})
        endif()
    endforeach()
endif()
