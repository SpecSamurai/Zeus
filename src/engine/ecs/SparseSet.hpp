#pragma once

#include "Entity.hpp"

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace Zeus::ECS
{
class SparseSet
{
public:
    SparseSet(std::size_t capacity = 0) : m_sparse{}, m_dense{}, m_size{ 0 }
    {
        if (capacity > 0)
            Reserve(capacity);
    }

    virtual ~SparseSet() = default;

    SparseSet(const SparseSet&) = delete;
    SparseSet& operator=(const SparseSet&) = delete;

    SparseSet(SparseSet&& other) noexcept
        : m_sparse{ std::move(other.m_sparse) },
          m_dense{ std::move(other.m_dense) },
          m_size{ other.m_size }
    {
    }

    SparseSet& operator=(SparseSet&& other)
    {
        if (this != &other)
        {
            m_sparse = std::move(other.m_sparse);
            m_dense = std::move(other.m_dense);
            m_size = other.m_size;
        }

        return *this;
    }

    virtual void Push(Entity entity)
    {
        assert(!Contains(entity) && "Set contains entity");

        if (entity >= m_sparse.size())
        {
            m_sparse.resize(entity + 1u);
        }

        m_dense.push_back(entity);
        // dense[size] = entity;
        m_sparse[entity] = static_cast<Entity>(m_size);

        ++m_size;
    }

    void Pop(Entity entity)
    {
        assert(Contains(entity) && "Set does not contain entity");

        m_dense[m_sparse[entity]] = m_dense[m_size - 1];
        m_sparse[m_dense[m_size - 1]] = m_sparse[entity];

        m_dense.pop_back();
        --m_size;
    }

    void Reserve(std::size_t capacity)
    {
        assert(capacity > m_size && "Invalid capacity");

        m_dense.reserve(capacity);
        m_sparse.reserve(capacity);
    }

    bool Contains(const Entity entity) const
    {
        assert(
            entity >= 0 && entity < std::numeric_limits<Entity>::max() &&
            "Index out of bounds");
        return entity < m_sparse.size() && m_sparse[entity] >= 0 &&
               m_sparse[entity] < m_size && m_dense[m_sparse[entity]] == entity;
    }

    std::size_t Index(const Entity entity) const
    {
        assert(Contains(entity) && "Set does not contain entity");
        return m_sparse[entity];
    }

    void Clear()
    {
        m_size = 0;
    }

    std::size_t Capacity() const
    {
        return m_dense.capacity();
    }

    bool Empty() const
    {
        return m_size == 0;
    }

    const Entity* Data() const
    {
        return m_dense.data();
    }

    std::size_t Size() const
    {
        return m_size;
    }

private:
    std::vector<Entity> m_sparse;
    std::vector<Entity> m_dense;
    std::size_t m_size;
};
}
