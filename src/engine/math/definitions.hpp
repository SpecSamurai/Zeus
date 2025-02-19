#pragma once

#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Zeus::Math
{
inline constexpr float PI{ 3.14159265358979323846f };
inline constexpr float DEG2RAD_MULTIPLIER{ PI / 180.0f };
inline constexpr float RAD2DEG_MULTIPLIER{ 180.0f / PI };

using Vector2f = Vector2<float>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;

using Matrix3x3f = Matrix3x3<float>;
using Matrix4x4f = Matrix4x4<float>;

using Color = Vector4f;

namespace Colors
{
inline constexpr Color BLACK(0, 0, 0, 1);
inline constexpr Color WHITE(1, 1, 1, 1);

inline constexpr Color RED(1, 0, 0, 1);
inline constexpr Color GREEN(0, 1, 0, 1);
inline constexpr Color BLUE(0, 0, 1, 1);

inline constexpr Color YELLOW(1, 1, 0, 1);
inline constexpr Color MAGENTA(1, 0, 1, 1);
inline constexpr Color CYAN(0, 1, 1, 1);
}
}
