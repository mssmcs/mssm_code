#ifndef SVGPATH_H
#define SVGPATH_H

#include "svgshape.h"
#include "vec2d.h"





/*
 * svg-path:
    wsp* moveto-drawto-command-groups? wsp*
moveto-drawto-command-groups:
    moveto-drawto-command-group
    | moveto-drawto-command-group wsp* moveto-drawto-command-groups
moveto-drawto-command-group:
    moveto wsp* drawto-commands?
drawto-commands:
    drawto-command
    | drawto-command wsp* drawto-commands
drawto-command:
    closepath
    | lineto
    | horizontal-lineto
    | vertical-lineto
    | curveto
    | smooth-curveto
    | quadratic-bezier-curveto
    | smooth-quadratic-bezier-curveto
    | elliptical-arc
    | bearing
moveto:
    ( "M" | "m" ) wsp* moveto-argument-sequence
moveto-argument-sequence:
    coordinate-pair
    | coordinate-pair comma-wsp? lineto-argument-sequence
closepath:
    ("Z" | "z")
lineto:
    ( "L" | "l" ) wsp* lineto-argument-sequence
lineto-argument-sequence:
    coordinate-pair
    | coordinate-pair comma-wsp? lineto-argument-sequence
horizontal-lineto:
    ( "H" | "h" ) wsp* horizontal-lineto-argument-sequence
horizontal-lineto-argument-sequence:
    coordinate
    | coordinate comma-wsp? horizontal-lineto-argument-sequence
vertical-lineto:
    ( "V" | "v" ) wsp* vertical-lineto-argument-sequence
vertical-lineto-argument-sequence:
    coordinate
    | coordinate comma-wsp? vertical-lineto-argument-sequence
curveto:
    ( "C" | "c" ) wsp* curveto-argument-sequence
curveto-argument-sequence:
    curveto-argument
    | curveto-argument comma-wsp? curveto-argument-sequence
curveto-argument:
    coordinate-pair comma-wsp? coordinate-pair comma-wsp? coordinate-pair
smooth-curveto:
    ( "S" | "s" ) wsp* smooth-curveto-argument-sequence
smooth-curveto-argument-sequence:
    smooth-curveto-argument
    | smooth-curveto-argument comma-wsp? smooth-curveto-argument-sequence
smooth-curveto-argument:
    coordinate-pair comma-wsp? coordinate-pair
quadratic-bezier-curveto:
    ( "Q" | "q" ) wsp* quadratic-bezier-curveto-argument-sequence
quadratic-bezier-curveto-argument-sequence:
    quadratic-bezier-curveto-argument
    | quadratic-bezier-curveto-argument comma-wsp?
        quadratic-bezier-curveto-argument-sequence
quadratic-bezier-curveto-argument:
    coordinate-pair comma-wsp? coordinate-pair
smooth-quadratic-bezier-curveto:
    ( "T" | "t" ) wsp* smooth-quadratic-bezier-curveto-argument-sequence
smooth-quadratic-bezier-curveto-argument-sequence:
    coordinate-pair
    | coordinate-pair comma-wsp? smooth-quadratic-bezier-curveto-argument-sequence
elliptical-arc:
    ( "A" | "a" ) wsp* elliptical-arc-argument-sequence
elliptical-arc-argument-sequence:
    elliptical-arc-argument
    | elliptical-arc-argument comma-wsp? elliptical-arc-argument-sequence
elliptical-arc-argument:
    number comma-wsp? number comma-wsp?
        number comma-wsp flag comma-wsp? flag comma-wsp? coordinate-pair
bearing:
    ( "B" | "b" ) wsp* bearing-argument-sequence
bearing-argument-sequence:
    number
    | number comma-wsp? bearing-argument-sequence
coordinate-pair:
    coordinate comma-wsp? coordinate
coordinate:
    number
nonnegative-number:
    integer-constant
    | floating-point-constant
number:
    sign? integer-constant
    | sign? floating-point-constant
flag:
    "0" | "1"
integer-constant:
    digit-sequence
floating-point-constant:
    fractional-constant exponent?
    | digit-sequence exponent
fractional-constant:
    digit-sequence? "." digit-sequence
    | digit-sequence "."
exponent:
    ( "e" | "E" ) sign? digit-sequence
sign:
    "+" | "-"
digit-sequence:
    digit
    | digit digit-sequence
digit:
    "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 * */


class SvgPathElement;

using SvgPathElementPtr = std::unique_ptr<SvgPathElement>;

class SvgPathElement
{
    // enum class ElementType
    // {
    //     MoveTo,
    //     LineTo,
    // };
};

class SvgPathMoveTo : public SvgPathElement
{
    std::string   name{"MoveTo"};
public:
    bool          relative;
    std::vector<Vec2d> coords;
};

class SvgPathLineTo : public SvgPathElement
{
    std::string   name{"LineTo"};
public:
    bool          relative;
    std::vector<Vec2d> coords;
};

class SvgPathClosePath : public SvgPathElement
{
    std::string   name{"ClosePath"};
};



class SvgPathHVLineTo : public SvgPathElement
{
    std::string   name{"HVLineTo"};
public:
    bool isVertical;
    std::vector<double> values;
};

class SvgPathCurveTo : public SvgPathElement
{
    std::string   name{"CurveTo"};
public:
    std::vector<Vec2d> coords;
};

class SvgPathSmoothCurveTo : public SvgPathElement
{
    std::string   name{"SmoothCurveTo"};
public:
    std::vector<Vec2d> coords;
};

class SvgQuadraticBezierCurveTo : public SvgPathElement
{
    std::string   name{"QuadraticBezierCurveTo"};
public:
    std::vector<Vec2d> coords;
};

class SvgSmoothQuadraticBezierCurveTo : public SvgPathElement
{
    std::string   name{"SmoothQuadraticBezierCurveTo"};
public:
    std::vector<Vec2d> coords;
};

class SvgEllipticalArcSegment : public SvgPathElement
{
    std::string   name{"EllipticalArcSeg"};
public:
    double v1, v2, v3;
    int flag1;
    int flag2;
    Vec2d pt;
};

class SvgBearing : public SvgPathElement
{
    std::string   name{"Bearing"};
public:
    std::vector<double> values;
};


class SvgPath : public SvgShape
{
private:

public:
    SvgPath();

    virtual void beforeSave() override;
    virtual void afterSave() override;
    virtual void afterLoad() override;

    void load(tinyxml2::XMLElement* xmlElement) override;
    void saveAsChildOf(tinyxml2::XMLElement* xmlElement) override;
};



#endif // SVGPATH_H
