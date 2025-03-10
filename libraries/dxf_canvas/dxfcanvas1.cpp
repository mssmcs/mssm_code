#include "DXFCanvas.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// Constructor
DXFCanvas::DXFCanvas(int width, int height, const std::string& filename)
    : m_width(width), m_height(height), m_backgroundColor(mssm::BLACK), m_filename(filename) {
    
    // Initialize the DXF model
    m_model.init();
    
    // Get the entities section or create it if it doesn't exist
    m_entitiesSection = static_cast<dimeEntitiesSection*>(m_model.findSection("ENTITIES"));
    if (!m_entitiesSection) {
        m_entitiesSection = new dimeEntitiesSection;
        m_model.insertSection(m_entitiesSection);
    }
    
    // Create a default layer
    m_layerStack.push("0");  // Layer "0" is the default in DXF
    m_colorMap["0"] = 7;     // White color index in DXF
    
    // Create a layer table if it doesn't exist
    dimeTablesSection* tablesSection = static_cast<dimeTablesSection*>(m_model.findSection("TABLES"));
    if (!tablesSection) {
        tablesSection = new dimeTablesSection;
        m_model.insertSection(tablesSection);
    }
    
    // Create Layer 0 if it doesn't exist
    dimeTable* layerTable = tablesSection->getTable(0);
    if (!layerTable) {
        layerTable = new dimeTable(m_model.getMemHandler());
        layerTable->setTableType("LAYER");
        tablesSection->insertTable(layerTable);
        
        // Create the default layer
        dimeLayerTable* layer0 = new dimeLayerTable;
        layer0->setLayerName("0", nullptr);
        layer0->setColorNumber(7); // White
        layerTable->insertTableRecord(layer0);
    }
}

// Destructor
DXFCanvas::~DXFCanvas() {
    save();
}

// Save the DXF file
void DXFCanvas::save() const {
    dimeOutput out;
    if (out.setFilename(m_filename.c_str())) {
        m_model.write(&out);
    } else {
        std::cerr << "Error: Could not save DXF file to " << m_filename << std::endl;
    }
}

// Helper methods
dimeEntity* DXFCanvas::currentTarget() {
    // In DXF, we don't have a true "current target" like in SVG
    // Instead, we create entities and add them to the entities section
    return nullptr;
}

int DXFCanvas::colorToDxfColorIndex(const mssm::Color& color) {
    // DXF uses color indices rather than RGB values
    // We'll use a simple mapping based on closest basic colors
    
    if (color.a == 0) {
        return 0;  // Transparent/None color in DXF (actually, DXF uses layer color)
    }
    
    // Basic color mapping to DXF color indices
    // DXF Standard Colors:
    // 1 = Red, 2 = Yellow, 3 = Green, 4 = Cyan, 5 = Blue, 6 = Magenta, 7 = White
    
    // Find the primary color component
    int max = std::max(std::max(color.r, color.g), color.b);
    
    if (max < 30) return 0;  // Black or very dark
    
    if (color.r == max && color.g == max && color.b == max) return 7;  // White/Gray
    if (color.r == max && color.g > 128 && color.b < 50) return 2;     // Yellow
    if (color.r == max && color.g < 50 && color.b < 50) return 1;      // Red
    if (color.g == max && color.r < 50 && color.b < 50) return 3;      // Green
    if (color.g == max && color.b == max && color.r < 50) return 4;    // Cyan
    if (color.b == max && color.r < 50 && color.g < 50) return 5;      // Blue
    if (color.r == max && color.b == max && color.g < 50) return 6;    // Magenta
    
    // Default to white
    return 7;
}

