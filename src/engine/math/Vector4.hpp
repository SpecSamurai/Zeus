#pragma once

#include <cassert>
#include <cstddef>

namespace Zeus::Math
{
template <typename T>
class Vector4
{
public:
    constexpr Vector4() : x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 }
    {
    }

    constexpr Vector4(T x, T y, T z, T w) : x{ x }, y{ y }, z{ z }, w{ w }
    {
    }

    constexpr Vector4(T scalar)
        : x{ scalar },
          y{ scalar },
          z{ scalar },
          w{ scalar }
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

    union {
        T w, a, q;
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
constexpr Vector4<T> operator+(const Vector4<T>& left, const Vector4<T>& right)
{
    return Vector4<T>(
        left.x + right.x,
        left.y + right.y,
        left.z + right.z,
        left.w + right.w);
}

template <typename T>
constexpr Vector4<T> operator+(const T& scalar, const Vector4<T>& right)
{
    return Vector4<T>(
        scalar + right.x,
        scalar + right.y,
        scalar + right.z,
        scalar + right.w);
}

template <typename T>
constexpr Vector4<T> operator+(const Vector4<T>& left, const T& scalar)
{
    return Vector4<T>(
        left.x + scalar,
        left.y + scalar,
        left.z + scalar,
        left.w + scalar);
}

template <typename T>
constexpr Vector4<T> operator-(const Vector4<T>& vector)
{
    return Vector4<T>(-vector.x, -vector.y, -vector.z, -vector.w);
}

template <typename T>
constexpr Vector4<T> operator-(const Vector4<T>& left, const Vector4<T>& right)
{
    return Vector4<T>(
        left.x - right.x,
        left.y - right.y,
        left.z - right.z,
        left.w - right.w);
}

template <typename T>
constexpr Vector4<T> operator-(const Vector4<T>& left, const T& scalar)
{
    return Vector4<T>(
        left.x - scalar,
        left.y - scalar,
        left.z - scalar,
        left.w - scalar);
}

template <typename T>
constexpr Vector4<T> operator-(const T& scalar, const Vector4<T>& right)
{
    return Vector4<T>(
        scalar - right.x,
        scalar - right.y,
        scalar - right.z,
        scalar - right.w);
}

template <typename T>
constexpr Vector4<T> operator*(const Vector4<T>& left, const T& scalar)
{
    return Vector4<T>(
        left.x * scalar,
        left.y * scalar,
        left.z * scalar,
        left.w * scalar);
}

template <typename T>
constexpr Vector4<T> operator*(const T& scalar, const Vector4<T>& right)
{
    return Vector4<T>(
        scalar * right.x,
        scalar * right.y,
        scalar * right.z,
        scalar * right.w);
}

template <typename T>
constexpr Vector4<T> operator*(const Vector4<T>& left, const Vector4<T>& right)
{
    return Vector4<T>(
        left.x * right.x,
        left.y * right.y,
        left.z * right.z,
        left.w * right.w);
}

template <typename T>
constexpr Vector4<T> operator/(const Vector4<T>& left, const T& scalar)
{
    return Vector4<T>(
        left.x / scalar,
        left.y / scalar,
        left.z / scalar,
        left.w / scalar);
}

template <typename T>
constexpr Vector4<T> operator/(const T& scalar, const Vector4<T>& right)
{
    return Vector4<T>(
        scalar / right.x,
        scalar / right.y,
        scalar / right.z,
        scalar / right.w);
}
}
