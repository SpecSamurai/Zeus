#pragma once

#include <cstddef>

namespace Zeus
{
template <typename T>
class Quaternion
{
public:
    Quaternion() : x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 }
    {
    }

    Quaternion(T x, T y, T z, T w) : x{ x }, y{ y }, z{ z }, w{ w }
    {
    }

    Quaternion(T scalar) : x{ scalar }, y{ scalar }, z{ scalar }, w{ scalar }
    {
    }

    T x, y, z, w;

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
};
}
