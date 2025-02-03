set(NAME "vulk")
# DEPENDS_ON: stbi image window color




CPMAddPackage(
    NAME  Vulkan-Headers
    GIT_REPOSITORY  https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG v1.3.298
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)


CPMAddPackage(
    NAME  Volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG 1.3.295
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    # DOWNLOAD_ONLY   TRUE
)

add_library(Volk INTERFACE)



target_include_directories(
    Volk
    INTERFACE
    $<BUILD_INTERFACE:${Volk_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${Volk_SOURCE_DIR}>
)



# https://github.com/StableCoder/cmake-scripts?tab=readme-ov-file#glsl-shader-file-targeted-compilationglsl-shaderscmake

# include(${PROJECT_SOURCE_DIR}/cmake/glsl-shaders.cmake)

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC

vulkinstance.h vulkinstance.cpp
vulkdevice.h vulkdevice.cpp
vulkutil.h
vk_enum_string_helper.h

vulkmemory.h vulkmemory.cpp
vulkbuffer.h vulkbuffer.cpp

vulkcommandbuffers.h vulkcommandbuffers.cpp

vulkimage.h vulkimage.cpp
vulkimageview.h vulkimageview.cpp

vulkStbi.h vulkStbi.cpp


vulksurface.h vulksurface.cpp
vulkswapchain.h vulkswapchain.cpp
# vfontrenderer.h vfontrenderer.cpp

vulkpipeline.h vulkpipeline.cpp
vulkshaders.h vulkshaders.cpp
vulksynchronization.h vulksynchronization.cpp
vulkrenderpass.h vulkrenderpass.cpp
vulkframebuffer.h vulkframebuffer.cpp
vulkvertex.h vulkvertex.cpp
vulkabstractwindow.h vulkabstractwindow.cpp
vulksurfacerendermanager.h vulksurfacerendermanager.cpp
# vulkangraphicswindow.h vulkangraphicswindow.cpp
# vulkcanvas.h vulkcanvas.cpp
vulkdescriptorset.h vulkdescriptorset.cpp

vulksmartbuffer.h vulksmartbuffer.cpp
vulkbufferpool.h vulkbufferpool.cpp

vulkcanvasbase.h vulkcanvasbase.cpp
vulkwindow.h vulkwindow.cpp
# meshloader.cpp
# meshloader.h
# tiny_obj_loader.h
# halfedgeloader.cpp
# halfedgeloader.h
# linmath.h
# mesh.h
# loopiterable.h
# geometry.h
)


target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${NAME} PUBLIC 
Vulkan::Headers
Volk
stbi
image_base
mssm_window
mssm_color
# mssm_graphics 
# poly_partition
)

# Platform-specific definitions
if(WIN32)
    # Windows-specific macro for Vulkan
    target_compile_definitions(${NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR)
elseif(UNIX AND NOT APPLE)
    # Linux-specific macro for Vulkan (X11 is most common, but can also be XCB)
    target_compile_definitions(${NAME} PUBLIC VK_USE_PLATFORM_XCB_KHR)
elseif(APPLE)
    # macOS-specific macro for Vulkan
    target_compile_definitions(${NAME} PUBLIC VK_USE_PLATFORM_MACOS_MVK)
endif()


