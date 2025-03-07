#ifndef VECUTIL_H
#define VECUTIL_H

#include <cmath>
#include <ranges>
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

constexpr auto cross(const is3dVector auto& a, const is3dVector auto& b) {
    using VectorType = std::remove_cvref_t<decltype(a)>; // Deduce return vector type
    using Scalar = decltype(a.x); // Deduce scalar type

    return VectorType{
        static_cast<Scalar>(a.y * b.z - a.z * b.y),
        static_cast<Scalar>(a.z * b.x - a.x * b.z),
        static_cast<Scalar>(a.x * b.y - a.y * b.x)
    };
}

bool isDiagonal(const is2dVector auto& inner, const is2dVector auto& outer) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    return inner.x != outer.x && inner.y != outer.y;
#pragma GCC diagnostic pop
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

// Translation functions
// For collections of 2D vectors
template<typename Collection, typename OffsetVector>
    requires std::ranges::range<Collection> &&
             is2dVector<std::ranges::range_value_t<Collection>> &&
             is2dVector<OffsetVector>
constexpr void translate(Collection& collection, const OffsetVector& offset) {
    for (auto& vec : collection) {
        vec.x += offset.x;
        vec.y += offset.y;
    }
}

// For collections of 3D vectors
template<typename Collection, typename OffsetVector>
    requires std::ranges::range<Collection> &&
             is3dVector<std::ranges::range_value_t<Collection>> &&
             is3dVector<OffsetVector>
constexpr void translate(Collection& collection, const OffsetVector& offset) {
    for (auto& vec : collection) {
        vec.x += offset.x;
        vec.y += offset.y;
        vec.z += offset.z;
    }
}

// For collections of 4D vectors
template<typename Collection, typename OffsetVector>
    requires std::ranges::range<Collection> &&
             is4dVector<std::ranges::range_value_t<Collection>> &&
             is4dVector<OffsetVector>
constexpr void translate(Collection& collection, const OffsetVector& offset) {
    for (auto& vec : collection) {
        vec.x += offset.x;
        vec.y += offset.y;
        vec.z += offset.z;
        vec.w += offset.w;
    }
}

// Scaling functions
// UNIFORM SCALING (by scalar)

// Scale a collection of 2D vectors by a scalar
template<typename Collection, typename Scalar>
    requires std::ranges::range<Collection> &&
             is2dVector<std::ranges::range_value_t<Collection>> &&
             Arithmetic<Scalar>
constexpr void scale(Collection& collection, const Scalar factor) {
    for (auto& vec : collection) {
        vec.x *= factor;
        vec.y *= factor;
    }
}

// Scale a collection of 3D vectors by a scalar
template<typename Collection, typename Scalar>
    requires std::ranges::range<Collection> &&
             is3dVector<std::ranges::range_value_t<Collection>> &&
             Arithmetic<Scalar>
constexpr void scale(Collection& collection, const Scalar factor) {
    for (auto& vec : collection) {
        vec.x *= factor;
        vec.y *= factor;
        vec.z *= factor;
    }
}

// Scale a collection of 4D vectors by a scalar
template<typename Collection, typename Scalar>
    requires std::ranges::range<Collection> &&
             is4dVector<std::ranges::range_value_t<Collection>> &&
             Arithmetic<Scalar>
constexpr void scale(Collection& collection, const Scalar factor) {
    for (auto& vec : collection) {
        vec.x *= factor;
        vec.y *= factor;
        vec.z *= factor;
        vec.w *= factor;
    }
}

// NON-UNIFORM SCALING (by vector of scale factors)

// Scale a collection of 2D vectors by a scale vector
template<typename Collection, typename ScaleVector>
    requires std::ranges::range<Collection> &&
             is2dVector<std::ranges::range_value_t<Collection>> &&
             is2dVector<ScaleVector>
constexpr void scale(Collection& collection, const ScaleVector& factors) {
    for (auto& vec : collection) {
        vec.x *= factors.x;
        vec.y *= factors.y;
    }
}

// Scale a collection of 3D vectors by a scale vector
template<typename Collection, typename ScaleVector>
    requires std::ranges::range<Collection> &&
             is3dVector<std::ranges::range_value_t<Collection>> &&
             is3dVector<ScaleVector>
constexpr void scale(Collection& collection, const ScaleVector& factors) {
    for (auto& vec : collection) {
        vec.x *= factors.x;
        vec.y *= factors.y;
        vec.z *= factors.z;
    }
}

// Scale a collection of 4D vectors by a scale vector
template<typename Collection, typename ScaleVector>
    requires std::ranges::range<Collection> &&
             is4dVector<std::ranges::range_value_t<Collection>> &&
             is4dVector<ScaleVector>
constexpr void scale(Collection& collection, const ScaleVector& factors) {
    for (auto& vec : collection) {
        vec.x *= factors.x;
        vec.y *= factors.y;
        vec.z *= factors.z;
        vec.w *= factors.w;
    }
}

#endif // VECUTIL_H
