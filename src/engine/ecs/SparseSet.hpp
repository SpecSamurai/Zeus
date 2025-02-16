#pragma once

#include "Entity.hpp"
#include "SparseSetIterator.hpp"

#include <cassert>
#include <cstddef>
#include <vector>

namespace Zeus::ECS
{
class SparseSet
{
private:
    using Container = std::vector<Entity>;

public:
    using size_type = typename Container::size_type;
    using iterator = SparseSetIterator<Container>;

    SparseSet(std::size_t maxEntity = 0);
    SparseSet(SparseSet&& other) noexcept;

    virtual ~SparseSet() = default;

    SparseSet(const SparseSet&) = delete;
    SparseSet& operator=(const SparseSet&) = delete;

    SparseSet& operator=(SparseSet&& other);

    virtual void Push(Entity entity);
    virtual void Pop(Entity entity);

    std::size_t Index(const Entity entity) const;
    bool Contains(const Entity entity) const;
    virtual void Reserve(std::size_t capacity);
    void Clear();

    std::size_t Capacity() const;
    bool Empty() const;
    const Entity* Data() const;
    std::size_t Size() const;

    static_assert(std::random_access_iterator<iterator>);
    [[nodiscard]] iterator begin() const noexcept;
    [[nodiscard]] iterator end() const noexcept;

private:
    static constexpr size_type SPARSE_PAGE_SIZE{ 4096 };

    Container m_sparse;
    Container m_dense;
    size_type m_size;
};
}
