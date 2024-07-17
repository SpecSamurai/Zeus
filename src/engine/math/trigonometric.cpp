#include "trigonometric.hpp"

#include "definitions.hpp"

namespace Zeus
{
float radians(const float degrees)
{
    return degrees * DEG2RAD_MULTIPLIER;
}

float degrees(const float radians)
{
    return radians * RAD2DEG_MULTIPLIER;
}
}
