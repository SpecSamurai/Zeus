#pragma once

#include "Vector3.hpp"

#include <cassert>
#include <cstddef>

namespace Zeus::Math
{
template <typename T>
class Matrix3x3
{
public:
    Matrix3x3()
        : m_data{ Vector3<T>(0, 0, 0),
                  Vector3<T>(0, 0, 0),
                  Vector3<T>(0, 0, 0) }
    {
    }

    Matrix3x3(
        // clang-format off
        const T& x0, const T& y0, const T& z0,
        const T& x1, const T& y1, const T& z1,
        const T& x2, const T& y2, const T& z2)
        // clang-format on
        : m_data{ Vector3<T>(x0, y0, z0),
                  Vector3<T>(x1, y1, z1),
                  Vector3<T>(x2, y2, z2) }
    {
    }

    Matrix3x3(Vector3<T> column1, Vector3<T> column2, Vector3<T> column3)
        : m_data{ column1, column2, column3 }
    {
    }

    Matrix3x3(T scalar)
        : m_data{ Vector3<T>(scalar, 0, 0),
                  Vector3<T>(0, scalar, 0),
                  Vector3<T>(0, 0, scalar) }
    {
    }

    constexpr Vector3<T>& operator[](const std::size_t i)
    {
        assert(0 <= i && i < size());
        return m_data[i];
    }

    constexpr Vector3<T> const& operator[](const std::size_t i) const
    {
        assert(0 <= i && i < size());
        return m_data[i];
    }

private:
    constexpr std::size_t size() const
    {
        return 3;
    }

    Vector3<T> m_data[3];
};

template <typename T>
constexpr Matrix3x3<T> operator+(
    const Matrix3x3<T>& left,
    const Matrix3x3<T>& right)
{
    return Matrix3x3<T>(
        left[0] + right[0],
        left[1] + right[1],
        left[2] + right[2]);
}

template <typename T>
constexpr Matrix3x3<T> operator+(const T& scalar, const Matrix3x3<T>& right)
{
    return Matrix3x3<T>(
        scalar + right[0],
        scalar + right[1],
        scalar + right[2]);
}

template <typename T>
constexpr Matrix3x3<T> operator+(const Matrix3x3<T>& left, const T& scalar)
{
    return Matrix3x3<T>(left[0] + scalar, left[1] + scalar, left[2] + scalar);
}

template <typename T>
constexpr Matrix3x3<T> operator-(
    const Matrix3x3<T>& left,
    const Matrix3x3<T>& right)
{
    return Matrix3x3<T>(
        left[0] - right[0],
        left[1] - right[1],
        left[2] - right[2]);
}

template <typename T>
constexpr Matrix3x3<T> operator-(const Matrix3x3<T>& left, const T& scalar)
{
    return Matrix3x3<T>(left[0] - scalar, left[1] - scalar, left[2] - scalar);
}

template <typename T>
constexpr Matrix3x3<T> operator-(const T& scalar, const Matrix3x3<T>& right)
{
    return Matrix3x3<T>(
        scalar - right[0],
        scalar - right[1],
        scalar - right[2]);
}

template <typename T>
constexpr Matrix3x3<T> operator*(const Matrix3x3<T>& left, const T& scalar)
{
    return Matrix3x3<T>(left[0] * scalar, left[1] * scalar, left[2] * scalar);
}

template <typename T>
constexpr Matrix3x3<T> operator*(const T& scalar, const Matrix3x3<T>& right)
{
    return Matrix3x3<T>(
        scalar * right[0],
        scalar * right[1],
        scalar * right[2]);
}

template <typename T>
constexpr Matrix3x3<T> operator*(
    const Matrix3x3<T>& left,
    const Matrix3x3<T>& right)
{
    const Vector3<T>& ColA0{ left[0] };
    const Vector3<T>& ColA1{ left[1] };
    const Vector3<T>& ColA2{ left[2] };

    const Vector3<T>& ColB0{ right[0] };
    const Vector3<T>& ColB1{ right[1] };
    const Vector3<T>& ColB2{ right[2] };

    Matrix3x3<T> result;

    result[0] = ColA2 * ColB0.z + ColA1 * ColB0.y + ColA0 * ColB0.x;
    result[1] = ColA2 * ColB1.z + ColA1 * ColB1.y + ColA0 * ColB1.x;
    result[2] = ColA2 * ColB2.z + ColA1 * ColB2.y + ColA0 * ColB2.x;

    return result;
}

template <typename T>
constexpr Vector3<T> operator*(
    const Matrix3x3<T>& left,
    const Vector3<T>& right)
{

    return Vector3<T>(
        // clang-format off
        left[0][0] * right[0] + left[1][0] * right[1] + left[2][0] * right[2],
        left[0][1] * right[0] + left[1][1] * right[1] + left[2][1] * right[2],
        left[0][2] * right[0] + left[1][2] * right[1] + left[2][2] * right[2]
        // clang-format on
    );
}

template <typename T>
constexpr Matrix3x3<T> operator/(const Matrix3x3<T>& left, const T& scalar)
{
    return Matrix3x3<T>(left[0] / scalar, left[1] / scalar, left[2] / scalar);
}

template <typename T>
constexpr Matrix3x3<T> operator/(const T& scalar, const Matrix3x3<T>& right)
{
    return Matrix3x3<T>(
        scalar / right[0],
        scalar / right[1],
        scalar / right[2]);
}
}
