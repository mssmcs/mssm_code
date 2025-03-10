#include "SVGCanvas.h"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "image.h"

// Helper methods
SVG::Element* SVGCanvas::currentTarget() {
    if (!m_groupStack.empty()) {
        return m_groupStack.top();
    }
    return &m_svg;
}

std::string SVGCanvas::colorToString(const mssm::Color& color) const {
    if (color.a == 0) {
        return "none";
    }

    char buffer[10];
    if (color.a < 255) {
        snprintf(buffer, sizeof(buffer), "#%02x%02x%02x%02x", color.r, color.g, color.b, color.a);
    } else {
        snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", color.r, color.g, color.b);
    }
    return std::string(buffer);
}

void SVGCanvas::applyStyle(SVG::Element* shape, const mssm::Color& stroke, const mssm::Color& fill) {
    shape->set_attr("stroke", colorToString(stroke));
    shape->set_attr("fill", colorToString(fill));

    if (stroke.a > 0) {
        shape->set_attr("stroke-width", "1");
    }
}

// Constructor and destructor
SVGCanvas::SVGCanvas(int width, int height, const std::string& filename)
    : m_width(width), m_height(height), m_backgroundColor(mssm::BLACK), m_filename(filename) {

    m_svg.set_attr("width", width);
    m_svg.set_attr("height", height);
    m_svg.set_attr("viewBox", "0 0 " + std::to_string(width) + " " + std::to_string(height));
}

SVGCanvas::~SVGCanvas() {
    save();
}

// Public methods
void SVGCanvas::save() const {
    std::ofstream out(m_filename);
    if (out) {
        // Convert the SVG to string using the operator std::string() which calls svg_to_string internally
        out << std::string(m_svg);
    }
}

bool SVGCanvas::isDrawable() {
    return true;
}

int SVGCanvas::width() {
    return m_width;
}

int SVGCanvas::height() {
    return m_height;
}

void SVGCanvas::setBackground(mssm::Color c) {
    m_backgroundColor = c;

    // Find or create the background rectangle
    SVG::Element* background = m_svg.get_element_by_id("background");
    if (!background) {
        background = m_svg.add_child<SVG::Rect>();
        background->set_attr("id", "background");
        background->set_attr("x", "0");
        background->set_attr("y", "0");
        background->set_attr("width", std::to_string(m_width));
        background->set_attr("height", std::to_string(m_height));
    }

    background->set_attr("fill", colorToString(c));
}

void SVGCanvas::line(Vec2d p1, Vec2d p2, mssm::Color c) {
    auto line = currentTarget()->add_child<SVG::Line>(p1.x, p2.x, p1.y, p2.y);
    applyStyle(line, c);
}

void SVGCanvas::ellipse(Vec2d center, double w, double h, mssm::Color c, mssm::Color f) {
    auto ellipse = currentTarget()->add_child<SVG::Ellipse>(center.x, center.y, w/2, h/2);
    applyStyle(ellipse, c, f);
}

void SVGCanvas::arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c) {
    // SVG path for arc
    auto path = currentTarget()->add_child<SVG::Path>();

    // Convert angles from degrees to radians
    double startAngle = a * M_PI / 180.0;
    double endAngle = (a + alen) * M_PI / 180.0;

    // Calculate start and end points
    double rx = w / 2;
    double ry = h / 2;
    double x1 = center.x + rx * cos(startAngle);
    double y1 = center.y + ry * sin(startAngle);
    double x2 = center.x + rx * cos(endAngle);
    double y2 = center.y + ry * sin(endAngle);

    // Create the path
    path->start(x1, y1);

    // SVG large-arc-flag is 0 for arcs less than 180 degrees, 1 for arcs greater than 180
    int largeArcFlag = (alen > 180) ? 1 : 0;

    // SVG sweep-flag is 1 for clockwise, 0 for counterclockwise
    int sweepFlag = (alen > 0) ? 1 : 0;

    path->curve_to(rx, ry, 0.0, largeArcFlag, sweepFlag, x2, y2);

    applyStyle(path, c);
    path->set_attr("fill", "none");
}

