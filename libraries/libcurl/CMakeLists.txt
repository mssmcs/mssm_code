set(NAME "libcurl")
# MSSM_LIB_DESC: libcurl HTTP/HTTPS client (include <curl/curl.h>)

if(WIN32)
    set(curl_schannel ON)
    set(curl_openssl OFF)
else()
    set(curl_schannel OFF)
    set(curl_openssl ON)
endif()

CPMAddPackage(
    NAME curl
    VERSION 8.23.0
    GITHUB_REPOSITORY curl/curl
    GIT_TAG curl-8_23_0
    GIT_SHALLOW TRUE
    EXCLUDE_FROM_ALL TRUE
    OPTIONS
        "BUILD_CURL_EXE OFF"
        "BUILD_SHARED_LIBS OFF"
        "BUILD_STATIC_LIBS ON"
        "CURL_DISABLE_INSTALL ON"
        "CURL_ENABLE_SSL ON"
        "CURL_USE_SCHANNEL ${curl_schannel}"
        "CURL_USE_OPENSSL ${curl_openssl}"
        "CURL_USE_LIBPSL OFF"
)

# The upstream build provides the libcurl target used by IntroProg apps.
