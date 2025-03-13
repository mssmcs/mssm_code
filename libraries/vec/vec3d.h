#ifndef VEC3D_H
#define VEC3D_H

#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <ostream>

#include <string>

#include "print_compat.h"

#include "vecUtil.h"


template <typename T>
class Vec3base
{
public:
    T x;
    T y;
    T z;

    constexpr Vec3base() : x{0}, y{0}, z{0} {}

    static constexpr Vec3base zero() { return {0, 0, 0}; }
    static constexpr Vec3base one() { return {1, 1, 1}; }
    static constexpr Vec3base unitX() { return {1, 0, 0}; }
    static constexpr Vec3base unitY() { return {0, 1, 0}; }
    static constexpr Vec3base unitZ() { return {0, 0, 1}; }
    static constexpr Vec3base max() { return {std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max()}; }

    template<typename V>
    explicit constexpr Vec3base(const V& o) : x{static_cast<T>(o.x)}, y{static_cast<T>(o.y)}, z{static_cast<T>(o.z)} {}

    template<typename TX, typename TY, typename TZ>
    constexpr Vec3base(TX xvalue, TY yvalue, TZ zvalue) : x{static_cast<T>(xvalue)}, y{static_cast<T>(yvalue)}, z{static_cast<T>(zvalue)} {}

    template<is2dVector XY, typename TZ>
    constexpr Vec3base(XY xy, TZ zvalue) : x{static_cast<T>(xy.x)}, y{static_cast<T>(xy.y)}, z{static_cast<T>(zvalue)} {}

    template<typename V>
    explicit constexpr Vec3base(V vec[3]) : x{static_cast<T>(vec[0])}, y{static_cast<T>(vec[1])}, z{static_cast<T>(vec[2])} {}

    template<typename V>
    constexpr void assignTo(T v[3]) const { v[0] = static_cast<V>(x); v[1] = static_cast<T>(y); v[2] = static_cast<T>(z); }

    // methods
    constexpr T magnitude()  const { return std::hypot(x,y,z); }
    constexpr T magSquared() const { return x*x+y*y+z*z; }

    constexpr void scale(T s) { x *= s; y *= s; z *= s; }
    constexpr void rotate(double radians) { *this = { x * cos(radians) - y * sin(radians), x * sin(radians) + y * cos(radians) };}
    constexpr void translate(const Vec3base& offset) { x += offset.x; y += offset.y; z += offset.z; }

    constexpr Vec3base rotated(double radians) const { Vec3base res = *this; res.rotate(radians); return res; }
    constexpr Vec3base scaled(T s) const { Vec3base res = *this; res.scale(s); return res; }
    constexpr Vec3base translated(Vec3base offset) const { Vec3base res = *this; res.translate(offset); return res; }

    constexpr bool equals(const Vec3base& other, T threshold) const {
        return std::abs(x - other.x) <= threshold &&
               std::abs(y - other.y) <= threshold &&
               std::abs(z - other.z) <= threshold; }
    constexpr bool exactlyEquals(const Vec3base& other) const {
        return x == other.x && y == other.y && z == other.z; }

    constexpr Vec3base unit() const { auto m = magnitude(); return { x/m, y/m, z/m }; }
    constexpr Vec3base unit(double& vmag) const { vmag = magnitude(); return { x/vmag, y/vmag, z/vmag }; }

    constexpr Vec3base operator-() const { return {-x, -y, -z}; }
    std::string toIntString() const;
    std::string toString() const;

    constexpr auto dot(const Vec3base& other) const { return ::dot(*this, other); }
    constexpr auto cross(const Vec3base& other) const { return ::cross(*this, other); }

    template <typename V>
    constexpr V xy() const { return V{x, y}; }
    template <typename V>
    constexpr V xz() const { return V{x, z}; }
    template <typename V>
    constexpr V yz() const { return V{y, z}; }

};

template <typename T>
constexpr Vec3base<T> operator-(const Vec3base<T>& p1, const Vec3base<T>& p2)
{
    return Vec3base<T>{p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
}

template <typename T>
constexpr Vec3base<T> operator+(const Vec3base<T>& p1, const Vec3base<T>& p2)
{
    return Vec3base<T>{p1.x + p2.x, p1.y + p2.y, p1.z + p2.z };
}

template <typename T>
constexpr Vec3base<T> operator*(const Vec3base<T>& v, double s)
{
    return Vec3base<T>{ v.x * s, v.y * s, v.z * s };
}

template <typename T>
constexpr Vec3base<T> operator/(const Vec3base<T>& v, double s)
{
    return Vec3base<T>{ v.x / s, v.y / s, v.z / s };
}

template <typename T>
constexpr Vec3base<T> operator*(double s, const Vec3base<T>& v)
{
    return Vec3base<T>{ v.x * s, v.y * s, v.z * s };
}

template <typename T>
constexpr Vec3base<T>& operator+=(Vec3base<T>& v, const Vec3base<T>& other)
{
    v.x += other.x;
    v.y += other.y;
    v.z += other.z;
    return v;
}

template <typename T>
constexpr Vec3base<T>& operator-=(Vec3base<T>& v, const Vec3base<T>& other)
{
    v.x -= other.x;
    v.y -= other.y;
    v.z -= other.z;
    return v;
}

template <typename T>
constexpr Vec3base<T>& operator/=(Vec3base<T>& v, double s)
{
    v.x /= s;
    v.y /= s;
    v.z /= s;
    return v;
}

template <typename T>
constexpr Vec3base<T>& operator*=(Vec3base<T>& v, double s)
{
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec3base<T>& vec)
{
    os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    return os;
}

typedef Vec3base<double>  Vec3d;
typedef Vec3base<float>   Vec3f;
typedef Vec3base<char>    Vec3c;
typedef Vec3base<int32_t> Vec3i32;
typedef Vec3base<int64_t> Vec3i64;

template<typename T>
constexpr Vec3base<T> crossProduct(const Vec3base<T>& a, const Vec3base<T>& b)
{
    return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-b.x*a.y};
}
template<typename T>
constexpr double dotProduct(const Vec3base<T>& a, const Vec3base<T>& b)
{
    return a.x*b.x+b.y*a.y+b.z*a.z;
}
template<typename T>
std::string Vec3base<T>::toIntString() const
{
    return "(" + std::to_string(int(x)) + ", " + std::to_string(int(y))+ ", " + std::to_string(int(z)) +")";
}
template<typename T>
std::string Vec3base<T>::toString() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y)+ ", " + std::to_string(z) +")";
}


// Generate string representation of Vec2base for formatting
template <typename T>
std::string vec3_to_string(const Vec3base<T>& v) {
    return "{" + std::to_string(v.x) + ", " + std::to_string(v.y)+ ", " + std::to_string(v.z) + "}";
}

// Use the appropriate formatter based on whether C++23 print is available
#if MSSM_HAS_CPP23_PRINT
// C++23 formatter
template <typename T>
struct std::formatter<Vec3base<T>> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const Vec3base<T>& v, FormatContext& ctx) const {
        return std::formatter<std::string>::format(vec3_to_string(v), ctx);
    }
};
#else
// fmt formatter
template <typename T>
struct fmt::formatter<Vec3base<T>> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Vec3base<T>& v, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", vec3_to_string(v));
    }
};
#endif


#endif // VEC3D_H
