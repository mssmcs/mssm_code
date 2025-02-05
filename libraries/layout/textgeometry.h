#ifndef TEXTGEOMETRY_H
#define TEXTGEOMETRY_H

#include "textinfo.h"
#include <functional>
#include <string>
#include <vector>

class TextMetrics {
public:
    virtual void   textExtents(const FontInfo &sizeAndFace, const std::string& str, TextExtents& extents) = 0;
    virtual double textWidth(const FontInfo &sizeAndFace, const std::string& str) = 0;
    virtual std::vector<double> getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) = 0;
};

class TextGeometry
{
    TextMetrics& metrics;
    FontInfo sizeAndFace;
    std::string text;
    std::vector<double> characterXOffsets;
    TextExtents extents;
    HAlign hAlign{HAlign::left};
    VAlign vAlign{VAlign::baseline};
    double xPos{0};
    double yPos{0};
public:
    TextGeometry(TextMetrics& metrics, const FontInfo &sizeAndFace = 20);
    void update(double x, double y, const FontInfo& sizeAndFace, const std::string& text, HAlign hAlign, VAlign vAlign);
    void iterateCharRects(std::function<void(int idx, double x, double y, double w, double h)> visitor);

    double getCharX(int idx) const { return characterXOffsets[idx]; }

    int getCursorIndex(double x); // returns index of character after the closest cursor pos to the x coordinate (use to place the cursor)
    int getCharIndex(double x); // returns index of the character closest to the x coordinate (use to pick a character)
    bool isPointInText(double x, double y);
    void getYBounds(double& top, double& bottom);
    void getXBounds(double& left, double& right);
    void getRect(double& x, double& y, double& w, double& h);
    void getRectForRange(int startChar, int endChar, double& x, double& y, double& w, double& h);
    void getCursorLine(int cursorIndex, double& x1, double& y1, double& x2, double& y2);

    const TextExtents& textExtents() const { return extents; }
};

#endif // TEXTGEOMETRY_H
