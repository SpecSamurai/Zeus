#pragma once

#include "input/MouseButtonCode.hpp"
#include "math/definitions.hpp"

namespace Zeus
{
struct MouseButtonPressedEvent
{
    MouseButtonCode buttonCode;
};

struct MouseButtonReleasedEvent
{
    MouseButtonCode buttonCode;
};

struct MouseScrolledEvent
{
    Vector2f offset;
};

struct MouseMovedEvent
{
    Vector2f position;
};
}