void SVGCanvas::chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f) {
    // SVG path for chord
    auto path = currentTarget()->add_child<SVG::Path>();

    // Convert angles from degrees to radians
    double startAngle = a * M_PI / 180.0;
    double endAngle = (a + alen) * M_PI / 180.0;

    // Calculate start and end points
    double rx = w / 2;
    double ry = h / 2;
    double x1 = center.x + rx * cos(startAngle);
    double y1 = center.y + ry * sin(startAngle);
    double x2 = center.x + rx * cos(endAngle);
    double y2 = center.y + ry * sin(endAngle);

    // Create the path
    path->start(x1, y1);

    // SVG large-arc-flag is 0 for arcs less than 180 degrees, 1 for arcs greater than 180
    int largeArcFlag = (alen > 180) ? 1 : 0;

    // SVG sweep-flag is 1 for clockwise, 0 for counterclockwise
    int sweepFlag = (alen > 0) ? 1 : 0;

    path->curve_to(rx, ry, 0.0, largeArcFlag, sweepFlag, x2, y2);
    path->line_to(x1, y1); // Close the chord

    applyStyle(path, c, f);
}

void SVGCanvas::pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f) {
    // SVG path for pie slice
    auto path = currentTarget()->add_child<SVG::Path>();

    // Convert angles from degrees to radians
    double startAngle = a * M_PI / 180.0;
    double endAngle = (a + alen) * M_PI / 180.0;

    // Calculate start and end points
    double rx = w / 2;
    double ry = h / 2;
    double x1 = center.x + rx * cos(startAngle);
    double y1 = center.y + ry * sin(startAngle);
    double x2 = center.x + rx * cos(endAngle);
    double y2 = center.y + ry * sin(endAngle);

    // Create the path
    path->start(center.x, center.y); // Start at center
    path->line_to(x1, y1); // Line to first point

    // SVG large-arc-flag is 0 for arcs less than 180 degrees, 1 for arcs greater than 180
    int largeArcFlag = (alen > 180) ? 1 : 0;

    // SVG sweep-flag is 1 for clockwise, 0 for counterclockwise
    int sweepFlag = (alen > 0) ? 1 : 0;

    path->curve_to(rx, ry, 0.0, largeArcFlag, sweepFlag, x2, y2);
    path->line_to(center.x, center.y); // Close the pie slice

    applyStyle(path, c, f);
}

void SVGCanvas::rect(Vec2d corner, double w, double h, mssm::Color c, mssm::Color f) {
    auto rect = currentTarget()->add_child<SVG::Rect>(corner.x, corner.y, w, h, 0);
    applyStyle(rect, c, f);
}

void SVGCanvas::polygon(const std::vector<Vec2d>& points, mssm::Color c, mssm::Color f) {
    std::vector<SVG::Point> svgPoints;
    for (const auto& pt : points) {
        svgPoints.emplace_back(pt.x, pt.y);
    }

    auto polygon = currentTarget()->add_child<SVG::Polygon>(svgPoints);
    applyStyle(polygon, c, f);
}

void SVGCanvas::polyline(const std::vector<Vec2d>& points, mssm::Color c) {
    // Create a path instead since SVG doesn't have a direct polyline
    if (points.empty()) return;

    auto path = currentTarget()->add_child<SVG::Path>();
    path->start(points[0].x, points[0].y);

    for (size_t i = 1; i < points.size(); i++) {
        path->line_to(points[i].x, points[i].y);
    }

    applyStyle(path, c);
    path->set_attr("fill", "none");
}

void SVGCanvas::points(const std::vector<Vec2d>& points, mssm::Color c) {
    for (const auto& pt : points) {
        point(pt, c);
    }
}

#ifdef SUPPORT_MSSM_ARRAY
void SVGCanvas::polygon(const mssm::Array<Vec2d>& points, mssm::Color border, mssm::Color fill) {
    std::vector<Vec2d> vec(points.begin(), points.end());
    polygon(vec, border, fill);
}

void SVGCanvas::polyline(const mssm::Array<Vec2d>& points, mssm::Color color) {
    std::vector<Vec2d> vec(points.begin(), points.end());
    polyline(vec, color);
}

void SVGCanvas::points(const mssm::Array<Vec2d>& points, mssm::Color c) {
    std::vector<Vec2d> vec(points.begin(), points.end());
    this->points(vec, c);
}
#endif // SUPPORT_MSSM_ARRAY

