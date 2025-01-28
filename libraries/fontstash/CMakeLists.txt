set(NAME "fontstash")


# LIBRARY_EXAMPLE: headers-only pulled from github with commit hash
# gets zip archive because GIT_SHALLOW doesn't work with
# arbitrary commit hashes

set(COMMIT_HASH "b5ddc9741061343740d85d636d782ed3e07cf7be")

CPMAddPackage(
    NAME  fontstash
    URL  https://github.com/memononen/fontstash/archive/${COMMIT_HASH}.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

add_library(fontstash INTERFACE)

target_include_directories(
    fontstash
    INTERFACE
    $<BUILD_INTERFACE:${fontstash_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${fontstash_SOURCE_DIR}/src>
)

