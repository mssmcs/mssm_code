#ifndef TEXTEDITLINE_H
#define TEXTEDITLINE_H

#include <string>
#include <algorithm>

class TextEditLine
{
protected:
    std::string text;
    int cursorPos;
    int selectionDelta; // 0  no selection,  + extends to right,  - extends to left
public:
    TextEditLine(std::string text = "")
        : text{text}
        , cursorPos{0}
        , selectionDelta{0}
    {}
    void addChar(char c);
    void addChar(char c, bool shift);

    void moveCursor(int delta, bool extendSelection);
    void clearSelection() { selectionDelta = 0; }
    void insertAt(int offset, std::string text);
    std::string removeAt(int offset, int count);
    void deleteSelection() { removeAt(selectionStart(), selectionSize()); }
    void onDelete();
    void onBackspace();
    void onLeft(bool shift) { moveCursor(-1, shift); }
    void onRight(bool shift) { moveCursor(1, shift); }

    std::string getText() const { return text; }

    int selectionStart() const { return selectionDelta > 0 ? cursorPos : cursorPos + selectionDelta; }
    int selectionEnd() const { return selectionDelta > 0 ? cursorPos + selectionDelta : cursorPos; }
    int selectionSize() const { return std::abs(selectionDelta); }
    bool hasSelection() const { return selectionDelta != 0; }
    std::string selectedText() const { return text.substr(selectionStart(), selectionSize()); }

private:
    void constrainInsertionPoint(int &pos) { pos = std::min(std::max(0, pos), int(text.size())); }
    void constrainSpan(int &start, int &count);
};


#endif // TEXTEDITLINE_H
