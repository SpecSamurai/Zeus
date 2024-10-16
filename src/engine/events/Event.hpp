#pragma once

#include "events/EventDispatcher.hpp"
#include "events/EventQueue.hpp"
#include "events/KeyEvent.hpp"
#include "events/MouseEvent.hpp"
#include "events/WindowEvent.hpp"

namespace Zeus::Event
{
inline EventDispatcher<
    KeyPressedEvent,
    KeyReleasedEvent,
    KeyTypedEvent,
    MouseButtonPressedEvent,
    MouseButtonReleasedEvent,
    MouseScrolledEvent,
    MouseMovedEvent,
    WindowResizedEvent,
    WindowFullscreenToggledEvent,
    WindowClosedEvent>
    Dispatcher(10);

inline EventQueue<
    KeyPressedEvent,
    KeyReleasedEvent,
    KeyTypedEvent,
    MouseButtonPressedEvent,
    MouseButtonReleasedEvent,
    MouseScrolledEvent,
    MouseMovedEvent,
    WindowResizedEvent,
    WindowFullscreenToggledEvent,
    WindowClosedEvent>
    Queue(10, 100);
}
