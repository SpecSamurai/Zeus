#pragma once

#include <memory>

namespace Zeus::ECS
{
template <typename Container>
class ComponentSparseSetIterator final
{
public:
    using value_type = typename Container::value_type;
    using pointer = typename Container::const_pointer;
    using const_reference = typename Container::const_reference;
    using difference_type = typename Container::difference_type;
    using size_type = typename Container::size_type;

    constexpr ComponentSparseSetIterator() noexcept : m_components{}, m_index{}
    {
    }

    constexpr ComponentSparseSetIterator(
        const Container& components,
        const size_type index) noexcept
        : m_components{ &components },
          m_index{ index }
    {
    }

    constexpr ComponentSparseSetIterator& operator++() noexcept
    {
        return ++m_index, *this;
    }

    constexpr ComponentSparseSetIterator operator++(int) noexcept
    {
        const ComponentSparseSetIterator copy{ *this };
        return operator++(), copy;
    }

    constexpr ComponentSparseSetIterator& operator+=(
        const difference_type diff) noexcept
    {
        m_index += static_cast<size_type>(diff);
        return *this;
    }

    constexpr ComponentSparseSetIterator operator+(
        const difference_type diff) const noexcept
    {
        ComponentSparseSetIterator copy{ *this };
        return (copy += diff);
    }

    constexpr ComponentSparseSetIterator& operator--() noexcept
    {
        return --m_index, *this;
    }

    constexpr ComponentSparseSetIterator operator--(int) noexcept
    {
        const ComponentSparseSetIterator copy{ *this };
        return operator--(), copy;
    }

    constexpr ComponentSparseSetIterator& operator-=(
        const difference_type diff) noexcept
    {
        m_index -= static_cast<size_type>(diff);
        return *this;
    }

    constexpr ComponentSparseSetIterator operator-(
        const difference_type diff) const noexcept
    {
        ComponentSparseSetIterator copy{ *this };
        return (copy -= diff);
    }

    [[nodiscard]] friend constexpr ComponentSparseSetIterator<
        Container>::difference_type
    operator-(
        const ComponentSparseSetIterator<Container>& left,
        const ComponentSparseSetIterator<Container>& right) noexcept
    {
        return static_cast<
            ComponentSparseSetIterator<Container>::difference_type>(
            left.m_index - right.m_index);
    }

    [[nodiscard]] constexpr const_reference operator[](
        const difference_type diff) const noexcept
    {
        return (*m_components)[m_index + static_cast<size_type>(diff)];
    }

    [[nodiscard]] constexpr const_reference operator*() const noexcept
    {
        return (*m_components)[m_index];
    }

    [[nodiscard]] constexpr pointer operator->() const noexcept
    {
        return std::addressof(operator[](0));
    }

    constexpr bool operator==(
        const ComponentSparseSetIterator& other) const noexcept
    {
        return m_index == other.m_index;
    }

    constexpr bool operator<(
        const ComponentSparseSetIterator& other) const noexcept
    {
        return m_index < other.m_index;
    }

    constexpr bool operator>(
        const ComponentSparseSetIterator& other) const noexcept
    {
        return m_index > other.m_index;
    }

    constexpr bool operator<=(
        const ComponentSparseSetIterator& other) const noexcept
    {
        return m_index <= other.m_index;
    }

    constexpr bool operator>=(
        const ComponentSparseSetIterator& other) const noexcept
    {
        return m_index >= other.m_index;
    }

private:
    const Container* m_components;
    size_type m_index;
};

template <typename Container>
constexpr ComponentSparseSetIterator<Container> operator+(
    typename ComponentSparseSetIterator<Container>::difference_type diff,
    const ComponentSparseSetIterator<Container>& iterator) noexcept
{
    return iterator + diff;
}
}