void DXFCanvas::createLayer(const std::string& layerName, int colorIndex) {
    // Check if the layer already exists
    if (m_colorMap.find(layerName) != m_colorMap.end()) {
        return;
    }
    
    // Add to our layer map
    m_colorMap[layerName] = colorIndex;
    
    // Get the tables section
    dimeTablesSection* tablesSection = static_cast<dimeTablesSection*>(m_model.findSection("TABLES"));
    if (!tablesSection) {
        tablesSection = new dimeTablesSection;
        m_model.insertSection(tablesSection);
    }
    
    // Get or create the layer table
    dimeTable* layerTable = tablesSection->getTable("LAYER");
    if (!layerTable) {
        layerTable = new dimeTable;
        layerTable->setTableType("LAYER");
        tablesSection->insertTable(layerTable);
    }
    
    // Create and add the layer
    dimeLayerTable* layer = new dimeLayerTable;
    layer->setLayerName(layerName.c_str());
    layer->setColorNumber(colorIndex);
    layerTable->insertTableRecord(layer);
}

dimeVec3f DXFCanvas::toVec3f(const Vec2d& vec, double z) {
    return dimeVec3f(vec.x, vec.y, z);
}

// Canvas2d interface implementation
bool DXFCanvas::isDrawable() {
    return true;
}

int DXFCanvas::width() {
    return m_width;
}

int DXFCanvas::height() {
    return m_height;
}

void DXFCanvas::setBackground(mssm::Color c) {
    // DXF doesn't have a background color concept like SVG
    m_backgroundColor = c;  // Store it anyway for reference
}

void DXFCanvas::line(Vec2d p1, Vec2d p2, mssm::Color c) {
    // Create a DXF line entity
    dimeLine* line = new dimeLine;
    
    // Set line properties
    line->setLayer(m_layerStack.top().c_str());
    line->setColorNumber(colorToDxfColorIndex(c));
    
    // Set the coordinates
    line->setCoords(0, toVec3f(p1));
    line->setCoords(1, toVec3f(p2));
    
    // Add line to entities section
    m_entitiesSection->insertEntity(line);
}

void DXFCanvas::ellipse(Vec2d center, double w, double h, mssm::Color c, mssm::Color f) {
    // DXF doesn't have a true ellipse entity in older versions
    // For simplicity, if w == h, we'll create a circle
    // Otherwise, we'll approximate with a polyline
    
    if (w == h) {
        // Perfect circle case
        dimeCircle* circle = new dimeCircle;
        circle->setLayer(m_layerStack.top().c_str());
        circle->setColorNumber(colorToDxfColorIndex(c));
        
        circle->setCenter(toVec3f(center));
        circle->setRadius(w / 2.0);
        
        m_entitiesSection->insertEntity(circle);
    } else {
        // Ellipse case - approximate with polyline
        const int numSegments = 36;  // Number of segments to use
        std::vector<Vec2d> points;
        
        for (int i = 0; i < numSegments; i++) {
            double angle = 2.0 * M_PI * i / numSegments;
            double x = center.x + (w / 2.0) * cos(angle);
            double y = center.y + (h / 2.0) * sin(angle);
            points.push_back(Vec2d(x, y));
        }
        
        // Close the ellipse
        points.push_back(points[0]);
        
        // Create the polyline
        polyline(points, c);
    }
}

void DXFCanvas::arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c) {
    // If width equals height, we can use a proper DXF arc entity
    if (w == h) {
        dimeArc* arc = new dimeArc;
        arc->setLayer(m_layerStack.top().c_str());
        arc->setColorNumber(colorToDxfColorIndex(c));
        
        arc->setCenter(toVec3f(center));
        arc->setRadius(w / 2.0);
        
        // Convert angles to the format DXF expects
        // DXF angles are in degrees, counterclockwise from positive X axis
        double startAngle = a;
        double endAngle = a + alen;
        
        // Ensure angles are within 0-360 range
        while (startAngle < 0) startAngle += 360;
        while (startAngle >= 360) startAngle -= 360;
        while (endAngle < 0) endAngle += 360;
        while (endAngle >= 360) endAngle -= 360;
        
        arc->setStartAngle(startAngle);
        arc->setEndAngle(endAngle);
        
        m_entitiesSection->insertEntity(arc);
    } else {
        // Elliptical arc - approximate with polyline
        const int numSegments = 36;
        std::vector<Vec2d> points;
        
        double startRad = a * M_PI / 180.0;
        double endRad = (a + alen) * M_PI / 180.0;
        
        for (int i = 0; i <= numSegments; i++) {
            double t = i / static_cast<double>(numSegments);
            double angle = startRad + t * (endRad - startRad);
            double x = center.x + (w / 2.0) * cos(angle);
            double y = center.y + (h / 2.0) * sin(angle);
            points.push_back(Vec2d(x, y));
        }
        
        polyline(points, c);
    }
}

