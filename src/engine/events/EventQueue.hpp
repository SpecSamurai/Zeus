#pragma once

#include <tuple>
#include <vector>

namespace Zeus
{
template <typename... Type>
class EventQueue
{
public:
    EventQueue()
    {
        (std::get<std::vector<Type>>(m_eventPools).reserve(10), ...);
    }

    template <typename T, typename... Args>
    void Publish(Args&&... args)
    {
        T event(std::forward<Args>(args)...);
        std::vector<T>& pool{ std::get<std::vector<T>>(m_eventPools) };

        pool.emplace_back(event);
    }

    template <typename T>
    void Publish(const T& event)
    {
        std::get<std::vector<T>>(m_eventPools).push_back(event);
    }

    template <typename T>
    void Dispatch()
    {
        auto pool{ std::get<std::vector<T>>(m_eventPools) };

        // for (auto& event)
    }

private:
    std::tuple<std::vector<Type>...> m_eventPools;
};
}
