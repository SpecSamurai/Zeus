#pragma once

#include "ComponentSparseSet.hpp"
#include "Entity.hpp"
#include "FamilyId.hpp"
#include "Query.hpp"
#include "SparseSet.hpp"

#include <cassert>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace Zeus::ECS
{
class Registry
{
public:
    Registry() : m_pools{}, m_entities()
    {
    }

    Entity Create()
    {
        Entity newEntity{ m_lastEntity++ };
        m_entities.Push(newEntity);
        return newEntity;
    }

    template <typename Component, typename... Args>
    Entity Create(Args&&... args)
    {
        Entity newEntity{ Create() };
        Emplace<Component>(newEntity, std::forward<Args>(args)...);

        return newEntity;
    }

    void Destroy(const Entity entity)
    {
        for (auto& pool : m_pools)
        {
            pool.second->Pop(entity);
        }

        m_entities.Pop(entity);
    }

    void Clear()
    {
        for (auto& pool : m_pools)
        {
            pool.second->Clear();
        }

        m_entities.Clear();
    }

    template <typename Component, typename... Args>
    decltype(auto) Emplace(const Entity entity, Args&&... args)
    {
        if (!m_entities.Contains(entity))
            m_entities.Push(entity);

        Family family{ FamilyId::Type<Component>() };

        if (!m_pools.contains(family))
        {
            m_pools[family] = new ComponentSparseSet<Component>();
        }

        return GetPool<Component>()->Emplace(
            entity,
            std::forward<Args>(args)...);
    }

    template <typename Component>
    decltype(auto) Patch(
        const Entity entity,
        std::function<void(Component&)>&& func)
    {
        return GetPool<Component>()->Patch(
            entity,
            std::forward<std::function<void(Component&)>>(func));
    }

    template <typename... Components>
    void Erase(const Entity entity)
    {
        static_assert(sizeof...(Components) > 0);
        if constexpr (sizeof...(Components) == 1u)
        {
            GetPool<Components...>()->Pop(entity);
        }
        else
        {
            (Erase<Components>(entity), ...);
        }
    }

    template <typename... Components>
    [[nodiscard]] decltype(auto) Get(const Entity entity)
    {
        static_assert(sizeof...(Components) > 0);
        if constexpr (sizeof...(Components) == 1u)
        {
            return (GetPool<Components>()->Get(entity), ...);
        }
        else
        {
            return std::forward_as_tuple(GetPool<Components>()->Get(entity)...);
        }
    }

    template <typename... Components>
    decltype(auto) QueryAll()
    {
        static_assert(sizeof...(Components) > 0);
        return Query<Components...>(GetPool<Components>()...);
    }

    template <typename... Components>
    bool AnyOf(const Entity entity)
    {
        return (AllOf<Components>(entity) || ...);
    }

    template <typename... Components>
    bool AllOf(const Entity entity)
    {
        static_assert(sizeof...(Components) > 0);
        if constexpr (sizeof...(Components) == 1u)
        {
            return GetPool<Components...>()->Contains(entity);
        }
        else
        {
            return (AllOf<Components>(entity) && ...);
        }
    }

    bool IsValid(const Entity entity)
    {
        return m_entities.Contains(entity);
    }

private:
    template <typename Component>
    ComponentSparseSet<Component>* GetPool()
    {
        Family family{ FamilyId::Type<Component>() };

        if (!m_pools.contains(family))
        {
            m_pools[family] = new ComponentSparseSet<Component>();
        }

        return static_cast<ComponentSparseSet<Component>*>(m_pools[family]);
    }

private:
    Entity m_lastEntity{};

    std::unordered_map<std::uint32_t, SparseSet*> m_pools;
    SparseSet m_entities;
};
}
