#ifndef VULKANWINDOW_H
#define VULKANWINDOW_H

#include "image.h"
#include "corewindow.h"
#include "vulkabstractwindow.h"
#include "vulkcanvasbase.h"
#include "vulkinstance.h"
#include "vulksurfacerendermanager.h"


template <typename T>
concept SupportsVulkanWindow =
    std::is_base_of<gjh::CoreWindow, T>::value &&
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

    std::chrono::microseconds::rep start_time;
    std::chrono::steady_clock::time_point lastDrawTime;
    std::chrono::microseconds::rep elapsed;

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
    virtual std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename) override;

    double timeMicros() const
    {
        auto microseconds_since_epoch =
            std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::steady_clock::now().time_since_epoch()).count();

        return microseconds_since_epoch - start_time;
    }

    double timeSeconds() const { return timeMicros() / 1000000.0; }
    double timeMs() const { return timeMicros() / 1000.0; }
    double elapsedMs() const { return elapsed/1000.0; }
    double elapsedMicros() const { return elapsed; }
    double elapsedSeconds() const { return elapsed/1000000.0; }
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

    start_time =
        std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
    lastDrawTime = std::chrono::steady_clock::now();
}

template <typename WINDOW, typename CANVAS> requires SupportsVulkanWindow<WINDOW> && IsCanvas<CANVAS>
void VulkanGraphicsWindow<WINDOW, CANVAS>::beginDrawing(bool wasResized)
{
    renderManager->beginDrawing(wasResized);
    if (renderManager->isDrawable()) {
        canvas->beginPaint();
    }
    auto currDrawTime = std::chrono::steady_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currDrawTime - lastDrawTime).count();
    lastDrawTime = currDrawTime;
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
std::shared_ptr<mssm::ImageInternal> VulkanGraphicsWindow<WINDOW, CANVAS>::loadImg(std::string filename)
{
    return renderManager->loadImg(filename);
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
