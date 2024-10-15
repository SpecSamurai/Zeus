#pragma once

#include "Event.hpp"

#include <tuple>
#include <vector>

namespace Zeus
{
template <typename... EventTypes>
class EventDispatcher
{
    template <typename EventType>
    struct RegisteredEventHandler
    {
        const char* name;
        EventHandler<EventType> eventHandler;
    };

    template <typename EventType>
    using EventHandlerPool = std::vector<RegisteredEventHandler<EventType>>;

public:
    EventDispatcher(std::uint64_t capacity)
    {
        (std::get<EventHandlerPool<EventTypes>>(m_eventHandlers)
             .reserve(capacity),
         ...);
    }

    template <typename EventType>
    void Register(
        const char* name,
        bool (*eventHandler)(const EventType& event))
    {
        EventHandlerPool<EventType>& pool{
            std::get<EventHandlerPool<EventType>>(m_eventHandlers)
        };

        pool.emplace_back(RegisteredEventHandler{
            .name = name,
            .eventHandler = std::forward<EventHandler<EventType>>(eventHandler),
        });
    }

    template <typename EventType>
    void Register(const char* name, EventHandler<EventType>&& eventHandler)
    {
        EventHandlerPool<EventType>& pool{
            std::get<EventHandlerPool<EventType>>(m_eventHandlers)
        };

        pool.emplace_back(RegisteredEventHandler{
            .name = name,
            .eventHandler = std::forward<EventHandler<EventType>>(eventHandler),
        });
    }

    template <typename EventType>
    void Unregister(const char* name)
    {
        EventHandlerPool<EventType>& pool{
            std::get<EventHandlerPool<EventType>>(m_eventHandlers)
        };

        for (std::size_t i{ 0 }; i < pool.size(); ++i)
        {
            if (strcmp(pool[i].name, name) == 0)
            {
                auto item{ pool.begin() + static_cast<std::int64_t>(i) };
                pool.erase(item);
            }
        }
    }

    template <typename EventType>
    bool Dispatch(const EventType& event)
    {
        EventHandlerPool<EventType>& pool{
            std::get<EventHandlerPool<EventType>>(m_eventHandlers)
        };

        if (pool.size() == 0)
            return false;

        bool isHandled{ true };
        for (const RegisteredEventHandler<EventType>& listener : pool)
        {
            isHandled &= listener.eventHandler(event);
        }

        return isHandled;
    }

private:
    std::tuple<EventHandlerPool<EventTypes>...> m_eventHandlers;
};
}
