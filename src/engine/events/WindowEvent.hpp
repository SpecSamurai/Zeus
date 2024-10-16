#pragma once

#include <cstdint>

namespace Zeus
{
struct WindowResizedEvent
{
    std::uint32_t width;
    std::uint32_t height;
};

struct WindowFullscreenToggledEvent
{
};

struct WindowClosedEvent
{
};
}
