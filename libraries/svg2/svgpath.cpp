#include "svgpath.h"
#include "vec2d.h"
#include <vector>
#include "parseinput.h"
#include <algorithm>

using namespace std;




class SvgPathVisitor
{
    std::vector<SvgPathElementPtr> elements;
public:
    void begin();
    void accept();
    void rollback();
    void addElement(SvgPathElementPtr ptr);
    void moveTo(const Vec2d& pt);
    void lineTo(const Vec2d& pt);
    void closePath();
    void horizontalLineTo(double x);
    void verticalLineTo(double y);
    void curveTo(const Vec2d& c1, const Vec2d& c2, const Vec2d& pt);
    void smoothCurveTo(const Vec2d& c2, const Vec2d& pt);
    void quadraticBezierCurveTo(const Vec2d& c, const Vec2d& pt);
    void smoothQuadraticBezierCurveTo(const Vec2d& pt);
    void ellipticalArc(double rx, double ry, double xAxisRotation, int largeArcFlag, int sweepFlag, const Vec2d& pt);
    void bearing(double angle, double x, double y);
};

void SvgPathVisitor::begin()
{
    SvgPathElementPtr sentinal;
    elements.push_back(std::move(sentinal));
}

void SvgPathVisitor::accept()
{
    auto ptr = std::find(elements.rbegin(), elements.rend(), nullptr);
    elements.erase((++ptr).base());
}

void SvgPathVisitor::rollback()
{
    auto ptr = std::find_if(elements.rbegin(), elements.rend(), [](const SvgPathElementPtr& ptr) { return !ptr; });
    elements.erase((++ptr).base(), elements.end());
}

void SvgPathVisitor::addElement(SvgPathElementPtr ptr)
{
    elements.push_back(std::move(ptr));
}


class SvgPathParser
{
public:
    bool parsePath(const std::string& path, SvgPathVisitor& visitor);
private:
    bool parse(ParseInput input, SvgPathVisitor& visitor);
    bool parseMoveToDrawToGrp(ParseInput input, SvgPathVisitor& visitor);

    bool parseCoords(ParseInput input, vector<Vec2d>& coords);
    bool parseCoord(ParseInput input, Vec2d& pt);
    bool parseValues(ParseInput input, vector<double>& values);
    bool parseValue(ParseInput input, double& value);

    bool parseMoveTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseDrawTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseClosePath(ParseInput input, SvgPathVisitor& visitor);
    bool parseLineTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseHorizontalLineTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseVerticalLineTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseCurveTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseSmoothCurveTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseQuadraticBezierCurveTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseSmoothQuadraticBezierCurveTo(ParseInput input, SvgPathVisitor& visitor);
    bool parseEllipticalArc(ParseInput input, SvgPathVisitor& visitor);
    bool parseEllipticalArcSegment(ParseInput input, SvgPathVisitor& visitor);
    bool parseBearing(ParseInput input, SvgPathVisitor& visitor);
};

bool SvgPathParser::parsePath(const std::string& path, SvgPathVisitor& visitor)
{
    ParseInput pt(path);
    bool result = parse(pt, visitor);

    auto remainingText = pt.text();

    return result;
}

class SvgParserContext
{
    ParseInput& input;
    SvgPathVisitor& visitor;
public:
    SvgParserContext(ParseInput& input, SvgPathVisitor& visitor) : input(input), visitor(visitor)
    {
        visitor.begin();
    }

    ~SvgParserContext()
    {
        if (input.gotFail())
        {
            visitor.rollback();
        }
        else
        {
            visitor.accept();
        }
    }
};

bool SvgPathParser::parse(ParseInput input, SvgPathVisitor& visitor)
{
    if (!parseMoveToDrawToGrp(input, visitor))
    {
        return input.fail();
    }

    while (parseMoveToDrawToGrp(input, visitor))
    {

    }

    return input.success();
}

bool SvgPathParser::parseMoveToDrawToGrp(ParseInput input, SvgPathVisitor& visitor)
{
    if (!parseMoveTo(input, visitor))
    {
        return input.fail();
    }

    while (parseDrawTo(input, visitor))
    {

    }

    return input.success();
}

bool SvgPathParser::parseDrawTo(ParseInput input, SvgPathVisitor& visitor)
{
    SvgParserContext context(input, visitor);

    return parseClosePath(input, visitor)
        || parseLineTo(input, visitor)
        || parseHorizontalLineTo(input, visitor)
        || parseVerticalLineTo(input, visitor)
        || parseCurveTo(input, visitor)
        || parseSmoothCurveTo(input, visitor)
        || parseQuadraticBezierCurveTo(input, visitor)
        || parseSmoothQuadraticBezierCurveTo(input, visitor)
        || parseEllipticalArc(input, visitor)
        || parseBearing(input, visitor)
        || input.fail();
}

bool SvgPathParser::parseClosePath(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("zZ", matchIdx))
    {
        return input.fail();
    }

    return input.success();
}


bool SvgPathParser::parseHorizontalLineTo(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("hH", matchIdx))
    {
        return input.fail();
    }

    auto lineTo = std::make_unique<SvgPathHVLineTo>();

    lineTo->isVertical = false;

    if (!parseValues(input, lineTo->values))
    {
        return input.fail();
    }

    visitor.addElement(std::move(lineTo));

    return input.success();
}

bool SvgPathParser::parseVerticalLineTo(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("vV", matchIdx))
    {
        return input.fail();
    }

    auto lineTo = std::make_unique<SvgPathHVLineTo>();

    lineTo->isVertical = true;

    if (!parseValues(input, lineTo->values))
    {
        return input.fail();
    }

    visitor.addElement(std::move(lineTo));

    return input.success();
}

