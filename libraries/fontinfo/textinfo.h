#ifndef TEXTINFO_H
#define TEXTINFO_H

#include <string>

enum class HAlign {
    left   = 1<<0,	// to match FONS_ALIGN_LEFT,
    center = 1<<1,	// to match FONS_ALIGN_CENTER,
    right  = 1<<2,	// to match FONS_ALIGN_RIGHT
};

enum class VAlign {
    top      = 1<<3,// to match FONS_ALIGN_TOP,
    center   = 1<<4,// to match FONS_ALIGN_MIDDLE,
    bottom   = 1<<5,// to match FONS_ALIGN_BOTTOM,
    baseline = 1<<6,// to match FONS_ALIGN_BASELINE,
};

class TextExtents {
public:
    float fontAscent;
    float fontDescent;
    float fontHeight;
    float textHeight;
    float textWidth;
    float textAdvance;
};

enum class FontFace {
    Roboto,
    RobotoBold,
    RobotoLight,
    User1,
    User2,
    User3,
    User4,
    User5,
    User6,
    User7,
    User8,
    User9,
    User10
};

class FontInfo {
private:
    int  size;
    FontFace face{FontFace::Roboto};
public:
    FontInfo(const FontInfo& other) : size{other.size}, face{other.face} { }
    FontInfo(int size) : size{size} { }
    FontInfo(FontFace face, int size) : size{size}, face{face} { }
    int getSize() const { return size; }
    FontFace getFace() const { return face; }
    int getFaceIdx() const { return static_cast<int>(face); }
public:
    static FontInfo Bold(int size) { return FontInfo{FontFace::RobotoBold, size}; }
    static FontInfo Light(int size) { return FontInfo{FontFace::RobotoLight, size}; }
    static FontInfo Normal(int size) { return FontInfo{FontFace::Roboto, size}; }

    bool operator==(const FontInfo& other) const {
        return size == other.size && face == other.face;
    }
};

#endif // TEXTINFO_H
