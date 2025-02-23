set(NAME "inja")

# DEPENDS_ON: json

CPMAddPackage(
    NAME  inja
    URL  https://github.com/pantor/inja/releases/download/v3.4.0/inja.hpp
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY   TRUE
    DOWNLOAD_NO_EXTRACT TRUE
)

add_library(inja INTERFACE)

target_include_directories(
    inja
    INTERFACE
    $<BUILD_INTERFACE:${inja_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${inja_SOURCE_DIR}>
)