void SVGCanvas::polygon(std::initializer_list<Vec2d> points, mssm::Color border, mssm::Color fill) {
    std::vector<Vec2d> vec(points);
    polygon(vec, border, fill);
}

void SVGCanvas::polyline(std::initializer_list<Vec2d> points, mssm::Color color) {
    std::vector<Vec2d> vec(points);
    polyline(vec, color);
}

void SVGCanvas::points(std::initializer_list<Vec2d> points, mssm::Color c) {
    std::vector<Vec2d> vec(points);
    this->points(vec, c);
}

void SVGCanvas::text(Vec2d pos, const FontInfo& sizeAndFace, const std::string& str,
                     mssm::Color textcolor, HAlign hAlign, VAlign vAlign) {

    auto text = currentTarget()->add_child<SVG::Text>(pos.x, pos.y, str);
    text->set_attr("fill", colorToString(textcolor));
  //  text->set_attr("font-family", sizeAndFace.family);
    text->set_attr("font-size", std::to_string(sizeAndFace.getSize()) + "px");

    // Set text alignment
    if (hAlign == HAlign::center) {
        text->set_attr("text-anchor", "middle");
    } else if (hAlign == HAlign::right) {
        text->set_attr("text-anchor", "end");
    }

    // Set vertical alignment
    if (vAlign == VAlign::top) {
        text->set_attr("dominant-baseline", "text-before-edge");
    } else if (vAlign == VAlign::center) {
        text->set_attr("dominant-baseline", "middle");
    } else if (vAlign == VAlign::bottom) {
        text->set_attr("dominant-baseline", "text-after-edge");
    }
}

void SVGCanvas::textExtents(const FontInfo& sizeAndFace, const std::string& str, TextExtents& extents) {
    // This is a rough approximation, as SVG doesn't provide direct text measurement
    extents.textWidth = textWidth(sizeAndFace, str);
    extents.textAdvance = extents.textWidth;  // TODO: improve this
    extents.fontHeight = sizeAndFace.getSize();
    extents.textHeight = extents.fontHeight; // TODO: improve this
    extents.fontAscent = sizeAndFace.getSize() * 0.8;
    extents.fontDescent = sizeAndFace.getSize() * 0.2;
}

double SVGCanvas::textWidth(const FontInfo& sizeAndFace, const std::string& str) {
    // Rough approximation - average character width is about 0.6 times the font size
    return str.length() * sizeAndFace.getSize() * 0.6;
}

std::vector<double> SVGCanvas::getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) {
    std::vector<double> offsets;
    offsets.reserve(text.length() + 1);

    double avgCharWidth = sizeAndFace.getSize() * 0.6;
    double currentX = startX;

    offsets.push_back(currentX);
    for (size_t i = 0; i < text.length(); i++) {
        currentX += avgCharWidth;
        offsets.push_back(currentX);
    }

    return offsets;
}

void SVGCanvas::point(Vec2d pos, mssm::Color c) {
    // Draw a small circle to represent a point
    auto circle = currentTarget()->add_child<SVG::Circle>(pos.x, pos.y, 1);
    applyStyle(circle, c, c);
}

void SVGCanvas::image(Vec2d pos, const mssm::Image& img) {
    // SVG can't directly embed images, would need to use <image> with base64 data
    // For simplicity, just draw a placeholder rectangle
    auto rect = currentTarget()->add_child<SVG::Rect>(pos.x, pos.y, img.width(), img.height(), 0);
    rect->set_attr("fill", "grey");
    rect->set_attr("stroke", "black");

    // Add text label
    auto text = currentTarget()->add_child<SVG::Text>(pos.x + img.width()/2, pos.y + img.height()/2, "Image");
    text->set_attr("text-anchor", "middle");
    text->set_attr("dominant-baseline", "middle");
    text->set_attr("fill", "black");
}

void SVGCanvas::image(Vec2d pos, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    // Simplified placeholder
    image(pos, img);
}