void DXFCanvas::chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f) {
    // DXF doesn't have a built-in chord entity
    // We'll create it as a closed polyline
    
    std::vector<Vec2d> points;
    
    // Convert angles to radians
    double startAngle = a * M_PI / 180.0;
    double endAngle = (a + alen) * M_PI / 180.0;
    
    // Start point
    double x1 = center.x + (w / 2.0) * cos(startAngle);
    double y1 = center.y + (h / 2.0) * sin(startAngle);
    points.push_back(Vec2d(x1, y1));
    
    // Arc points
    const int numSegments = 36;
    for (int i = 1; i < numSegments; i++) {
        double t = i / static_cast<double>(numSegments);
        double angle = startAngle + t * (endAngle - startAngle);
        double x = center.x + (w / 2.0) * cos(angle);
        double y = center.y + (h / 2.0) * sin(angle);
        points.push_back(Vec2d(x, y));
    }
    
    // End point
    double x2 = center.x + (w / 2.0) * cos(endAngle);
    double y2 = center.y + (h / 2.0) * sin(endAngle);
    points.push_back(Vec2d(x2, y2));
    
    // Close the chord
    points.push_back(Vec2d(x1, y1));
    
    // Create a polyline
    polyline(points, c);
}

void DXFCanvas::pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f) {
    // DXF doesn't have a built-in pie entity
    // We'll create it as a closed polyline
    
    std::vector<Vec2d> points;
    
    // Add center point
    points.push_back(center);
    
    // Convert angles to radians
    double startAngle = a * M_PI / 180.0;
    double endAngle = (a + alen) * M_PI / 180.0;
    
    // Start point
    double x1 = center.x + (w / 2.0) * cos(startAngle);
    double y1 = center.y + (h / 2.0) * sin(startAngle);
    points.push_back(Vec2d(x1, y1));
    
    // Arc points
    const int numSegments = 36;
    for (int i = 1; i < numSegments; i++) {
        double t = i / static_cast<double>(numSegments);
        double angle = startAngle + t * (endAngle - startAngle);
        double x = center.x + (w / 2.0) * cos(angle);
        double y = center.y + (h / 2.0) * sin(angle);
        points.push_back(Vec2d(x, y));
    }
    
    // End point
    double x2 = center.x + (w / 2.0) * cos(endAngle);
    double y2 = center.y + (h / 2.0) * sin(endAngle);
    points.push_back(Vec2d(x2, y2));
    
    // Close the pie by returning to center
    points.push_back(center);
    
    // Create polygon
    polygon(points, c, f);
}

void DXFCanvas::rect(Vec2d corner, double w, double h, mssm::Color c, mssm::Color f) {
    // Create a rectangle as a polyline
    std::vector<Vec2d> points = {
        corner,
        Vec2d(corner.x + w, corner.y),
        Vec2d(corner.x + w, corner.y + h),
        Vec2d(corner.x, corner.y + h),
        corner  // Close the rectangle
    };
    
    polygon(points, c, f);
}

