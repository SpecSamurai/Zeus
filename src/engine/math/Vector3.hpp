#pragma once

#include <cassert>
#include <cstddef>

namespace Zeus::Math
{
template <typename T>
class Vector3
{
public:
    constexpr Vector3() : x{ 0 }, y{ 0 }, z{ 0 }
    {
    }

    constexpr Vector3(T x, T y, T z) : x{ x }, y{ y }, z{ z }
    {
    }

    constexpr Vector3(T scalar) : x{ scalar }, y{ scalar }, z{ scalar }
    {
    }

    union {
        T x, r, s;
    };

    union {
        T y, g, t;
    };

    union {
        T z, b, p;
    };

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
        }
    }

private:
    constexpr std::size_t size() const
    {
        return 3;
    }
};

template <typename T>
constexpr Vector3<T> operator+(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(left.x + right.x, left.y + right.y, left.z + right.z);
}

template <typename T>
constexpr Vector3<T> operator+(const T& scalar, const Vector3<T>& right)
{
    return Vector3<T>(scalar + right.x, scalar + right.y, scalar + right.z);
}

template <typename T>
constexpr Vector3<T> operator+(const Vector3<T>& left, const T& scalar)
{
    return Vector3<T>(left.x + scalar, left.y + scalar, left.z + scalar);
}

template <typename T>
constexpr Vector3<T> operator-(const Vector3<T>& vector)
{
    return Vector3<T>(-vector.x, -vector.y, -vector.z);
}

template <typename T>
constexpr Vector3<T> operator-(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(left.x - right.x, left.y - right.y, left.z - right.z);
}

template <typename T>
constexpr Vector3<T> operator-(const Vector3<T>& left, const T& scalar)
{
    return Vector3<T>(left.x - scalar, left.y - scalar, left.z - scalar);
}

template <typename T>
constexpr Vector3<T> operator-(const T& scalar, const Vector3<T>& right)
{
    return Vector3<T>(scalar - right.x, scalar - right.y, scalar - right.z);
}

template <typename T>
constexpr Vector3<T> operator*(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(left.x * right.x, left.y * right.y, left.z * right.z);
}

template <typename T>
constexpr Vector3<T> operator*(const Vector3<T>& left, const T& scalar)
{
    return Vector3<T>(left.x * scalar, left.y * scalar, left.z * scalar);
}

template <typename T>
constexpr Vector3<T> operator*(const T& scalar, const Vector3<T>& right)
{
    return Vector3<T>(scalar * right.x, scalar * right.y, scalar * right.z);
}

template <typename T>
constexpr Vector3<T> operator/(const Vector3<T>& left, const T& scalar)
{
    return Vector3<T>(left.x / scalar, left.y / scalar, left.z / scalar);
}

template <typename T>
constexpr Vector3<T> operator/(const T& scalar, const Vector3<T>& right)
{
    return Vector3<T>(scalar / right.x, scalar / right.y, scalar / right.z);
}
}