void SVGCanvas::image(Vec2d pos, double w, double h, const mssm::Image& img) {
    // Simplified placeholder
    auto rect = currentTarget()->add_child<SVG::Rect>(pos.x, pos.y, w, h, 0);
    rect->set_attr("fill", "grey");
    rect->set_attr("stroke", "black");

    auto text = currentTarget()->add_child<SVG::Text>(pos.x + w/2, pos.y + h/2, "Image");
    text->set_attr("text-anchor", "middle");
    text->set_attr("dominant-baseline", "middle");
    text->set_attr("fill", "black");
}

void SVGCanvas::image(Vec2d pos, double w, double h, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    // Simplified placeholder
    image(pos, w, h, img);
}

void SVGCanvas::imageC(Vec2d center, double angle, const mssm::Image& img) {
    // SVG can do transforms, but for simplicity with placeholder
    double w = img.width();
    double h = img.height();
    Vec2d topLeft(center.x - w/2, center.y - h/2);

    auto group = currentTarget()->add_child<SVG::Group>();
    group->set_attr("transform", "rotate(" + std::to_string(angle) + " " +
                                     std::to_string(center.x) + " " + std::to_string(center.y) + ")");

    auto rect = group->add_child<SVG::Rect>(topLeft.x, topLeft.y, w, h, 0);
    rect->set_attr("fill", "grey");
    rect->set_attr("stroke", "black");

    auto text = group->add_child<SVG::Text>(center.x, center.y, "Image");
    text->set_attr("text-anchor", "middle");
    text->set_attr("dominant-baseline", "middle");
    text->set_attr("fill", "black");
}

void SVGCanvas::imageC(Vec2d center, double angle, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    // Simplified placeholder
    imageC(center, angle, img);
}

void SVGCanvas::imageC(Vec2d center, double angle, double w, double h, const mssm::Image& img) {
    Vec2d topLeft(center.x - w/2, center.y - h/2);

    auto group = currentTarget()->add_child<SVG::Group>();
    group->set_attr("transform", "rotate(" + std::to_string(angle) + " " +
                                     std::to_string(center.x) + " " + std::to_string(center.y) + ")");

    auto rect = group->add_child<SVG::Rect>(topLeft.x, topLeft.y, w, h, 0);
    rect->set_attr("fill", "grey");
    rect->set_attr("stroke", "black");

    auto text = group->add_child<SVG::Text>(center.x, center.y, "Image");
    text->set_attr("text-anchor", "middle");
    text->set_attr("dominant-baseline", "middle");
    text->set_attr("fill", "black");
}

void SVGCanvas::imageC(Vec2d center, double angle, double w, double h, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    // Simplified placeholder
    imageC(center, angle, w, h, img);
}

bool SVGCanvas::isClipped(Vec2d pos) const {
    // Simple bounds check, doesn't account for complex clipping paths
    return pos.x < 0 || pos.y < 0 || pos.x >= m_width || pos.y >= m_height;
}

void SVGCanvas::pushClip(int x, int y, int w, int h, bool replace) {
    // SVG clipping would require defining a clipPath and applying it
    // This is a simplified implementation
    auto group = currentTarget()->add_child<SVG::Group>();
    m_groupStack.push(group);

    // Define a clip path
    std::string clipId = "clip_" + std::to_string(rand());
    group->set_attr("clip-path", "url(#" + clipId + ")");

    // The actual clip path definition would go here in a real implementation
}

void SVGCanvas::popClip() {
    if (!m_groupStack.empty()) {
        m_groupStack.pop();
    }
}

void SVGCanvas::setClip(int x, int y, int w, int h) {
    // Reset clip then push new one
    resetClip();
    pushClip(x, y, w, h, true);
}

void SVGCanvas::resetClip() {
    // Clear all clip state
    while (!m_groupStack.empty()) {
        m_groupStack.pop();
    }
}

void SVGCanvas::setViewport(int x, int y, int w, int h) {
    // SVG viewBox can be used for this
    m_svg.set_attr("viewBox", std::to_string(x) + " " + std::to_string(y) + " " +
                                  std::to_string(w) + " " + std::to_string(h));
}

void SVGCanvas::resetViewport() {
    m_svg.set_attr("viewBox", "0 0 " + std::to_string(m_width) + " " + std::to_string(m_height));
}

void SVGCanvas::pushGroup(std::string groupName) {
    auto group = currentTarget()->add_child<SVG::Group>();
    if (!groupName.empty()) {
        group->set_attr("id", groupName);
    }
    m_groupStack.push(group);
}

