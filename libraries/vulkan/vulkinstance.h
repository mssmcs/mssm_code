#ifndef VULKINSTANCE_H
#define VULKINSTANCE_H

#define VK_USE_64_BIT_PTR_DEFINES 1
#include "volk.h"
#include <vector>

bool validationLayersEnabled();
const std::vector<const char*>& getValidationLayers();

class VulkInstance
{
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debugMessenger{};
public:
    VulkInstance(std::vector<const char*> requiredExtensions);
    ~VulkInstance();
    VulkInstance(const VulkInstance&) = delete;
    VulkInstance& operator=(const VulkInstance&) = delete;
    operator VkInstance() { return instance; }
private:
    void createInstance(std::vector<const char *> requiredExtensions);
    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

};

#endif // VULKINSTANCE_H