bool SvgPathParser::parseCurveTo(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("cC", matchIdx))
    {
        return input.fail();
    }

    auto curveTo = std::make_unique<SvgPathCurveTo>();

    if (!(parseCoords(input, curveTo->coords) && curveTo->coords.size() % 3 == 0))
    {
        return input.fail();
    }

    visitor.addElement(std::move(curveTo));

    return input.success();
}

bool SvgPathParser::parseSmoothCurveTo(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("sS", matchIdx))
    {
        return input.fail();
    }

    auto curveTo = std::make_unique<SvgPathSmoothCurveTo>();

    if (!(parseCoords(input, curveTo->coords) && curveTo->coords.size() % 2 == 0))
    {
        return input.fail();
    }

    visitor.addElement(std::move(curveTo));

    return input.success();
}

bool SvgPathParser::parseQuadraticBezierCurveTo(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("qQ", matchIdx))
    {
        return input.fail();
    }

    auto curveTo = std::make_unique<SvgQuadraticBezierCurveTo>();

    if (!(parseCoords(input, curveTo->coords) && curveTo->coords.size() % 2 == 0))
    {
        return input.fail();
    }

    visitor.addElement(std::move(curveTo));

    return input.success();
}

bool SvgPathParser::parseSmoothQuadraticBezierCurveTo(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("tT", matchIdx))
    {
        return input.fail();
    }

    auto curveTo = std::make_unique<SvgSmoothQuadraticBezierCurveTo>();

    if (!parseCoords(input, curveTo->coords))
    {
        return input.fail();
    }

    visitor.addElement(std::move(curveTo));

    return input.success();
}

bool SvgPathParser::parseEllipticalArcSegment(ParseInput input, SvgPathVisitor& visitor)
{
    auto arcSeg = std::make_unique<SvgEllipticalArcSegment>();

    if (!(input.readDouble(arcSeg->v1) &&
            input.readDouble(arcSeg->v2) &&
            input.readDouble(arcSeg->v3) &&
            input.readInt(arcSeg->flag1) &&
            input.readInt(arcSeg->flag2) &&
          parseCoord(input, arcSeg->pt))) {
        return input.fail();
    }

    visitor.addElement(std::move(arcSeg));

    return input.success();
}

bool SvgPathParser::parseEllipticalArc(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("aA", matchIdx))
    {
        return input.fail();
    }

    if (!parseEllipticalArcSegment(input, visitor))
    {
        return input.fail();
    }

    while (parseEllipticalArcSegment(input, visitor))
    {
    }

    return input.success();
}

bool SvgPathParser::parseBearing(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("bB", matchIdx))
    {
        return input.fail();
    }

    auto bearing = std::make_unique<SvgBearing>();

    if (!parseValues(input, bearing->values))
    {
        return input.fail();
    }

    visitor.addElement(std::move(bearing));

    return input.success();
}


bool SvgPathParser::parseCoords(ParseInput input, vector<Vec2d>& coords)
{
    Vec2d pt;

    while (parseCoord(input, pt))
    {
        coords.push_back(pt);
    }

    return !coords.empty() || input.fail();
}

bool SvgPathParser::parseValues(ParseInput input, vector<double>& values)
{
    double value;

    while (parseValue(input, value))
    {
        values.push_back(value);
    }

    return !values.empty() || input.fail();
}

bool SvgPathParser::parseValue(ParseInput input, double &value)
{
    return input.readDouble(value) || input.fail();
}

bool SvgPathParser::parseCoord(ParseInput input, Vec2d& pt)
{
    return (input.readDouble(pt.x) && input.skip(',',true) && input.readDouble(pt.y)) || input.fail();
}


bool SvgPathParser::parseMoveTo(ParseInput input, SvgPathVisitor& visitor)
{
    SvgParserContext context(input, visitor);

    int matchIdx;

    if (!input.readOneOf("mM", matchIdx))
    {
        return input.fail();
    }

    auto moveTo = std::make_unique<SvgPathMoveTo>();

    moveTo->relative = matchIdx == 0;

    if (!parseCoords(input, moveTo->coords))
    {
        return input.fail();
    }

    visitor.addElement(std::move(moveTo));

    return true;
}

bool SvgPathParser::parseLineTo(ParseInput input, SvgPathVisitor& visitor)
{
    int matchIdx;

    if (!input.readOneOf("lL", matchIdx))
    {
        return input.fail();
    }

    auto lineTo = std::make_unique<SvgPathLineTo>();

    lineTo->relative = matchIdx == 0;

    if (!parseCoords(input, lineTo->coords))
    {
        return input.fail();
    }

    visitor.addElement(std::move(lineTo));

    return true;
}

SvgPath::SvgPath()
{

}

void SvgPath::beforeSave()
{

}

void SvgPath::afterSave()
{
  //  attributes.erase(attributes.find("d"));
}

void SvgPath::afterLoad()
{
  auto d = attributes.find("d");
  if (d != attributes.end())
  {
      SvgPathVisitor vis;
      SvgPathParser parser;

      parser.parsePath(d->second, vis);
   }
}

void SvgPath::load(tinyxml2::XMLElement* xmlElement) {
    loadAttributes(xmlElement);
    afterLoad();
}

void SvgPath::saveAsChildOf(tinyxml2::XMLElement* parent) {
    tinyxml2::XMLElement* element = parent->GetDocument()->NewElement("path");
    parent->InsertEndChild(element);
    beforeSave();
    saveAttributes(element);
    afterSave();
}

