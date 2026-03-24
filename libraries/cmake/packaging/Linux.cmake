# Copy .so files on Linux to the target App build folder.
# For development:
# add_custom_command(TARGET ${NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#   $<TARGET_FILE:SDL2::SDL2>
#   $<TARGET_FILE_DIR:${NAME}>)

# For distribution:
# install(FILES $<TARGET_FILE:SDL2::SDL2>
#   DESTINATION ${CMAKE_INSTALL_BINDIR}
#   RENAME ${NAME}_$<TARGET_FILE_NAME:SDL2::SDL2>)

# Copy assets into app bundle
# For development:
# add_custom_command(TARGET ${NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory
#   ${PROJECT_SOURCE_DIR}/assets
#   $<TARGET_FILE_DIR:${NAME}>/../share)

# # For distribution:
# install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_INSTALL_DATADIR})

# Linux app icon setup
# NAME: binary basename for Exec= (OUTPUT_NAME when set, else CMake target / project name)
get_target_property(_mssm_linux_exec ${PROJECT_NAME} OUTPUT_NAME)
if(NOT _mssm_linux_exec OR _mssm_linux_exec STREQUAL "_mssm_linux_exec-NOTFOUND")
  set(_mssm_linux_exec "${PROJECT_NAME}")
endif()
set(NAME "${_mssm_linux_exec}")

configure_file(
  ${PROJECT_PACKAGING_SOURCE_FOLDER}/Manifests/App.desktop.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.desktop
  @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.desktop
  DESTINATION share/applications)
install(FILES ${ASSETS_ICONS_SOURCE_FOLDER}/BaseAppIcon.png
  DESTINATION share/pixmaps
  RENAME ${PROJECT_NAME}_icon.png)
