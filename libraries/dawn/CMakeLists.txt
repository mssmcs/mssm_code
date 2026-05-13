set(NAME "mssm_dawn")

if (NOT WIN32)
    message(FATAL_ERROR "mssm_dawn prebuilt mode currently supports Windows only.")
endif()

set(MSSM_DAWN_VERSION "127.0.6535")
set(MSSM_DAWN_ARCHIVE_NAME "webgpu-dawn-127.0.6535-win64.zip")
set(MSSM_DAWN_URL "https://github.com/jspanchu/webgpu-dawn-binaries/releases/download/v127.0.6535.0/${MSSM_DAWN_ARCHIVE_NAME}")
set(MSSM_DAWN_SHA256 "2D4370B125E3134AFC9BAAB754F21D0469EDD60C9ED1996D2961D7383C6D47F6")

set(_dawn_prebuilt_cache_dir "${CMAKE_BINARY_DIR}/_prebuilt/dawn")
set(_dawn_archive "${_dawn_prebuilt_cache_dir}/${MSSM_DAWN_ARCHIVE_NAME}")
set(_dawn_extract_root "${_dawn_prebuilt_cache_dir}/extracted")
set(_dawn_package_root "${_dawn_extract_root}/webgpu-dawn-127.0.6535-win64")

file(MAKE_DIRECTORY "${_dawn_prebuilt_cache_dir}")

if (NOT EXISTS "${_dawn_archive}")
    message(STATUS "mssm_dawn: downloading Dawn prebuilt ${MSSM_DAWN_ARCHIVE_NAME}")
    file(DOWNLOAD
        "${MSSM_DAWN_URL}"
        "${_dawn_archive}"
        EXPECTED_HASH "SHA256=${MSSM_DAWN_SHA256}"
        SHOW_PROGRESS
        TLS_VERIFY ON
    )
else()
    message(STATUS "mssm_dawn: using cached Dawn prebuilt archive ${_dawn_archive}")
endif()

if (NOT EXISTS "${_dawn_package_root}/lib/cmake/dawn/dawnConfig.cmake")
    message(STATUS "mssm_dawn: extracting Dawn prebuilt package")
    file(ARCHIVE_EXTRACT
        INPUT "${_dawn_archive}"
        DESTINATION "${_dawn_extract_root}"
    )
endif()

find_package(dawn CONFIG REQUIRED
    PATHS "${_dawn_package_root}/lib/cmake/dawn"
    NO_DEFAULT_PATH
)

add_library(${NAME} INTERFACE)

if (TARGET dawn::dawn)
    target_link_libraries(${NAME} INTERFACE dawn::dawn)
else()
    message(FATAL_ERROR "Unable to find expected target dawn::dawn from prebuilt Dawn package.")
endif()

target_compile_features(${NAME} INTERFACE cxx_std_20)

message(STATUS "mssm_dawn: linked prebuilt Dawn target into ${NAME}")

