#pragma once

#include <memory>

namespace Zeus::ECS
{
template <typename Container>
class SparseSetIterator final
{
public:
    // using iterator_category = std::random_access_iterator_tag;
    using value_type = typename Container::value_type;
    using pointer = typename Container::pointer;
    using const_reference = typename Container::const_reference;
    using difference_type = typename Container::difference_type;
    using size_type = typename Container::size_type;

    constexpr SparseSetIterator() noexcept : m_dense{}, m_index{}
    {
    }

    constexpr SparseSetIterator(
        const Container& dense,
        const size_type index) noexcept
        : m_dense{ &dense },
          m_index{ index }
    {
    }

    constexpr SparseSetIterator& operator++() noexcept
    {
        return ++m_index, *this;
    }

    constexpr SparseSetIterator operator++(int) noexcept
    {
        const SparseSetIterator copy{ *this };
        return operator++(), copy;
    }

    constexpr SparseSetIterator& operator+=(const difference_type diff) noexcept
    {
        m_index += static_cast<size_type>(diff);
        return *this;
    }

    constexpr SparseSetIterator operator+(
        const difference_type diff) const noexcept
    {
        SparseSetIterator copy{ *this };
        return (copy += diff);
    }

    constexpr SparseSetIterator& operator--() noexcept
    {
        return --m_index, *this;
    }

    constexpr SparseSetIterator operator--(int) noexcept
    {
        const SparseSetIterator copy{ *this };
        return operator--(), copy;
    }

    constexpr SparseSetIterator& operator-=(const difference_type diff) noexcept
    {
        m_index -= static_cast<size_type>(diff);
        return *this;
    }

    constexpr SparseSetIterator operator-(
        const difference_type diff) const noexcept
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
        return (*m_dense)[m_index + static_cast<size_type>(diff)];
    }

    [[nodiscard]] constexpr const_reference operator*() const noexcept
    {
        return (*m_dense)[m_index];
    }

    [[nodiscard]] constexpr pointer operator->() const noexcept
    {
        return std::addressof(operator[](0));
    }

    constexpr bool operator==(const SparseSetIterator& other) const noexcept
    {
        return m_index == other.m_index;
    }

    constexpr bool operator<(const SparseSetIterator& other) const noexcept
    {
        return m_index < other.m_index;
    }

    constexpr bool operator>(const SparseSetIterator& other) const noexcept
    {
        return m_index > other.m_index;
    }

    constexpr bool operator<=(const SparseSetIterator& other) const noexcept
    {
        return m_index <= other.m_index;
    }

    constexpr bool operator>=(const SparseSetIterator& other) const noexcept
    {
        return m_index >= other.m_index;
    }

private:
    const Container* m_dense;
    size_type m_index;
};

template <typename Container>
constexpr SparseSetIterator<Container> operator+(
    typename SparseSetIterator<Container>::difference_type diff,
    const SparseSetIterator<Container>& iterator) noexcept
{
    return iterator + diff;
}
}
