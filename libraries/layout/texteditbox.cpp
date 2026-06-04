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

Vec2d TextEditBox::textDrawOrigin() const
{
    return cast<Vec2d>(rect.upperLeft()) + Vec2d{2, 2};
}

void TextEditBox::syncTextGeometry()
{
    const Vec2d origin = textDrawOrigin();
    tg.update(origin.x + textOffset, origin.y, sizeAndFace, getText(), hAlign, vAlign);
}

void TextEditBox::adjustTextOffsetForCursor()
{
    double x1;
    double y1;
    double x2;
    double y2;
    tg.getCursorLine(cursorPos, x1, y1, x2, y2);

    if (x1 < rect.left()) {
        textOffset += rect.left() - x1;
    } else if (x1 > rect.right()) {
        textOffset -= x1 - rect.right();
    }
}

void TextEditBox::draw(Canvas2d &g, bool hasFocus)
{
    const Vec2d origin = textDrawOrigin();

    g.pushClip(rect.left(), rect.top(), rect.width, rect.height, false);

    drawRect(g, rect, rgb(35, 35, 35), hasFocus ? BLACK : rgb(10,10,10));

    syncTextGeometry();
    adjustTextOffsetForCursor();
    syncTextGeometry();

    double x1;
    double y1;
    double x2;
    double y2;
    tg.getCursorLine(cursorPos, x1, y1, x2, y2);

    const Vec2d drawPos{origin.x + textOffset, origin.y};

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

        g.text({tg.getCharX(0), drawPos.y}, sizeAndFace, first, WHITE, hAlign, vAlign);
        g.text({tg.getCharX(start), drawPos.y}, sizeAndFace, middle, BLACK, hAlign, vAlign);
        g.text({tg.getCharX(end), drawPos.y}, sizeAndFace, second, WHITE, hAlign, vAlign);
    } else {
        g.text(drawPos, sizeAndFace, text, WHITE, hAlign, vAlign);
    }

    if (hasFocus) {
        const int alpha = static_cast<int>(180 + 75 * (sin(getCurrentTimeSeconds() * 5) * 0.5 + 0.5));
        const Color cursorColor{255, 255, 255, alpha};
        g.line({x1, y1}, {x2, y2}, cursorColor);
        g.line({x1 + 1, y1}, {x2 + 1, y2}, cursorColor);
    }

    g.popClip();
}

bool TextEditBox::onClick(Vec2d pos)
{
    if (rect.within(pos)) {
        syncTextGeometry();
        selectionAnchor = tg.getCursorIndex(pos.x);
        cursorPos = selectionAnchor;
        clearSelection();
        return true;
    }
    return false;
}

void TextEditBox::onDrag(Vec2d pos)
{
    syncTextGeometry();

    // Use raw x so dragging past the widget edge can reach the start/end of the string.
    const int endPos = tg.getCursorIndex(pos.x);

    // Match keyboard selection model: cursorPos is the active end, anchor stays fixed.
    cursorPos = endPos;
    selectionDelta = selectionAnchor - endPos;

    adjustTextOffsetForCursor();
    syncTextGeometry();
}
