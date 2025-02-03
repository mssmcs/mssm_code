#ifndef IMAGE_H
#define IMAGE_H

#include "color.h"
#include <memory>
#include <string>

namespace mssm {

    class ImageInternal
    {
    protected:
        int w;
        int h;
        Color* pixels{nullptr};
    public:
        virtual ~ImageInternal() {}
        constexpr int width() const { return w; };
        constexpr int height() const { return h; };
        virtual uint32_t textureIndex() const = 0;
        Color* getPixels() {
            return pixels;
        }
    private:
//        virtual void freeCachedPixels() = 0;
        virtual void updatePixels() = 0;
        void setPixel(int x, int y, Color c) {
            pixels[y*w+x] = c;
        }
        Color getPixel(int x, int y) {
            return pixels[y*w+x];
        }
        friend class Image;
        friend class Graphics;
    };

    class ImageLoader
    {
    public:
        virtual std::shared_ptr<ImageInternal> loadImg(std::string filename, bool cachePixels) = 0;
        virtual std::shared_ptr<ImageInternal> createImg(int width, int height, mssm::Color c, bool cachePixels) = 0;
        virtual std::shared_ptr<ImageInternal> initImg(int width, int height, Color* pixels, bool cachePixels) = 0;
        virtual void saveImg(std::shared_ptr<ImageInternal> img, std::string filename) = 0;
    };

    class Image
    {
    protected:
        ImageLoader& imageLoader;
    public: // TODO: hack
        std::shared_ptr<ImageInternal> img;
    public:
        Image(ImageLoader& imageLoader);
        Image(ImageLoader& imageLoader, const std::string &filename, bool cachePixels = false);
        Image(ImageLoader& imageLoader, int width, int height, mssm::Color c, bool cachePixels = false);
        void set(int width, int height, mssm::Color c, bool cachePixels = false);
        void load(const std::string& fileName, bool cachePixels = false);
        void save(const std::string& pngFileName);
        Color* pixels() { return img->pixels; } // changes won't take effect until updatePixels
        void  setPixel(int x, int y, Color c) { img->setPixel(x, y, c); } // won't take effect until updatePixels
        Color getPixel(int x, int y)          { return img->getPixel(x, y); }
        void updatePixels() { img->updatePixels(); }
        int width() const { return img->width(); }
        int height() const { return img->height(); }
        uint32_t textureIndex() const { return img->textureIndex(); }
    };

}

#endif // IMAGE_H
