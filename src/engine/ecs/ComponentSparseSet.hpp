#pragma once

#include "SparseSet.hpp"

#include <functional>

namespace Zeus
{
template <typename Type>
class ComponentSparseSet : public SparseSet
{
public:
    ComponentSparseSet() : SparseSet(), m_components{}
    {
    }

    void Push(Entity entity) override
    {
        SparseSet::Push(entity);
        m_components.push_back({});
    }

    Type& Emplace(const Entity entity, Type&& component)
    {
        Push(entity);
        return m_components.emplace_back(component);
    }

    template <typename... Args>
    Type& Emplace(const Entity entity, Args&&... args)
    {
        return Emplace(entity, Type{ std::forward<Args>(args)... });
    }

    Type& Patch(const Entity entt, std::function<Type>&& func)
    {
        // const auto idx = Index(entt);
        auto& elem = Get(entt); // components(idx);
        (func)(elem);
        return elem;
    }

    void Erase(Entity entity)
    {
        auto index = Index(entity);
        Pop(entity);

        std::swap(m_components[index], m_components[m_components.size() - 1]);
        m_components.pop_back();
    }

    Type& Get(Entity entity)
    {
        const auto index = Index(entity);
        return m_components[index];
    }

    const Type* Componenets() const
    {
        return m_components.data();
    }

private:
    std::vector<Type> m_components;
};
}
