#pragma once

#include "Matrix4x4.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

#include <cmath>

namespace Zeus
{
template <typename T>
constexpr T identity()
{
    return T(1.f);
}

template <typename T>
constexpr Matrix4x4<T> reflectionOrigin()
{
    Matrix4x4<T> result(1.f);

    result[0][0] = -1;
    result[1][1] = -1;
    result[2][2] = -1;

    return result;
}

template <typename T>
constexpr Matrix4x4<T> reflectionX()
{
    Matrix4x4<T> result(1.f);

    result[0][0] = -1;

    return result;
}

template <typename T>
constexpr Matrix4x4<T> reflectionY()
{
    Matrix4x4<T> result(1.f);

    result[1][1] = -1;

    return result;
}

template <typename T>
constexpr Matrix4x4<T> translation(const Vector3<T>& v)
{
    Matrix4x4<T> result(1.f);

    result[3] = Vector4<T>(v.x, v.y, v.z, 1.f);

    return result;
}

template <typename T>
constexpr Matrix4x4<T> scale(const Vector3<T>& v)
{
    Matrix4x4<T> result(1.f);

    result[0][0] = v.x;
    result[1][1] = v.y;
    result[2][2] = v.z;

    return result;
}

template <typename T>
constexpr Matrix4x4<T> scale(const T scale)
{
    Matrix4x4<T> result(1.f);

    result[0][0] = scale;
    result[1][1] = scale;
    result[2][2] = scale;

    return result;
}

// Rotation matrix from axis and angle
// https://en.wikipedia.org/wiki/Rotation_matrix
template <typename T>
constexpr Matrix4x4<T> rotation(const T angle, Vector3<T> axis)
{
    float c{ std::cos(angle) };
    float s{ std::sin(angle) };

    Vector3<T> unit{ normalize(axis) };

    Matrix4x4<T> result;

    constexpr T ONE = static_cast<T>(1);

    result[0][0] = c + (ONE - c) * unit.x * unit.x;
    result[0][1] = (ONE - c) * unit.x * unit.y + s * unit.z;
    result[0][2] = (ONE - c) * unit.x * unit.z - s * unit.y;
    result[0][3] = 0;

    result[1][0] = (ONE - c) * unit.y * unit.x - s * unit.z;
    result[1][1] = c + (ONE - c) * unit.y * unit.y;
    result[1][2] = (ONE - c) * unit.y * unit.z + s * unit.x;
    result[1][3] = 0;

    result[2][0] = (ONE - c) * unit.z * unit.x + s * unit.y;
    result[2][1] = (ONE - c) * unit.z * unit.y - s * unit.x;
    result[2][2] = c + (ONE - c) * unit.z * unit.z;
    result[2][3] = 0;

    result[3] = Vector4<T>(0, 0, 0, ONE);

    return result;
}

template <typename T>
Matrix4x4<T> lookAt(
    const Vector3<T>& eye,
    const Vector3<T>& target,
    const Vector3<T>& upDir)
{
    // the scene is actually rotated, not the camera
    Vector3<T> dir{ eye - target };

    Vector3<T> forward(normalize(dir));
    Vector3<T> right(normalize(cross(upDir, forward))); // left
    Vector3<T> up(cross(forward, right));

    Matrix4x4<T> result(1.f);

    result[0][0] = right.x;
    result[1][0] = right.y;
    result[2][0] = right.z;

    result[0][1] = up.x;
    result[1][1] = up.y;
    result[2][1] = up.z;

    result[0][2] = forward.x;
    result[1][2] = forward.y;
    result[2][2] = forward.z;

    result[3][0] = -dot(right, eye);
    result[3][1] = -dot(up, eye);
    result[3][2] = -dot(forward, eye);

    return result;
}

template <typename T>
Matrix4x4<T> orthographic(
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far)
{
    Matrix4x4<T> result(static_cast<T>(1));

    result[0][0] = static_cast<T>(2) / (right - left);
    result[1][1] = static_cast<T>(2) / (bottom - top);
    result[2][2] = -static_cast<T>(1) / (far - near);

    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(bottom + top) / (bottom - top);
    result[3][2] = -near / (far - near);

    return result;
}

template <typename T>
Matrix4x4<T> perspective(
    float fovYRadians,
    float aspectRatio,
    float near,
    float far)
{
    float invHalfTangent = 1.0f / tan(0.5f * fovYRadians);

    // float tangent = tan(0.5f * fovYRadians);
    // float top = near * tangent;
    // float right = top * aspectRatio;
    // float bottom = -top;

    Matrix4x4<T> result;

    result[0][0] = invHalfTangent / aspectRatio; // near / right
    result[1][1] = -invHalfTangent;              // near / bottom;
    result[2][2] = far / (near - far);
    result[2][3] = -1.0f;
    result[3][2] = (near * far) / (near - far);

    return result;
}
}
