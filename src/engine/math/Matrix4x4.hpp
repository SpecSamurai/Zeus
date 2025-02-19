#pragma once

#include "Vector4.hpp"

#include <cassert>
#include <cstddef>

namespace Zeus::Math
{
template <typename T>
class Matrix4x4
{
public:
    Matrix4x4()
        : m_data{ Vector4<T>(0, 0, 0, 0),
                  Vector4<T>(0, 0, 0, 0),
                  Vector4<T>(0, 0, 0, 0),
                  Vector4<T>(0, 0, 0, 0) }
    {
    }

    Matrix4x4(
        // clang-format off
        const T& x0, const T& y0, const T& z0, const T& w0,
        const T& x1, const T& y1, const T& z1, const T& w1,
        const T& x2, const T& y2, const T& z2, const T& w2,
        const T& x3, const T& y3, const T& z3, const T& w3)
        // clang-format on
        : m_data{ Vector4<T>(x0, y0, z0, w0),
                  Vector4<T>(x1, y1, z1, w1),
                  Vector4<T>(x2, y2, z2, w2),
                  Vector4<T>(x3, y3, z3, w3) }
    {
    }

    Matrix4x4(
        Vector4<T> column1,
        Vector4<T> column2,
        Vector4<T> column3,
        Vector4<T> column4)
        : m_data{ column1, column2, column3, column4 }
    {
    }

    Matrix4x4(T scalar)
        : m_data{ Vector4<T>(scalar, 0, 0, 0),
                  Vector4<T>(0, scalar, 0, 0),
                  Vector4<T>(0, 0, scalar, 0),
                  Vector4<T>(0, 0, 0, scalar) }
    {
    }

    constexpr Vector4<T>& operator[](const std::size_t i)
    {
        assert(0 <= i && i < size());
        return m_data[i];
    }

    constexpr Vector4<T> const& operator[](const std::size_t i) const
    {
        assert(0 <= i && i < size());
        return m_data[i];
    }

private:
    constexpr std::size_t size() const
    {
        return 4;
    }

    Vector4<T> m_data[4];
};

template <typename T>
constexpr Matrix4x4<T> operator+(
    const Matrix4x4<T>& left,
    const Matrix4x4<T>& right)
{
    return Matrix4x4<T>(
        left[0] + right[0],
        left[1] + right[1],
        left[2] + right[2],
        left[3] + right[3]);
}

template <typename T>
constexpr Matrix4x4<T> operator+(const T& scalar, const Matrix4x4<T>& right)
{
    return Matrix4x4<T>(
        scalar + right[0],
        scalar + right[1],
        scalar + right[2],
        scalar + right[3]);
}

template <typename T>
constexpr Matrix4x4<T> operator+(const Matrix4x4<T>& left, const T& scalar)
{
    return Matrix4x4<T>(
        left[0] + scalar,
        left[1] + scalar,
        left[2] + scalar,
        left[3] + scalar);
}

template <typename T>
constexpr Matrix4x4<T> operator-(
    const Matrix4x4<T>& left,
    const Matrix4x4<T>& right)
{
    return Matrix4x4<T>(
        left[0] - right[0],
        left[1] - right[1],
        left[2] - right[2],
        left[3] - right[3]);
}

template <typename T>
constexpr Matrix4x4<T> operator-(const Matrix4x4<T>& left, const T& scalar)
{
    return Matrix4x4<T>(
        left[0] - scalar,
        left[1] - scalar,
        left[2] - scalar,
        left[3] - scalar);
}

template <typename T>
constexpr Matrix4x4<T> operator-(const T& scalar, const Matrix4x4<T>& right)
{
    return Matrix4x4<T>(
        scalar - right[0],
        scalar - right[1],
        scalar - right[2],
        scalar - right[3]);
}

template <typename T>
constexpr Matrix4x4<T> operator*(const Matrix4x4<T>& left, const T& scalar)
{
    return Matrix4x4<T>(
        left[0] * scalar,
        left[1] * scalar,
        left[2] * scalar,
        left[3] * scalar);
}

template <typename T>
constexpr Matrix4x4<T> operator*(const T& scalar, const Matrix4x4<T>& right)
{
    return Matrix4x4<T>(
        scalar * right[0],
        scalar * right[1],
        scalar * right[2],
        scalar * right[3]);
}

template <typename T>
constexpr Matrix4x4<T> operator*(
    const Matrix4x4<T>& left,
    const Matrix4x4<T>& right)
{
    const Vector4<T>& ColA0{ left[0] };
    const Vector4<T>& ColA1{ left[1] };
    const Vector4<T>& ColA2{ left[2] };
    const Vector4<T>& ColA3{ left[3] };

    const Vector4<T>& ColB0{ right[0] };
    const Vector4<T>& ColB1{ right[1] };
    const Vector4<T>& ColB2{ right[2] };
    const Vector4<T>& ColB3{ right[3] };

    Matrix4x4<T> result;

    result[0] =
        ColA3 * ColB0.w + ColA2 * ColB0.z + ColA1 * ColB0.y + ColA0 * ColB0.x;
    result[1] =
        ColA3 * ColB1.w + ColA2 * ColB1.z + ColA1 * ColB1.y + ColA0 * ColB1.x;
    result[2] =
        ColA3 * ColB2.w + ColA2 * ColB2.z + ColA1 * ColB2.y + ColA0 * ColB2.x;
    result[3] =
        ColA3 * ColB3.w + ColA2 * ColB3.z + ColA1 * ColB3.y + ColA0 * ColB3.x;

    return result;
}

template <typename T>
constexpr Vector4<T> operator*(
    const Matrix4x4<T>& left,
    const Vector4<T>& right)
{
    return Vector4<T>(
        // clang-format off
        left[0][0] * right[0] + left[1][0] * right[1] + left[2][0] * right[2] + left[3][0] * right[3],
        left[0][1] * right[0] + left[1][1] * right[1] + left[2][1] * right[2] + left[3][1] * right[3],
        left[0][2] * right[0] + left[1][2] * right[1] + left[2][2] * right[2] + left[3][2] * right[3],
        left[0][3] * right[0] + left[1][3] * right[1] + left[2][3] * right[2] + left[3][3] * right[3]
        // clang-format on
    );
}

template <typename T>
constexpr Matrix4x4<T> operator/(const Matrix4x4<T>& left, const T& scalar)
{
    return Matrix4x4<T>(
        left[0] / scalar,
        left[1] / scalar,
        left[2] / scalar,
        left[3] / scalar);
}

template <typename T>
constexpr Matrix4x4<T> operator/(const T& scalar, const Matrix4x4<T>& right)
{
    return Matrix4x4<T>(
        scalar / right[0],
        scalar / right[1],
        scalar / right[2],
        scalar / right[3]);
}
}
