#pragma once

#include "ComponentSparseSetIterator.hpp"
#include "Entity.hpp"
#include "SparseSet.hpp"

#include <algorithm>
#include <functional>
#include <utility>

namespace Zeus::ECS
{
template <typename Type>
class ComponentSparseSet : public SparseSet
{
private:
    using Container = std::vector<Type>;

public:
    using size_type = typename Container::size_type;
    using iterator = ComponentSparseSetIterator<Container>;

    ComponentSparseSet(std::size_t maxEntity = 0)
        : SparseSet(maxEntity),
          m_components{}
    {
        if (maxEntity > 0)
            Reserve(maxEntity);
    }

    ComponentSparseSet(ComponentSparseSet&& other) noexcept
        : SparseSet(static_cast<SparseSet&&>(other)),
          m_components{ std::move(other.m_components) }
    {
    }

    ComponentSparseSet& operator=(ComponentSparseSet&& other)
    {
        if (this != &other)
        {
            static_cast<SparseSet&>(*this) = std::move(other);
            m_components = std::move(other.m_components);
        }

        return *this;
    }

    void Push(const Entity entity) override
    {
        SparseSet::Push(entity);
        m_components.push_back({});
    }

    void Pop(const Entity entity) override
    {
        auto index{ Index(entity) };
        SparseSet::Pop(entity);

        std::swap(m_components[index], m_components[m_components.size() - 1]);
        m_components.pop_back();
    }

    template <typename... Args>
    decltype(auto) Emplace(const Entity entity, Args&&... args)
    {
        SparseSet::Push(entity);
        return m_components.emplace_back(std::forward<Args>(args)...);
    }

    decltype(auto) Patch(const Entity entity, std::function<void(Type&)>&& func)
    {
        auto& elem{ Get(entity) };
        (func)(elem);
        return elem;
    }

    decltype(auto) Get(const Entity entity)
    {
        const auto index{ Index(entity) };
        return m_components[index];
    }

    void Reserve(std::size_t capacity) override
    {
        SparseSet::Reserve(capacity);
        m_components.reserve(capacity);
    }

    const Type* Componenets() const
    {
        return m_components.data();
    }

    static_assert(std::random_access_iterator<iterator>);
    [[nodiscard]] iterator begin() const noexcept
    {
        return iterator(m_components, 0);
    }

    [[nodiscard]] iterator end() const noexcept
    {
        return iterator(m_components, m_components.size());
    }

private:
    Container m_components;
};
}
