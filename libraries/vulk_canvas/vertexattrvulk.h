#ifndef VERTEXATTRVULK_H
#define VERTEXATTRVULK_H

#include "vertextypes3d.h"
#include "vulkvertex.h"

template<>
VkVertexInputRate vulkVertexRate<Vertex2d>()
{
    return VK_VERTEX_INPUT_RATE_VERTEX;
}

template<>
void vulkVertexAttributes<Vertex2d>(
    std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&Vertex2d::pos));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32B32A32_SFLOAT, offset_of(&Vertex2d::color));
}

template<>
VkVertexInputRate vulkVertexRate<Vertex2dUV>()
{
    return VK_VERTEX_INPUT_RATE_VERTEX;
}

template<>
void vulkVertexAttributes<Vertex2dUV>(
    std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&Vertex2dUV::pos));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&Vertex2dUV::uv));
    addAttribute(attributeDescriptions,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offset_of(&Vertex2dUV::color));
}

template<>
VkVertexInputRate vulkVertexRate<Vertex3dUV>()
{
    return VK_VERTEX_INPUT_RATE_VERTEX;
}

template<>
void vulkVertexAttributes<Vertex3dUV>(
    std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32B32_SFLOAT, offset_of(&Vertex3dUV::pos));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32B32_SFLOAT, offset_of(&Vertex3dUV::normal));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&Vertex3dUV::uv));
    addAttribute(attributeDescriptions,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offset_of(&Vertex3dUV::color));
}

template<>
VkVertexInputRate vulkVertexRate<RectVert>()
{
    return VK_VERTEX_INPUT_RATE_INSTANCE;
}

template<>
void vulkVertexAttributes<RectVert>(
    std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&RectVert::pos));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&RectVert::size));
    addAttribute(attributeDescriptions,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offset_of(&RectVert::borderColor));
    addAttribute(attributeDescriptions,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offset_of(&RectVert::fillColor));
}

template<>
VkVertexInputRate vulkVertexRate<TexturedRectVert>()
{
    return VK_VERTEX_INPUT_RATE_INSTANCE;
}

template<>
void vulkVertexAttributes<TexturedRectVert>(
    std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&TexturedRectVert::pos));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&TexturedRectVert::size));
    addAttribute(attributeDescriptions,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offset_of(&TexturedRectVert::fillColor));
    addAttribute(attributeDescriptions,
                 VK_FORMAT_R32_UINT,
                 offset_of(&TexturedRectVert::textureIndex));
}

template<>
VkVertexInputRate vulkVertexRate<RectVertUV>()
{
    return VK_VERTEX_INPUT_RATE_INSTANCE;
}

template<>
void vulkVertexAttributes<RectVertUV>(
    std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&RectVertUV::pos));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&RectVertUV::size));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&RectVertUV::uvPos));
    addAttribute(attributeDescriptions, VK_FORMAT_R32G32_SFLOAT, offset_of(&RectVertUV::uvSize));
    addAttribute(attributeDescriptions,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offset_of(&RectVertUV::fillColor));
    addAttribute(attributeDescriptions, VK_FORMAT_R32_UINT, offset_of(&RectVertUV::textureIndex));
}

#endif // VERTEXATTRVULK_H
