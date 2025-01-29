set(NAME "csv_reader")

set(COMMIT_HASH "758f5665705a38da55322511d5b22fe2679918fe")

CPMAddPackage(
    NAME  ${NAME}
    URL  https://github.com/ben-strasser/fast-cpp-csv-parser/archive/${COMMIT_HASH}.zip
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
