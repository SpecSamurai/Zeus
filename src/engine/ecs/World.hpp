#pragma once

#include "Entity.hpp"
#include "ecs/ComponentSparseSet.hpp"
#include "ecs/FamilyId.hpp"
#include "ecs/SparseSet.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Zeus::ECS
{
// TODO:
// - Versions
// - Systems
// - Archetype
// Query - iterator
// Entity LifeCycle
// Grouping and filtering
// Observe changes
// A tombstone is a marker or reserved value used to represent an entity that
// has been destroyed or is no longer valid in an entity-component system (ECS).

class World
{
public:
    World() : pools{}, entities()
    {
    }

    Entity Create()
    {
        static std::uint32_t i{ 0 };
        Entity newEntity{ lastEntity++ }; // Generator::Generate() };
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

    // emplace_or_replace<position>(entity, 0., 0.);
    // erase componenets
    // erase<position>(entity);
    template <typename... Components>
    void Erase(Entity entity)
    {
        /*Family family{ FamilyId::Type<Component>() };*/
        /**/
        /*ComponentSparseSet<Component> pool{ pools[family] };*/
        /**/
        /*pool.Pop(entity);*/

        if constexpr (sizeof...(Components) == 1u)
        {

            Family family{ FamilyId::Type<Components>()... };
            auto& pool{ pools[family] };
            /*auto* cpool = assure<std::remove_const_t<Type>...>();*/
            /*return cpool && cpool->contains(entt);*/
            // ComponentSparseSet<Component>* pool{ GetPool<Component>() };
            pool->Contains(entity);
        }
        else
        {
            return (AllOf<Components>(entity) && ...);
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

    // try_get

    // iterator
    template <typename... Components>
    std::array<Entity, 10> Query()
    {
    }

    // auto view = registry.view<a_component, another_component>();
    // registry.destroy(view.begin(), view.end());

    template <typename... Components>
    bool AnyOf(Entity entity)
    {
        return (AllOf<Components>(entity) || ...);
    }

    template <typename... Components>
    bool AllOf(Entity entity)
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

    bool IsValid(Entity entity)
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
