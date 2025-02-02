#pragma once

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
public:
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

    template <typename... Args>
    Type& Emplace(const Entity entity, Args&&... args)
    {
        SparseSet::Push(entity);
        return m_components.emplace_back(std::forward<Args>(args)...);
    }

    Type& Patch(const Entity entt, std::function<Type>&& func)
    {
        // const auto idx = Index(entt);
        auto& elem = Get(entt); // components(idx);
        (func)(elem);
        return elem;
    }

    void Erase(const Entity entity)
    {
        auto index = Index(entity);
        Pop(entity);

        std::swap(m_components[index], m_components[m_components.size() - 1]);
        m_components.pop_back();
    }

    Type& Get(const Entity entity)
    {
        const auto index = Index(entity);
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

private:
    std::vector<Type> m_components;
};
}
