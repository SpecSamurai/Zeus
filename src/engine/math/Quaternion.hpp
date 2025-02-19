#pragma once

#include "Vector3.hpp"

#include <cassert>
#include <cstddef>
#include <type_traits>

namespace Zeus::Math
{
template <typename T>
class Quaternion
{
    static_assert(std::is_floating_point_v<T>);

public:
    T x, y, z, w;

    Quaternion() : x{ 0 }, y{ 0 }, z{ 0 }, w{ 1 }
    {
    }

    Quaternion(const Quaternion<T>& quaternion)
        : x{ quaternion.x },
          y{ quaternion.y },
          z{ quaternion.z },
          w{ quaternion.w }
    {
    }

    Quaternion(T real, const Vector3<T>& complex)
        : x{ complex.x },
          y{ complex.y },
          z{ complex.z },
          w{ real }
    {
    }

    Quaternion(T x, T y, T z, T w) : x{ x }, y{ y }, z{ z }, w{ w }
    {
    }

    constexpr Vector3<T> complex() const
    {
        return Vector3(x, y, z);
    }

    constexpr T real() const
    {
        return w;
    }

    constexpr T& operator[](const std::size_t i)
    {
        assert(0 <= i && i < size());

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    constexpr const T& operator[](const std::size_t i) const
    {
        assert(0 <= i && i < size());

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

private:
    constexpr std::size_t size() const
    {
        return 4;
    }
};

template <typename T>
constexpr Quaternion<T> operator-(const Quaternion<T>& quaternion)
{
    return Quaternion<T>(
        -quaternion.x,
        -quaternion.y,
        -quaternion.z,
        -quaternion.w);
}

template <typename T>
constexpr Quaternion<T> operator+(
    const Quaternion<T>& left,
    const Quaternion<T>& right)
{
    return Quaternion<T>(
        left.x + right.x,
        left.y + right.y,
        left.z + right.z,
        left.w + right.w);
}

template <typename T>
constexpr Quaternion<T> operator+(const T scalar, const Quaternion<T>& right)
{
    return Quaternion<T>(
        scalar + right.x,
        scalar + right.y,
        scalar + right.z,
        scalar + right.w);
}

template <typename T>
constexpr Quaternion<T> operator+(const Quaternion<T>& left, const T scalar)
{
    return Quaternion<T>(
        left.x + scalar,
        left.y + scalar,
        left.z + scalar,
        left.w + scalar);
}

template <typename T>
constexpr Quaternion<T> operator-(
    const Quaternion<T>& left,
    const Quaternion<T>& right)
{
    return Quaternion<T>(
        left.x - right.x,
        left.y - right.y,
        left.z - right.z,
        left.w - right.w);
}

template <typename T>
constexpr Quaternion<T> operator-(const Quaternion<T>& left, const T scalar)
{
    return Quaternion<T>(
        left.x - scalar,
        left.y - scalar,
        left.z - scalar,
        left.w - scalar);
}

template <typename T>
constexpr Quaternion<T> operator-(const T scalar, const Quaternion<T>& right)
{
    return Quaternion<T>(
        scalar - right.x,
        scalar - right.y,
        scalar - right.z,
        scalar - right.w);
}

template <typename T>
constexpr Quaternion<T> operator*(const Quaternion<T>& left, const T scalar)
{
    return Quaternion<T>(
        left.x * scalar,
        left.y * scalar,
        left.z * scalar,
        left.w * scalar);
}

template <typename T>
constexpr Quaternion<T> operator*(const T scalar, const Quaternion<T>& right)
{
    return Quaternion<T>(
        scalar * right.x,
        scalar * right.y,
        scalar * right.z,
        scalar * right.w);
}

template <typename T>
constexpr Quaternion<T> operator*(
    const Quaternion<T>& left,
    const Quaternion<T>& right)
{
    // Hamilton product
    return Quaternion<T>(
        left.w * right.x + left.x * right.w + left.y * right.z -
            left.z * right.y,
        left.w * right.y - left.x * right.z + left.y * right.w +
            left.z * right.x,
        left.w * right.z + left.x * right.y - left.y * right.x +
            left.z * right.w,
        left.w * right.w - left.x * right.x - left.y * right.y -
            left.z * right.z);
}

template <typename T>
constexpr Quaternion<T> operator/(const Quaternion<T>& left, const T scalar)
{
    return Quaternion<T>(
        left.x / scalar,
        left.y / scalar,
        left.z / scalar,
        left.w / scalar);
}

template <typename T>
constexpr Quaternion<T> operator/(const T scalar, const Quaternion<T>& right)
{
    return Quaternion<T>(
        scalar / right.x,
        scalar / right.y,
        scalar / right.z,
        scalar / right.w);
}
}
