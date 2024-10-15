#pragma once

#include <functional>

namespace Zeus
{
template <typename EventType>
using EventHandler = std::function<bool(const EventType& event)>;
}
