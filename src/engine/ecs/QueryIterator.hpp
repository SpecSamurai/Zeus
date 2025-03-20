#pragma once

#include "ecs/Entity.hpp"
#include "ecs/SparseSet.hpp"

#include <cstddef>

namespace Zeus::ECS
{
template <typename Container>
class QueryIterator
{
public:
    using value_type = typename Container::value_type;
    using pointer = typename Container::const_pointer;
    using const_reference = typename Container::const_reference;
    using difference_type = typename Container::difference_type;
    using size_type = typename Container::size_type;

    constexpr QueryIterator() noexcept
        : m_pools{},
          m_current{},
          m_minIndex{ m_pools->size() }
    {
    }

    constexpr QueryIterator(
        SparseSet::iterator first,
        const Container& pools,
        const size_type minIndex) noexcept
        : m_pools{ &pools },
          m_current{ first },
          m_minIndex{ minIndex }
    {
    }

    constexpr QueryIterator& operator++() noexcept
    {
        /*return ++m_index, *this;*/
    }

    constexpr QueryIterator operator++(int) noexcept
    {
        const SparseSetIterator copy{ *this };
        return operator++(), copy;
    }

    constexpr QueryIterator& operator+=(const difference_type diff) noexcept
    {
        /*m_index += static_cast<size_type>(diff);*/
        return *this;
    }

    constexpr QueryIterator operator+(const difference_type diff) const noexcept
    {
        SparseSetIterator copy{ *this };
        return (copy += diff);
    }

    constexpr QueryIterator& operator--() noexcept
    {
        /*return --m_index, *this;*/
    }

    constexpr QueryIterator operator--(int) noexcept
    {
        const SparseSetIterator copy{ *this };
        return operator--(), copy;
    }

    constexpr QueryIterator& operator-=(const difference_type diff) noexcept
    {
        /*m_index -= static_cast<size_type>(diff);*/
        return *this;
    }

    constexpr QueryIterator operator-(const difference_type diff) const noexcept
    {
        SparseSetIterator copy{ *this };
        return (copy -= diff);
    }

    [[nodiscard]] friend constexpr SparseSetIterator<Container>::difference_type
    operator-(
        const SparseSetIterator<Container>& left,
        const SparseSetIterator<Container>& right) noexcept
    {
        return static_cast<SparseSetIterator<Container>::difference_type>(
            left.m_index - right.m_index);
    }

    [[nodiscard]] constexpr const_reference operator[](
        const difference_type diff) const noexcept
    {
        /*return (*m_dense)[m_index + static_cast<size_type>(diff)];*/
    }

    [[nodiscard]] constexpr const_reference operator*() const noexcept
    {
        /*return (*m_dense)[m_index];*/
    }

    [[nodiscard]] constexpr pointer operator->() const noexcept
    {
        return std::addressof(operator[](0));
    }

    constexpr bool operator==(const QueryIterator& other) const noexcept
    {
        /*return m_index == other.m_index;*/
    }

    constexpr bool operator<(const QueryIterator& other) const noexcept
    {
        /*return m_index < other.m_index;*/
    }

    constexpr bool operator>(const QueryIterator& other) const noexcept
    {
        /*return m_index > other.m_index;*/
    }

    constexpr bool operator<=(const QueryIterator& other) const noexcept
    {
        /*return m_index <= other.m_index;*/
    }

    constexpr bool operator>=(const QueryIterator& other) const noexcept
    {
        /*return m_index >= other.m_index;*/
    }

private:
    bool AllOf(Entity entity)
    {
        for (std::size_t i{ 0 }; i < m_pools->size(); ++i)
        {
            if (!m_pools[i]->Contains(entity))
                return false;
        }

        return true;
    }

    void Next()
    {
        for (constexpr SparseSet::iterator end{ m_pools[m_minIndex]->end() };
             m_current != end && AllOf(*m_current);
             ++m_current)
        {
        }
    }

private:
    const Container* m_pools;
    SparseSet::iterator m_current;
    std::size_t m_minIndex;
};

template <typename Container>
constexpr QueryIterator<Container> operator+(
    typename QueryIterator<Container>::difference_type diff,
    const QueryIterator<Container>& iterator) noexcept
{
    return iterator + diff;
}
}
