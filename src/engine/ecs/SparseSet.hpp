#pragma once

#include "Entity.hpp"

#include <cassert>
#include <cstddef>
#include <vector>

namespace Zeus::ECS
{
class SparseSet
{
public:
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

private:
    static constexpr std::size_t SPARSE_PAGE_SIZE{ 4096 };

    std::vector<Entity> m_sparse;
    std::vector<Entity> m_dense;
    std::size_t m_size;
};
}
