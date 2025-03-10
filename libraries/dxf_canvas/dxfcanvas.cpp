#include "dxfcanvas.h"
#include "image.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>

// Constructor
DXFCanvas::DXFCanvas(int width, int height, const std::string& filename)
    : m_width(width), m_height(height), m_backgroundColor(mssm::BLACK), 
      m_filename(filename), m_nextLayerColor(1) {
    
    initializeDXFModel();
    
    // Create a default layer
    m_layerStack.push_back(m_model.getLayer("0"));
}

// Destructor
DXFCanvas::~DXFCanvas() {
    save();
}

// Save the DXF file
void DXFCanvas::save()  {
    dimeOutput out;
    if (out.setFilename(m_filename.c_str())) {
        m_model.write(&out);
    } else {
        std::cerr << "Error: Could not save DXF file to " << m_filename << std::endl;
    }
}

// Initialize DXF model with required sections and tables
void DXFCanvas::initializeDXFModel() {
    // Initialize the DXF model
    m_model.init();
    
    // Create and set up header section
    m_headerSection = new dimeHeaderSection(m_model.getMemHandler());
    m_model.insertSection(m_headerSection);
    
    // Set common header variables
    addHeaderEntry("$ACADVER", 1, "AC1014");       // AutoCAD 14 format for compatibility
    addHeaderEntry("$INSUNITS", 70, 1);            // Set inches as default unit
    addHeaderEntry("$DIMSCALE", 40, 1.0);          // Dimension scale factor
    addHeaderEntry("$DIMTXT", 40, 12.0);           // Dimension text height
    addHeaderEntry("$EXTMIN", 10, 0.0);            // Drawing extents min x
    addHeaderEntry("$EXTMIN", 20, 0.0);            // Drawing extents min y
    addHeaderEntry("$EXTMIN", 30, 0.0);            // Drawing extents min z
    addHeaderEntry("$EXTMAX", 10, m_width);        // Drawing extents max x
    addHeaderEntry("$EXTMAX", 20, m_height);       // Drawing extents max y
    addHeaderEntry("$EXTMAX", 30, 0.0);            // Drawing extents max z
    
    // Create tables section
    dimeTablesSection* tablesSection = new dimeTablesSection;
    m_model.insertSection(tablesSection);
    
    // Set up layer table
    m_layerTable = new dimeTable(NULL);
    m_layerTable->setTableName("LAYER");
    tablesSection->insertTable(m_layerTable);
    
    // Create default layer (Layer 0)
    dimeLayerTable* layer0 = new dimeLayerTable;
    layer0->setLayerName("0", m_model.getMemHandler());
    layer0->setColorNumber(7); // White
    
    // Set additional properties needed by AutoCAD
    dimeParam param;
    param.string_data = "CONTINUOUS";
    layer0->setRecord(6, param);
    param.int16_data = 64;
    layer0->setRecord(70, param);
    
    layer0->registerLayer(&m_model);
    m_layerTable->insertTableEntry(layer0);
    
    // Create entities section
    m_entitiesSection = new dimeEntitiesSection;
    m_model.insertSection(m_entitiesSection);
}

// Helper to add integer header entries
void DXFCanvas::addHeaderEntry(const std::string& name, int group, int value) {
    dimeParam param;
    param.int16_data = value;
    int groupcode = group;
    m_headerSection->setVariable(name.c_str(), &groupcode, &param, 1, m_model.getMemHandler());
}

// Helper to add floating-point header entries
void DXFCanvas::addHeaderEntry(const std::string& name, int group, double value) {
    dimeParam param;
    param.double_data = value;
    int groupcode = group;
    m_headerSection->setVariable(name.c_str(), &groupcode, &param, 1, m_model.getMemHandler());
}

// Helper to add string header entries
void DXFCanvas::addHeaderEntry(const std::string& name, int group, const std::string& value) {
    dimeParam param;
    param.string_data = value.c_str();
    int groupcode = group;
    m_headerSection->setVariable(name.c_str(), &groupcode, &param, 1, m_model.getMemHandler());
}

// Add entity to the model with proper layer and handle setup
void DXFCanvas::addEntity(dimeEntity* entity) {
    // Set the entity's layer
    entity->setLayer(m_layerStack.back());
    
    // Create a unique handle for the entity (needed for AutoCAD)
    const int BUFSIZE = 1024;
    char buf[BUFSIZE];
    const char* handle = m_model.getUniqueHandle(buf, BUFSIZE);
    
    dimeParam param;
    param.string_data = handle;
    entity->setRecord(5, param);
    
    // Add the entity to the entities section
    m_entitiesSection->insertEntity(entity);
}

