#include "image.h"

using namespace std;
using namespace mssm;


Image::Image(ImageLoader& imageLoader)
    : imageLoader(imageLoader)
{
}

Image::Image(ImageLoader& imageLoader, const std::string &filename, bool cachePixels)
    : imageLoader(imageLoader)
{
    load(filename, cachePixels);
}

Image::Image(ImageLoader& imageLoader, int width, int height, mssm::Color c, bool cachePixels)
    : imageLoader(imageLoader)
{
    set(width, height, c, cachePixels);
}

void Image::set(int width, int height, mssm::Color c, bool cachePixels)
{
    img = imageLoader.createImg(width, height, c, cachePixels);
}

void Image::load(const std::string& filename, bool cachePixels)
{
    img = imageLoader.loadImg(filename, cachePixels);
}

void Image::save(const std::string& pngFileName)
{
    imageLoader.saveImg(img, pngFileName);
}
