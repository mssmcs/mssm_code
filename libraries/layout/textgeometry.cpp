#include "textgeometry.h"
#include <algorithm>

TextGeometry::TextGeometry(TextMetrics& metrics, const FontInfo& sizeAndFace) : metrics{metrics}, sizeAndFace{sizeAndFace} {}

void TextGeometry::update(double x,
                          double y,
                          const FontInfo &sizeAndFace,
                          const std::string &text,
                          HAlign hAlign,
                          VAlign vAlign)
{
    bool sizeChanged = this->sizeAndFace != sizeAndFace;
    bool textChanged = this->text != text;
    bool alignChanged = this->hAlign != hAlign || this->vAlign != vAlign;
    bool posChanged = x != xPos || y != yPos;
    xPos = x;
    yPos = y;
    this->hAlign = hAlign;
    this->vAlign = vAlign;
    this->sizeAndFace = sizeAndFace;
    this->text = text;
    if (sizeChanged || textChanged) {
        metrics.textExtents(sizeAndFace.getSize(), text, extents);
    }
    if (sizeChanged || textChanged || posChanged || alignChanged) {
        characterXOffsets = metrics.getCharacterXOffsets(sizeAndFace, x, text);

        // update textWidth (handles spaces at end of string better)
        extents.textWidth = characterXOffsets.back() - characterXOffsets.front();

        if (hAlign == HAlign::center) {
            for (int i = 0; i < characterXOffsets.size(); ++i) {
                characterXOffsets[i] -= extents.textWidth/2;
            }
        }
        if (hAlign == HAlign::right) {
            for (int i = 0; i < characterXOffsets.size(); ++i) {
                characterXOffsets[i] -= extents.textWidth;
            }
        }
    }
}

void TextGeometry::iterateCharRects(std::function<void(int, double, double, double, double)> visitor)
{
    for (int i = 0; i < text.size(); ++i) {
        double x = characterXOffsets[i];
        double y = yPos;
        double w = characterXOffsets[i+1] - x;
        double h = extents.fontHeight;
        switch (vAlign) {
            case VAlign::top:
                break;
            case VAlign::center:
                y -= extents.fontHeight/2;
                break;
            case VAlign::baseline:
                y -= extents.fontAscent;
                break;
            case VAlign::bottom:
                y -= extents.fontHeight;
                break;
        }
        visitor(i, x, y, w, h);
    }
}

int TextGeometry::getCursorIndex(double x)
{
    if (x < characterXOffsets.front()) {
        return 0;
    }
    for (int i = 1; i < characterXOffsets.size(); ++i) {
        if (x < (characterXOffsets[i-1] + characterXOffsets[i])/2.0) {
            return i-1;
        }
    }
    return text.size();
}

int TextGeometry::getCharIndex(double x)
{
    if (x < characterXOffsets.front()) {
        return -1;
    }
    for (int i = 1; i < characterXOffsets.size(); ++i) {
        if (x < characterXOffsets[i]) {
            return i-1;
        }
    }
    return -1;
}

bool TextGeometry::isPointInText(double x, double y)
{
    if (x >= characterXOffsets.front() && x <= characterXOffsets.back()) {
        double top;
        double bottom;
        getYBounds(top, bottom);
        if (y >= top && y <= bottom) {
            return true;
        }
    }
    return false;
}

void TextGeometry::getYBounds(double &top, double &bottom)
{
    top = yPos;
    switch (vAlign) {
    case VAlign::top:
        break;
    case VAlign::center:
        top -= extents.fontHeight/2;
        break;
    case VAlign::baseline:
        top -= extents.fontAscent;
        break;
    case VAlign::bottom:
        top -= extents.fontHeight;
        break;
    }
    bottom = top + extents.fontHeight;
}

void TextGeometry::getXBounds(double &left, double &right)
{
    left = characterXOffsets.front();
    right = characterXOffsets.back();
}

void TextGeometry::getRect(double &x, double &y, double &w, double &h)
{
    double left;
    double right;
    double top;
    double bottom;
    getXBounds(left, right);
    getYBounds(top, bottom);
    x = left;
    y = top;
    w = right - left;
    h = bottom - top;
}

void TextGeometry::getRectForRange(int startChar, int endChar, double &x, double &y, double &w, double &h)
{
    if (startChar > endChar) {
        std::swap(startChar, endChar);
    }

    double left;
    double right;

    if (startChar >= text.size()) {
        left = characterXOffsets.back();
        right = left;
    }
    else if (endChar < 0) {
        left = characterXOffsets.front();
        right = left;
    }
    else {
        if (endChar >= text.size()) {
            endChar = text.size()-1;
        }
        left = characterXOffsets[startChar];
        right = characterXOffsets[endChar+1];
    }

    double top;
    double bottom;
    getYBounds(top, bottom);
    x = left;
    y = top;
    w = right - left;
    h = bottom - top;
}

void TextGeometry::getCursorLine(int cursorIndex, double &x1, double &y1, double &x2, double &y2)
{
    x1 = characterXOffsets[cursorIndex];
    x2 = x1;
    double top;
    double bottom;
    getYBounds(top, bottom);
    y1 = top;
    y2 = bottom;
}
