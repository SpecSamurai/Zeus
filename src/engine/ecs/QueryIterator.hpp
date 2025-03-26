#pragma once

#include "ecs/ComponentSparseSet.hpp"
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
    constexpr QueryIterator(
        const std::array<SparseSet*, sizeof...(Components)>& pools,
        std::size_t minIndex,
        SparseSet::iterator current)
        : m_pools{ pools },
          m_minIndex{ minIndex },
          m_current{ current }
    {
    }

    constexpr QueryIterator& operator++() noexcept
    {
        return ++m_current, SeekNext(), *this;
    }

    constexpr QueryIterator operator++(int) noexcept
    {
        const QueryIterator copy{ *this };
        return operator++(), copy;
    }

    [[nodiscard]] constexpr decltype(auto) operator*() const noexcept
    {
        return Get(std::index_sequence_for<Components...>{});
    }

    constexpr bool operator==(const QueryIterator& other) const noexcept
    {
        return m_current == other.m_current;
    }

private:
    template <std::size_t... Index>
    constexpr decltype(auto) Get(std::index_sequence<Index...>) const noexcept
    {
        static_assert(sizeof...(Index) > 0);

        if constexpr (sizeof...(Index) == 1)
            return (
                reinterpret_cast<ComponentSparseSet<Components>*>(
                    m_pools[Index])
                    ->Get(*m_current),
                ...);
        else
            return std::forward_as_tuple(
                reinterpret_cast<ComponentSparseSet<Components>*>(
                    m_pools[Index])
                    ->Get(*m_current)...);
    }

    constexpr void SeekNext() noexcept
    {
        for (SparseSet::iterator end{ m_pools[m_minIndex]->end() };
             m_current != end && !IsValid();
             ++m_current)
            ;
    }

    constexpr bool IsValid()
    {
        for (std::size_t i{ 0 }; i < m_pools.size(); ++i)
        {
            if (!m_pools[i]->Contains(*m_current))
                return false;
        }

        return true;
    }

private:
    const std::array<SparseSet*, sizeof...(Components)>& m_pools;
    std::size_t m_minIndex;
    SparseSet::iterator m_current;
};
}