void DXFCanvas::polygon(const std::vector<Vec2d>& points, mssm::Color c, mssm::Color f) {
    if (points.size() < 3) return;
    
    // Create a DXF LWPolyline (lightweight polyline)
    dimeLWPolyline* polyline = new dimeLWPolyline;
    polyline->setLayer(m_layerStack.top().c_str());
    polyline->setColorNumber(colorToDxfColorIndex(c));
    
    // Set polyline properties
    polyline->setFlags(1);  // Closed polyline
    
    // Add vertices
    for (const auto& pt : points) {
        polyline->appendVertex(dimeVec2f(pt.x, pt.y));
    }
    
    m_entitiesSection->insertEntity(polyline);
    
    // Note: DXF doesn't have a direct fill concept like SVG
    // In a real implementation, you'd need to use hatch entities for fills
    // This is simplified for the exercise
}

void DXFCanvas::polyline(const std::vector<Vec2d>& points, mssm::Color c) {
    if (points.empty()) return;
    
    // Create a DXF LWPolyline (lightweight polyline)
    dimeLWPolyline* polyline = new dimeLWPolyline;
    polyline->setLayer(m_layerStack.top().c_str());
    polyline->setColorNumber(colorToDxfColorIndex(c));
    
    // Set polyline properties
    polyline->setFlags(0);  // Open polyline
    
    // Add vertices
    for (const auto& pt : points) {
        polyline->appendVertex(dimeVec2f(pt.x, pt.y));
    }
    
    m_entitiesSection->insertEntity(polyline);
}

void DXFCanvas::points(const std::vector<Vec2d>& points, mssm::Color c) {
    for (const auto& pt : points) {
        point(pt, c);
    }
}

#ifdef SUPPORT_MSSM_ARRAY
void DXFCanvas::polygon(const mssm::Array<Vec2d>& points, mssm::Color border, mssm::Color fill) {
    std::vector<Vec2d> vec(points.begin(), points.end());
    polygon(vec, border, fill);
}

void DXFCanvas::polyline(const mssm::Array<Vec2d>& points, mssm::Color color) {
    std::vector<Vec2d> vec(points.begin(), points.end());
    polyline(vec, color);
}

void DXFCanvas::points(const mssm::Array<Vec2d>& points, mssm::Color c) {
    std::vector<Vec2d> vec(points.begin(), points.end());
    this->points(vec, c);
}
#endif // SUPPORT_MSSM_ARRAY

void DXFCanvas::polygon(std::initializer_list<Vec2d> points, mssm::Color border, mssm::Color fill) {
    std::vector<Vec2d> vec(points);
    polygon(vec, border, fill);
}

void DXFCanvas::polyline(std::initializer_list<Vec2d> points, mssm::Color color) {
    std::vector<Vec2d> vec(points);
    polyline(vec, color);
}

void DXFCanvas::points(std::initializer_list<Vec2d> points, mssm::Color c) {
    std::vector<Vec2d> vec(points);
    this->points(vec, c);
}

void DXFCanvas::text(Vec2d pos, const mssm::FontInfo& sizeAndFace, const std::string& str, 
                     mssm::Color textcolor, mssm::HAlign hAlign, mssm::VAlign vAlign) {
    
    dimeText* text = new dimeText;
    text->setLayer(m_layerStack.top().c_str());
    text->setColorNumber(colorToDxfColorIndex(textcolor));
    
    // Set the text properties
    text->setTextString(str.c_str());
    text->setPosition(toVec3f(pos));
    text->setHeight(sizeAndFace.size);
    
    // Set alignment
    int horizontalJustification = 0;  // Left
    int verticalJustification = 0;    // Baseline
    
    if (hAlign == mssm::HAlign::center) {
        horizontalJustification = 1;  // Center
    } else if (hAlign == mssm::HAlign::right) {
        horizontalJustification = 2;  // Right
    }
    
    if (vAlign == mssm::VAlign::top) {
        verticalJustification = 3;  // Top
    } else if (vAlign == mssm::VAlign::middle) {
        verticalJustification = 2;  // Middle
    } else if (vAlign == mssm::VAlign::bottom) {
        verticalJustification = 1;  // Bottom
    }
    
    text->setHorizontalJustification(horizontalJustification);
    text->setVerticalJustification(verticalJustification);
    
    m_entitiesSection->insertEntity(text);
}

