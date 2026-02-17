include(${CMAKE_SOURCE_DIR}/../libraries/cmake/util.cmake)

function(escape_cmake_path path result)
    # Escape parentheses with backslashes
    string(REGEX REPLACE "\\(" "\\\\(" escaped_path "${path}")
    string(REGEX REPLACE "\\)" "\\\\)" escaped_path "${escaped_path}")
    set(${result} "${escaped_path}" PARENT_SCOPE)
endfunction()

# Assets for all platforms
# set(STATIC_ASSETS ${PROJECT_SOURCE_DIR}/assets/fonts/Manrope.ttf)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

get_filename_component(MSSM_CODE_ROOT "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)

if(EXISTS "${PROJECT_SOURCE_DIR}/assets")
    # look for project level assets
    set(ASSETS_SOURCE_FOLDER "${PROJECT_SOURCE_DIR}/assets")
else()
    # revert to mssm_code assets
    set(ASSETS_SOURCE_FOLDER "${MSSM_CODE_ROOT}/assets")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/assets/vulkan")
    # look for project level assets
    set(ASSETS_VULKAN_SOURCE_FOLDER "${PROJECT_SOURCE_DIR}/assets/vulkan")
    set(ASSETS_VULKAN_PARENT_FOLDER "${PROJECT_SOURCE_DIR}/assets")
else()
    # revert to mssm_code assets
    set(ASSETS_VULKAN_SOURCE_FOLDER "${MSSM_CODE_ROOT}/assets/vulkan")
    set(ASSETS_VULKAN_PARENT_FOLDER "${MSSM_CODE_ROOT}/assets")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/assets/icons")
    # look for project level icons
    set(ASSETS_ICONS_SOURCE_FOLDER "${PROJECT_SOURCE_DIR}/assets/icons")
else()
    # revert to mssm_code icons
    set(ASSETS_ICONS_SOURCE_FOLDER "${MSSM_CODE_ROOT}/assets/icons")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/packaging")
    # look for project level packaging info
    set(PROJECT_PACKAGING_SOURCE_FOLDER "${PROJECT_SOURCE_DIR}/packaging")
else()
    # revert to mssm_code packaging info
    set(PROJECT_PACKAGING_SOURCE_FOLDER "${MSSM_CODE_ROOT}/packaging")
endif()

# Set up the directory structure based on platform
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Create a project-specific folder inside bin for Linux
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}"
    )

    # Set assets directory for Linux
    set(TARGET_ASSET_DIR "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/assets")
elseif(WIN32)
    # For Windows, create a project-specific folder structure similar to Linux
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}"
    )

    # Set assets to be in a subfolder of the project folder
    set(TARGET_ASSET_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/assets")
else()
    # For other platforms (like macOS)
    set(TARGET_ASSET_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
endif()

# Platform specific static assets
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(${PROJECT_NAME} PUBLIC
    ${STATIC_ASSETS}
    ${ASSETS_ICONS_SOURCE_FOLDER}/icon.ico
    ${PROJECT_PACKAGING_SOURCE_FOLDER}/Manifests/app.rc
    ${PROJECT_PACKAGING_SOURCE_FOLDER}/Manifests/App.manifest)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(MACOSX_STATIC_ASSETS
    ${STATIC_ASSETS}
    ${ASSETS_ICONS_SOURCE_FOLDER}/icon.icns)
  target_sources(${PROJECT_NAME} PUBLIC ${MACOSX_STATIC_ASSETS} ${ASSETS})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(${PROJECT_NAME} PRIVATE ${STATIC_ASSETS})
endif ()

if(EXISTS "${ASSETS_SOURCE_FOLDER}")

    # --- Packaging Step ---
    # Install the entire assets directory for the package/installer.
    # This is the robust way to handle directory copying for CPack.
    # The trailing slash on the source folder is important - it copies contents.
    install(
        DIRECTORY ${ASSETS_SOURCE_FOLDER}/
        DESTINATION "assets"
        COMPONENT ${PROJECT_NAME}
    )

    # --- Local Build / IDE Step ---
    # The following logic ensures assets are copied to the build directory
    # so the application can be run directly from the IDE.

    # Build a list of all individual asset files
    file(GLOB_RECURSE ALL_ASSET_PATHS "${ASSETS_SOURCE_FOLDER}/*")
    filter_regex(EXCLUDE "\\.DS_Store" ALL_ASSET_PATHS ${ALL_ASSET_PATHS})

    # Filter out directories from the list, keeping only files
    set(ASSETS "")
    foreach(path ${ALL_ASSET_PATHS})
        if(NOT IS_DIRECTORY "${path}")
            list(APPEND ASSETS "${path}")
        endif()
    endforeach()

    # Add asset files to the project for visibility in the IDE
    foreach (FILE ${ASSETS})
        # make sure these files don't actually get compiled
        # (wavefront obj mesh files were being treated as compiled files to be linked)
        set_source_files_properties(${FILE} PROPERTIES HEADER_FILE_ONLY TRUE)
    endforeach ()
    target_sources(${PROJECT_NAME} PUBLIC ${ASSETS})

    # Loop over asset files and set up copy commands for local build
    foreach (FILE ${ASSETS})
        # Get the relative path from the data-folder to the particular file.
        file(RELATIVE_PATH NEW_FILE "${ASSETS_SOURCE_FOLDER}" ${FILE})

        # Get the relative path to the file.
        get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)

        # MAC:  Set its location inside the app package (under Resources).
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${NEW_FILE_PATH}")

        # This custom command copies files for local runs (non-Apple)
        if(NOT APPLE)
            if(WIN32)
                add_custom_command(
                    TARGET ${PROJECT_NAME} PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${TARGET_ASSET_DIR}/${NEW_FILE_PATH}"
                )
                add_custom_command(
                    TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILE}" "${TARGET_ASSET_DIR}/${NEW_FILE}"
                )
            else()
                # For Linux and other non-Windows/non-Apple platforms
                escape_cmake_path("${FILE}" ESCAPED_FILE)
                escape_cmake_path("${TARGET_ASSET_DIR}/${NEW_FILE_PATH}" ESCAPED_DEST_PATH)
                escape_cmake_path("${TARGET_ASSET_DIR}/${NEW_FILE}" ESCAPED_DEST_FILE)

                add_custom_command(
                    TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${TARGET_ASSET_DIR}/${NEW_FILE_PATH}"
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ESCAPED_FILE}" "${ESCAPED_DEST_FILE}"
                )
            endif()
        endif()

        # Make sure it shows up in the IDE Assets folder
        source_group("Assets/${NEW_FILE_PATH}" FILES "${FILE}")

    endforeach ()

