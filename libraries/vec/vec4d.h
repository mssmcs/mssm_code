#ifndef VEC4D_H
#define VEC4D_H

#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <ostream>

#include <string>

#include "fmt/format.h"
#include "fmt/color.h"

#include "vecUtil.h"

template <typename T>
class Vec4base
{
public:
    T x;
    T y;
    T z;
    T w;

    constexpr Vec4base() : x{}, y{}, z{}, w{} {}

    template<typename V>
    explicit constexpr Vec4base(const V& o) : x{static_cast<T>(o.x)}, y{static_cast<T>(o.y)}, z{static_cast<T>(o.z)}, w{static_cast<T>(o.w)} {}

    template<typename TX, typename TY, typename TZ>
    constexpr Vec4base(TX xvalue, TY yvalue, TZ zvalue, TZ wvalue) : x{static_cast<T>(xvalue)}, y{static_cast<T>(yvalue)}, z{static_cast<T>(zvalue)}, w{static_cast<T>(wvalue)} {}

    template<typename V>
    explicit constexpr Vec4base(V vec[3]) : x{static_cast<T>(vec[0])}, y{static_cast<T>(vec[1])}, z{static_cast<T>(vec[2])}, w{static_cast<T>(vec[3])} {}

    template<typename V>
    constexpr void assignTo(T v[4]) const { v[0] = static_cast<V>(x); v[1] = static_cast<T>(y); v[2] = static_cast<T>(z); v[2] = static_cast<T>(w); }

    // methods
    constexpr T magnitude()  const { return std::hypot(x,y,z,w); }
    constexpr T magSquared() const { return x*x + y*y + z*z + w*w; }

    constexpr void scale(T s) { x *= s; y *= s; z *= s; w *= s; }
    // constexpr void rotate(double radians) { *this = { x * cos(radians) - y * sin(radians), x * sin(radians) + y * cos(radians) };}
    constexpr void translate(const Vec4base& offset) { x += offset.x; y += offset.y; z += offset.z; w += offset.w; }

    // constexpr Vec4base rotated(double radians) const { Vec4base res = *this; res.rotate(radians); return res; }
    constexpr Vec4base scaled(T s) const { Vec4base res = *this; res.scale(s); return res; }
    constexpr Vec4base translated(Vec4base offset) const { Vec4base res = *this; res.translate(offset); return res; }

    constexpr bool equals(const Vec4base& other, T threshold) const {
        return std::abs(x - other.x) <= threshold &&
               std::abs(y - other.y) <= threshold &&
               std::abs(z - other.z) <= threshold &&
               std::abs(w - other.w) <= threshold; }
    constexpr bool exactlyEquals(const Vec4base& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w; }

    constexpr Vec4base unit() const { auto m = magnitude(); return { x/m, y/m, z/m, w/m }; }

    constexpr Vec4base operator-() const { return {-x, -y, -z, -w}; }

    std::string toIntString() const;
    std::string toString() const;

    constexpr auto dot(const Vec4base& other) const { return ::dot(*this, other); }
    constexpr auto cross(const Vec4base& other) const { return ::cross(*this, other); }
};

template <typename T>
constexpr Vec4base<T> operator-(const Vec4base<T>& p1, const Vec4base<T>& p2)
{
    return Vec4base<T>{p1.x - p2.x, p1.y - p2.y, p1.z - p2.z, p1.w - p2.w };
}

template <typename T>
constexpr Vec4base<T> operator+(const Vec4base<T>& p1, const Vec4base<T>& p2)
{
    return Vec4base<T>{p1.x + p2.x, p1.y + p2.y, p1.z + p2.z, p1.w + p2.w };
}

template <typename T>
constexpr Vec4base<T> operator*(const Vec4base<T>& v, double s)
{
    return Vec4base<T>{ v.x * s, v.y * s, v.z * s, v.w * s };
}

template <typename T>
constexpr Vec4base<T> operator/(const Vec4base<T>& v, double s)
{
    return Vec4base<T>{ v.x / s, v.y / s, v.z / s, v.w / s };
}

template <typename T>
constexpr Vec4base<T> operator*(double s, const Vec4base<T>& v)
{
    return Vec4base<T>{ v.x * s, v.y * s, v.z * s, v.w * s };
}

template <typename T>
constexpr Vec4base<T>& operator+=(Vec4base<T>& v, const Vec4base<T>& other)
{
    v.x += other.x;
    v.y += other.y;
    v.z += other.z;
    v.w += other.w;
    return v;
}

template <typename T>
constexpr Vec4base<T>& operator-=(Vec4base<T>& v, const Vec4base<T>& other)
{
    v.x -= other.x;
    v.y -= other.y;
    v.z -= other.z;
    v.w -= other.w;
    return v;
}

template <typename T>
constexpr Vec4base<T>& operator/=(Vec4base<T>& v, double s)
{
    v.x /= s;
    v.y /= s;
    v.z /= s;
    v.w /= s;
    return v;
}

template <typename T>
constexpr Vec4base<T>& operator*=(Vec4base<T>& v, double s)
{
    v.x *= s;
    v.y *= s;
    v.z *= s;
    v.w *= s;
    return v;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec4base<T>& vec)
{
    os << "{" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "}";
    return os;
}

typedef Vec4base<double>  Vec4d;
typedef Vec4base<float>   Vec4f;
typedef Vec4base<char>    Vec4c;
typedef Vec4base<int32_t> Vec4i32;
typedef Vec4base<int64_t> Vec4i64;

// template<typename T>
// constexpr Vec4base<T> crossProduct(const Vec4base<T>& a, const Vec4base<T>& b)
// {
//     return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-b.x*a.y};
// }

template<typename T>
constexpr double dotProduct(const Vec4base<T>& a, const Vec4base<T>& b)
{
    return a.x*b.x+b.y*a.y+b.z*a.z+b.w*a.w;
}

template<typename T>
std::string Vec4base<T>::toIntString() const
{
    return "(" + std::to_string(int(x)) + ", " + std::to_string(int(y))+ ", " + std::to_string(int(z)) + ", " + std::to_string(int(w)) +")";
}

template<typename T>
std::string Vec4base<T>::toString() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y)+ ", " + std::to_string(z) + "," + std::to_string(w) +")";
}

template <typename T>
struct fmt::formatter<Vec4base<T>> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const Vec4d& v, auto& ctx) const {
        return fmt::format_to(ctx.out(), "{{{}, {}, {}, {}}}", v.x, v.y, v.z, v.w);
    }
};


#endif // VEC4D_H
