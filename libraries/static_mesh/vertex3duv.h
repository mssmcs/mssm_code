#ifndef VERTEX3DUV_H
#define VERTEX3DUV_H

#include "color.h"
#include "vec2d.h"
#include "vec3d.h"
#include "vec4d.h"

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

#endif // VERTEX3DUV_H
