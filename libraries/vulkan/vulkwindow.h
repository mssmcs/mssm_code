#ifndef VULKANWINDOW_H
#define VULKANWINDOW_H

#include "image.h"
#include "windowbase.h"
#include "vulkabstractwindow.h"
#include "vulkcanvasbase.h"
#include "vulkinstance.h"
#include "vulksurfacerendermanager.h"


template <typename T>
concept SupportsVulkanWindow =
    std::is_base_of<mssm::CoreWindow, T>::value &&
    std::is_base_of<VulkAbstractWindow, T>::value;

template <typename T>
concept IsCanvas =
    std::is_base_of<VulkCanvasBase0, T>::value;

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
class VulkanGraphicsWindow : public WINDOW, public mssm::ImageLoader
{
    using CreateCanvasFunc = std::function<std::unique_ptr<CANVAS>(VulkSurfaceRenderManager &renderManager)>;

    CreateCanvasFunc createCanvas;

    VulkInstance instance;

    std::unique_ptr<VulkSurfaceRenderManager> renderManager;

    std::unique_ptr<CANVAS> canvas;



public:
    VulkanGraphicsWindow(const std::string &title,
                         int width, int height,
                         CreateCanvasFunc createCanvas);
    ~VulkanGraphicsWindow();

    //Vec2d  mousePos() const { return {mousePosX(), mousePosY()}; }

    CANVAS* getCanvas() { return canvas.get(); }

protected:
    virtual void configure() override;
    virtual void beginDrawing(bool wasResized) override;
    virtual void endDrawing() override;
    void detachRenderer(bool releaseSurface) override;
    // ImageLoader interface
public:
    virtual std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override;

    // ImageLoader interface
public:
    std::shared_ptr<mssm::ImageInternal> createImg(int width,
                                                   int height,
                                                   mssm::Color c,
                                                   bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> initImg(int width,
                                                 int height,
                                                 mssm::Color *pixels,
                                                 bool cachePixels) override;
    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override;

};

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
VulkanGraphicsWindow<WINDOW, CANVAS>::VulkanGraphicsWindow(const std::string &title,
                                                   int width, int height,
                                                   CreateCanvasFunc createCanvas)
    : WINDOW(title, width, height),
    createCanvas{createCanvas},
    instance(WINDOW::getRequiredExtensions())
{

    setbuf(stdout, nullptr);

    // defer setup to configure, so we can use virtual methods on this class in the setup
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
VulkanGraphicsWindow<WINDOW, CANVAS>::~VulkanGraphicsWindow()
{
    if (renderManager) {
        renderManager->waitForIdle();
    }
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
void VulkanGraphicsWindow<WINDOW, CANVAS>::configure()
{
    WINDOW::configure(); // CALL BASE CLASS CONFIGURE!

    int maxFramesInFlight = 2;

    renderManager = std::make_unique<VulkSurfaceRenderManager>();

    renderManager->beginInitialization(instance, this, true, maxFramesInFlight);

    canvas = createCanvas(*renderManager);

    canvas->initializePipelines();

}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
void VulkanGraphicsWindow<WINDOW, CANVAS>::beginDrawing(bool wasResized)
{
    renderManager->beginDrawing(wasResized);
    if (renderManager->isDrawable()) {
        canvas->beginPaint();
    }

}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
void VulkanGraphicsWindow<WINDOW, CANVAS>::endDrawing()
{
    //std::cout << "EndDrawing" << std::endl;
    if (renderManager->isDrawable()) {
        canvas->endPaint();
    }
    renderManager->endDrawing();
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
std::shared_ptr<mssm::ImageInternal> VulkanGraphicsWindow<WINDOW, CANVAS>::loadImg(std::string filename, bool cachePixels)
{
    return renderManager->loadImg(filename, cachePixels);
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
std::shared_ptr<mssm::ImageInternal> VulkanGraphicsWindow<WINDOW, CANVAS>::createImg(int width, int height, mssm::Color c, bool cachePixels)
{
    return renderManager->createImg(width, height, c, cachePixels);
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
std::shared_ptr<mssm::ImageInternal> VulkanGraphicsWindow<WINDOW, CANVAS>::initImg(int width, int height, mssm::Color *pixels, bool cachePixels)
{
    return renderManager->initImg(width, height, pixels, cachePixels);
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
void VulkanGraphicsWindow<WINDOW, CANVAS>::saveImg(std::shared_ptr<mssm::ImageInternal> img,
                                          std::string filename)
{
    renderManager->saveImg(img, filename);
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
void VulkanGraphicsWindow<WINDOW, CANVAS>::detachRenderer(bool releaseSurface)
{
    renderManager->waitForIdle();
    if (releaseSurface) {
        renderManager->releaseSurface();
    }
    canvas.reset();
    renderManager.reset();
}

#endif // VULKANWINDOW_H
