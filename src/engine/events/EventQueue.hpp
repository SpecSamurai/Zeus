#pragma once

#include "EventHandler.hpp"

#include <tuple>
#include <vector>

namespace Zeus
{
template <typename... EventTypes>
class EventQueue
{
private:
    template <typename EventType>
    struct RegisteredEventHandler
    {
        const char* name;
        EventHandler<EventType> eventHandler;
    };

    template <typename EventType>
    using EventHandlerPool = std::vector<RegisteredEventHandler<EventType>>;

    template <typename EventType>
    using EventPool = std::vector<EventType>;

public:
    EventQueue(std::uint64_t handlersCapacity, std::uint64_t eventsCapacity)
    {
        (std::get<EventHandlerPool<EventTypes>>(m_eventHandlers)
             .reserve(handlersCapacity),
         ...);

        (std::get<EventPool<EventTypes>>(m_eventPools).reserve(eventsCapacity),
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
    void Publish(EventType&& event)
    {
        EventPool<EventType>& pool{ std::get<EventPool<EventType>>(
            m_eventPools) };

        pool.emplace_back(std::forward<EventType>(event));
    }

    void Dispatch()
    {
        (Dispatch<EventTypes>(), ...);
    }

    template <typename EventType>
    void Dispatch()
    {
        EventPool<EventType>& eventPool{ std::get<EventPool<EventType>>(
            m_eventPools) };

        if (eventPool.size() == 0)
            return;

        EventHandlerPool<EventType>& handlerPool{
            std::get<EventHandlerPool<EventType>>(m_eventHandlers)
        };

        for (const RegisteredEventHandler<EventType>& handler : handlerPool)
        {
            for (const EventType& event : eventPool)
            {
                handler.eventHandler(event);
            }
        }
    }

private:
    std::tuple<EventHandlerPool<EventTypes>...> m_eventHandlers;
    std::tuple<EventPool<EventTypes>...> m_eventPools;
};
}
