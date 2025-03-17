#pragma once

#include "ComponentSparseSet.hpp"
#include "Entity.hpp"
#include "FamilyId.hpp"
#include "Query.hpp"
#include "SparseSet.hpp"

#include <array>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace Zeus::ECS
{
class Registry
{
public:
    Registry() : pools{}, entities()
    {
    }

    Entity Create()
    {
        Entity newEntity{ lastEntity++ };
        entities.Push(newEntity);
        return newEntity;
    }

    void Destroy(const Entity entity)
    {
        for (auto& pool : pools)
        {
            pool.second->Pop(entity);
        }

        entities.Pop(entity);
    }

    void Clear()
    {
        for (auto& pool : pools)
        {
            pool.second->Clear();
        }

        entities.Clear();
    }

    template <typename Component, typename... Args>
    decltype(auto) Emplace(const Entity entity, Args&&... args)
    {
        if (!entities.Contains(entity))
            entities.Push(entity);

        Family family{ FamilyId::Type<Component>() };

        if (!pools.contains(family))
        {
            pools[family] = new ComponentSparseSet<Component>();
        }

        auto pool{ static_cast<ComponentSparseSet<Component>*>(pools[family]) };

        return pool->Emplace(entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    decltype(auto) Patch(
        const Entity entity,
        std::function<void(Component&)>&& func)
    {
        ComponentSparseSet<Component>* pool{ TryGetPool<Component>() };
        assert(pool != nullptr && "Component not added to the pools");

        return pool->Patch(
            entity,
            std::forward<std::function<void(Component&)>>(func));
    }

    template <typename... Components>
    void Erase(const Entity entity)
    {
        static_assert(sizeof...(Components) > 0);
        if constexpr (sizeof...(Components) == 1u)
        {
            auto* pool{ TryGetPool<Components...>() };
            if (pool)
                pool->Pop(entity);
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
            return (TryGetPool<Components>()->Get(entity), ...);
        }
        else
        {
            return std::forward_as_tuple(
                TryGetPool<Components>()->Get(entity)...);
        }
    }

    template <typename... Components>
    decltype(auto) GetQuery()
    {
        static_assert(sizeof...(Components) > 0);
        return Query<Components...>(TryGetPool<Components>()...);
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
            auto* pool{ TryGetPool<Components...>() };
            return pool && pool->Contains(entity);
        }
        else
        {
            return (AllOf<Components>(entity) && ...);
        }
    }

    bool IsValid(const Entity entity)
    {
        return entities.Contains(entity);
    }

private:
    template <typename Component>
    ComponentSparseSet<Component>* TryGetPool()
    {
        Family family{ FamilyId::Type<Component>() };
        return pools.contains(family)
                   ? static_cast<ComponentSparseSet<Component>*>(pools[family])
                   : nullptr;
    }

private:
    Entity lastEntity{};

    std::unordered_map<std::uint32_t, SparseSet*> pools;
    SparseSet entities;
};
}
