set(NAME "json")

# # nlohmann json

# # include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

# CPMAddPackage(
#     NAME  json
#     GIT_REPOSITORY  https://github.com/nlohmann/json.git
#     GIT_TAG v3.11.3
#     GIT_SHALLOW     TRUE
#     EXCLUDE_FROM_ALL TRUE 
#     OPTIONS "JSON_BuildTests OFF" "JSON_MultipleHeaders OFF" "JSON_Install OFF"
# )

# add_library(${NAME} INTERFACE)
# target_link_libraries(${NAME} INTERFACE nlohmann_json::nlohmann_json)

CPMAddPackage(
    NAME  json
    URL  https://github.com/nlohmann/json/releases/download/v3.11.3/include.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY   TRUE
)

add_library(json INTERFACE)

target_include_directories(
    json
    INTERFACE
    $<BUILD_INTERFACE:${json_SOURCE_DIR}/single_include>
    $<INSTALL_INTERFACE:${json_SOURCE_DIR}/single_include>
)