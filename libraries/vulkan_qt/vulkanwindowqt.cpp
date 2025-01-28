#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>
//#include <minwindef.h>
// #include <stringapiset.h>
#endif


#include <QVulkanInstance>

#include "vulkanwindowqt.h"
#include "paths.h"

std::unique_ptr<VulkCanvas> createCanvas(VulkSurfaceRenderManager &renderManager)
{
    return std::make_unique<VulkCanvas>(renderManager);
}


VulkanWindowQT::VulkanWindowQT(std::string title, int width, int height, std::function<void(CoreWindowQt*)> drawCallback)
    : CoreWindowQt(drawCallback)
{

}

VkExtent2D VulkanWindowQT::getExtent()
{
    int width = QWindow::width();
    int height = QWindow::height();

    VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    return actualExtent;
}

VkSurfaceKHR VulkanWindowQT::createSurface(VkInstance instance)
{
    vInst.setVkInstance(instance);
    vInst.create();

    QWindow::setVulkanInstance(&vInst);

    return vInst.surfaceForWindow(this);
}

std::vector<const char *> VulkanWindowQT::getRequiredExtensions()
{
    std::vector<const char *> extensions;
    extensions.push_back("VK_KHR_surface");
    extensions.push_back("VK_KHR_win32_surface");

    return extensions;
    // return
    // uint32_t glfwExtensionCount = 0;
    // const char** glfwExtensions;
    // glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // return extensions;
}

std::shared_ptr<mssm::ImageInternal> VulkBackendWindowQt::loadImg(std::string filename)
{
    return VulkanGraphicsWindow<VulkanWindowQT, VulkCanvas>::loadImg(Paths::findAsset(filename));
}

void VulkBackendWindowQt::beginDrawing(bool wasResized)
{
    VulkanGraphicsWindow<VulkanWindowQT, VulkCanvas>::beginDrawing(wasResized);
}

void VulkBackendWindowQt::endDrawing()
{
    VulkanGraphicsWindow<VulkanWindowQT, VulkCanvas>::endDrawing();
}

void VulkBackendWindowQt::setCursor(gjh::CoreWindowCursor cursor)
{
    VulkanGraphicsWindow<VulkanWindowQT, VulkCanvas>::setCursor(cursor);
}


// void VulkanWindowQt::exposeEvent(QExposeEvent *)
// {
//     if (isExposed())
//     {
//         if (!m_initialized) {
//             m_initialized = true;
//             // initialize device, swapchain, etc.
//             QVulkanInstance *inst = vulkanInstance();
//            // // QVulkanFunctions *f = inst->functions();
//            //  uint32_t devCount = 0;
//            //  f->vkEnumeratePhysicalDevices(inst->vkInstance(), &devCount, nullptr);
//            //  // ...
//             // build the first frame
//             render();
//         }
//     }
// }

// bool VulkanWindowQt::event(QEvent *e)
// {
//     if (e->type() == QEvent::UpdateRequest) {
//         render();
//     }

//     return QWindow::event(e);
// }

// void VulkanWindowQt::render()
// {
//     // ...
//     requestUpdate(); // render continuously
// }


//#include <libloaderapi.h>

// VulkanWindowQt::VulkanWindowQt(VulkInstance& inst, QWidget *parent)
//     : QWidget{parent}, instance{inst}
// {

// }


// VkExtent2D VulkanWindowQt::getExtent()
// {
//     return {static_cast<uint32_t>(width()), static_cast<uint32_t>(height())};
// }

// VkSurfaceKHR VulkanWindowQt::createSurface(VkInstance instance)
// {
//     VkSurfaceKHR surface;

//     VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
//     surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
//     surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
//     surfaceCreateInfo.hwnd = reinterpret_cast<HWND>(winId());
//     if (vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
//         qFatal("Failed to create Vulkan surface");
//     }

//     return surface;
// }
