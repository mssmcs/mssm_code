#ifndef COLOR_H
#define COLOR_H

#include <array>
#include <algorithm>
#include <concepts>
#include <iostream>

namespace mssm {

#undef TRANSPARENT


class Color
{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
public:
    constexpr Color(const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
   // constexpr Color(int c) : r((c >> 16)&0xFF), g((c >> 8)&0xFF), b(c&0xFF), a(0xFF) {}
    constexpr Color()  : r(0), g(0), b(0), a(255) {}
    constexpr Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)  : r(_r), g(_g), b(_b), a(_a) {}
    constexpr Color(int _r, int _g, int _b, int _a = 255)  : r(_r), g(_g), b(_b), a(_a) {}
    constexpr Color& operator=(const Color& c) { r = c.r; g = c.g; b = c.b; a = c.a; return *this; }

    constexpr unsigned int toUIntRGBA() const { return a | (b << 8) | (g << 16) | (r << 24); }
    constexpr unsigned int toUIntABGR() const { return r | (g << 8) | (b << 16) | (a << 24); }
    constexpr unsigned int toUIntARGB() const { return b | (g << 8) | (r << 16) | (a << 24); }
    constexpr unsigned int toUIntRGB() const { return b | (g << 8) | (r << 16); }

    template <typename T>
    constexpr void setRealArrayRGBA(T arr) const { arr[0] = r/255.0; arr[1] = g/255.0; arr[2] = b/255.0; arr[3] = a/255.0; }
    template <typename T>
    constexpr void setRealArrayRGB(T arr) const { arr[0] = r/255.0; arr[1] = g/255.0; arr[2] = b/255.0; }

    template <typename VEC4>
    constexpr VEC4 toRealVec4() const { return VEC4(r/255.0f, g/255.0f, b/255.0f, a/255.0f); }
    template <typename VEC3>
    constexpr VEC3 toRealVec3() const { return VEC3(r/255.0f, g/255.0f, b/255.0f); }

    constexpr bool operator== (const Color& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
    static constexpr Color fromHSV(double h, double s, double v);
    static constexpr Color fromUIntRGBA(unsigned int c) { return Color(static_cast<uint8_t>(c >> 24), static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c)); }
    static constexpr Color fromUIntABGR(unsigned int c) { return Color(static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 24)); }
    static constexpr Color fromUIntARGB(unsigned int c) { return Color(static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 24)); }
    static constexpr Color fromUIntRGB(unsigned int c) { return Color(static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c)); }
    static constexpr Color fromUIntBGR(unsigned int c) { return Color(static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c >> 16)); }
    static constexpr Color fromIntRGBA(int c) { return Color(static_cast<uint8_t>(c >> 24), static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c)); }
    static constexpr Color fromIntABGR(int c) { return Color(static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 24)); }
    static constexpr Color fromIntARGB(int c) { return Color(static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 24)); }
    static constexpr Color fromIntRGB(int c) { return Color(static_cast<uint8_t>(c >> 16), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c)); }
    static constexpr Color fromIntBGR(int c) { return Color(static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c >> 16)); }
    static constexpr Color fromFloats(float r, float g, float b, float a = 1.0f) { return Color(static_cast<uint8_t>(r*255), static_cast<uint8_t>(g*255), static_cast<uint8_t>(b*255), static_cast<uint8_t>(a*255)); }
    static constexpr Color fromDoubles(double r, double g, double b, double a = 1.0) { return Color(static_cast<uint8_t>(r*255), static_cast<uint8_t>(g*255), static_cast<uint8_t>(b*255), static_cast<uint8_t>(a*255)); }

    static constexpr Color TRANSPARENT() { return {0,0,0,0}; }
    static constexpr Color WHITE(uint8_t alpha = 0xFF) { return {255u,255u,255u,alpha}; }
    static constexpr Color GREY(uint8_t alpha = 0xFF) { return {128u,128u,128u,alpha}; }
    static constexpr Color BLACK(uint8_t alpha = 0xFF) { return {0u,0u,0u,alpha}; }
    static constexpr Color LTGREY(uint8_t alpha = 0xFF) { return {211u,211u,211u,alpha}; }
    static constexpr Color PINK(uint8_t alpha = 0xFF) { return {0xFFu,0xC0u,0xCBu,alpha}; }
    static constexpr Color RED(uint8_t alpha = 0xFF) { return {255u, 0u, 0u,alpha}; }
    static constexpr Color ORANGE(uint8_t alpha = 0xFF) { return {255u, 128u, 0u,alpha}; }
    static constexpr Color YELLOW(uint8_t alpha = 0xFF) { return {255u, 255u, 0u,alpha}; }
    static constexpr Color CHARTREUSE(uint8_t alpha = 0xFF) { return {128u, 255u, 0u,alpha}; }
    static constexpr Color GREEN(uint8_t alpha = 0xFF) { return {0u, 255u, 0u,alpha}; }
    static constexpr Color SPRING_GREEN(uint8_t alpha = 0xFF) { return {0u, 255u, 128u,alpha}; }
    static constexpr Color CYAN(uint8_t alpha = 0xFF) { return {0u, 255u, 255u,alpha}; }
    static constexpr Color DODGER_BLUE(uint8_t alpha = 0xFF) { return {0u, 128u, 255u,alpha}; }
    static constexpr Color BLUE(uint8_t alpha = 0xFF) { return {0u, 0u, 255u,alpha}; }
    static constexpr Color PURPLE(uint8_t alpha = 0xFF) { return {128u, 0u, 255u,alpha}; }
    static constexpr Color MAGENTA(uint8_t alpha = 0xFF) { return {255u, 0u, 255u,alpha}; }
    static constexpr Color NEON_ROSE(uint8_t alpha = 0xFF) { return {255u, 0u, 128u,alpha}; }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        // Format the output as RGBA (or you can use another format like RGB if desired)
        os << "RGBA(" << static_cast<int>(color.r) << ", "
           << static_cast<int>(color.g) << ", "
           << static_cast<int>(color.b) << ", "
           << static_cast<int>(color.a) << ")";
        return os;
    }
};

