
# if (MINGW)
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
#   # Static link MinGW standard libraries
#   # set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
#   # set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
# else ()
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /entry:mainCRTStartup")
# endif ()

# Copy .dll files on Windows to the target App build folder.
# For development:
# add_custom_command(TARGET ${NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#   $<TARGET_FILE:SDL2::SDL2>
#   $<TARGET_FILE_DIR:${NAME}>)

# For distribution:
# install(FILES $<TARGET_FILE:SDL2::SDL2> DESTINATION ${CMAKE_INSTALL_BINDIR})

option(MSSM_WINDOWS_AUTO_RUNTIME_DEPS "Automatically package runtime DLL dependencies on Windows." ON)

# Automatically gather and install transitive runtime DLL dependencies
# for this app target into the same component.
if(MSSM_WINDOWS_AUTO_RUNTIME_DEPS)
  set(_mssm_runtime_dep_dirs "")
  set(_mssm_runtime_dep_dir_args "")
  if(MINGW)
    get_filename_component(_mssm_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    list(APPEND _mssm_runtime_dep_dirs "${_mssm_mingw_bin_dir}")
  endif()
  if(_mssm_runtime_dep_dirs)
    list(APPEND _mssm_runtime_dep_dir_args DIRECTORIES ${_mssm_runtime_dep_dirs})
  endif()

  install(
    RUNTIME_DEPENDENCY_SET "${PROJECT_NAME}_runtime_deps"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
    ${_mssm_runtime_dep_dir_args}
    PRE_EXCLUDE_REGEXES
      "api-ms-win-.*"
      "ext-ms-.*"
    POST_EXCLUDE_REGEXES
      ".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss]/[Ss]ystem32/.*\\.dll"
      ".*kernel32\\.dll"
      ".*user32\\.dll"
      ".*gdi32\\.dll"
      ".*advapi32\\.dll"
      ".*shell32\\.dll"
      ".*ole32\\.dll"
      ".*oleaut32\\.dll"
      ".*ws2_32\\.dll"
      ".*comdlg32\\.dll"
      ".*winmm\\.dll"
  )
endif()

# Bundle MinGW runtime DLLs so packaged executables run on machines
# without a MinGW toolchain installed.
if(MINGW)
  get_filename_component(_mssm_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)

  foreach(_mssm_runtime_dll
      libgcc_s_seh-1.dll
      libstdc++-6.dll
      libwinpthread-1.dll)
    set(_mssm_runtime_dll_path "${_mssm_mingw_bin_dir}/${_mssm_runtime_dll}")
    if(EXISTS "${_mssm_runtime_dll_path}")
      install(
        FILES "${_mssm_runtime_dll_path}"
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT ${PROJECT_NAME}
      )
    endif()
  endforeach()
endif()

# Copy assets into app bundle
# For development:
# add_custom_command(TARGET ${NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory
#   ${PROJECT_SOURCE_DIR}/assets
#   $<TARGET_FILE_DIR:${NAME}>/../share)

# # For distribution:
# install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_INSTALL_DATADIR})
