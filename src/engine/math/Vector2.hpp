#pragma once

#include <cassert>
#include <cstddef>

namespace Zeus::Math
{
template <typename T>
class Vector2
{
public:
    constexpr Vector2() : x{ 0 }, y{ 0 }
    {
    }

    constexpr Vector2(T x, T y) : x{ x }, y{ y }
    {
    }

    constexpr Vector2(T scalar) : x{ scalar }, y{ scalar }
    {
    }

    union {
        T x, r, s;
    };

    union {
        T y, g, t;
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
        }
    }

private:
    constexpr std::size_t size() const
    {
        return 2;
    }
};

template <typename T>
constexpr Vector2<T> operator+(const Vector2<T>& left, const Vector2<T>& right)
{
    return Vector2<T>(left.x + right.x, left.y + right.y);
}

template <typename T>
constexpr Vector2<T> operator+(const T& scalar, const Vector2<T>& right)
{
    return Vector2<T>(scalar + right.x, scalar + right.y);
}

template <typename T>
constexpr Vector2<T> operator+(const Vector2<T>& left, const T& scalar)
{
    return Vector2<T>(left.x + scalar, left.y + scalar);
}

template <typename T>
constexpr Vector2<T> operator-(const Vector2<T>& vector)
{
    return Vector2<T>(-vector.x, -vector.y);
}

template <typename T>
constexpr Vector2<T> operator-(const Vector2<T>& left, const Vector2<T>& right)
{
    return Vector2<T>(left.x - right.x, left.y - right.y);
}

template <typename T>
constexpr Vector2<T> operator-(const Vector2<T>& left, const T& scalar)
{
    return Vector2<T>(left.x - scalar, left.y - scalar);
}

template <typename T>
constexpr Vector2<T> operator-(const T& scalar, const Vector2<T>& right)
{
    return Vector2<T>(scalar - right.x, scalar - right.y);
}

template <typename T>
constexpr Vector2<T> operator*(const Vector2<T>& left, const T& scalar)
{
    return Vector2<T>(left.x * scalar, left.y * scalar);
}

template <typename T>
constexpr Vector2<T> operator*(const T& scalar, const Vector2<T>& right)
{
    return Vector2<T>(scalar * right.x, scalar * right.y);
}

template <typename T>
constexpr Vector2<T> operator*(const Vector2<T>& left, const Vector2<T>& right)
{
    return Vector2<T>(left.x * right.x, left.y * right.y);
}

template <typename T>
constexpr Vector2<T> operator/(const Vector2<T>& left, const T& scalar)
{
    return Vector2<T>(left.x / scalar, left.y / scalar);
}

template <typename T>
constexpr Vector2<T> operator/(const T& scalar, const Vector2<T>& right)
{
    return Vector2<T>(scalar / right.x, scalar / right.y);
}
}