void DXFCanvas::textExtents(const mssm::FontInfo& sizeAndFace, const std::string& str, mssm::TextExtents& extents) {
    // Approximation since DXF doesn't provide direct text measurement
    extents.width = textWidth(sizeAndFace, str);
    extents.height = sizeAndFace.size;
    extents.ascent = sizeAndFace.size * 0.8;
    extents.descent = sizeAndFace.size * 0.2;
}

double DXFCanvas::textWidth(const mssm::FontInfo& sizeAndFace, const std::string& str) {
    // Rough approximation - average character width is about 0.6 times the font size
    return str.length() * sizeAndFace.size * 0.6;
}

std::vector<double> DXFCanvas::getCharacterXOffsets(const mssm::FontInfo& sizeAndFace, double startX, const std::string& text) {
    std::vector<double> offsets;
    offsets.reserve(text.length() + 1);
    
    double avgCharWidth = sizeAndFace.size * 0.6;
    double currentX = startX;
    
    offsets.push_back(currentX);
    for (size_t i = 0; i < text.length(); i++) {
        currentX += avgCharWidth;
        offsets.push_back(currentX);
    }
    
    return offsets;
}

void DXFCanvas::point(Vec2d pos, mssm::Color c) {
    // Create a DXF point entity
    dimePoint* point = new dimePoint;
    point->setLayer(m_layerStack.top().c_str());
    point->setColorNumber(colorToDxfColorIndex(c));
    
    point->setCoords(toVec3f(pos));
    
    m_entitiesSection->insertEntity(point);
}

void DXFCanvas::image(Vec2d pos, const mssm::Image& img) {
    // DXF can support images but it's complex
    // For simplicity, we'll just create a rectangle with text as a placeholder
    rect(pos, img.width(), img.height(), mssm::WHITE, mssm::TRANSPARENT);
    
    mssm::FontInfo font("Arial", 10);
    Vec2d textPos(pos.x + img.width()/2, pos.y + img.height()/2);
    text(textPos, font, "Image", mssm::WHITE, mssm::HAlign::center, mssm::VAlign::middle);
}

void DXFCanvas::image(Vec2d pos, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    image(pos, img);  // Simplified implementation
}

void DXFCanvas::image(Vec2d pos, double w, double h, const mssm::Image& img) {
    rect(pos, w, h, mssm::WHITE, mssm::TRANSPARENT);
    
    mssm::FontInfo font("Arial", 10);
    Vec2d textPos(pos.x + w/2, pos.y + h/2);
    text(textPos, font, "Image", mssm::WHITE, mssm::HAlign::center, mssm::VAlign::middle);
}

void DXFCanvas::image(Vec2d pos, double w, double h, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    image(pos, w, h, img);  // Simplified implementation
}

void DXFCanvas::imageC(Vec2d center, double angle, const mssm::Image& img) {
    // DXF supports rotated elements, but for simplicity we'll create a placeholder
    double w = img.width();
    double h = img.height();
    Vec2d pos(center.x - w/2, center.y - h/2);
    
    // Create a basic outline as a rotated rectangle
    std::vector<Vec2d> points = {
        Vec2d(-w/2, -h/2),
        Vec2d(w/2, -h/2),
        Vec2d(w/2, h/2),
        Vec2d(-w/2, h/2),
        Vec2d(-w/2, -h/2)  // Close the rectangle
    };
    
    // Rotate the points
    double radian = angle * M_PI / 180.0;
    for (auto& pt : points) {
        double x = pt.x;
        double y = pt.y;
        pt.x = x * cos(radian) - y * sin(radian) + center.x;
        pt.y = x * sin(radian) + y * cos(radian) + center.y;
    }
    
    polyline(points, mssm::WHITE);
    
    // Add "Image" text (not rotated for simplicity)
    mssm::FontInfo font("Arial", 10);
    text(center, font, "Image", mssm::WHITE, mssm::HAlign::center, mssm::VAlign::middle);
}

