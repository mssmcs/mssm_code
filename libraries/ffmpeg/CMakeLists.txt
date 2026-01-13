set(NAME "ffmpeg")


set(COMMIT_HASH "80229c161b6c04eb24c0f0319e1be4dfa939fd5d")

CPMAddPackage(
    NAME  ffmpeg
    URL  https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl-shared.zip
    # GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY   TRUE
)


# FFmpeg

# install libraries from https://github.com/BtbN/FFmpeg-Builds/releases
#  On windows, I'm using ffmpeg-master-latest-win64-lgpl-shared.zip   (lgpl license)

#set(FFmpeg_ROOT "" CACHE PATH "C:/Users/hamling/Documents/Github/ffmpeg-master-latest-win64-lgpl-shared")

#set(FFmpeg_ROOT "C:/Users/hamling/Documents/Github/ffmpeg-master-latest-win64-lgpl-shared")

add_library(${NAME} STATIC
    ffmpeg.cpp
)

set(FFmpeg_ROOT "${ffmpeg_SOURCE_DIR}")

FIND_PATH(FFMPEG_INCLUDE_DIR_AVUTIL NAMES libavutil/avutil.h PATHS ${FFmpeg_ROOT}/include)
FIND_PATH(FFMPEG_INCLUDE_DIR_AVCODEC NAMES libavcodec/avcodec.h PATHS ${FFmpeg_ROOT}/include)
FIND_PATH(FFMPEG_INCLUDE_DIR_AVFORMAT NAMES libavformat/avformat.h PATHS ${FFmpeg_ROOT}/include)
FIND_PATH(FFMPEG_INCLUDE_DIR_AVDEVICE NAMES libavdevice/avdevice.h PATHS ${FFmpeg_ROOT}/include)
FIND_PATH(FFMPEG_INCLUDE_DIR_AVFILTER NAMES libavfilter/avfilter.h PATHS ${FFmpeg_ROOT}/include)
FIND_PATH(FFMPEG_INCLUDE_DIR_SWSCALE NAMES libswscale/swscale.h PATHS ${FFmpeg_ROOT}/include)
FIND_PATH(FFMPEG_INCLUDE_DIR_SWRESAMPLE NAMES libswresample/swresample.h PATHS ${FFmpeg_ROOT}/include)

target_include_directories(${NAME} PRIVATE
    ${FFMPEG_INCLUDE_DIR_AVUTIL}
    ${FFMPEG_INCLUDE_DIR_AVCODEC}
    ${FFMPEG_INCLUDE_DIR_AVFORMAT}
    ${FFMPEG_INCLUDE_DIR_AVDEVICE}
    ${FFMPEG_INCLUDE_DIR_AVFILTER}
    ${FFMPEG_INCLUDE_DIR_SWSCALE}
    ${FFMPEG_INCLUDE_DIR_SWRESAMPLE}
)

target_include_directories(${NAME} INTERFACE
    ${FFmpeg_ROOT}/include
)

FIND_LIBRARY(FFMPEG_AVUTIL_LIBRARY NAMES avutil PATHS ${FFmpeg_ROOT}/lib )
FIND_LIBRARY(FFMPEG_AVCODEC_LIBRARY NAMES avcodec PATHS ${FFmpeg_ROOT}/lib )
FIND_LIBRARY(FFMPEG_AVFORMAT_LIBRARY NAMES avformat PATHS ${FFmpeg_ROOT}/lib )
FIND_LIBRARY(FFMPEG_AVDEVICE_LIBRARY NAMES avdevice PATHS ${FFmpeg_ROOT}/lib )
FIND_LIBRARY(FFMPEG_AVFILTER_LIBRARY NAMES avfilter PATHS ${FFmpeg_ROOT}/lib )
FIND_LIBRARY(FFMPEG_SWSCALE_LIBRARY NAMES swscale PATHS ${FFmpeg_ROOT}/lib )
FIND_LIBRARY(FFMPEG_SWRESAMPLE_LIBRARY NAMES swresample PATHS ${FFmpeg_ROOT}/lib )

target_link_libraries(${NAME}
    ${FFMPEG_AVUTIL_LIBRARY}
    ${FFMPEG_AVCODEC_LIBRARY}
    ${FFMPEG_AVFORMAT_LIBRARY}
    ${FFMPEG_AVDEVICE_LIBRARY}
    ${FFMPEG_AVFILTER_LIBRARY}
    ${FFMPEG_SWSCALE_LIBRARY}
    ${FFMPEG_SWRESAMPLE_LIBRARY}
)
