set(NAME "http")
# DEPENDS_ON: libcurl
# MSSM_LIB_DESC: Asynchronous HTTP/HTTPS requests (include "http.h")

find_package(Threads REQUIRED)

add_library(${NAME} STATIC http.cpp http.h)
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PUBLIC cxx_std_17)
target_link_libraries(${NAME} PRIVATE libcurl Threads::Threads)
