#pragma once

#include "ecs/ComponentSparseSet.hpp"
#include "ecs/Entity.hpp"
#include "ecs/SparseSet.hpp"

#include <array>
#include <cstddef>
#include <functional>
#include <utility>

namespace Zeus::ECS
{
template <typename... Components>
class Query
{
public:
    using iterator = Query<Components...>;

    Query(std::array<SparseSet*, sizeof...(Components)> pools)
        : m_pools{ pools },
          m_minIndex{ sizeof...(Components) }
    {
        Refresh();
    }

    Query(ComponentSparseSet<Components>*... pool)
        : m_pools{ pool... },
          m_minIndex{ sizeof...(Components) }
    {
        Refresh();
    }

    void Refresh()
    {
        m_minIndex = 0;

        for (std::size_t i{ 1 }; i < m_pools.size(); ++i)
        {
            if (m_pools[i]->Size() < m_pools[m_minIndex]->Size())
            {
                m_minIndex = i;
            }
        }
    }

    void Each(std::function<void(Components&...)>&& function)
    {
        for (auto entity : *m_pools[m_minIndex])
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

    [[nodiscard]] iterator begin() const noexcept
    {
        return iterator(*m_pools[m_minIndex]->begin(), m_pools, m_minIndex);
    }

    [[nodiscard]] iterator end() const noexcept
    {
        return iterator(*(m_pools[m_minIndex]->end() - 1), m_pools, m_minIndex);
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
    void Each(
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
    std::size_t m_minIndex;
};
}