void SVGCanvas::popGroup() {
    if (!m_groupStack.empty()) {
        m_groupStack.pop();
    }
}

void SVGCanvas::polygonPattern(const std::vector<Vec2d>& points, mssm::Color c, mssm::Color f) {
    if (points.size() < 3) return;

    // First, create the polygon with transparent fill and the specified border
    auto polygonElement = currentTarget()->add_child<SVG::Polygon>(std::vector<SVG::Point>());

    // Convert points to SVG format and find bounds
    std::string pointsStr;
    double minX = points[0].x, minY = points[0].y;
    double maxX = points[0].x, maxY = points[0].y;

    for (const auto& pt : points) {
        pointsStr += std::to_string(pt.x) + "," + std::to_string(pt.y) + " ";
        minX = std::min(minX, pt.x);
        minY = std::min(minY, pt.y);
        maxX = std::max(maxX, pt.x);
        maxY = std::max(maxY, pt.y);
    }

    polygonElement->set_attr("points", pointsStr);
    polygonElement->set_attr("stroke", colorToString(c));
    polygonElement->set_attr("fill", "url(#crosshatch)");

    // // Create a pattern definition if it doesn't exist
    // std::string patternId = "crosshatch";

    // // Check if pattern already exists
    // bool patternExists = false;
    // // Use get_element_by_id to check if our defs element exists
    // SVG::Element* defsElement = m_svg.get_element_by_id("defs");
    // if (defsElement) {
    //     // If defs exists, check if it has a pattern child with our ID
    //     SVG::Element* patternElement = m_svg.get_element_by_id(patternId);
    //     if (patternElement) {
    //         patternExists = true;
    //     }
    // }

    // if (!patternExists) {
    //     // Get defs element or create one
    //     SVG::Element* defs = m_svg.get_element_by_id("defs");
    //     if (!defs) {
    //         defs = m_svg.add_child<SVG::Element>("defs");
    //         defs->set_attr("id", "defs");
    //     }

    //     // Create pattern with cross-hatching
    //     auto pattern = defs->add_child<SVG::Element>("pattern");
    //     pattern->set_attr("id", patternId);
    //     pattern->set_attr("patternUnits", "userSpaceOnUse");
    //     pattern->set_attr("width", "10");
    //     pattern->set_attr("height", "10");

    //     // First set of lines (/)
    //     auto line1 = pattern->add_child<SVG::Line>(0, 10, 10, 0);
    //     line1->set_attr("stroke", colorToString(f.a > 0 ? f : c));
    //     line1->set_attr("stroke-width", "1");

    //     // Second set of lines (\)
    //     auto line2 = pattern->add_child<SVG::Line>(0, 0, 10, 10);
    //     line2->set_attr("stroke", colorToString(f.a > 0 ? f : c));
    //     line2->set_attr("stroke-width", "1");
    // }
// }

// if (!patternExists) {
//     // Get defs element or create one
//     SVG::Element* defs = m_svg.get_element_by_id("defs");
//     if (!defs) {
//         defs = m_svg.add_child<SVG::Element>("defs");
//         defs->set_attr("id", "defs");
//     }

//     // Create pattern with cross-hatching
//     auto pattern = defs->add_child<SVG::Element>("pattern");
//     pattern->set_attr("id", patternId);
//     pattern->set_attr("patternUnits", "userSpaceOnUse");
//     pattern->set_attr("width", "10");
//     pattern->set_attr("height", "10");

//     // First set of lines (/)
//     auto line1 = pattern->add_child<SVG::Line>(0, 10, 10, 0);
//     line1->set_attr("stroke", colorToString(f.a > 0 ? f : c));
//     line1->set_attr("stroke-width", "1");

//     // Second set of lines (\)
//     auto line2 = pattern->add_child<SVG::Line>(0, 0, 10, 10);
//     line2->set_attr("stroke", colorToString(f.a > 0 ? f : c));
//     line2->set_attr("stroke-width", "1");
}


void SVGCanvas::polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c, mssm::Color f) {
    std::vector<Vec2d> vec(points);
    polygonPattern(vec, c, f);
}
