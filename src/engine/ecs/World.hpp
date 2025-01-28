#pragma once

#include "Entity.hpp"

#include <array>
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
// Sparse Set
// Entity LifeCycle
// Grouping and filtering
// Observe changes
// A tombstone is a marker or reserved value used to represent an entity that
// has been destroyed or is no longer valid in an entity-component system (ECS).

class IStorage
{
};

template <typename Type>
class Storage : IStorage
{
};

class World
{
public:
    inline Entity Create()
    {
        Entity newEntity{}; // Generator::Generate() };
        entities.insert(newEntity);
        return newEntity;
    }

    inline void Destroy(Entity entity)
    {
        entities.erase(entity);
    }

    // clear()

    template <typename Component, typename... Args>
    inline Component Emplace(Entity entity, Args&&... args)
    {
        auto component = Component{ std::forward<Args>(args)... };
        return component;
    }

    template <typename Component>
    inline Component Emplace(Entity entity, Component&& component)
    {
        return component;
    }

    // replace<position>(entity, 0., 0.);
    // emplace_or_replace<position>(entity, 0., 0.);

    // erase componenets
    // erase<position>(entity);

    template <typename... Components>
    inline decltype(auto) Get(Entity entity)
    {
        if constexpr (sizeof...(Components) == 1u)
        {
            return (Components{}, ...); //(Get<Components>(entity), ...);
        }
        else
        {
            return std::forward_as_tuple(Get<Components>(entity)...);
        }
    }

    // try_get

    // iterator
    template <typename... Components>
    inline std::array<Entity, 10> Query()
    {
    }

    template <typename... Components>
    inline bool AnyOf(Entity entity)
    {
    }

    template <typename... Components>
    inline bool AllOf(Entity entity)
    {
    }

    inline bool IsValid(Entity entity)
    {
        return entities.contains(entity);
    }

private:
    Entity lastEntity{};

    std::unordered_map<std::uint32_t, std::vector<IStorage>> components;
    std::unordered_set<Entity> entities;
};
}
