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
    Math::Vector2f offset;
};

struct MouseMovedEvent
{
    Math::Vector2f position;
};
}
