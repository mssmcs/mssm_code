#ifndef DXFCANVAS_H
#define DXFCANVAS_H

#include "canvas2d.h"
#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/Model.h>
#include <dime/entities/Line.h>
#include <dime/entities/Circle.h>
#include <dime/entities/Arc.h>
#include <dime/entities/Polyline.h>
#include <dime/entities/Point.h>
#include <dime/entities/LWPolyline.h>
#include <dime/entities/Text.h>
#include <dime/sections/EntitiesSection.h>
#include <dime/sections/TablesSection.h>
#include <dime/tables/Table.h>
#include <dime/tables/LayerTable.h>
#include <string>
#include <map>
#include <memory>
#include <stack>

class DXFCanvas : public mssm::Canvas2d {
private:
    int m_width;
    int m_height;
    mssm::Color m_backgroundColor;
    dimeModel m_model;
    dimeEntitiesSection* m_entitiesSection;
    std::string m_filename;
    std::stack<std::string> m_layerStack;
    std::map<std::string, int> m_colorMap;  // Map layer names to DXF color indices
    
    // Helper methods
    dimeEntity* currentTarget();
    int colorToDxfColorIndex(const mssm::Color& color);
    void createLayer(const std::string& layerName, int colorIndex);
    dimeVec3f toVec3f(const Vec2d& vec, double z = 0.0);
    
public:
    DXFCanvas(int width, int height, const std::string& filename = "output.dxf");
    ~DXFCanvas();
    
    // Save the DXF file
    void save() const;
    
    // Canvas2d interface implementation
    bool isDrawable() override;
    int width() override;
    int height() override;
    void setBackground(mssm::Color c) override;
    void line(Vec2d p1, Vec2d p2, mssm::Color c = mssm::WHITE) override;
    void ellipse(Vec2d center, double w, double h, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
    void arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c = mssm::WHITE) override;
    void chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
    void pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
    void rect(Vec2d corner, double w, double h, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
    void polygon(const std::vector<Vec2d>& points, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
    void polyline(const std::vector<Vec2d>& points, mssm::Color c = mssm::WHITE) override;
    void points(const std::vector<Vec2d>& points, mssm::Color c = mssm::WHITE) override;
    
#ifdef SUPPORT_MSSM_ARRAY
    void polygon(const mssm::Array<Vec2d>& points, mssm::Color border, mssm::Color fill = mssm::TRANSPARENT) override;
    void polyline(const mssm::Array<Vec2d>& points, mssm::Color color) override;
    void points(const mssm::Array<Vec2d>& points, mssm::Color c) override;
#endif // SUPPORT_MSSM_ARRAY
    
    void polygon(std::initializer_list<Vec2d> points, mssm::Color border, mssm::Color fill = mssm::TRANSPARENT) override;
    void polyline(std::initializer_list<Vec2d> points, mssm::Color color) override;
    void points(std::initializer_list<Vec2d> points, mssm::Color c) override;
    
    void text(Vec2d pos, const FontInfo& sizeAndFace, const std::string& str,
              mssm::Color textcolor = mssm::WHITE, HAlign hAlign = HAlign::left,
              VAlign vAlign = VAlign::baseline) override;
    void textExtents(const FontInfo& sizeAndFace, const std::string& str, TextExtents& extents) override;
    double textWidth(const FontInfo& sizeAndFace, const std::string& str) override;
    std::vector<double> getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) override;
    
    void point(Vec2d pos, mssm::Color c) override;
    void image(Vec2d pos, const mssm::Image& img) override;
    void image(Vec2d pos, const mssm::Image& img, Vec2d src, int srcw, int srch) override;
    void image(Vec2d pos, double w, double h, const mssm::Image& img) override;
    void image(Vec2d pos, double w, double h, const mssm::Image& img, Vec2d src, int srcw, int srch) override;
    void imageC(Vec2d center, double angle, const mssm::Image& img) override;
    void imageC(Vec2d center, double angle, const mssm::Image& img, Vec2d src, int srcw, int srch) override;
    void imageC(Vec2d center, double angle, double w, double h, const mssm::Image& img) override;
    void imageC(Vec2d center, double angle, double w, double h, const mssm::Image& img, Vec2d src, int srcw, int srch) override;
    
    bool isClipped(Vec2d pos) const override;
    void pushClip(int x, int y, int w, int h, bool replace) override;
    void popClip() override;
    void setClip(int x, int y, int w, int h) override;
    void resetClip() override;
    void setViewport(int x, int y, int w, int h) override;
    void resetViewport() override;
    
    void pushGroup(std::string groupName) override;
    void popGroup() override;
    void polygonPattern(const std::vector<Vec2d>& points, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
    void polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
};

#endif // DXFCANVAS_H
