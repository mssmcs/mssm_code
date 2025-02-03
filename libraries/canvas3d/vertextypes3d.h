#ifndef VERTEXTYPES_H
#define VERTEXTYPES_H

#include "color.h"
#include "vec2d.h"
#include "vec3d.h"
#include "vec4d.h"

struct Vertex2d
{
public:
    Vec2f pos;
    Vec4f color;

public:
    constexpr Vertex2d()
        : pos{}
        , color{}
    {}
    constexpr Vertex2d(const Vertex2d& other) = default;
    constexpr Vertex2d(const Vec2d &pos, const mssm::Color &color)
        : pos{pos}
        , color{color.toRealVec4<Vec4f>()}
    {}
};

struct Vertex2dUV
{
public:
    Vec2f pos;
    Vec2f uv;
    Vec4f color;

public:
    constexpr Vertex2dUV()
        : pos{}
        , color{}
        , uv{}
    {}
    constexpr Vertex2dUV(const Vertex2dUV& other) = default;
    constexpr Vertex2dUV(const Vec2d &pos, const mssm::Color &color, const Vec2f &uv)
        : pos{pos}
        , uv{uv}
        , color{color.toRealVec4<Vec4f>()}
    {}
};

struct Vertex3dUV
{
public:
    Vec3f pos;
    Vec3f normal;
    Vec2f uv;
    Vec4f color;

public:
    constexpr Vertex3dUV()
        : pos{}
        , normal{}
        , color{}
        , uv{}
    {}
    constexpr Vertex3dUV(const Vertex3dUV& other) = default;
    constexpr Vertex3dUV(const Vec3d& pos, const Vec3d& normal, const mssm::Color &color, const Vec2f &uv)
        : pos{pos}
        , normal{normal}
        , uv{uv}
        , color{color.toRealVec4<Vec4f>()}
    {}
};


struct RectVert
{
public:
    Vec2f pos;
    Vec2f size;
    Vec4f borderColor;
    Vec4f fillColor;
public:
    constexpr RectVert()
        : pos{}
        , size{}
        , fillColor{}
        , borderColor{}
    {}
    constexpr RectVert(const RectVert& other) = default;
    constexpr RectVert(const Vec2d &pos,
                       double width,
                       double height,
                       const mssm::Color &borderColor,
                       const mssm::Color &fillColor)
        : pos{pos}
        , size{width, height}
        , borderColor{borderColor.toRealVec4<Vec4f>()}
        , fillColor{fillColor.toRealVec4<Vec4f>()}
    {}
};

struct TexturedRectVert
{
public:
    Vec2f pos;
    Vec2f size;
    Vec4f fillColor;
    uint32_t textureIndex;
public:
    constexpr TexturedRectVert()
        : pos{}
        , size{}
        , fillColor{}
        , textureIndex{}
    {}
    constexpr TexturedRectVert(const TexturedRectVert& other) = default;
    constexpr TexturedRectVert(const Vec2d &pos,
                               double width,
                               double height,
                               const mssm::Color &fillColor,
                               uint32_t textureIndex)
        : pos{pos}
        , size{width, height}
        , fillColor{fillColor.toRealVec4<Vec4f>()}
        , textureIndex{textureIndex}
    {}
};

struct RectVertUV
{
public:
    Vec2f pos{};
    Vec2f size{};
    Vec2f uvPos{};
    Vec2f uvSize{};
    Vec4f fillColor;
    uint32_t textureIndex{};
public:
    constexpr RectVertUV() {}
    constexpr RectVertUV(const RectVertUV& other) = default;
    constexpr RectVertUV(Vec2f pos,
                         Vec2f size,
                         Vec2f uvPos,
                         Vec2f uvSize,
                         Vec4f fillColor,
                         uint32_t textureIndex)
        : pos{pos}, size{size}, uvPos{uvPos}, uvSize{uvSize},
        fillColor{fillColor}, textureIndex{textureIndex}
    {}
};

#endif // VERTEXTYPES_H
