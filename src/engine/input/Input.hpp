#pragma once

#include "KeyCode.hpp"
#include "MouseButtonCode.hpp"
#include "math/definitions.hpp"

namespace Zeus
{
class Input
{
public:
    static bool IsKeyDown(const KeyCode keyCode);
    static bool IsKeyUp(const KeyCode keyCode);

    static bool IsMouseButtonDown(const MouseButtonCode buttonCode);
    static bool IsMouseButtonUp(const MouseButtonCode buttonCode);

    static Math::Vector2f GetMousePosition();
};
}