constexpr Color TRANSPARENT = Color::TRANSPARENT();
constexpr Color TRANS = Color::TRANSPARENT();

constexpr Color WHITE = Color::WHITE();
constexpr Color GREY = Color::GREY();
constexpr Color BLACK = Color::BLACK();
constexpr Color LTGREY = Color::LTGREY();
constexpr Color PINK = Color::PINK();

constexpr Color RED = Color::RED();
constexpr Color ORANGE = Color::ORANGE();
constexpr Color YELLOW = Color::YELLOW();
constexpr Color CHARTREUSE = Color::CHARTREUSE();
constexpr Color GREEN = Color::GREEN();
constexpr Color SPRING_GREEN = Color::SPRING_GREEN();
constexpr Color CYAN = Color::CYAN();
constexpr Color DODGER_BLUE = Color::DODGER_BLUE();
constexpr Color BLUE = Color::BLUE();
constexpr Color PURPLE = Color::PURPLE();
constexpr Color MAGENTA = Color::MAGENTA();
constexpr Color NEON_ROSE = Color::NEON_ROSE();

constexpr std::array<Color, 12> colors12{
    RED, ORANGE, YELLOW, CHARTREUSE, GREEN, SPRING_GREEN, CYAN, DODGER_BLUE, BLUE, PURPLE, MAGENTA, NEON_ROSE
};



// takes rgb values 0-1
template <std::floating_point T>
constexpr Color rgbFloat(T r, T g, T b)
{
    Color out;
    r *= 255;
    g *= 255;
    b *= 255;
    out.r = r < 0 ? 0 : (r > 255 ? 255 : r);
    out.g = g < 0 ? 0 : (g > 255 ? 255 : g);
    out.b = b < 0 ? 0 : (b > 255 ? 255 : b);
    return out;
}





// h   0-360
// s   0-1
// v   0-1
template <std::floating_point T>
constexpr Color hsv2rgb(T h, T s, T v)
{
    if (s <= 0.0) // < 0 shouldn't happen...
    {
        return rgbFloat(v,v,v);
    }

    T hh = (h >= 360 ? 0.0 : h) / 60.0;
    int    i{static_cast<int>(hh)};
    T ff = hh - i;

    T p = v * (1.0 - s);
    T q = v * (1.0 - (s * ff));
    T t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        return rgbFloat(v,t,p);
    case 1:
        return rgbFloat(q,v,p);
    case 2:
        return rgbFloat(p,v,t);
    case 3:
        return rgbFloat(p,q,v);
    case 4:
        return rgbFloat(t,p,v);
    case 5:
    default:
        return rgbFloat(v,p,q);
    }
}

// h:0-360.0, s:0.0-1.0, v:0.0-1.0
template <std::floating_point T>
constexpr void rgb2hsv(Color c, T &h, T &s, T &v)
{
    T r = c.r / 255.0;
    T g = c.g / 255.0;
    T b = c.b / 255.0;

    T mx = std::max(r, std::max(g, b));
    T mn = std::min(r, std::min(g, b));

    v = mx;

    if (mx == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (mx - mn == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (mx - mn) / mx;

        if (mx == r) {
            h = 60 * ((g - b) / (mx - mn)) + 0;
        }
        else if (mx == g) {
            h = 60 * ((b - r) / (mx - mn)) + 120;
        }
        else {
            h = 60 * ((r - g) / (mx - mn)) + 240;
        }
    }

    if (h < 0) h += 360.0f;
}

class ColorHSV {
    float h;
    float s;
    float v;
public:
    constexpr ColorHSV(float h, float s, float v) : h(h), s(s), v(v) {}
    constexpr ColorHSV(Color c) { rgb2hsv(c, h, s, v); }
    constexpr operator Color() const { return hsv2rgb(h, s, v); }
    constexpr void darken(double factor) { v *= factor; s += (1.0 - s) * factor; }
    constexpr void lighten(double factor) { v += (1.0 - v) * factor; s *= factor; }
};

constexpr Color rgb(int r, int g, int b) { return Color(r, g, b); }

constexpr Color dark(Color c, double factor = 0.5)
{
    ColorHSV hsv(c);
    hsv.darken(factor);
    return hsv;
}

constexpr Color light(Color c, double factor = 0.5)
{
    ColorHSV hsv(c);
    hsv.lighten(factor);
    return hsv;
}


Color constexpr Color::fromHSV(double h, double s, double v)
{
    return hsv2rgb(h, s, v);
}

} // namespace mssm

#endif // COLOR_H
