#ifndef IMAGE_H
#define IMAGE_H

//#include "color.h"
#include <memory>
#include <string>

namespace mssm {

    class ImageInternal
    {
    public:
        virtual int width() const = 0;
        virtual int height() const = 0;
        virtual uint32_t textureIndex() const = 0;
     //   friend class Image;
    };

    class ImageLoader
    {
    public:
        virtual std::shared_ptr<ImageInternal> loadImg(std::string filename) = 0;
    };

    class Image
    {
    private:
        std::shared_ptr<ImageInternal> img;
    public:
        Image();
        Image(ImageLoader& imageLoader, const std::string &filename);
        int width() const { return img->width(); }
        int height() const { return img->height(); }
        uint32_t textureIndex() const { return img->textureIndex(); }
    };

}

#endif // IMAGE_H
