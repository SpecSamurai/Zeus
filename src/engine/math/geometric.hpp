#pragma once

#include "Quaternion.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

#include <cmath>

namespace Zeus
{
template <typename T>
constexpr T dot(const Vector2<T>& left, const Vector2<T>& right)
{
    static_assert(std::is_floating_point_v<T>);
    return (left.x * right.x) + (left.y * right.y);
}

template <typename T>
constexpr T dot(const Vector3<T>& left, const Vector3<T>& right)
{
    static_assert(std::is_floating_point_v<T>);
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z);
}

template <typename T>
constexpr T dot(const Vector4<T>& left, const Vector4<T>& right)
{
    static_assert(std::is_floating_point_v<T>);
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z) +
           (left.w * right.w);
}

template <typename T>
constexpr T dot(const Quaternion<T>& left, const Quaternion<T>& right)
{
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z) +
           (left.w * right.w);
}

template <typename T>
constexpr Vector3<T> cross(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x);
}

template <typename T>
constexpr Vector4<T> cross(const Vector4<T>& left, const Vector4<T>& right)
{
    return Vector4<T>(
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x,
        0.0f);
}

template <typename T>
constexpr T length(const Vector2<T>& v)
{
    static_assert(std::is_floating_point_v<T>);
    return std::sqrt(dot(v, v));
}

template <typename T>
constexpr T length(const Vector3<T>& v)
{
    static_assert(std::is_floating_point_v<T>);
    return std::sqrt(dot(v, v));
}

template <typename T>
constexpr T length(const Vector4<T>& v)
{
    static_assert(std::is_floating_point_v<T>);
    return std::sqrt(dot(v, v));
}

// length: std::sqrt(dot(quaternion, quaternion))
template <typename T>
constexpr T norm(const Quaternion<T>& quaternion)
{
    return std::sqrt(
        quaternion.x * quaternion.x + quaternion.y * quaternion.y +
        quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

template <typename T>
constexpr Vector2<T> normalize(const Vector2<T>& v)
{
    static_assert(std::is_floating_point_v<T>);
    auto invLength = static_cast<T>(1) / length(v);
    return Vector2<T>(v.x * invLength, v.y * invLength);
}

template <typename T>
constexpr Vector3<T> normalize(const Vector3<T>& v)
{
    static_assert(std::is_floating_point_v<T>);
    auto invLength = static_cast<T>(1) / length(v);
    return Vector3<T>(v.x * invLength, v.y * invLength, v.z * invLength);
}

template <typename T>
constexpr Vector4<T> normalize(const Vector4<T>& v)
{
    static_assert(std::is_floating_point_v<T>);
    auto invLength{ static_cast<T>(1) / length(v) };
    return Vector4<T>(
        v.x * invLength,
        v.y * invLength,
        v.z * invLength,
        v.w * invLength);
}

template <typename T>
constexpr Quaternion<T> normalize(const Quaternion<T>& quaternion)
{
    T qNorm{ norm(quaternion) };
    if (qNorm <= static_cast<T>(0)) // Problem
        return Quaternion<T>();

    const T ONE_OVER_NORM{ static_cast<T>(1) / qNorm };

    return Quaternion<T>(
        quaternion.x * ONE_OVER_NORM,
        quaternion.y * ONE_OVER_NORM,
        quaternion.z * ONE_OVER_NORM,
        quaternion.w * ONE_OVER_NORM);
}

template <typename T>
constexpr Quaternion<T> conjugate(const Quaternion<T>& quaternion)
{
    return Quaternion(
        -quaternion.x,
        -quaternion.y,
        -quaternion.z,
        quaternion.w);
}

template <typename T>
constexpr Quaternion<T> inverse(const Quaternion<T>& quaternion)
{
    return conjugate(quaternion) / dot(quaternion, quaternion);
}

// angle is expected to be in radians
template <typename T>
constexpr Quaternion<T> angleAxis(const T angle, const Vector3<T>& axis)
{
    float HALF_ANGLE{ angle * static_cast<T>(0.5) };
    float SIN_HALF_ANGLE{ std::sin(HALF_ANGLE) };

    auto w{ std::cos(HALF_ANGLE) };
    auto x{ axis.x * SIN_HALF_ANGLE };
    auto y{ axis.y * SIN_HALF_ANGLE };
    auto z{ axis.z * SIN_HALF_ANGLE };

    return Quaternion<T>(x, y, z, w);
}

// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// roll (x), pitch (y), yaw (z), angles are in radians
template <typename T>
constexpr Quaternion<T> euler2quat(const Vector3<T>& eulerAngle)
{
    constexpr T HALF{ static_cast<T>(0.5) };

    double cosRoll{ std::cos(eulerAngle.x * HALF) };
    double sinRoll{ std::sin(eulerAngle.x * HALF) };
    double cosPitch{ std::cos(eulerAngle.y * HALF) };
    double sinPitch{ std::sin(eulerAngle.y * HALF) };
    double cosYaw{ std::cos(eulerAngle.z * HALF) };
    double sinYaw{ std::sin(eulerAngle.z * HALF) };

    Quaternion<T> quaternion;
    quaternion.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
    quaternion.x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    quaternion.y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    quaternion.z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;

    return quaternion;
}
}
