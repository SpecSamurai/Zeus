#pragma once

#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Zeus
{
inline constexpr float PI{ 3.14159265358979323846f };
inline constexpr float DEG2RAD_MULTIPLIER{ PI / 180.0f };
inline constexpr float RAD2DEG_MULTIPLIER{ 180.0f / PI };

using Vector2f = Vector2<float>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;

using Matrix3x3f = Matrix3x3<float>;
using Matrix4x4f = Matrix4x4<float>;
}
