#pragma once

#include "input/KeyCode.hpp"

namespace Zeus
{
struct KeyPressedEvent
{
    KeyCode keyCode;
    bool isRepeated;
};

struct KeyReleasedEvent
{
    KeyCode keyCode;
};

struct KeyTypedEvent
{
    KeyCode keyCode;
};
}
