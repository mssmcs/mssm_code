#ifndef SVG_H
#define SVG_H

#include "color.h"
#include "vec2d.h"
#include <string>

struct NSVGimage;

class SvgRenderer
{
public:
    virtual void polygon(const std::vector<Vec2d>& points, mssm::Color& stroke, mssm::Color& fill) = 0;
    virtual void polyline(const std::vector<Vec2d>& points, mssm::Color& stroke) = 0;
};

class Svg
{
    struct NSVGimage* image;
public:
    Svg(std::string filename);
    ~Svg();
    void draw(SvgRenderer& renderer, Vec2d position);
    void draw(SvgRenderer& renderer, Vec2d position, mssm::Color stroke, mssm::Color fill);
    void drawLines(SvgRenderer& renderer, Vec2d position, mssm::Color stroke);
};



#endif // SVG_H