void DXFCanvas::imageC(Vec2d center, double angle, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    imageC(center, angle, img);  // Simplified implementation
}

void DXFCanvas::imageC(Vec2d center, double angle, double w, double h, const mssm::Image& img) {
    // Similar to the above, but with custom dimensions
    std::vector<Vec2d> points = {
        Vec2d(-w/2, -h/2),
        Vec2d(w/2, -h/2),
        Vec2d(w/2, h/2),
        Vec2d(-w/2, h/2),
        Vec2d(-w/2, -h/2)  // Close the rectangle
    };
    
    // Rotate the points
    double radian = angle * M_PI / 180.0;
    for (auto& pt : points) {
        double x = pt.x;
        double y = pt.y;
        pt.x = x * cos(radian) - y * sin(radian) + center.x;
        pt.y = x * sin(radian) + y * cos(radian) + center.y;
    }
    
    polyline(points, mssm::WHITE);
    
    // Add "Image" text (not rotated for simplicity)
    mssm::FontInfo font("Arial", 10);
    text(center, font, "Image", mssm::WHITE, mssm::HAlign::center, mssm::VAlign::middle);
}

void DXFCanvas::imageC(Vec2d center, double angle, double w, double h, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    imageC(center, angle, w, h, img);  // Simplified implementation
}

bool DXFCanvas::isClipped(Vec2d pos) const {
    // Simple bounds check - DXF doesn't have true clipping regions
    return pos.x < 0 || pos.y < 0 || pos.x >= m_width || pos.y >= m_height;
}

void DXFCanvas::pushClip(int x, int y, int w, int h, bool replace) {
    // DXF doesn't support clipping the way SVG does
    // For simplicity, we'll do nothing here in this implementation
}

void DXFCanvas::popClip() {
    // DXF doesn't support clipping the way SVG does
    // For simplicity, we'll do nothing here in this implementation
}

void DXFCanvas::setClip(int x, int y, int w, int h) {
    // DXF doesn't support clipping the way SVG does
    // For simplicity, we'll do nothing here in this implementation
}

void DXFCanvas::resetClip() {
    // DXF doesn't support clipping the way SVG does
    // For simplicity, we'll do nothing here in this implementation
}

void DXFCanvas::setViewport(int x, int y, int w, int h) {
    // DXF has viewport concepts but they're more complex
    // For simplicity, we'll do nothing here in this implementation
}

void DXFCanvas::resetViewport() {
    // DXF has viewport concepts but they're more complex
    // For simplicity, we'll do nothing here in this implementation
}

void DXFCanvas::pushGroup(std::string groupName) {
    // In DXF, we can use layers as a way to group objects
    // Creating a new layer with the group name
    std::string layerName = groupName.empty() ? 
                          ("Layer_" + std::to_string(m_layerStack.size())) : 
                          groupName;
    
    int colorIndex = 7;  // Default to white
    createLayer(layerName, colorIndex);
    
    m_layerStack.push(layerName);
}

void DXFCanvas::popGroup() {
    if (m_layerStack.size() > 1) {  // Always keep at least one layer (Layer 0)
        m_layerStack.pop();
    }
}

void DXFCanvas::polygonPattern(const std::vector<Vec2d>& points, mssm::Color c, mssm::Color f) {
    // In DXF, hatching is done through the HATCH entity
    // For simplicity, we'll just create a closed polyline
    // A proper implementation would use the HATCH entity with a cross-hatch pattern
    polygon(points, c, f);
}

void DXFCanvas::polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c, mssm::Color f) {
    std::vector<Vec2d> vec(points);
    polygonPattern(vec, c, f);
}
