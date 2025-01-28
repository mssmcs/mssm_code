#include "vulkvertex.h"


VkPipelineVertexInputStateCreateInfo *VertexInfo::info()
{
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertex_input_info.vertexBindingDescriptionCount = bindingDescriptions.size();
    vertex_input_info.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertex_input_info.pVertexBindingDescriptions = bindingDescriptions.data();
    vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data();

    return &vertex_input_info;
}


// VertexInfo::VertexInfo()
// {
//     vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

//     auto bindingNum = bindingDescriptions.size();

//     bindingDescriptions.push_back(T::getBindingDescription());
//     bindingDescriptions.back().binding = bindingNum;

//     for (auto& aDesc : T::getAttributeDescriptions()) {
//         aDesc.binding = bindingNum;
//         attributeDescriptions.push_back(aDesc);
//     }

//     vertex_input_info.vertexBindingDescriptionCount = bindingDescriptions.size();
//     vertex_input_info.vertexAttributeDescriptionCount = attributeDescriptions.size();
//     vertex_input_info.pVertexBindingDescriptions = bindingDescriptions.data();
//     vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data();
// }
