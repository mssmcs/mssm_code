#include "nanovgwindow.h"
#include "paths.h"
#include "sstream"
#include <deque>
#undef max
#undef min
#include <algorithm>



#include "fontstash.h"

#ifdef NANOVG_GLEW
#	include <GL/glew.h>
#endif
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#define GLFW_INCLUDE_GLEXT
#include "GLFW/glfw3.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3native.h"
#endif

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace mssm;
#undef max
#undef min

int nvgCreateImageCache(NVGcontext* ctx, unsigned char*& cachedImg, const char* filename, int imageFlags)
{
    int w, h, n, image;
    stbi_set_unpremultiply_on_load(1);
    stbi_convert_iphone_png_to_rgb(1);
    cachedImg = stbi_load(filename, &w, &h, &n, 4);
    if (cachedImg == NULL) {
        //		printf("Failed to load %s - %s\n", filename, stbi_failure_reason());
        return 0;
    }
    image = nvgCreateImageRGBA(ctx, w, h, imageFlags, cachedImg);
    //stbi_image_free(img);
    return image;
}

// void Image::set(int width, int height, Color c, bool cachePixels)
// {
//     Color* pixels = reinterpret_cast<Color*>(malloc(width * height * sizeof(Color)));

//     for (int i = 0; i < width*height; i++) {
//         pixels[i] = c;
//     }

//     int idx = nvgCreateImageRGBA(vg, width, height, NVG_IMAGE_NEAREST, reinterpret_cast<const unsigned char*>(pixels));

//     pixmap = std::make_shared<ImageInternal>(vg, idx, width, height, cachePixels ? pixels : nullptr);

//     if (!cachePixels) {
//         free(pixels);
//     }
// }

std::shared_ptr<mssm::ImageInternal> NanovgWindow::loadImg(std::string filename, bool cachePixels)
{
    auto fpath = Paths::findAsset(filename);
    unsigned char* cachedImage = nullptr;
    int idx;

    if (cachePixels) {
        idx = nvgCreateImageCache(vg, cachedImage, fpath.c_str(), 0);
    }
    else {
        idx = nvgCreateImage(vg, fpath.c_str(), 0);
    }

    if (idx) {
        int w;
        int h;
        nvgImageSize(vg, idx, &w, &h);
        return std::make_shared<ImageInternalVG>(vg, idx, w, h, reinterpret_cast<Color*>(cachedImage));
    }
    else {
        throw std::runtime_error("failed to load texture image!");
        // if (cachedImage) {
        //     stbi_image_free(cachedImage);
        // }
        // set(20,20,RED,cachePixels);
    }
}

std::shared_ptr<ImageInternal> NanovgWindow::createImg(int width,
                                                       int height,
                                                       mssm::Color c,
                                                       bool cachePixels)
{
    Color* pixels = reinterpret_cast<Color*>(malloc(width * height * sizeof(Color)));

    for (int i = 0; i < width*height; i++) {
        pixels[i] = c;
    }

    int idx = nvgCreateImageRGBA(vg, width, height, NVG_IMAGE_NEAREST, reinterpret_cast<const unsigned char*>(pixels));

    auto img = std::make_shared<ImageInternalVG>(vg, idx, width, height, cachePixels ? pixels : nullptr);

    if (!cachePixels) {
        free(pixels);
    }

    return img;
}

std::shared_ptr<ImageInternal> NanovgWindow::initImg(int width,
                                                     int height,
                                                     mssm::Color *pixels,
                                                     bool cachePixels)
{
    int idx = nvgCreateImageRGBA(vg, width, height, NVG_IMAGE_NEAREST, reinterpret_cast<const unsigned char*>(pixels));

    auto img = std::make_shared<ImageInternalVG>(vg, idx, width, height, cachePixels ? pixels : nullptr);

    if (!cachePixels) {
        delete [] pixels;
    }

    return img;
}

void NanovgWindow::saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename)
{
    stbi_write_png(filename.c_str(), img->width(), img->height(), 4, img->getPixels(), 4*img->width());
}

