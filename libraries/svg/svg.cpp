#include "svg.h"
#include "nanosvg.h"
#include "vec2d.h"
#include "color.h"

#include <functional>

template <is2dVector V>
constexpr float distPtSeg(const V& p, const V& a, const V& b) {
    auto ab = b - a;
    auto ap = p - a;
    float d = ab.magSquared();
    float t = ap.dot(ab);

    if (d > 0) t /= d;
    if (t < 0) t = 0;
    else if (t > 1) t = 1;

    auto closest = V{a.x + ab.x * t, a.y + ab.y * t}; // Closest point on the segment
    auto diff = p - closest; // Vector from the point to the closest point

    return diff.magSquared();
}

template <is2dVector VIN>
void cubicBezier(const VIN& p1, const VIN& p2, const VIN& p3, const VIN& p4,
                 float tol, int level, std::function<void(const VIN&)> appendPoint) {
    if (level > 12) return;

    // Compute midpoints
    auto p12 = (p1 + p2) * 0.5f;
    auto p23 = (p2 + p3) * 0.5f;
    auto p34 = (p3 + p4) * 0.5f;
    auto p123 = (p12 + p23) * 0.5f;
    auto p234 = (p23 + p34) * 0.5f;
    auto p1234 = (p123 + p234) * 0.5f;

    // Check if the curve is flat enough
    float d = distPtSeg(p1234, p1, p4);
    if (d > tol) {
        // Subdivide the curve
        cubicBezier(p1, p12, p123, p1234, tol, level + 1, appendPoint);
        cubicBezier(p1234, p234, p34, p4, tol, level + 1, appendPoint);
    } else {
        // Add endpoint
        appendPoint(p4);
    }
}

template <is2dVector VIN, is2dVector VOUT>
void cubicBezier(const VIN& p1, const VIN& p2, const VIN& p3, const VIN& p4,
                 float tol, bool include1st, std::vector<VOUT>& vertices) {
    if (include1st) {
        vertices.push_back(cast<VOUT>(p1));
    }
    cubicBezier<VIN>(p1, p2, p3, p4, tol, 0, [&vertices](const VIN& p) { vertices.push_back(cast<VOUT>(p)); });
}

Svg::Svg(std::string filename)
{
    image = nsvgParseFromFile(filename.c_str(), "px", 96);
}

Svg::~Svg()
{
    nsvgDelete(image);
}

void Svg::draw(SvgRenderer& renderer, Vec2d position)
{
    std::vector<Vec2d> points;

  //  printf("size: %f x %f\n", image->width, image->height);
    // Use...
    for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
        mssm::Color fill = mssm::Color::fromIntBGR(shape->fill.color);
        mssm::Color stroke = mssm::Color::fromIntBGR(shape->stroke.color);
        for (auto path = shape->paths; path != NULL; path = path->next) {
            points.clear();
            for (int i = 0; i < path->npts-1; i += 3) {
                Vec2f* p = reinterpret_cast<Vec2f*>(&path->pts[i*2]);
                cubicBezier<Vec2f, Vec2d>(p[0]*10, p[1]*10, p[2]*10, p[3]*10, 1000, true, points);
            }
            if (path->closed) {
                std::reverse(points.begin(), points.end());
                renderer.polygon(points, stroke, fill);
            }
            else {
                for (auto& p : points) {
                    p += position;
                }
                renderer.polyline(points, stroke);
            }
        }
    }
}

void Svg::draw(SvgRenderer &renderer, Vec2d position, mssm::Color stroke, mssm::Color fill)
{
    std::vector<Vec2d> points;

    //  printf("size: %f x %f\n", image->width, image->height);
    // Use...
    for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
        // mssm::Color fill = mssm::Color::fromIntBGR(shape->fill.color);
        // mssm::Color stroke = mssm::Color::fromIntBGR(shape->stroke.color);
        for (auto path = shape->paths; path != NULL; path = path->next) {
            points.clear();
            for (int i = 0; i < path->npts-1; i += 3) {
                Vec2f* p = reinterpret_cast<Vec2f*>(&path->pts[i*2]);
                cubicBezier<Vec2f, Vec2d>(p[0]*10, p[1]*10, p[2]*10, p[3]*10, 1000, true, points);
            }
            for (auto& p : points) {
                p += position;
            }
            if (path->closed) {
                std::reverse(points.begin(), points.end());
                renderer.polygon(points, stroke, fill);
            }
            else {
                renderer.polyline(points, stroke);
            }
            break;
        }
        break;
    }
}

void Svg::drawLines(SvgRenderer &renderer, Vec2d position, mssm::Color stroke)
{
    std::vector<Vec2d> points;

    //  printf("size: %f x %f\n", image->width, image->height);
    // Use...
    for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
        // mssm::Color fill = mssm::Color::fromIntBGR(shape->fill.color);
        // mssm::Color stroke = mssm::Color::fromIntBGR(shape->stroke.color);
        for (auto path = shape->paths; path != NULL; path = path->next) {
            points.clear();
            if (path->npts == 0) {
                continue;
            }
            for (int i = 0; i < path->npts-1; i += 3) {
                Vec2f* p = reinterpret_cast<Vec2f*>(&path->pts[i*2]);
                cubicBezier<Vec2f, Vec2d>(p[0], p[1], p[2], p[3], 1, true, points);
            }
            if (path->closed) {
                points.push_back(points.front());
            }
            for (auto& p : points) {
                p += position;
            }
            renderer.polyline(points, stroke);
        }
    }
}
