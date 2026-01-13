set(NAME "camera")


# LIBRARY_EXAMPLE: headers-only pulled from github with commit hash
# gets zip archive because GIT_SHALLOW doesn't work with
# arbitrary commit hashes

set(COMMIT_HASH "32a9bdd3e8e3a31b12cb6573e7c6076208421651")

CPMAddPackage(
    NAME  camera
    URL  https://github.com/openpnp/openpnp-capture/archive/${COMMIT_HASH}.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

add_library(camera STATIC
    ${camera_SOURCE_DIR}/common/libmain.cpp
    ${camera_SOURCE_DIR}/common/context.cpp
    ${camera_SOURCE_DIR}/common/logging.cpp
    ${camera_SOURCE_DIR}/common/stream.cpp
)

target_include_directories(
    ${NAME}
    PUBLIC
    $<BUILD_INTERFACE:${camera_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${camera_SOURCE_DIR}/include>
)

# target_sources(${NAME} PUBLIC
#     graphics/camera.h
#     graphics/camera.cpp
# )

if (WIN32)

    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    target_sources(${NAME} PUBLIC
        ${camera_SOURCE_DIR}/win/platformcontext.cpp
        ${camera_SOURCE_DIR}/win/platformstream.cpp
    )
    target_link_libraries(${NAME} strmiids)
endif()

if (APPLE)

endif()

if (LINUX)
    include_directories(${camera_SOURCE_DIR}/linux/contrib/libjpeg-turbo-dev)
    target_sources(${NAME} PUBLIC
        ${camera_SOURCE_DIR}/linux/platformcontext.cpp
        ${camera_SOURCE_DIR}/linux/platformstream.cpp
        ${camera_SOURCE_DIR}/linux/mjpeghelper.cpp
        ${camera_SOURCE_DIR}/linux/yuvconverters.cpp
    )
            # force include directories for libjpeg-turbo
    #include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/linux/contrib/libjpeg-turbo-dev")
    # add pthreads library
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME}  Threads::Threads)

    # add turbojpeg library
    find_package(PkgConfig REQUIRED)
    pkg_search_module(TurboJPEG libturbojpeg)
    if( TurboJPEG_FOUND )
        link_directories(${TurboJPEG_LIBDIR})
        target_include_directories(${PROJECT_NAME}  ${TurboJPEG_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME}  ${TurboJPEG_LIBRARIES})
    else()
        # compile libjpeg-turbo for MJPEG decoding support
        # right now, we need to disable SIMD because it
        # causes a compile problem.. we need to fix this
        # later...
        set(ENABLE_SHARED OFF)
        set(WITH_SIMD OFF)
        set(TurboJPEG_LIBRARIES turbojpeg-static)
        add_subdirectory(${camera_SOURCE_DIR}/linux/contrib/libjpeg-turbo-dev)
        target_link_libraries(${PROJECT_NAME}  ${TurboJPEG_LIBRARIES})
    endif()
endif()