void NanovgWindow::polygonPattern(const std::vector<Vec2d> &points, mssm::Color c, mssm::Color f)
{
    throw std::logic_error("Not implemented");
}

void NanovgWindow::polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c, mssm::Color f)
{
    throw std::logic_error("Not implemented");
}

bool NanovgWindow::isDrawable()
{
    return true;
}


void NanovgWindow::setBackground(mssm::Color c)
{
    throw std::logic_error("Not implemented");
}

ImageInternalVG::ImageInternalVG(NVGcontext* vg, int idx, int width, int height, Color *cached)
    : vg{vg}, vgImageIdx{idx}
{
    w = width;
    h = height;
    pixels = cached;
}

ImageInternalVG::~ImageInternalVG()
{
    if (vgImageIdx) {
        nvgDeleteImage(vg, vgImageIdx);
    }
}

void ImageInternalVG::freeCachedPixels()
{
    if (pixels) {
        stbi_image_free(pixels);
        pixels = nullptr;
    }
}

void ImageInternalVG::updatePixels()
{
    if (!pixels) {
        throw std::logic_error("Cannot updatePixels unless image pixels are cached!");
    }
    nvgUpdateImage(vg, vgImageIdx, reinterpret_cast<const unsigned char*>(pixels));
}

NanovgWindow::NanovgWindow(std::string title, int width, int height)
    : mssm::CoreWindowGLFW(title, width, height, true)
{

}

NanovgWindow::~NanovgWindow()
{
    keepImages.clear();
    nvgDeleteGL3(vg);
#ifdef INCLUDE_SOUND
    soundPlayer.deinit();
#endif
}

void NanovgWindow::configure()
{
    mssm::CoreWindowGLFW::configure(); // call base class

#ifdef INCLUDE_SOUND
    soundPlayer.init();
#endif

//     glfwSetErrorCallback(errorcb);

//     if (!glfwInit()) {
//         printf("Failed to init GLFW.");
//         return;
//     }




// #ifndef _WIN32 // don't require this on win32, and works with more cards
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
// #endif
//     glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE );

// #ifdef DEMO_MSAA
//     glfwWindowHint(GLFW_SAMPLES, 4);
// #endif
//     //    window =   glfwCreateWindow(glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
//     //                                               glfwGetVideoMode(glfwGetPrimaryMonitor())->height, "My Title",
//     //                                               glfwGetPrimaryMonitor(), nullptr);

//     window = glfwCreateWindow(currentWidth, currentHeight, title.c_str(), NULL, NULL);
//     //	window = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);
//     if (!window) {
//         glfwTerminate();
//         return;
//     }

// #ifdef _WIN32
//     hdc = GetDC(glfwGetWin32Window(window));
// #endif

//     //glfwSetWindowIcon(window, count, icons);

//     glfwSetWindowUserPointer(window, this);

//     glfwSetKeyCallback(window, key);
//     glfwSetMouseButtonCallback(window, mouseButtonCallback);
//     glfwSetCursorPosCallback(window, mousePosCallback);
//     glfwSetScrollCallback(window, scrollWheelCallback);

//     glfwSetWindowPosCallback (window, windowPosCallback);
//     glfwSetWindowSizeCallback (window, windowSizeCallback);
//     glfwSetWindowCloseCallback (window, windowCloseCallback);
//     glfwSetWindowRefreshCallback (window, windowRefreshCallback);
//     glfwSetWindowFocusCallback (window, windowFocusCallback);
//     glfwSetWindowIconifyCallback (window, windowIconifyCallback);
//     glfwSetWindowMaximizeCallback (window, windowMaximizeCallback);
//     glfwSetFramebufferSizeCallback (window, frameBufferSizeCallback);
//     glfwSetWindowContentScaleCallback (window, windowContentScaleCallback);

//     glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        printf("Could not init glew.\n");
        return;
    }
    // GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
    glGetError();
