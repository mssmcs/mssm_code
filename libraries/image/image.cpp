#include "image.h"

using namespace std;
using namespace mssm;

Image::Image()
{
}

Image::Image(ImageLoader& imageLoader, const std::string& filename)
{
    img = imageLoader.loadImg(filename);
}

