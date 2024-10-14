#pragma once

#include "Event.hpp"

#include <algorithm>
#include <tuple>
#include <vector>

namespace Zeus
{
template <typename... EventTypes>
class EventDispatcher
{
    template <typename T>
    using EventListenerPool = std::vector<EventListener<T>>;

public:
    EventDispatcher(std::uint64_t poolCapacity)
    {
        (std::get<EventListenerPool<EventTypes>>(m_listenerPools)
             .reserve(poolCapacity),
         ...);
    }

    template <typename EventType>
    void Register(EventListener<EventType> listener)
    {
        EventListenerPool<EventType>& pool{
            std::get<EventListenerPool<EventType>>(m_listenerPools)
        };

        pool.emplace_back(listener);
    }

    template <typename EventType>
    void Unregister(EventListener<EventType> listener)
    {
        EventListenerPool<EventType>& pool{
            std::get<EventListenerPool<EventType>>(m_listenerPools)
        };

        auto item{ std::find(pool.begin(), pool.end(), listener) };
        if (item != pool.end())
        {
            pool.erase(item);
        }
    }

    template <typename EventType>
    bool Dispatch(const EventType& event)
    {
        EventListenerPool<EventType>& pool{
            std::get<EventListenerPool<EventType>>(m_listenerPools)
        };

        bool isHandeled{ true };
        for (EventListener<EventType> listener : pool)
        {
            isHandeled &= listener(event);
        }

        return isHandeled;
    }

private:
    std::tuple<EventListenerPool<EventTypes>...> m_listenerPools;
};
}