#endif

#ifdef DEMO_MSAA
    vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
    if (vg == NULL) {
        printf("Could not init nanovg.\n");
        return;
    }

    fontRegular = nvgCreateFont(vg, "sans", Paths::findAsset("Roboto-Regular.ttf").c_str());
    if (fontRegular == -1) {
        printf("Could not add font italic.\n");
    }
    fontBold = nvgCreateFont(vg, "sans-bold", Paths::findAsset("Roboto-Bold.ttf").c_str());
    if (fontBold == -1) {
        printf("Could not add font bold.\n");
    }
    fontLight = nvgCreateFont(vg, "sans-light", Paths::findAsset("Roboto-Light.ttf").c_str());
    if (fontLight == -1) {
        printf("Could not add font light.\n");
    }
}

void NanovgWindow::setClip(int x, int y, int w, int h)
{
    nvgScissor(vg, x, y, w, h);
}

void NanovgWindow::resetClip()
{
    nvgResetScissor(vg);
}

void NanovgWindow::setViewport(int x, int y, int w, int h)
{
    throw std::logic_error("Not implemented");
}

void NanovgWindow::resetViewport()
{

}

// void NanovgWindow::setClosed()
// {


//     closed  = true;

// }

// bool NanovgWindow::isClosed()
// {
//     return closed;
// }



// bool NanovgWindow::appendOutputText(const std::string& txt)
// {
//     stringOutput.append(txt);
//     return true;
// }


// void NanovgWindow::music(const std::string& filename, double volume)
// {
//     musicFile = Paths::findAsset(filename);
//     musicVolume = volume;
// #ifdef INCLUDE_SOUND
//     stopMusic();
//     musicStream.load(musicFile.c_str());
//     musicHandle = soundPlayer.playBackground(musicStream, musicVolume);
// #endif
// }

// void NanovgWindow::stopMusic()
// {
// #ifdef INCLUDE_SOUND
//     if (musicHandle) {
//         soundPlayer.stop(musicHandle);
//         musicHandle = 0;
//     }
// #endif
// }

// void NanovgWindow::setMusicVolume(double volume)
// {
//     if (musicVolume != volume) {
//         musicVolume = volume;
// #ifdef INCLUDE_SOUND
//         if (musicHandle) {
//             soundPlayer.setVolume(musicHandle, musicVolume);
//         }
// #endif
//     }
// }

// bool NanovgWindow::isMusicPlaying()
// {
// #ifdef INCLUDE_SOUND
//     return soundPlayer.isValidVoiceHandle(musicHandle);
// #else
//     return false;
// #endif
// }

// SoundHandle NanovgWindow::play(Sound sound, double volume)
// {
// #ifdef INCLUDE_SOUND
//     return sound.sound->play(volume);
// #else
//     return 0;
// #endif
// }

// bool NanovgWindow::isPlaying(SoundHandle handle)
// {
// #ifdef INCLUDE_SOUND
//     return soundPlayer.isValidVoiceHandle(handle);
// #else
//     return 0;
// #endif
// }

// void splitRows(std::deque<std::string>& lines, const std::string& str, size_t keepLast)
// {
//     const char* ptr = str.c_str();
//     const char* eptr = ptr + str.size();
//     const char* sptr = ptr;
//     while (ptr < eptr) {
//         if (*ptr == 10) {
//             lines.emplace_back(sptr, ptr-sptr);
//             if (keepLast > 0 && lines.size() > keepLast) {
//                 lines.erase(lines.begin(), lines.begin() + lines.size() - keepLast);
//             }
//             ptr++;
//             if (ptr < eptr && *ptr == 13) {
//                 ptr++;
//             }
//             sptr = ptr;
//         }
//         else if (*ptr == 13) {
//             lines.emplace_back(sptr, ptr-sptr);
//             if (keepLast > 0 && lines.size() > keepLast) {
//                 lines.erase(lines.begin(), lines.begin() + lines.size() - keepLast);
//             }
//             ptr++;
//             if (ptr < eptr && *ptr == 10) {
//                 ptr++;
//             }
//             sptr = ptr;
//         }
//         else {
//             ptr++;
//         }
//     }
//     if (ptr > sptr) {
//         lines.emplace_back(sptr, ptr-sptr);
//         if (keepLast > 0 && lines.size() > keepLast) {
//             lines.erase(lines.begin(), lines.begin() + lines.size() - keepLast);
//         }
//     }
// }


