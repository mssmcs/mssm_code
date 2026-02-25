#include "opencv_bridge.h"

#include <stdexcept>

namespace {
void validateImageForRead(mssm::Image& img)
{
    if (img.width() <= 0 || img.height() <= 0) {
        throw std::invalid_argument("mssm::Image has invalid dimensions.");
    }
    if (!img.pixels()) {
        throw std::invalid_argument("mssm::Image has no cached pixels. Create/load with cachePixels=true.");
    }
}

void validateImageForWrite(mssm::Image& img, int width, int height)
{
    validateImageForRead(img);
    if (img.width() != width || img.height() != height) {
        throw std::invalid_argument("mssm::Image dimensions do not match source cv::Mat.");
    }
}
}

cv::Mat mssm::opencv_bridge::toMatRGBA(mssm::Image& img)
{
    validateImageForRead(img);

    cv::Mat out(img.height(), img.width(), CV_8UC4);
    const auto* src = img.pixels();
    for (int y = 0; y < img.height(); ++y) {
        auto* row = out.ptr<cv::Vec4b>(y);
        for (int x = 0; x < img.width(); ++x) {
            const auto& c = src[y * img.width() + x];
            row[x] = cv::Vec4b(c.r, c.g, c.b, c.a);
        }
    }
    return out;
}

cv::Mat mssm::opencv_bridge::toMatBGR(mssm::Image& img)
{
    validateImageForRead(img);

    cv::Mat bgr(img.height(), img.width(), CV_8UC3);
    const auto* src = img.pixels();
    for (int y = 0; y < img.height(); ++y) {
        auto* row = bgr.ptr<cv::Vec3b>(y);
        for (int x = 0; x < img.width(); ++x) {
            const auto& c = src[y * img.width() + x];
            row[x] = cv::Vec3b(c.b, c.g, c.r);
        }
    }
    return bgr;
}

cv::Mat mssm::opencv_bridge::rgbBufferToMat(const uint8_t* rgb, int width, int height)
{
    if (!rgb) {
        throw std::invalid_argument("rgbBufferToMat received null RGB buffer.");
    }
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("rgbBufferToMat received invalid dimensions.");
    }

    cv::Mat view(height, width, CV_8UC3, const_cast<uint8_t*>(rgb));
    return view.clone();
}

void mssm::opencv_bridge::fromMat(const cv::Mat& src, mssm::Image& dst)
{
    if (src.empty()) {
        throw std::invalid_argument("fromMat received an empty cv::Mat.");
    }

    validateImageForWrite(dst, src.cols, src.rows);

    auto* dstPixels = dst.pixels();
    switch (src.type()) {
    case CV_8UC1:
        for (int y = 0; y < dst.height(); ++y) {
            const auto* row = src.ptr<uint8_t>(y);
            for (int x = 0; x < dst.width(); ++x) {
                auto& c = dstPixels[y * dst.width() + x];
                const uint8_t v = row[x];
                c.r = v;
                c.g = v;
                c.b = v;
                c.a = 255;
            }
        }
        break;
    case CV_8UC3:
        for (int y = 0; y < dst.height(); ++y) {
            const auto* row = src.ptr<cv::Vec3b>(y);
            for (int x = 0; x < dst.width(); ++x) {
                auto& c = dstPixels[y * dst.width() + x];
                const auto& px = row[x];
                c.r = px[2];
                c.g = px[1];
                c.b = px[0];
                c.a = 255;
            }
        }
        break;
    case CV_8UC4:
        for (int y = 0; y < dst.height(); ++y) {
            const auto* row = src.ptr<cv::Vec4b>(y);
            for (int x = 0; x < dst.width(); ++x) {
                auto& c = dstPixels[y * dst.width() + x];
                const auto& px = row[x];
                c.r = px[0];
                c.g = px[1];
                c.b = px[2];
                c.a = px[3];
            }
        }
        break;
    default:
        throw std::invalid_argument("fromMat supports only CV_8UC1, CV_8UC3, and CV_8UC4.");
    }
}

void mssm::opencv_bridge::applyMatToImage(const cv::Mat& src, mssm::Image& dst)
{
    fromMat(src, dst);
    dst.updatePixels();
}
