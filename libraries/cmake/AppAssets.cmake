include(${CMAKE_SOURCE_DIR}/../libraries/cmake/util.cmake)

# Assets for all platforms
# set(SHARED_STATIC_ASSETS ${PROJECT_SOURCE_DIR}/assets/fonts/Manrope.ttf)

# Platform specific static assets
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(${PROJECT_NAME} PUBLIC
    ${SHARED_STATIC_ASSETS}
    ${PROJECT_SOURCE_DIR}/assets/icons/icon.ico
    ${PROJECT_SOURCE_DIR}/packaging/Manifests/app.rc
    ${PROJECT_SOURCE_DIR}/packaging/Manifests/App.manifest)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(MACOSX_STATIC_ASSETS
    ${SHARED_STATIC_ASSETS}
    ${PROJECT_SOURCE_DIR}/assets/icons/icon.icns)
  target_sources(${PROJECT_NAME} PUBLIC ${MACOSX_STATIC_ASSETS} ${ASSETS})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(${PROJECT_NAME} PRIVATE ${SHARED_STATIC_ASSETS})
endif ()

# build list of assets
set(ASSETS_SOURCE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/assets")
file(GLOB_RECURSE ASSETS "${ASSETS_SOURCE_FOLDER}/*")
filter_regex(EXCLUDE "\\.DS_Store" ASSETS ${ASSETS})

foreach (FILE ${ASSETS})
    # make sure these files don't actually get compiled
    # (wavefront obj mesh files were being treated as compiled files to be linked)
    set_source_files_properties(${FILE} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach ()

target_sources(${PROJECT_NAME} PUBLIC ${ASSETS})

# loop over assets and make sure they get copied to the executable location
foreach (FILE ${ASSETS})

    # message("Procesing file: ${FILE}")

    # Get the relative path from the data-folder to the particular file.
    file(RELATIVE_PATH NEW_FILE "${ASSETS_SOURCE_FOLDER}" ${FILE})

    # Get the relative path to the file.
    get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)

    # MAC:  Set it's location inside the app package (under Resources).
    set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${NEW_FILE_PATH}")

    install(FILES ${FILE} DESTINATION ${CMAKE_INSTALL_BINDIR}/assets/${NEW_FILE_PATH})

    # message("NEWFILE PATH: ${NEW_FILE_PATH}")

    if(NOT APPLE)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${FILE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/${NEW_FILE}"
        )

    endif()

    # message("Assets/${NEW_FILE} FILE ${FILE}")
    # Make sure it shows up in the IDE Assets folder
    source_group("Assets/${NEW_FILE_PATH}" FILES "${FILE}")

endforeach ()

if ("vulkan" IN_LIST LIBRARIES)
    message("Vulkan library being used: setting up shader compilation...")

    set(ASSETS_SOURCE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(GLOB_RECURSE GLSL_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/vulkan/*.glsl")

    include(${CMAKE_SOURCE_DIR}/../libraries/cmake/glsl-shaders.cmake)

    target_glsl_shaders(${PROJECT_NAME} PRIVATE "${GLSL_SOURCE_FILES}" COMPILE_OPTIONS --target-env vulkan1.1)

    foreach (FILE ${GLSL_SOURCE_FILES})

        set(ORIG_FILE ${FILE})
        set(FILE "${FILE}.spv")

        message("Processing GLSL file: ${FILE}")

        # Get the relative path from the data-folder to the particular file.
        file(RELATIVE_PATH NEW_FILE "${ASSETS_SOURCE_FOLDER}" ${FILE})

        # Get the relative path to the file.
        get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)

        # MAC:  Set it's location inside the app package (under Resources).
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${NEW_FILE_PATH}")

        install(FILES ${FILE} DESTINATION ${CMAKE_INSTALL_BINDIR}/assets/${NEW_FILE_PATH})

    #    message("Setting property")

        if(NOT APPLE)
            add_custom_command(
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${FILE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/${NEW_FILE}"
            )
        endif()

        # message("Assets/${NEW_FILE} FILE ${FILE}")
        # Make sure it shows up in the IDE Assets folder
        source_group("Assets/${NEW_FILE_PATH}" FILES "${FILE}")

    endforeach ()
endif()

# # SEE THIS! https://discourse.cmake.org/t/how-to-add-resources-to-macos-bundle/9323
# if(APPLE)
#     # Bundling macOS application
#     set_target_properties(${PROJECT_NAME} PROPERTIES
#         BUNDLE True
#         MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
#         MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
#         MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME}
#         MACOSX_BUNDLE_ICON_FILE AppIcon
#         MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
#     )
#     set_source_files_properties(${SHADERS}
#         PROPERTIES
#         MACOSX_PACKAGE_LOCATION "Resources/shaders"
#     )
#     set_source_files_properties(${MODELS}
#         PROPERTIES
#         MACOSX_PACKAGE_LOCATION "Resources/models"
#     )
#     install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)
# endif()

# # ---------------

# # fonts
# set(resource_files
#     ${CMAKE_SOURCE_DIR}/JetBrainsMono-ExtraLight.ttf
# )

# # icon
# set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
# set(application_icon "${CMAKE_SOURCE_DIR}/${MACOSX_BUNDLE_ICON_FILE}")
# set_source_files_properties(${application_icon}
#     PROPERTIES
#         MACOSX_PACKAGE_LOCATION "Resources"
# )

# # configs
# file(GLOB_RECURSE json_configs "${CMAKE_SOURCE_DIR}/configs/*.json")
# foreach (FILE ${json_configs})
#     file(RELATIVE_PATH NEW_FILE "${CMAKE_SOURCE_DIR}/configs" ${FILE})
#     get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)
#     set_source_files_properties(${FILE}
#         PROPERTIES
#             MACOSX_PACKAGE_LOCATION "Resources/configs/${NEW_FILE_PATH}"
#     )
# endforeach()

# add_executable(${PROJECT_NAME}
#     MACOSX_BUNDLE
#     ${application_icon}
#     # bundle resources
#     "${resource_files}"
#     "${json_configs}"
# )

# set_target_properties(${PROJECT_NAME}
#     PROPERTIES # https://cmake.org/cmake/help/latest/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html
#         MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}" # CFBundleIdentifier
#         MACOSX_BUNDLE_GUI_IDENTIFIER "com.our-company"
#         MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION} # CFBundleLongVersionString, deprecated
#         MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION} # CFBundleShortVersionString
#         # and bundle resources again
#         RESOURCE "${resource_files}"
# )

# # ...


# include(GNUInstallDirs)

# install(TARGETS ${PROJECT_NAME}
#     BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
# )


# # On linux we need to ship the system libraries
# if(UNIX AND NOT APPLE)
#     get_filename_component(GCC_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)
#     find_library(LIBGCC NAMES libgcc_s.so.1 HINTS ${GCC_PATH}/../lib64)
#     get_filename_component(LIBGCC_DIR ${LIBGCC} DIRECTORY)
#     file(GLOB LIBGCC_ALL ${LIBGCC_DIR}/libgcc*.so.1)
#     find_library(STDCPP NAMES libstdc++.so.6 HINTS ${GCC_PATH}/../lib64)
#     file(GLOB STDCPP_ALL ${STDCPP}*)
#     set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
#     ${LIBGCC_ALL}
#     ${STDCPP_ALL})
# endif()

# # We need to ship libraries on windows, too, with slightly tweaked file names
# if(WIN32 AND NOT MSVC)
#     GET_FILENAME_COMPONENT(GCC_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)
#     FIND_LIBRARY(LIBGCC NAMES libgcc_s_seh-1.dll HINTS ${GCC_PATH})
#     FIND_LIBRARY(STDCPP NAMES libstdc++-6.dll HINTS ${GCC_PATH})
#     FIND_LIBRARY(LIBSZIP NAMES libszip-0.dll HINTS ${GCC_PATH})
#     FIND_LIBRARY(ZLIB NAMES zlib1.dll HINTS ${GCC_PATH})
#     FIND_LIBRARY(LIPWINPTHREAD NAMES libwinpthread-1.dll HINTS ${GCC_PATH})
#     SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
#     ${LIBGCC}
#     ${STDCPP}
#     ${LIBSZIP}
#     ${ZLIB}
#     ${LIPWINPTHREAD}
#     ${CMAKE_THREAD_LIBS_INIT})
# endif()

# message(${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
