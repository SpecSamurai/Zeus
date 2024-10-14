#pragma once

#include <functional>

namespace Zeus
{
// template <typename EventType>
// using EventListener = std::function<bool(const EventType& event)>;

template <typename EventType>
using EventListener = bool (*)(const EventType& event);

// enum class EventType
// {
//     WindowResized,
//     WindowFullscreenToggled,
//
//     KeyPressed,
//     KeyReleased,
//
//     MouseClicked,
//     MouseButtonPressed,
//     MouseButtonReleased,
//     MouseMoved,
//     MouseScrolled,
// };
//
// class EventData
// {
// public:
//     constexpr EventType GetEventType()
//     {
//         return EventType::MouseScrolled;
//     }
// };
//
// using Event = std::variant<int, void*, EventData>;

// register
// unregster
// immediate disptach
// minimize allocations
}