// Convert mssm::Color to DXF color index
int DXFCanvas::colorToDxfColorIndex(const mssm::Color& color) const {
    // DXF uses color indices rather than RGB values
    // Basic color mapping to DXF color indices:
    // 1=Red, 2=Yellow, 3=Green, 4=Cyan, 5=Blue, 6=Magenta, 7=White, 8=Gray, 9=Light Gray, 0=ByBlock, 256=ByLayer
    
    if (color.a == 0) {
        return 0;  // Transparent/None color in DXF
    }
    
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

// Create a new layer with the specified name and color
const dimeLayer* DXFCanvas::createLayer(const std::string& layerName, int colorIndex) {
    // Check if the layer already exists
    const dimeLayer* layer = m_model.getLayer(layerName.c_str());
    if (layer) {
        return layer;
    }
    
    // Create a new layer table record
    dimeLayerTable* layerTable = new dimeLayerTable;
    layerTable->setLayerName(layerName.c_str(), m_model.getMemHandler());
    layerTable->setColorNumber(colorIndex);
    
    // Set additional properties needed by AutoCAD
    dimeParam param;
    param.string_data = "CONTINUOUS";
    layerTable->setRecord(6, param);
    param.int16_data = 64;
    layerTable->setRecord(70, param);
    
    layerTable->registerLayer(&m_model);
    m_layerTable->insertTableEntry(layerTable);
    
    return m_model.getLayer(layerName.c_str());
}

// Convert Vec2d to dimeVec3f
dimeVec3f DXFCanvas::toVec3f(const Vec2d& vec, double z) const {
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
    // DXF doesn't have a background color concept
    m_backgroundColor = c;  // Store it anyway for reference
}

void DXFCanvas::line(Vec2d p1, Vec2d p2, mssm::Color c) {
    // Create a DXF line entity
    dimeLine* line = new dimeLine;
    
    // Set the coordinates
    line->setCoords(0, toVec3f(p1));
    line->setCoords(1, toVec3f(p2));
    
    // Set color if not using layer color
    if (c != mssm::WHITE) {
        line->setColorNumber(colorToDxfColorIndex(c));
    }
    
    // Add line to entities section
    addEntity(line);
}

void DXFCanvas::ellipse(Vec2d center, double w, double h, mssm::Color c, mssm::Color f) {
    // If width equals height, create a circle
    if (std::abs(w - h) < 0.001) {
        dimeCircle* circle = new dimeCircle;
        circle->setCenter(toVec3f(center));
        circle->setRadius(w / 2.0);
        
        if (c != mssm::WHITE) {
            circle->setColorNumber(colorToDxfColorIndex(c));
        }
        
        addEntity(circle);
    } else {
        // Create an ellipse entity
        dimeEllipse* ellipse = new dimeEllipse;
        ellipse->setCenter(toVec3f(center));
        
        // Set major axis and ratio
        dimeVec3f majorAxis;
        double ratio;
        
        if (w > h) {
            majorAxis.x = center.x + w/2;
            majorAxis.y = center.y;
            majorAxis.z = 0;
            ratio = h / w;
        } else {
            majorAxis.x = center.x;
            majorAxis.y = center.y + h/2;
            majorAxis.z = 0;
            ratio = w / h;
        }
        
        ellipse->setMajorAxisEndpoint(majorAxis);
        ellipse->setMinorMajorRatio(ratio);
        
        if (c != mssm::WHITE) {
            ellipse->setColorNumber(colorToDxfColorIndex(c));
        }
        
        addEntity(ellipse);
    }
}

void DXFCanvas::arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c) {
    // If width equals height, we can use a proper DXF arc entity
    if (std::abs(w - h) < 0.001) {
        dimeArc* arc = new dimeArc;
        arc->setCenter(toVec3f(center));
        arc->setRadius(w / 2.0);
        
        // Convert angles to the format DXF expects (degrees, counterclockwise from positive X)
        double startAngle = a;
        double endAngle = a + alen;
        
        // Ensure angles are within 0-360 range
        while (startAngle < 0) startAngle += 360;
        while (startAngle >= 360) startAngle -= 360;
        while (endAngle < 0) endAngle += 360;
        while (endAngle >= 360) endAngle -= 360;
        
        arc->setStartAngle(startAngle);
        arc->setEndAngle(endAngle);
        
        if (c != mssm::WHITE) {
            arc->setColorNumber(colorToDxfColorIndex(c));
        }
        
        addEntity(arc);
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
    // Create it as a polyline
    
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
    
    // Create the polyline
    polyline(points, c);
}

void DXFCanvas::pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f) {
    // DXF doesn't have a built-in pie entity
    // Create it as a polyline
    
    std::vector<Vec2d> points;
    
    // Start at center
    points.push_back(center);
    
    // Convert angles to radians
    double startAngle = a * M_PI / 180.0;
    double endAngle = (a + alen) * M_PI / 180.0;
    
    // Start point on arc
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
    
    // End point on arc
    double x2 = center.x + (w / 2.0) * cos(endAngle);
    double y2 = center.y + (h / 2.0) * sin(endAngle);
    points.push_back(Vec2d(x2, y2));
    
    // Return to center
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
    
    // Create a DXF Polyline
    dimePolyline* polyline = new dimePolyline;
    
    // Set color if not using layer color
    if (c != mssm::WHITE) {
        polyline->setColorNumber(colorToDxfColorIndex(c));
    }
    
    // Create vertices
    std::vector<dimeVertex*> vertices(points.size() + 1);  // +1 to close the polygon
    
    for (size_t i = 0; i < points.size(); i++) {
        dimeVertex* vertex = vertices[i] = new dimeVertex;
        vertex->setFlags(dimeVertex::POLYLINE_3D_VERTEX);
        vertex->setCoords(toVec3f(points[i]));
        vertex->setLayer(m_layerStack.back());
    }
    
    // Close the polygon by adding the first point again
    vertices[points.size()] = new dimeVertex;
    vertices[points.size()]->setFlags(dimeVertex::POLYLINE_3D_VERTEX);
    vertices[points.size()]->setCoords(toVec3f(points[0]));
    vertices[points.size()]->setLayer(m_layerStack.back());
    
    polyline->setCoordVertices(vertices.data(), points.size() + 1);
    
    addEntity(polyline);
}

