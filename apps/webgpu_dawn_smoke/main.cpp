#include <cmath>
#include <iostream>
#include <string>

#include <GLFW/glfw3.h>

#include "webgpu_context.h"
#include "webgpu_glfw_surface.h"

int main()
{
    mssm::WebGpuContext context;
    std::string errorMessage;

    if (!glfwInit()) {
        std::cerr << "glfwInit failed" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "WebGPU Dawn surface smoke", nullptr, nullptr);
    if (!window) {
        std::cerr << "glfwCreateWindow failed" << std::endl;
        glfwTerminate();
        return 1;
    }

    if (!context.initialize(errorMessage)) {
        std::cerr << "WebGPU initialization failed: " << errorMessage << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    WGPUSurface surface{};
    if (!mssm::webgpuCreateSurfaceFromGlfwWindow(context.getInstance(), window, &surface, errorMessage)) {
        std::cerr << "Surface creation failed: " << errorMessage << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    if (!context.requestAdapterAndDevice(surface, errorMessage)) {
        std::cerr << "WebGPU adapter/device setup failed: " << errorMessage << std::endl;
        wgpuSurfaceRelease(surface);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwPollEvents();

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    if (framebufferWidth <= 0 || framebufferHeight <= 0) {
        glfwGetWindowSize(window, &framebufferWidth, &framebufferHeight);
    }
    if (framebufferWidth <= 0 || framebufferHeight <= 0) {
        framebufferWidth = 800;
        framebufferHeight = 600;
    }

    WGPUTextureFormat surfaceFormat = WGPUTextureFormat_Undefined;
    if (!mssm::webgpuConfigureSurfaceForDevice(surface,
                                               context.getAdapter(),
                                               context.getDevice(),
                                               static_cast<uint32_t>(framebufferWidth),
                                               static_cast<uint32_t>(framebufferHeight),
                                               &surfaceFormat,
                                               errorMessage)) {
        std::cerr << "Surface configure failed: " << errorMessage << std::endl;
        wgpuSurfaceRelease(surface);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    WGPUQueue queue = wgpuDeviceGetQueue(context.getDevice());
    if (!queue) {
        std::cerr << "wgpuDeviceGetQueue failed" << std::endl;
        wgpuSurfaceUnconfigure(surface);
        wgpuSurfaceRelease(surface);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "WebGPU Dawn smoke test: close window or press ESC to exit." << std::endl;

    double startTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        int w = 0;
        int h = 0;
        glfwGetFramebufferSize(window, &w, &h);
        if (w != framebufferWidth || h != framebufferHeight) {
            framebufferWidth = w;
            framebufferHeight = h;
            if (framebufferWidth > 0 && framebufferHeight > 0) {
                if (!mssm::webgpuConfigureSurfaceForDevice(surface,
                                                         context.getAdapter(),
                                                         context.getDevice(),
                                                         static_cast<uint32_t>(framebufferWidth),
                                                         static_cast<uint32_t>(framebufferHeight),
                                                         &surfaceFormat,
                                                         errorMessage)) {
                    std::cerr << "Surface reconfigure failed: " << errorMessage << std::endl;
                    break;
                }
            }
        }

        if (framebufferWidth <= 0 || framebufferHeight <= 0) {
            continue;
        }

        const double t = glfwGetTime() - startTime;
        WGPUColor clear{};
        clear.r = 0.15 + 0.1 * std::sin(t);
        clear.g = 0.25 + 0.1 * std::sin(t * 0.7);
        clear.b = 0.45 + 0.15 * std::sin(t * 0.5);
        clear.a = 1.0;

        if (!mssm::webgpuRenderSurfaceFrame(surface, context.getDevice(), queue, clear, errorMessage)) {
            std::cerr << "Frame failed: " << errorMessage << std::endl;
            break;
        }
    }

    wgpuSurfaceUnconfigure(surface);
    wgpuSurfaceRelease(surface);

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "WebGPU Dawn smoke test exited normally." << std::endl;
    return 0;
}
