#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

#include <cmath>

namespace Zeus
{
template <typename T>
float dot(const Vector2<T>& left, const Vector2<T>& right)
{
    return (left.x * right.x) + (left.y * right.y);
}

template <typename T>
float dot(const Vector3<T>& left, const Vector3<T>& right)
{
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z);
}

template <typename T>
float dot(const Vector4<T>& left, const Vector4<T>& right)
{
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z) +
           (left.w * right.w);
}

template <typename T>
Vector3<T> cross(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x);
}

template <typename T>
Vector4<T> cross(const Vector4<T>& left, const Vector4<T>& right)
{
    return Vector4<T>(
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x,
        0.0f);
}

template <typename T>
float length(const Vector2<T>& v)
{
    return std::sqrt(dot(v, v));
}

template <typename T>
float length(const Vector3<T>& v)
{
    return std::sqrt(dot(v, v));
}

template <typename T>
float length(const Vector4<T>& v)
{
    return std::sqrt(dot(v, v));
}

template <typename T>
Vector2<T> normalize(const Vector2<T>& v)
{
    auto invLength = static_cast<T>(1) / length(v);
    return Vector2<T>(v.x * invLength, v.y * invLength);
}

template <typename T>
Vector3<T> normalize(const Vector3<T>& v)
{
    auto invLength = static_cast<T>(1) / length(v);
    return Vector3<T>(v.x * invLength, v.y * invLength, v.z * invLength);
}

template <typename T>
Vector4<T> normalize(const Vector4<T>& v)
{
    auto invLength = static_cast<T>(1) / length(v);
    return Vector4<T>(
        v.x * invLength,
        v.y * invLength,
        v.z * invLength,
        v.w * invLength);
}
}
