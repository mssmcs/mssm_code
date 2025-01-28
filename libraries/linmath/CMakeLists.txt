set(NAME "mssm_linmath")

set(COMMIT_HASH "3eef82841046507e16a0f6194a61cee2eadd34b3")

CPMAddPackage(
    NAME  ${NAME}
    URL  https://github.com/datenwolf/linmath.h/archive/${COMMIT_HASH}.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
)

add_library(${NAME} INTERFACE)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(
    ${NAME}
    INTERFACE
    $<BUILD_INTERFACE:${${NAME}_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${${NAME}_SOURCE_DIR}>
)
