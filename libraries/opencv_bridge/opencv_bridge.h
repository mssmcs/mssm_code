#ifndef MSSM_OPENCV_BRIDGE_H
#define MSSM_OPENCV_BRIDGE_H

#include <cstdint>

#include "image.h"
#include <opencv2/core.hpp>

namespace mssm::opencv_bridge {

cv::Mat toMatRGBA(mssm::Image& img);
cv::Mat toMatBGR(mssm::Image& img);
cv::Mat rgbBufferToMat(const uint8_t* rgb, int width, int height);

void fromMat(const cv::Mat& src, mssm::Image& dst);
void applyMatToImage(const cv::Mat& src, mssm::Image& dst);

}

#endif
