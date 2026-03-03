#include "mssm.h"
#include "image.h"
#include "opencv_bridge.h"

#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace {
class CpuImageInternal : public mssm::ImageInternal
{
private:
    std::vector<mssm::Color> storage;

public:
    CpuImageInternal(int width, int height, mssm::Color fill)
    {
        w = width;
        h = height;
        storage.assign(static_cast<size_t>(w * h), fill);
        pixels = storage.empty() ? nullptr : storage.data();
    }

    CpuImageInternal(int width, int height, const mssm::Color* srcPixels)
    {
        w = width;
        h = height;
        storage.resize(static_cast<size_t>(w * h));
        if (srcPixels) {
            std::memcpy(storage.data(), srcPixels, storage.size() * sizeof(mssm::Color));
        }
        pixels = storage.empty() ? nullptr : storage.data();
    }

    uint32_t textureIndex() const override
    {
        return 0;
    }

private:
    void updatePixels() override
    {
        // CPU-only demo image: no GPU upload required.
    }
};

class CpuImageLoader : public mssm::ImageLoader
{
public:
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override
    {
        (void)filename;
        (void)cachePixels;
        throw std::runtime_error("CpuImageLoader::loadImg is not implemented in this demo.");
    }

    std::shared_ptr<mssm::ImageInternal> createImg(int width, int height, mssm::Color c, bool cachePixels) override
    {
        (void)cachePixels;
        return std::make_shared<CpuImageInternal>(width, height, c);
    }

    std::shared_ptr<mssm::ImageInternal> initImg(int width, int height, mssm::Color* srcPixels, bool cachePixels) override
    {
        (void)cachePixels;
        return std::make_shared<CpuImageInternal>(width, height, srcPixels);
    }

    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override
    {
        (void)img;
        (void)filename;
        throw std::runtime_error("CpuImageLoader::saveImg is not implemented in this demo.");
    }

    void queueForDestruction(std::shared_ptr<mssm::ImageInternal> img) override
    {
        (void)img;
    }
};
}

int main()
{
    println("OpenCV version: {}", cv::getVersionString());
    println("Step 1: create synthetic RGB frame");

    constexpr int width = 320;
    constexpr int height = 200;

    std::vector<uint8_t> rgbFrame(static_cast<size_t>(width * height * 3));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const size_t i = static_cast<size_t>((y * width + x) * 3);
            rgbFrame[i + 0] = static_cast<uint8_t>((x * 255) / width);       // R
            rgbFrame[i + 1] = static_cast<uint8_t>((y * 255) / height);      // G
            rgbFrame[i + 2] = static_cast<uint8_t>(255 - rgbFrame[i + 0]);   // B
        }
    }

    // Simulate the low-level camera path: OpenPnP gives 24-bit RGB bytes.
    cv::Mat rgb = mssm::opencv_bridge::rgbBufferToMat(rgbFrame.data(), width, height);
    println("Step 2: OpenCV preprocessing");
    cv::Mat bgr(height, width, CV_8UC3);
    for (int y = 0; y < height; ++y) {
        const auto* rgbRow = rgb.ptr<cv::Vec3b>(y);
        auto* bgrRow = bgr.ptr<cv::Vec3b>(y);
        for (int x = 0; x < width; ++x) {
            bgrRow[x] = cv::Vec3b(rgbRow[x][2], rgbRow[x][1], rgbRow[x][0]);
        }
    }

    cv::Mat invertedBgr(height, width, CV_8UC3);
    for (int y = 0; y < height; ++y) {
        const auto* srcRow = bgr.ptr<cv::Vec3b>(y);
        auto* dstRow = invertedBgr.ptr<cv::Vec3b>(y);
        for (int x = 0; x < width; ++x) {
            dstRow[x][0] = static_cast<uint8_t>(255 - srcRow[x][0]);
            dstRow[x][1] = static_cast<uint8_t>(255 - srcRow[x][1]);
            dstRow[x][2] = static_cast<uint8_t>(255 - srcRow[x][2]);
        }
    }
    println("Step 2b: preprocessing complete");

    CpuImageLoader loader;
    mssm::Image img(loader, width, height, mssm::BLACK, true);
    println("Step 3: write cv::Mat into mssm::Image");

    mssm::opencv_bridge::applyMatToImage(invertedBgr, img);
    println("Step 4: convert back to cv::Mat and save");

    cv::Mat out = mssm::opencv_bridge::toMatBGR(img);
    const std::string outFile = "opencv_bridge_demo.png";
    if (!cv::imwrite(outFile, out)) {
        throw std::runtime_error("Failed to write demo image output.");
    }

    println("Saved bridge output to {}", outFile);
    return 0;
}
