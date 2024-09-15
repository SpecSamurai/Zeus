#pragma once

#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"
#include "Quaternion.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "geometric.hpp"

#include <cmath>
#include <cstddef>
#include <utility>

namespace Zeus
{
template <typename T>
constexpr void transpose(Matrix4x4<T>& matrix)
{
    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ i + 1 }; j < 4; ++j)
            std::swap(matrix[i][j], matrix[j][i]);
}

template <typename T>
constexpr void transpose(Matrix3x3<T>& matrix)
{
    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ i + 1 }; j < 3; ++j)
            std::swap(matrix[i][j], matrix[j][i]);
}

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
    static_assert(std::is_floating_point_v<T>);
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
constexpr Matrix4x4<T> lookAt(
    const Vector3<T>& eye,
    const Vector3<T>& target,
    const Vector3<T>& upDir)
{
    static_assert(std::is_floating_point_v<T>);
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
constexpr Matrix4x4<T> orthographic(
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
constexpr Matrix4x4<T> perspective(
    float fovYRadians,
    float aspectRatio,
    float near,
    float far)
{
    static_assert(std::is_floating_point_v<T>);

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

// Quaternion * Vector3 (rotates vector)
// https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/
template <typename T>
constexpr Vector3<T> rotateVector(
    const Quaternion<T>& quaternion,
    const Vector3<T>& vector)
{
    const Vector3<T> q_xyz(quaternion.x, quaternion.y, quaternion.z);
    const Vector3<T> t{ 2.0f * cross(q_xyz, vector) };
    return vector + quaternion.w * t + cross(q_xyz, t);
}

template <typename T>
constexpr Matrix3x3<T> rotationMatrix3x3(const Quaternion<T>& quaternion)
{
    constexpr T ONE{ static_cast<T>(1) };
    constexpr T TWO{ static_cast<T>(2) };

    T xx{ quaternion.x * quaternion.x };
    T yy{ quaternion.y * quaternion.y };
    T zz{ quaternion.z * quaternion.z };
    T xz{ quaternion.x * quaternion.z };
    T xy{ quaternion.x * quaternion.y };
    T yz{ quaternion.y * quaternion.z };
    T wx{ quaternion.w * quaternion.x };
    T wy{ quaternion.w * quaternion.y };
    T wz{ quaternion.w * quaternion.z };

    Matrix3x3<T> matrix(ONE);
    matrix[0][0] = ONE - TWO * (yy + zz);
    matrix[0][1] = TWO * (xy + wz);
    matrix[0][2] = TWO * (xz - wy);

    matrix[1][0] = TWO * (xy - wz);
    matrix[1][1] = ONE - TWO * (xx + zz);
    matrix[1][2] = TWO * (yz + wx);

    matrix[2][0] = TWO * (xz + wy);
    matrix[2][1] = TWO * (yz - wx);
    matrix[2][2] = ONE - TWO * (xx + yy);

    return matrix;
}

template <typename T>
constexpr Matrix4x4<T> rotationMatrix4x4(const Quaternion<T>& quaternion)
{
    constexpr T ONE{ static_cast<T>(1) };
    constexpr T TWO{ static_cast<T>(2) };

    T xx{ quaternion.x * quaternion.x };
    T yy{ quaternion.y * quaternion.y };
    T zz{ quaternion.z * quaternion.z };
    T xz{ quaternion.x * quaternion.z };
    T xy{ quaternion.x * quaternion.y };
    T yz{ quaternion.y * quaternion.z };
    T wx{ quaternion.w * quaternion.x };
    T wy{ quaternion.w * quaternion.y };
    T wz{ quaternion.w * quaternion.z };

    Matrix4x4<T> matrix(ONE);
    matrix[0][0] = ONE - TWO * (yy + zz);
    matrix[0][1] = TWO * (xy + wz);
    matrix[0][2] = TWO * (xz - wy);
    matrix[0][3] = 0;

    matrix[1][0] = TWO * (xy - wz);
    matrix[1][1] = ONE - TWO * (xx + zz);
    matrix[1][2] = TWO * (yz + wx);
    matrix[1][3] = 0;

    matrix[2][0] = TWO * (xz + wy);
    matrix[2][1] = TWO * (yz - wx);
    matrix[2][2] = ONE - TWO * (xx + yy);
    matrix[2][3] = 0;

    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = ONE;

    return matrix;
}
}
