#pragma once

#include "ecs/Entity.hpp"
#include "ecs/SparseSet.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>

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

    void Each(std::function<void(Components...)>&& function)
    {
        auto i = m_pools[minIndex]->SparseSet::begin();

        auto e = *i;

        for (auto entity : *m_pools[minIndex])
        {
            if (AllOf(entity))
            {
                /*auto tuple = ;*/
                function(m_pools[0], 0);
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

private:
    std::array<SparseSet*, sizeof...(Components)> m_pools;
    std::size_t minIndex;
};
}