void DXFCanvas::polyline(const std::vector<Vec2d>& points, mssm::Color c) {
    if (points.empty()) return;
    
    // Create a DXF Polyline
    dimePolyline* polyline = new dimePolyline;
    
    // Set color if not using layer color
    if (c != mssm::WHITE) {
        polyline->setColorNumber(colorToDxfColorIndex(c));
    }
    
    // Create vertices
    std::vector<dimeVertex*> vertices(points.size());
    
    for (size_t i = 0; i < points.size(); i++) {
        dimeVertex* vertex = vertices[i] = new dimeVertex;
        vertex->setFlags(dimeVertex::POLYLINE_3D_VERTEX);
        vertex->setCoords(toVec3f(points[i]));
        vertex->setLayer(m_layerStack.back());
    }
    
    polyline->setCoordVertices(vertices.data(), points.size());
    
    addEntity(polyline);
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

void DXFCanvas::text(Vec2d pos, const FontInfo& sizeAndFace, const std::string& str, 
                     mssm::Color textcolor, HAlign hAlign, VAlign vAlign) {
    
    dimeText* text = new dimeText;
    
    // Set the text properties
    text->setTextString(str.c_str());
    text->setOrigin(toVec3f(pos));
    text->setHeight(sizeAndFace.getSize());
    
    // Set color if not using layer color
    if (textcolor != mssm::WHITE) {
        text->setColorNumber(colorToDxfColorIndex(textcolor));
    }
    
    // Set alignment
    int horizontalJustification = 0;  // Left
    int verticalJustification = 0;    // Baseline
    
    if (hAlign == HAlign::center) {
        horizontalJustification = 1;  // Center
    } else if (hAlign == HAlign::right) {
        horizontalJustification = 2;  // Right
    }
    
    if (vAlign == VAlign::top) {
        verticalJustification = 3;  // Top
    } else if (vAlign == VAlign::center) {
        verticalJustification = 2;  // Middle
    } else if (vAlign == VAlign::bottom) {
        verticalJustification = 1;  // Bottom
    }

    text->setHJust(horizontalJustification);
    text->setVJust(verticalJustification);
    
    addEntity(text);
}

void DXFCanvas::textExtents(const FontInfo& sizeAndFace, const std::string& str, TextExtents& extents) {
    // Approximation since DXF doesn't provide direct text measurement
    extents.textWidth = textWidth(sizeAndFace, str);
    extents.textAdvance = extents.textWidth;
    extents.fontHeight = sizeAndFace.getSize();
    extents.textHeight = extents.fontHeight;
    extents.fontAscent = sizeAndFace.getSize() * 0.8;
    extents.fontDescent = sizeAndFace.getSize() * 0.2;
}

double DXFCanvas::textWidth(const FontInfo& sizeAndFace, const std::string& str) {
    // Rough approximation - average character width is about 0.6 times the font size
    return str.length() * sizeAndFace.getSize() * 0.6;
}

std::vector<double> DXFCanvas::getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) {
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

void DXFCanvas::point(Vec2d pos, mssm::Color c) {
    // Create a DXF point entity
    dimePoint* point = new dimePoint;
    
    // Set the coordinates
    point->setCoords(toVec3f(pos));
    
    // Set color if not using layer color
    if (c != mssm::WHITE) {
        point->setColorNumber(colorToDxfColorIndex(c));
    }
    
    addEntity(point);
}

void DXFCanvas::image(Vec2d pos, const mssm::Image& img) {
    // DXF can support images but it's complex
    // For simplicity, we'll just create a rectangle with text as a placeholder
    rect(pos, img.width(), img.height(), mssm::WHITE, mssm::TRANSPARENT);
    
    FontInfo font(10);
    Vec2d textPos(pos.x + img.width()/2, pos.y + img.height()/2);
    text(textPos, font, "Image", mssm::WHITE, HAlign::center, VAlign::center);
}

void DXFCanvas::image(Vec2d pos, const mssm::Image& img, Vec2d src, int srcw, int srch) {
    image(pos, img);  // Simplified implementation
}

void DXFCanvas::image(Vec2d pos, double w, double h, const mssm::Image& img) {
    rect(pos, w, h, mssm::WHITE, mssm::TRANSPARENT);
    
    FontInfo font(10);
    Vec2d textPos(pos.x + w/2, pos.y + h/2);
    text(textPos, font, "Image", mssm::WHITE, HAlign::center, VAlign::center);
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
    FontInfo font(10);
    text(center, font, "Image", mssm::WHITE, HAlign::center, VAlign::center);
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
    FontInfo font(10);
    text(center, font, "Image", mssm::WHITE, HAlign::center, VAlign::center);
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
    // In DXF, we use layers as a way to group objects
    // Creating a new layer with the group name
    
    // Replace spaces with underscores (DXF layer names can't have spaces)
    std::string layerName = groupName;
    std::replace(layerName.begin(), layerName.end(), ' ', '_');
    
    if (layerName.empty()) {
        layerName = "Layer_" + std::to_string(m_nextLayerColor);
    }
    
    // Create the layer with the next color index
    const dimeLayer* layer = createLayer(layerName, m_nextLayerColor++);
    if (m_nextLayerColor > 7) m_nextLayerColor = 1;  // Cycle through colors 1-7
    
    m_layerStack.push_back(layer);
}

void DXFCanvas::popGroup() {
    if (m_layerStack.size() > 1) {  // Always keep at least one layer (Layer 0)
        m_layerStack.pop_back();
    }
}

void DXFCanvas::polygonPattern(const std::vector<Vec2d>& points, mssm::Color c, mssm::Color f) {
    // In DXF, hatching is done through the HATCH entity
    // A proper implementation would use the HATCH entity with a pattern
    // For simplicity, we'll create a closed polyline and add a special record to indicate hatching
    
    if (points.size() < 3) return;
    
    // Create a standard polyline for the boundary
    dimePolyline* polyline = new dimePolyline;
    
    // Set color if not using layer color
    if (c != mssm::WHITE) {
        polyline->setColorNumber(colorToDxfColorIndex(c));
    }
    
    // Create vertices
    std::vector<dimeVertex*> vertices(points.size() + 1);  // +1 to close the polygon
    
    for (size_t i = 0; i < points.size(); i++) {
        dimeVertex* vertex = vertices[i] = new dimeVertex;
        vertex->setFlags(dimeVertex::POLYLINE_3D_VERTEX);
        vertex->setCoords(toVec3f(points[i]));
        vertex->setLayer(m_layerStack.back());
    }
    
    // Close the polygon by adding the first point again
    vertices[points.size()] = new dimeVertex;
    vertices[points.size()]->setFlags(dimeVertex::POLYLINE_3D_VERTEX);
    vertices[points.size()]->setCoords(toVec3f(points[0]));
    vertices[points.size()]->setLayer(m_layerStack.back());
    
    polyline->setCoordVertices(vertices.data(), points.size() + 1);
    
    // Add a special record to indicate this is a hatched polygon
    dimeParam param;
    param.int16_data = 1;  // Simple flag to indicate hatching
    polyline->setRecord(70, param);
    
    addEntity(polyline);
    
    // Note: A real implementation would create a HATCH entity, but this
    // is more complex and requires defining boundary paths and hatch patterns
}

void DXFCanvas::polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c, mssm::Color f) {
    std::vector<Vec2d> vec(points);
    polygonPattern(vec, c, f);
}
