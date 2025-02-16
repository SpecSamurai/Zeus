#include "SparseSet.hpp"

#include "Entity.hpp"

#include <cassert>
#include <cstddef>
#include <utility>

namespace Zeus::ECS
{
SparseSet::SparseSet(std::size_t maxEntity) : m_sparse{}, m_dense{}, m_size{ 0 }
{
    if (maxEntity > 0)
        Reserve(maxEntity);
}

SparseSet::SparseSet(SparseSet&& other) noexcept
    : m_sparse{ std::move(other.m_sparse) },
      m_dense{ std::move(other.m_dense) },
      m_size{ other.m_size }
{
}

SparseSet& SparseSet::operator=(SparseSet&& other)
{
    if (this != &other)
    {
        m_sparse = std::move(other.m_sparse);
        m_dense = std::move(other.m_dense);
        m_size = other.m_size;
    }

    return *this;
}

void SparseSet::Push(Entity entity)
{
    assert(!Contains(entity) && "Set contains entity");

    if (entity >= m_sparse.size())
    {
        const auto page{ (entity / SPARSE_PAGE_SIZE) + 1u };
        m_sparse.resize(page * SPARSE_PAGE_SIZE);
    }

    m_dense.push_back(entity);
    m_sparse[entity] = static_cast<Entity>(m_size);

    ++m_size;
}

void SparseSet::Pop(Entity entity)
{
    assert(Contains(entity) && "Set does not contain entity");

    m_dense[m_sparse[entity]] = m_dense[m_size - 1];
    m_sparse[m_dense[m_size - 1]] = m_sparse[entity];

    m_dense.pop_back();
    --m_size;
}

std::size_t SparseSet::Index(const Entity entity) const
{
    assert(Contains(entity) && "Set does not contain entity");

    return m_sparse[entity];
}

bool SparseSet::Contains(const Entity entity) const
{
    assert(
        entity >= 0 && entity < std::numeric_limits<Entity>::max() &&
        "Entity is out of bounds");

    return entity < m_sparse.size() && m_sparse[entity] >= 0 &&
           m_sparse[entity] < m_size && m_dense[m_sparse[entity]] == entity;
}

void SparseSet::Reserve(std::size_t capacity)
{
    assert(capacity > m_dense.capacity() && "Invalid capacity");
    m_dense.reserve(capacity);
}

void SparseSet::Clear()
{
    m_size = 0;
}

std::size_t SparseSet::Capacity() const
{
    return m_dense.capacity();
}

bool SparseSet::Empty() const
{
    return m_size == 0;
}

const Entity* SparseSet::Data() const
{
    return m_dense.data();
}

std::size_t SparseSet::Size() const
{
    return m_size;
}

[[nodiscard]] SparseSet::iterator SparseSet::begin() const noexcept
{
    return iterator(m_dense, 0);
}

[[nodiscard]] SparseSet::iterator SparseSet::end() const noexcept
{
    return iterator(m_dense, m_size);
}
}
