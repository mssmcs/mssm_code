#include "texteditbox.h"
#include "rectdraw.h"

using namespace std;
using namespace mssm;

#include <chrono>

// TODO: move to library
inline double getCurrentTimeSeconds() {
    using Clock = std::chrono::high_resolution_clock;
    auto now = Clock::now();
    auto duration = std::chrono::duration<double>(now.time_since_epoch());
    return duration.count();
}


TextEditBox::TextEditBox(TextMetrics &metrics, const FontInfo &sizeAndFace, std::string text)
    : TextEditLine{text}
    , tg{metrics}
    , sizeAndFace{sizeAndFace}
{
    // initial update so that boxHeight
    tg.update(0, 0, sizeAndFace, text, hAlign, vAlign);
}

void TextEditBox::draw(Canvas2d &g, bool hasFocus)
{
    Vec2d pos = cast<Vec2d>(rect.upperLeft()) + Vec2d{2,2};

    g.pushClip(rect.left(), rect.top(), rect.width, rect.height, false);

    drawRect(g, rect, rgb(35, 35, 35), hasFocus ? BLACK : rgb(10,10,10));

    tg.update(pos.x+textOffset, pos.y, sizeAndFace, getText(), hAlign, vAlign);

    double x1;
    double y1;
    double x2;
    double y2;
    tg.getCursorLine(cursorPos, x1, y1, x2, y2);

    if (x1 < rect.left()) {
        textOffset += rect.left()-x1;
        tg.update(pos.x+textOffset, pos.y, sizeAndFace, getText(), hAlign, vAlign);
        tg.getCursorLine(cursorPos, x1, y1, x2, y2);
    }
    else if (x1 > rect.right()) {
        textOffset -= x1 - rect.right();
        tg.update(pos.x+textOffset, pos.y, sizeAndFace, getText(), hAlign, vAlign);
        tg.getCursorLine(cursorPos, x1, y1, x2, y2);
    }

    if (hasFocus && hasSelection()) {
        auto start = selectionStart();
        auto end = selectionEnd();
        auto first = text.substr(0, start);
        auto second = text.substr(end);
        auto middle = text.substr(start, end - start);

        double rx;
        double ry;
        double rw;
        double rh;
        tg.getRectForRange(start, end-1, rx, ry, rw, rh);
        g.rect({rx, ry}, rw, rh, TRANS, GREY);

        g.text({tg.getCharX(0), pos.y}, sizeAndFace, first, WHITE, hAlign, vAlign);
        g.text({tg.getCharX(start), pos.y}, sizeAndFace, middle, BLACK, hAlign, vAlign);
        g.text({tg.getCharX(end), pos.y}, sizeAndFace, second, WHITE, hAlign, vAlign);
    } else {
        g.text(pos+Vec2d{textOffset, 0}, sizeAndFace, text, WHITE, hAlign, vAlign);
    }

    if (hasFocus) {
        int alpha = static_cast<int>(254*(sin(getCurrentTimeSeconds()*5)*0.5+0.5));
        if (alpha < 128) {
            alpha = 0;
        }
        else {
            alpha = 255;
        }
        g.line({x1, y1}, {x2, y2}, {128,128,255,alpha});
        g.line({x1+1, y1}, {x2+1, y2}, {128,128,255,alpha});
        //g.println("{}",sin(g.time())*0.5+0.5);
    }

    g.popClip();
}

bool TextEditBox::onClick(Vec2d pos)
{
    //if (tg.isPointInText(pos.x, pos.y)) {
    if (rect.within(pos)) {
        cursorPos = tg.getCursorIndex(pos.x);
        clearSelection();
        return true;
    }
    return false;
}

void TextEditBox::onDrag(Vec2d pos)
{
    int endPos = tg.getCursorIndex(pos.x);
    if (endPos != cursorPos) {
        if (endPos > cursorPos) {
            selectionDelta = endPos - cursorPos;
        } else {
            selectionDelta = endPos - cursorPos;
        }
    }
}
