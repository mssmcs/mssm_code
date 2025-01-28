#ifndef VULKANWINDOWQT_H
#define VULKANWINDOWQT_H

#include <QVulkanInstance>
#include "corewindowqt.h"
#include "vulkabstractwindow.h"
#include "vulkcanvas.h"
#include "vulkwindow.h"

class VulkanWindowQT : public CoreWindowQt, public VulkAbstractWindow
{
    QVulkanInstance vInst;
public:
    VulkanWindowQT(std::string title, int width, int height, std::function<void(CoreWindowQt*)> drawCallback = nullptr);
    virtual VkExtent2D getExtent() override;
    virtual VkSurfaceKHR createSurface(VkInstance instance) override;

    static std::vector<const char*> getRequiredExtensions();
};

std::unique_ptr<VulkCanvas> createCanvas(VulkSurfaceRenderManager &renderManager);

class VulkBackendWindowQt : public VulkanGraphicsWindow<VulkanWindowQT, VulkCanvas>
{
public:
    VulkBackendWindowQt(std::string title, int width, int height)
        : VulkanGraphicsWindow<VulkanWindowQT, VulkCanvas>(title, width, height, ::createCanvas)
    {
    }
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename) override;

protected:
    void beginDrawing(bool wasResized) override;
    void endDrawing() override;
    void setCursor(gjh::CoreWindowCursor cursor) override;
};



// class VulkanWindowQt : public QWindow
// {
// public:
//     VulkanWindowQt() {
//         setSurfaceType(VulkanSurface);
//     }

//     void exposeEvent(QExposeEvent *);

//     bool event(QEvent *e);
//     void render();

// private:
//     bool m_initialized = false;
// };

// int main(int argc, char **argv)
// {
//     QGuiApplication app(argc, argv);

//     QVulkanInstance inst;
//     if (!inst.create()) {
//         qWarning("Vulkan not available");
//         return 1;
//     }

//     VulkanWindow window;
//     window.showMaximized();

//     return app.exec();

// }

#endif // VULKANWINDOWQT_H
