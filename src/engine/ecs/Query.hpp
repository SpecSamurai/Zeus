#pragma once

#include "ecs/ComponentSparseSet.hpp"
#include "ecs/Entity.hpp"
#include "ecs/SparseSet.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

namespace Zeus::ECS
{
template <typename... Components>
class Query
{
public:
    Query(std::array<SparseSet*, sizeof...(Components)> pools)
        : m_pools{ pools },
          minIndex{ sizeof...(Components) }
    {
        Refresh();
    }

    Query(ComponentSparseSet<Components>*... pool)
        : m_pools{ pool... },
          minIndex{ sizeof...(Components) }
    {
        Refresh();
    }

    void Refresh()
    {
        minIndex = 0;

        for (std::size_t i{ 1 }; i < m_pools.size(); ++i)
        {
            if (m_pools[i]->Size() < m_pools[minIndex]->Size())
            {
                minIndex = i;
            }
        }
    }

    void Each(std::function<void(Components&...)>&& function)
    {
        for (auto entity : *m_pools[minIndex])
        {
            if (AllOf(entity))
            {
                Each(
                    std::forward<std::function<void(Components & ...)>>(
                        function),
                    entity,
                    std::index_sequence_for<Components...>{});
            }
        }
    }

private:
    bool AllOf(Entity entity)
    {
        for (std::size_t i{ 0 }; i < m_pools.size(); ++i)
        {
            if (!m_pools[i]->Contains(entity))
                return false;
        }

        return true;
    }

    template <std::size_t... Index>
    [[nodiscard]] void Each(
        std::function<void(Components&...)>&& function,
        const Entity entity,
        std::index_sequence<Index...>) const noexcept
    {
        function(
            reinterpret_cast<ComponentSparseSet<Components>*>(m_pools[Index])
                ->Get(entity)...);
    }

private:
    std::array<SparseSet*, sizeof...(Components)> m_pools;
    std::size_t minIndex;
};
}
