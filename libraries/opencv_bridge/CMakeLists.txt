set(NAME "opencv_bridge")
# MSSM_LIB_DESC: Bridge utilities for mssm::Image, camera RGB buffers, and OpenCV cv::Mat
# DEPENDS_ON: opencv image

add_library(${NAME} STATIC
    opencv_bridge.h
    opencv_bridge.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${NAME} PUBLIC opencv image_base)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
