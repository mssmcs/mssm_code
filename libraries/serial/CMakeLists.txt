set(NAME "serial")

set(COMMIT_HASH "21b3dfe5f68c205be4086469335fd2fc2ce11ed2")

CPMAddPackage(
    NAME  libserialport
    URL https://github.com/sigrokproject/libserialport/archive/${COMMIT_HASH}.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY   TRUE
)


add_library(libserialport STATIC
    ${libserialport_SOURCE_DIR}/libserialport.h
    ${libserialport_SOURCE_DIR}/libserialport_internal.h
    ${libserialport_SOURCE_DIR}/serialport.c
    ${libserialport_SOURCE_DIR}/libserialport_internal.h
)



add_library(${NAME} STATIC
    serialport.h 
    serialport.cpp 
)

target_compile_definitions(libserialport PUBLIC SP_PRIV=)
target_compile_definitions(${NAME} PUBLIC SP_PRIV=)


if(WIN32)
    target_sources(libserialport
        PUBLIC
        ${libserialport_SOURCE_DIR}/windows.c
        ${libserialport_SOURCE_DIR}/timing.c
    )
    target_link_libraries(libserialport gdi32 opengl32 Cfgmgr32 SetupAPI)

elseif(APPLE)
    target_sources(libserialport
        PUBLIC
        ${libserialport_SOURCE_DIR}/macosx.c
        ${libserialport_SOURCE_DIR}/timing.c
    )

elseif(LINUX)
    target_sources(libserialport
        PUBLIC
        ${libserialport_SOURCE_DIR}/linux.c
        ${libserialport_SOURCE_DIR}/linux_termios.c
        ${libserialport_SOURCE_DIR}/linux_termios.h
#       ${libserialport_SOURCE_DIR}/freebsd.c
    )
endif()

target_include_directories(libserialport PUBLIC ${libserialport_SOURCE_DIR})

target_link_libraries(${NAME} libserialport)
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


