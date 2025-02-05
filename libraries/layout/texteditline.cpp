#include "texteditline.h"

#include <algorithm>

using namespace std;

void TextEditLine::moveCursor(int delta, bool extendSelection)
{
    // constrain delta to be within bounds of text
    if (delta < 0) {
        if (-delta > cursorPos) {
            delta = -cursorPos;
        }
    } else {
        delta = min(delta, int(text.size()) - cursorPos);
    }

    if (abs(delta) == 0) {
        return;
    }

    if (!extendSelection) {
        // not extending selection
        if (hasSelection()) {
            // collapse selection left or right
            if (delta > 0) {
                cursorPos = selectionEnd();
                clearSelection();
            } else {
                cursorPos = selectionStart();
                clearSelection();
            }
        }
        else {
            cursorPos += delta;
        }
    } else {
        // extending selection
        if (selectionDelta > 0) {
            // cursor at left end of selection
            cursorPos += delta;
            selectionDelta -= delta;
        } else if (selectionDelta < 0) {
            // cursor at right end of selection
            cursorPos += delta;
            selectionDelta -= delta;
        } else {
            // no selection... start one
            cursorPos += delta;
            selectionDelta = -delta;
        }
    }
}

void TextEditLine::insertAt(int offset, string t)
{
    constrainInsertionPoint(offset);
    text.insert(offset, t);
    cursorPos = offset + t.size();
}

string TextEditLine::removeAt(int offset, int count)
{
    constrainSpan(offset, count);
    auto result = text.substr(offset, count);
    text.erase(offset, count);
    selectionDelta = 0;
    cursorPos = offset;
    return result;
}

void TextEditLine::onDelete()
{
    if (hasSelection()) {
        deleteSelection();
    } else if (cursorPos < text.size()) {
        removeAt(cursorPos, 1);
    }
}

void TextEditLine::onBackspace()
{
    if (hasSelection()) {
        deleteSelection();
    } else if (cursorPos > 0) {
        removeAt(cursorPos - 1, 1);
    }
}

void TextEditLine::constrainSpan(int &start, int &count)
{
    if (start > text.size()) {
        start = text.size();
        count = 0;
    } else if (start < 0) {
        count += start;
        start = 0;
    }
    if (count < 0) {
        count = 0;
    } else if (start + count > text.size()) {
        count = text.size() - start;
    }
}

void TextEditLine::addChar(char c)
{
    if (hasSelection()) {
        removeAt(selectionStart(), selectionSize());
    }
    text.insert(cursorPos, 1, c);
    cursorPos++;
}

void TextEditLine::addChar(char c, bool shift)
{
    if (std::isupper(c) && !shift) {
        c = std::tolower(c);
    } else if (std::islower(c) && shift) {
        c = std::toupper(c);
    }
    addChar(c);
}
