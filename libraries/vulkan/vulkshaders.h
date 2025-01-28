#ifndef VULKSHADERS_H
#define VULKSHADERS_H

#include "vulkdevice.h"
#include <string>

class VulkShaders
{
private:
    VulkDevice& device;
    std::vector<VkPipelineShaderStageCreateInfo> stages{};
public:
    VulkShaders(VulkDevice& device);
    ~VulkShaders();
    void addStage(VkShaderStageFlagBits stage, std::string filename);
    void addVertStage(std::string filename);
    void addFragStage(std::string filename);
    int  numStages() const { return stages.size(); }
    const VkPipelineShaderStageCreateInfo *pStages() const { return stages.data(); }
};

#endif // VULKSHADERS_H
