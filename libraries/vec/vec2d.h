#ifndef VEC2D_H
#define VEC2D_H

#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <ostream>
#include <string>

#include "fmt/format.h"

#include "vecUtil.h"

using vec2 = float[2];

template <typename T>
class Vec2base
{
public:
    T x;
    T y;

    constexpr Vec2base() : x{0}, y{0} {}

    template<typename V>
    explicit constexpr Vec2base(const V& o) : x{static_cast<T>(o.x)}, y{static_cast<T>(o.y)} {}

    template<typename TX, typename TY>
    constexpr Vec2base(TX xvalue, TY yvalue) : x{static_cast<T>(xvalue)}, y{static_cast<T>(yvalue)} {}

    template<typename V>
    explicit constexpr Vec2base(V vec[2]) : x{static_cast<T>(vec[0])}, y{static_cast<T>(vec[1])} {}

    template<typename V>
    constexpr void assignTo(T v[2]) const { v[0] = static_cast<V>(x); v[1] = static_cast<T>(y); }

    // methods
    constexpr T magnitude()  const { return std::hypot(x,y); }
    constexpr T magSquared() const { return x*x+y*y; }

    constexpr void scale(T s) { x *= s; y *= s; }
    constexpr void rotate(double radians) { *this = { x * cos(radians) - y * sin(radians), x * sin(radians) + y * cos(radians) };}
    constexpr void translate(const Vec2base& offset) { x += offset.x; y += offset.y; }

    constexpr Vec2base rotated(double radians) const { Vec2base res = *this; res.rotate(radians); return res; }
    constexpr Vec2base scaled(T s) const { Vec2base res = *this; res.scale(s); return res; }
    constexpr Vec2base translated(Vec2base offset) const { Vec2base res = *this; res.translate(offset); return res; }

    constexpr bool equals(const Vec2base& other, T threshold) const { return std::abs(x - other.x) <= threshold && std::abs(y - other.y) <= threshold; }
    constexpr bool exactlyEquals(const Vec2base& other) const { return x == other.x && y == other.y; }

    constexpr Vec2base unit() const { auto m = magnitude(); return { x/m, y/m }; }

    constexpr Vec2base operator-() const { return {-x, -y}; }
    std::string toIntString() const;
    std::string toString() const;

    constexpr auto dot(const Vec2base& other) const { return ::dot(*this, other); }
    constexpr auto cross(const Vec2base& other) const { return ::cross(*this, other); }
};

template <typename T>
constexpr Vec2base<T> operator-(const Vec2base<T>& p1, const Vec2base<T>& p2)
{
    return Vec2base<T>{p1.x - p2.x, p1.y - p2.y };
}

template <typename T>
constexpr Vec2base<T> operator+(const Vec2base<T>& p1, const Vec2base<T>& p2)
{
    return Vec2base<T>{p1.x + p2.x, p1.y + p2.y };
}

template <typename T>
constexpr Vec2base<T> operator*(const Vec2base<T>& v, double s)
{
    return Vec2base<T>{ v.x * s, v.y * s };
}

template <typename T>
constexpr Vec2base<T> operator/(const Vec2base<T>& v, double s)
{
    return Vec2base<T>{ v.x / s, v.y / s };
}

template <typename T>
constexpr Vec2base<T> operator*(double s, const Vec2base<T>& v)
{
    return Vec2base<T>{ v.x * s, v.y * s };
}

template <typename T>
constexpr Vec2base<T>& operator+=(Vec2base<T>& v, const Vec2base<T>& other)
{
    v.x += other.x;
    v.y += other.y;
    return v;
}

template <typename T>
constexpr Vec2base<T>& operator-=(Vec2base<T>& v, const Vec2base<T>& other)
{
    v.x -= other.x;
    v.y -= other.y;
    return v;
}

template <typename T>
constexpr Vec2base<T>& operator/=(Vec2base<T>& v, double s)
{
    v.x /= s;
    v.y /= s;
    return v;
}

template <typename T>
constexpr Vec2base<T>& operator*=(Vec2base<T>& v, double s)
{
    v.x *= s;
    v.y *= s;
    return v;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec2base<T>& vec)
{
    os << "{" << vec.x << ", " << vec.y << "}";
    return os;
}

typedef Vec2base<double>  Vec2d;
typedef Vec2base<float>   Vec2f;
typedef Vec2base<char>    Vec2c;
typedef Vec2base<int32_t> Vec2i32;
typedef Vec2base<int64_t> Vec2i64;
typedef Vec2base<uint32_t> Vec2ui32;
typedef Vec2base<uint64_t> Vec2ui64;
typedef Vec2base<size_t>   Vec2size_t;

template<typename T>
constexpr double crossProduct(const Vec2base<T>& a, const Vec2base<T>& b)
{
    return a.x*b.y-b.x*a.y;
}
template<typename T>
constexpr double dotProduct(const Vec2base<T>& a, const Vec2base<T>& b)
{
    return a.x*b.x+b.y*a.y;
}
template<typename T>
std::string Vec2base<T>::toIntString() const
{
    return "(" + std::to_string(int(x)) + ", " + std::to_string(int(y)) +")";
}
template<typename T>
std::string Vec2base<T>::toString() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y) +")";
}

template <typename T>
struct fmt::formatter<Vec2base<T>> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const Vec2d& v, auto& ctx) const {
        return fmt::format_to(ctx.out(), "{{{}, {}}}", v.x, v.y);
    }
};


#endif // VEC2D_H
