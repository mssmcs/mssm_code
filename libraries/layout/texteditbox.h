#ifndef TEXTEDITBOX_H
#define TEXTEDITBOX_H

#include "texteditline.h"
#include "textgeometry.h"
#include "vec2d.h"
#include "recti.h"
#include "canvas2d.h"

class TextEditBox : public TextEditLine
{
    TextGeometry tg;
    HAlign hAlign = HAlign::left;
    VAlign vAlign = VAlign::top;
    RectI rect;
    int textOffset{0}; // allow shifting text to keep cursor inside rect
    FontInfo sizeAndFace;
public:
    TextEditBox(TextMetrics& metrics, const FontInfo &sizeAndFace, std::string text = "");
    void draw(mssm::Canvas2d &g, bool hasFocus);
    bool onClick(Vec2d pos);
    void onDrag(Vec2d pos);
    void setRect(const RectI& location) { rect = location; }
    int boxHeight() const { return tg.textExtents().fontHeight + 6; }
};


#endif // TEXTEDITBOX_H