void NanovgWindow::beginDrawing(bool wasResized)
{
    // int winWidth = width();
    // int winHeight = height();


    // int fbWidth = winWidth;
    // int fbHeight = winHeight;

    // // Calculate pixel ration for hi-dpi devices.
    // float pxRatio = (float)fbWidth / (float)winWidth;

    int winWidth, winHeight;
    glfwGetWindowSize(getWindow(), &winWidth, &winHeight);
    // currentWidth = winWidth;
    // currentHeight = winHeight;


    int fbWidth;
    int fbHeight;
    glfwGetFramebufferSize(getWindow(), &fbWidth, &fbHeight);

    // Calculate pixel ration for hi-dpi devices.
    float pxRatio = (float)fbWidth / (float)winWidth;

    // Update and render
    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    clipRects.clear();
    resetClip();
    resetViewport();

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
}

void NanovgWindow::endDrawing()
{

    nvgEndFrame(vg);

    keepImages.clear();
}

// bool NanovgWindow::draw()
// {
//     // std::deque<std::string> lines;
//     // drawFromStream(cout, lines, { 10, 20 }, PURPLE);
//     // drawFromStream(cerr, cerrLines, { width()/2, 20 }, RED);
//     return update(true);



//     // //    if (WGLEW_NV_delay_before_swap) {
//     // //       wglDelayBeforeSwapNV(hdc, 0.01);
//     // //    }


//     // int winWidth, winHeight;
//     // glfwGetWindowSize(window, &winWidth, &winHeight);
//     // currentWidth = winWidth;
//     // currentHeight = winHeight;

//     // if (gotResizeEvent) {
//     //     gotResizeEvent = false;
//     //     postEvent(currentWidth, currentHeight, EvtType::WindowResize, cvtMods(0), 0);
//     // }

//     // int fbWidth, fbHeight;
//     // glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

//     // // Calculate pixel ration for hi-dpi devices.
//     // float pxRatio = (float)fbWidth / (float)winWidth;

//     // // Update and render
//     // glViewport(0, 0, fbWidth, fbHeight);
//     // glClearColor(0,0,0,0);
//     // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

//     // nvgBeginFrame(vg, winWidth, winHeight, pxRatio);


//     // return true;
// }

// void NanovgWindow::drawFromStream(std::stringstream &ss, std::deque<std::string>& lines, Vec2d start, Color c)
// {
//     string tmp = ss.str();
//     if (!tmp.empty()) {
//         splitRows(lines, tmp, (currentHeight-40)/25);
//         ss.str("");
//     }

//     if (!lines.empty()) {
//         nvgFontSize(vg, 20.0f);
//         nvgFillColor(vg, nvgRGBA(c.r, c.g, c.b, 255));
//         nvgTextAlign(vg, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
//     }

//     double y = start.y;
//     for (const auto& row : lines) {
//         const char* ptr = row.c_str();
//         nvgText(vg, start.x, y, ptr, ptr + row.size());
//         y += 25;
//     }
// }

