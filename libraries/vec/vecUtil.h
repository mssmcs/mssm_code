#ifndef VECUTIL_H
#define VECUTIL_H

#include <cmath>
#include <type_traits>

template<typename T>
concept Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

template<typename T>
concept hasXField = requires (T v) { { v.x } -> Arithmetic; };
template<typename T>
concept hasYField = requires (T v) { { v.y } -> Arithmetic; };
template<typename T>
concept hasZField = requires (T v) { { v.z } -> Arithmetic; };
template<typename T>
concept hasWField = requires (T v) { { v.w } -> Arithmetic; };

template<typename T>
concept is2dVector = hasXField<T> && hasYField<T> && !hasZField<T> && !hasWField<T>;

template<typename T>
concept is3dVector = hasXField<T> && hasYField<T> && hasZField<T> && !hasWField<T>;

template<typename T>
concept is4dVector = hasXField<T> && hasYField<T> && hasZField<T> && hasWField<T>;

template<typename T>
concept is2dOr3dVector = is2dVector<T> || is3dVector<T>;

template<typename T>
concept is2dOr3dOr4dVector = is2dVector<T> || is3dVector<T> || is4dVector<T>;

constexpr auto cross(const is2dVector auto& a, const is2dVector auto& b) { return a.x*b.y-b.x*a.y; }
constexpr auto dot(const is2dVector auto& a, const is2dVector auto& b) { return a.x*b.x+b.y*a.y; }

constexpr auto cross(const is3dVector auto&& a, const is3dVector auto&& b) {
    using Scalar = decltype(a.x); // Deduce the type of components
    return decltype(a){
        .x = static_cast<Scalar>(a.y * b.z - a.z * b.y),
        .y = static_cast<Scalar>(a.z * b.x - a.x * b.z),
        .z = static_cast<Scalar>(a.x * b.y - a.y * b.x)
    };
}

constexpr auto dot(const is3dVector auto& a, const is3dVector auto& b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
constexpr auto dot(const is4dVector auto& a, const is4dVector auto& b) { return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w; }

constexpr auto magnitude(const is2dVector auto& a) { return std::hypot(a.x, a.y); }
constexpr auto magnitude(const is3dVector auto& a) { return std::hypot(a.x, a.y, a.z); }
constexpr auto magnitude(const is4dVector auto& a) { return std::hypot(a.x, a.y, a.z, a.w); }

template <typename VOUT>
constexpr auto cast(const is2dVector auto& v) -> VOUT requires is2dVector<VOUT>
{
    return {v.x, v.y};
}

template <typename VOUT>
constexpr auto cast(const is2dVector auto& v) -> VOUT requires is3dVector<VOUT>
{
    return {v.x, v.y, 0};
}

template <typename VOUT>
constexpr auto cast(const is3dVector auto& v) -> VOUT requires is2dVector<VOUT>
{
    return {v.x, v.y};
}

template <typename VOUT>
constexpr auto cast(const is3dVector auto& v) -> VOUT requires is3dVector<VOUT>
{
    return {v.x, v.y, v.z};
}


#endif // VECUTIL_H
