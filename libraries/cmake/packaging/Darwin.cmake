# # Get dynamic SDL2 lib into Frameworks folder in app bundle.
# # For development:

# foreach (FILE ${ASSETS})
#         # add_custom_command(
#         #     TARGET ${PROJECT_NAME} POST_BUILD
#         #     COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
#         #     COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${FILE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/${NEW_FILE}"
#         # )
#     # For distribution without XCode:
#     # if (NOT "${CMAKE_GENERATOR}" STREQUAL "Xcode")
#     #     message("NotXcode")
#     #   install(FILES ${FILE} DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
#     # endif ()

# endforeach ()

# macOS package settings
string(TIMESTAMP CURR_YEAR "%Y")

# Values must match MACOSX_BUNDLE_* target properties below (used for configure_file).
set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_COMPANY_NAMESPACE}.${PROJECT_NAME}")
set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
set(MACOSX_BUNDLE_BUNDLE_VERSION "${FULL_PROJECT_VERSION}")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${SHORT_PROJECT_VERSION}")
set(MACOSX_BUNDLE_COPYRIGHT "(c) ${CURR_YEAR} ${PROJECT_COMPANY_NAME}")

# TCC requires NSCameraUsageDescription only when the camera capture stack is linked.
# LIBRARIES is the fully resolved list (see processProjectLibraries.cmake).
set(CAMERA_USAGE_ENTRIES "")
if("camera" IN_LIST LIBRARIES)
  set(CAMERA_USAGE_ENTRIES "    <key>NSCameraUsageDescription</key>\n    <string>This application uses the camera for live preview and capture.</string>\n")
endif()

set(_mssm_macos_info_out "${CMAKE_CURRENT_BINARY_DIR}/mssm_${PROJECT_NAME}_Info.plist")
configure_file(
  "${PROJECT_PACKAGING_SOURCE_FOLDER}/Manifests/Info.plist"
  "${_mssm_macos_info_out}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  MACOSX_BUNDLE TRUE
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
  MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
  MACOSX_BUNDLE_BUNDLE_VERSION "${FULL_PROJECT_VERSION}"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${SHORT_PROJECT_VERSION}"
  MACOSX_BUNDLE_INFO_PLIST "${_mssm_macos_info_out}"
  MACOSX_BUNDLE_GUI_IDENTIFIER "${MACOSX_BUNDLE_GUI_IDENTIFIER}"
  MACOSX_BUNDLE_COPYRIGHT "${MACOSX_BUNDLE_COPYRIGHT}"
  INSTALL_RPATH @executable_path/../Frameworks
  RESOURCE "${MACOSX_STATIC_ASSETS}"
)