void NanovgWindow::image(Vec2d pos, const mssm::Image& img)
{
    keepImages.push_back(img.img);

    const float w = img.width();
    const float h = img.height();
    NVGpaint ip = nvgImagePattern(vg, pos.x, pos.y, w, h, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgBeginPath(vg);
    nvgRect(vg, pos.x, pos.y, w, h);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

void NanovgWindow::image(Vec2d pos, const mssm::Image& img, Vec2d src, int srcw, int srch)
{
    keepImages.push_back(img.img);

    const float w = img.width();
    const float h = img.height();
    //   _checkAlignPixelsAdjust(&dx, &dy);
    //   _checkAlignPixels(&dw, &dh);
    const float ratiox = w/srcw;
    const float ratioy = h/srch;
    float startX = pos.x - src.x * ratiox;
    float startY = pos.y - src.y * ratioy;
    float pw = img.width()  * ratiox;
    float ph = img.height() * ratioy;
    NVGpaint ip = nvgImagePattern(vg, startX, startY, pw, ph, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgBeginPath(vg);
    nvgRect(vg, pos.x, pos.y, w, h);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

void NanovgWindow::image(Vec2d pos, double w, double h, const mssm::Image& img)
{
    keepImages.push_back(img.img);

    if (w < 0) {
        pos.x -= w;
    }
    if (h < 0) {
        pos.y -= h;
    }
    //   _checkAlignPixelsAdjust(&dx, &dy);
    //   _checkAlignPixels(&dw, &dh);
    NVGpaint ip = nvgImagePattern(vg, pos.x, pos.y, w, h, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgBeginPath(vg);
    nvgRect(vg, pos.x, pos.y, w, h);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

void NanovgWindow::image(Vec2d pos, double w, double h, const mssm::Image& img, Vec2d src, int srcw, int srch)
{
    keepImages.push_back(img.img);

    if (w < 0) {
        pos.x -= w;
    }
    if (h < 0) {
        pos.y -= h;
    }
    //   _checkAlignPixelsAdjust(&dx, &dy);
    //   _checkAlignPixels(&dw, &dh);
    const float ratiox = w/srcw;
    const float ratioy = h/srch;
    float startX = pos.x - src.x * ratiox;
    float startY = pos.y - src.y * ratioy;
    float pw = img.width()  * ratiox;
    float ph = img.height() * ratioy;
    NVGpaint ip = nvgImagePattern(vg, startX, startY, pw, ph, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgBeginPath(vg);
    nvgRect(vg, pos.x, pos.y, w, h);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

void NanovgWindow::imageC(Vec2d center, double angle, const mssm::Image& img)
{
    keepImages.push_back(img.img);

    const float w = img.width();
    const float h = img.height();
    const float offx = -w/2;
    const float offy = -h/2;
    NVGpaint ip = nvgImagePattern(vg, offx, offy, w, h, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgTranslate(vg, center.x, center.y);
    if (angle != 0) {
        nvgRotate(vg, angle);
    }
    nvgBeginPath(vg);
    nvgRect(vg, offx, offy, w, h);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

void NanovgWindow::imageC(Vec2d center, double angle, const mssm::Image& img, Vec2d src, int srcw, int srch)
{
    keepImages.push_back(img.img);

    //   _checkAlignPixelsAdjust(&dx, &dy);
    //   _checkAlignPixels(&dw, &dh);
    const float offx = -srcw/2;
    const float offy = -srch/2;
    const float ratiox = srcw/srcw;
    const float ratioy = srch/srch;
    float startX = offx - src.x * ratiox;
    float startY = offy - src.y * ratioy;
    float pw = img.width()  * ratiox;
    float ph = img.height() * ratioy;
    NVGpaint ip = nvgImagePattern(vg, startX, startY, pw, ph, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgTranslate(vg, center.x, center.y);
    if (angle != 0) {
        nvgRotate(vg, angle);
    }
    nvgBeginPath(vg);
    nvgRect(vg, offx, offy, srcw, srch);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

void NanovgWindow::imageC(Vec2d center, double angle, double w, double h, const mssm::Image& img)
{
    keepImages.push_back(img.img);

    const float offx = -w/2;
    const float offy = -h/2;
    NVGpaint ip = nvgImagePattern(vg, offx, offy, w, h, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgTranslate(vg, center.x, center.y);
    if (angle != 0) {
        nvgRotate(vg, angle);
    }
    nvgBeginPath(vg);
    nvgRect(vg, offx, offy, w, h);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

void NanovgWindow::imageC(Vec2d center, double angle, double w, double h, const mssm::Image &img, Vec2d src, int srcw, int srch)
{
    keepImages.push_back(img.img);

    //   _checkAlignPixelsAdjust(&dx, &dy);
    //   _checkAlignPixels(&dw, &dh);
    const float offx = -w/2;
    const float offy = -h/2;
    const float ratiox = w/srcw;
    const float ratioy = h/srch;
    float startX = offx - src.x * ratiox;
    float startY = offy - src.y * ratioy;
    float pw = img.width()  * ratiox;
    float ph = img.height() * ratioy;
    NVGpaint ip = nvgImagePattern(vg, startX, startY, pw, ph, 0.0f, img.textureIndex(), 1.0f);
    nvgSave(vg);
    nvgTranslate(vg, center.x, center.y);
    if (angle != 0) {
        nvgRotate(vg, angle);
    }
    nvgBeginPath(vg);
    nvgRect(vg, offx, offy, w, h);
    nvgFillPaint(vg, ip);
    nvgFill(vg);
    nvgRestore(vg);
}

bool NanovgWindow::isClipped(Vec2d pos) const
{
    if (clipRects.empty()) {
        return false;
    }
    auto& clip = clipRects.back();
    return pos.x < clip.x ||
           pos.y < clip.y ||
           pos.x >= clip.x + clip.w ||
           pos.y >= clip.y + clip.h;
}

void NanovgWindow::pushClip(int x, int y, int w, int h, bool replace)
{
    w = std::max(0,w);
    h = std::max(0,h);
    if (replace || clipRects.empty()) {
        Scissor rect = {x, y, w, h};
        clipRects.push_back(rect);
        setClip(x, y, w, h);
    }
    else {
        // intersect with current clip
        Scissor rect;
        Scissor& prev = clipRects.back();
        rect.x = std::max(prev.x, x);
        rect.y = std::max(prev.y, y);
        auto oldEndX = prev.x + prev.w;
        auto oldEndY = prev.y + prev.w;
        auto endX = x + w;
        auto endY = y + h;
        rect.w = std::max(0, std::min(oldEndX, endX) - rect.x);
        rect.h = std::max(0, std::min(oldEndY, endY) - rect.y);
        clipRects.push_back(rect);
        setClip(rect.x, rect.y, rect.w, rect.h);
    }
}


void NanovgWindow::popClip()
{
    clipRects.pop_back();
    if (clipRects.empty()) {
        resetClip();
    }
    else {
        auto& rect = clipRects.back();
        setClip(rect.x, rect.y, rect.w, rect.h);
    }
}

void NanovgWindow::line(Vec2d p1, Vec2d p2, Color c)
{
    nvgBeginPath(vg);
    nvgMoveTo(vg, p1.x, p1.y);
    nvgLineTo(vg, p2.x, p2.y);
    nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    nvgStroke(vg);
}

void NanovgWindow::ellipse(Vec2d pos, double w, double h, Color c, Color fill)
{
    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    nvgFillColor(vg, nvgRGBA(fill.r, fill.g, fill.b, fill.a));
    nvgEllipse(vg, pos.x, pos.y, w/2.0, h/2.0);
    nvgFill(vg);
    nvgStroke(vg);
}
//d nvgArc2(NVGcontext* ctx, float cx, float cy, float rx, float ry, float a0, float a1, int dir);

void NanovgWindow::arc(Vec2d pos, double w, double h, double a, double alen, Color c)
{
    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    nvgArc2(vg, pos.x, pos.y, w/2.0, h/2.0, -a, -a-alen, NVG_CCW);
    nvgStroke(vg);
}

void NanovgWindow::chord(Vec2d pos, double w, double h, double a, double alen, Color c, Color fill)
{
    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    nvgFillColor(vg, nvgRGBA(fill.r, fill.g, fill.b, fill.a));
    nvgArc2(vg, pos.x, pos.y, w/2.0, h/2.0, -a, -a-alen, NVG_CCW);
    nvgClosePath(vg);
    nvgFill(vg);
    nvgStroke(vg);
}

void NanovgWindow::pie(Vec2d pos, double w, double h, double a, double alen, Color c, Color fill)
{
    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    nvgFillColor(vg, nvgRGBA(fill.r, fill.g, fill.b, fill.a));
    nvgMoveTo(vg, pos.x,pos.y);
    nvgArc2(vg, pos.x, pos.y, w/2.0, h/2.0, -a, -a-alen, NVG_CCW);
    nvgLineTo(vg, pos.x,pos.y);
    nvgFill(vg);
    nvgStroke(vg);
}

void NanovgWindow::rect(Vec2d corner, double w, double h, Color c, Color fill)
{
    nvgBeginPath(vg);
    nvgRect(vg, corner.x, corner.y, w, h);
    if (fill.a > 0) {
        nvgFillColor(vg, nvgRGBA(fill.r, fill.g, fill.b, fill.a));
        nvgFill(vg);
    }
    if (c.a > 0) {
        nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
        nvgStroke(vg);
    }
}


void NanovgWindow::text(Vec2d pos, const FontInfo& sizeAndFace, const std::string &str, Color c, HAlign hAlign, VAlign vAlign)
{
    nvgFontSize(vg, sizeAndFace.getSize());
    //nvgFontFace(vg, "sans");
    nvgFontFaceId(vg, sizeAndFace.getFaceIdx());
    nvgTextAlign(vg, static_cast<int>(hAlign) | static_cast<int>(vAlign));
    // nvgFontBlur(vg,2);
    nvgFillColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    auto s = str.c_str();
    nvgText(vg, pos.x, pos.y, s, s+str.length());
}

void NanovgWindow::textExtents(const FontInfo& sizeAndFace, const string &str, TextExtents &extents)
{
    float metrics[4];

    nvgFontSize(vg, sizeAndFace.getSize());
    //nvgFontFace(vg, "sans");
    nvgFontFaceId(vg, sizeAndFace.getFaceIdx());
    auto s = str.c_str();
    float advance = nvgTextBounds(vg, 0, 0, s, s+str.length(), metrics);

    extents.textHeight = metrics[3]-metrics[1];  // ymax-ymin
    extents.textWidth  = metrics[2]-metrics[0];  // xmax-xmin

    extents.textAdvance = advance;
    nvgTextMetrics(vg, &extents.fontAscent, &extents.fontDescent, &extents.fontHeight);
}

double NanovgWindow::textWidth(const FontInfo& sizeAndFace, const string &str)
{
    float metrics[4];

    nvgFontSize(vg, sizeAndFace.getSize());
    //nvgFontFace(vg, "sans");
    nvgFontFaceId(vg, sizeAndFace.getFaceIdx());
    auto s = str.c_str();
    /*float advance =*/ nvgTextBounds(vg, 0, 0, s, s+str.length(), metrics);

    return metrics[2]-metrics[0];  // xmax-xmin
}

// Function to compute x offsets for each character in a string
// NOTE: font, size must be set appropriately before calling this function
std::vector<double> getCharacterXOffsets(NVGcontext* ctx, const FontInfo &sizeAndFace, double startX, std::string text)
{
    std::vector<NVGglyphPosition> positions(text.size()+2);

    nvgFontSize(ctx, sizeAndFace.getSize());
    nvgFontFaceId(ctx, sizeAndFace.getFaceIdx());

    text.append(1, '_'); // Add a char at the end to get the last character's x position

    int count = nvgTextGlyphPositions(ctx, startX, 0, text.c_str(), text.c_str() + text.size(), positions.data(), positions.size());

    std::vector<double> xOffsets(count);

    for (int i = 0; i < count; i++) {
        xOffsets[i] = positions[i].x;
    }

    return xOffsets;
}


std::vector<double> NanovgWindow::getCharacterXOffsets(const FontInfo &sizeAndFace,
                                                       double startX,
                                                       const std::string &text)
{
    return ::getCharacterXOffsets(vg, sizeAndFace, startX, text);
}

void NanovgWindow::point(Vec2d p, Color c)
{
    nvgBeginPath(vg);
    nvgStrokeWidth(vg, 3);
    nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    nvgMoveTo(vg, p.x-1, p.y-1);
    nvgLineTo(vg, p.x+1, p.y+1);
    nvgStroke(vg);
    nvgStrokeWidth(vg, 1);
}


template<typename T>
void NanovgWindow::t_polygon(T points, Color border, Color fill)
{
    bool first = true;

    for (auto& p : points) {
        if (first) {
            first = false;
            nvgBeginPath(vg);
            nvgMoveTo(vg, p.x, p.y);
        }
        else {
            nvgLineTo(vg, p.x, p.y);
        }
    }

    if (!first) {
        nvgClosePath(vg);

        if (fill.a > 0) {
            if (fill.a > 0) {
                nvgFillColor(vg, nvgRGBA(fill.r, fill.g, fill.b, fill.a));
                nvgFill(vg);
            }
        }

        if (border.a > 0) {
            nvgStrokeColor(vg, nvgRGBA(border.r, border.g, border.b, border.a));
            nvgStroke(vg);
        }
    }
}

template<typename T>
void NanovgWindow::t_polyline(T points, Color color, bool closed)
{
    bool first = true;

    for (auto& p : points) {
        if (first) {
            first = false;
            nvgBeginPath(vg);
            nvgMoveTo(vg, p.x, p.y);
        }
        else {
            nvgLineTo(vg, p.x, p.y);
        }
    }

    if (!first) {
        if (color.a > 0) {
            nvgStrokeColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
            nvgStroke(vg);
        }
    }
}

template<typename T>
inline void NanovgWindow::t_points(T points, Color c)
{
    if (points.size() == 0) {
        return;
    }
    nvgBeginPath(vg);
    nvgStrokeWidth(vg, 3);
    nvgStrokeColor(vg, nvgRGBA(c.r, c.g, c.b, c.a));
    for (auto& p : points) {
        nvgMoveTo(vg, p.x-1,p.y-1);
        nvgLineTo(vg, p.x+1,p.y+1);
    }
    nvgStroke(vg);
    nvgStrokeWidth(vg, 1);
}





void NanovgWindow::polygon(const std::vector<Vec2d>& points, Color border, Color fill)
{
    t_polygon(points, border, fill);
}

void NanovgWindow::polyline(const std::vector<Vec2d>& points, Color color)
{
    t_polyline(points, color, false);
}

void NanovgWindow::points(const std::vector<Vec2d>& points, Color c)
{
    t_points(points, c);
}

// void NanovgWindow::polygon(const Array<Vec2d>& points, Color border, Color fill)
// {
//     t_polygon(points, border, fill);
// }

// void NanovgWindow::polyline(const Array<Vec2d>& points, Color color)
// {
//     t_polyline(points, color);
// }

// void NanovgWindow::points(const Array<Vec2d>& points, Color c)
// {
//     t_points(points, c);
// }

void NanovgWindow::polygon(std::initializer_list<Vec2d> points, Color border, Color fill)
{
    t_polygon(points, border, fill);
}

void NanovgWindow::polyline(std::initializer_list<Vec2d> points, Color color)
{
    t_polyline(points, color, false);
}

void NanovgWindow::points(std::initializer_list<Vec2d> points, Color c)
{
    t_points(points, c);
}

// int NanovgWindow::getDigitPressed()
// {
//     switch (getKeyPressed()) {
//     case '0': return 0;
//     case '1': return 1;
//     case '2': return 2;
//     case '3': return 3;
//     case '4': return 4;
//     case '5': return 5;
//     case '6': return 6;
//     case '7': return 7;
//     case '8': return 8;
//     case '9': return 9;
//     }

//     return -1;
// }