endif()

if ("vulkan" IN_LIST LIBRARIES)
    message("Vulkan library being used: setting up shader compilation...")

    file(GLOB_RECURSE GLSL_SOURCE_FILES "${ASSETS_VULKAN_SOURCE_FOLDER}/*.glsl")

    include(${CMAKE_SOURCE_DIR}/../libraries/cmake/glsl-shaders.cmake)

    target_glsl_shaders(${PROJECT_NAME} PRIVATE "${GLSL_SOURCE_FILES}" COMPILE_OPTIONS --target-env vulkan1.1)

    foreach (FILE ${GLSL_SOURCE_FILES})

        set(ORIG_FILE ${FILE})
        set(FILE "${FILE}.spv")

        message("Processing GLSL file: ${FILE}")

        # Get the relative path from the data-folder to the particular file.
        file(RELATIVE_PATH NEW_FILE "${ASSETS_VULKAN_PARENT_FOLDER}" ${FILE})

        # Get the relative path to the file.
        get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)

        # MAC:  Set it's location inside the app package (under Resources).
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${NEW_FILE_PATH}")

        install(FILES ${FILE} DESTINATION ${CMAKE_INSTALL_BINDIR}/assets/${NEW_FILE_PATH} COMPONENT ${PROJECT_NAME})

    #    message("Setting property")

        if(NOT APPLE)
            if(WIN32)
                # Create a unique and valid identifier for each file (no special chars)
                string(MAKE_C_IDENTIFIER "${NEW_FILE}" SAFE_FILE_ID)

                # Create the directory for the file
                add_custom_command(
                    TARGET ${PROJECT_NAME} PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${TARGET_ASSET_DIR}/${NEW_FILE_PATH}"
                )

                # Copy the file - use direct file path to avoid batch script escaping issues
                add_custom_command(
                    TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILE}" "${TARGET_ASSET_DIR}/${NEW_FILE}"
                )
            else()
                # For Linux and other non-Windows/non-Apple platforms
                escape_cmake_path("${FILE}" ESCAPED_FILE)
                escape_cmake_path("${TARGET_ASSET_DIR}/${NEW_FILE_PATH}" ESCAPED_DEST_PATH)
                escape_cmake_path("${TARGET_ASSET_DIR}/${NEW_FILE}" ESCAPED_DEST_FILE)

                add_custom_command(
                    TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${TARGET_ASSET_DIR}/${NEW_FILE_PATH}"
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ESCAPED_FILE}" "${ESCAPED_DEST_FILE}"
                )
            endif()
        endif()

        # message("Assets/${NEW_FILE} FILE ${FILE}")
        # Make sure it shows up in the IDE Assets folder
        source_group("Assets/${NEW_FILE_PATH}" FILES "${FILE}")

    endforeach ()
endif()
