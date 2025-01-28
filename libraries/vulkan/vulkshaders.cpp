#include "vulkshaders.h"
#include <fstream>

// expect to find this externally (think about having VulkDevice provide this functionality?)
std::string findVulkanShader(const std::string &filename);

VkShaderModule createShaderModule(VulkDevice& device, const std::vector<char> &code)
{
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (device.fn.vkCreateShaderModule(device, &create_info, nullptr, &shaderModule) != VK_SUCCESS) {
        return VK_NULL_HANDLE; // failed to create shader module
    }

    return shaderModule;
}


std::vector<char> readFile(const std::string &relativeFilename)
{
    std::string filename = findVulkanShader(relativeFilename);

    if (filename.empty()) {
        throw std::runtime_error("could not find shader file!");
    }

    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open shader file!");
    }

    size_t file_size = (size_t) file.tellg();
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(file_size));

    file.close();

    return buffer;
}

VulkShaders::VulkShaders(VulkDevice& device)
    : device{device}
{
}

VulkShaders::~VulkShaders()
{
    for (auto &stage : stages) {
        device.fn.vkDestroyShaderModule(device, stage.module, nullptr);
    }
}

void VulkShaders::addStage(VkShaderStageFlagBits stage, std::string filename)
{
    auto code = readFile(filename);

    VkShaderModule module = createShaderModule(device, code);
    \
        if (!module) {
        throw std::runtime_error("failed to create shader module");
    }

    VkPipelineShaderStageCreateInfo stage_info{};
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.stage = stage;
    stage_info.module = module;
    stage_info.pName = "main";

    stages.push_back(stage_info);
}

void VulkShaders::addVertStage(std::string filename)
{
    addStage(VK_SHADER_STAGE_VERTEX_BIT, filename);
}

void VulkShaders::addFragStage(std::string filename)
{
    addStage(VK_SHADER_STAGE_FRAGMENT_BIT, filename);
}
