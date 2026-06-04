# MinGW runtime DLLs required to run executables outside Qt Creator / without
# the compiler bin directory on PATH. No-op on Linux, macOS, and non-MinGW Windows.

set(MSSM_MINGW_RUNTIME_DLL_NAMES
    libgcc_s_seh-1.dll
    libstdc++-6.dll
    libwinpthread-1.dll
)

function(mssm_copy_mingw_runtime_dlls target)
    if(NOT WIN32 OR NOT MINGW)
        return()
    endif()
    if(NOT TARGET ${target})
        message(FATAL_ERROR "mssm_copy_mingw_runtime_dlls: unknown target '${target}'")
    endif()

    get_filename_component(_mssm_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)

    foreach(_mssm_runtime_dll IN LISTS MSSM_MINGW_RUNTIME_DLL_NAMES)
        set(_mssm_runtime_dll_path "${_mssm_mingw_bin_dir}/${_mssm_runtime_dll}")
        if(NOT EXISTS "${_mssm_runtime_dll_path}")
            message(WARNING "MinGW runtime DLL not found, skipping POST_BUILD copy: ${_mssm_runtime_dll_path}")
            continue()
        endif()
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_mssm_runtime_dll_path}"
                "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
    endforeach()
endfunction()
