#pragma once

#include "ecs/ComponentSparseSet.hpp"
#include "ecs/Entity.hpp"
#include "ecs/SparseSet.hpp"

#include <array>
#include <cstddef>
#include <tuple>

namespace Zeus::ECS
{
template <typename... Components>
class QueryIterator
{
public:
    QueryIterator(
        std::array<SparseSet*, sizeof...(Components)> pools,
        std::size_t minIndex,
        SparseSet::iterator current)
        : m_pools{ pools },
          m_minIndex{ minIndex },
          m_current{ current }
    {
    }

    constexpr QueryIterator& operator++() noexcept
    {
        ++m_current;
        return SeekNext(), *this;
    }

    constexpr QueryIterator operator++(int) noexcept
    {
        const QueryIterator orig{ *this };
        return ++(*this), orig;
    }

    [[nodiscard]] std::tuple<Components...> operator*() const noexcept
    {
        return std::make_tuple(
            reinterpret_cast<ComponentSparseSet<Components>*>(
                m_pools[m_minIndex])
                ->Get(*m_current)...);
    }

    constexpr bool operator==(const QueryIterator& other) const noexcept
    {
        return m_current == other.m_current;
    }

private:
    constexpr void SeekNext() noexcept
    {
        for (SparseSet::iterator end{}; m_current != end && AllOf(*m_current);
             ++m_current)
            ;
    }

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
    std::size_t m_minIndex;
    SparseSet::iterator m_current;
};
}
