#pragma once

#include "Config.hpp"
#include "Component.hpp"
#include "Pool.hpp"
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <memory>
#include <string>
#include <cstdint>

namespace entity
{

class World;
class EntityManager;

// Basically just an id.
class Entity
{
public:
    using Id = uint32_t;
    using Version = uint8_t;

    /*
        Id = index + version (kinda).
    */
    Entity(Id index = 0, Version version = 0) { id = (version << IndexBits) | index; }

    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;

    /*
        Comparison operators.
    */
    bool operator==(const Entity &e) const { return GetIndex() == e.GetIndex(); }
    bool operator!=(const Entity &e) const { return GetIndex() != e.GetIndex(); }
    bool operator<(const Entity &e) const { return GetIndex() < e.GetIndex(); }

    /*
        Returns the index part of the id.
    */
    Id GetIndex() const { return id & IndexMask; }

    /*
        Returns the version part of the id.
    */
    Version GetVersion() const { return (id >> IndexBits) & VersionMask; }

    /*
        Kills the entity (destroyed when the world updates).
    */
    void Kill();

    /*
        Checks whether the entity is still alive.
    */
    bool IsAlive() const;

    /*
        Component management.
    */
    template <typename T> void AddComponent(T component);
    template <typename T, typename ... Args> void AddComponent(Args && ... args);
    template <typename T> void RemoveComponent();
    template <typename T> bool HasComponent() const;
    template <typename T> T& GetComponent() const;

    /*
        Tags the entity.
    */
    void Tag(std::string tagName);
    bool HasTag(std::string tagName) const;

    /*
        Adds the entity to a certain group.
    */
    void Group(std::string groupName);
    bool HasGroup(std::string groupName) const;

    /*
        Returns a string of the entity (id + version).
    */
    std::string ToString() const;

private:
    EntityManager& GetEntityManager() const;

    static const uint32_t IndexBits = INDEX_BITS;
    static const uint32_t IndexMask = (1 << IndexBits) - 1;
    static const uint32_t VersionBits = VERSION_BITS;
    static const uint32_t VersionMask = (1 << VersionBits) - 1;

    // Id = index + version (kinda).
    Id id;

    EntityManager *entityManager = nullptr;
    friend class EntityManager;
};

class EntityManager
{
public:
    EntityManager(World &world) : world(world) {}

    /*
        Entity management.
    */
    Entity CreateEntity();
    void DestroyEntity(Entity e);
    void KillEntity(Entity e);
    bool IsEntityAlive(Entity e) const;
    Entity GetEntity(Entity::Id index);

    /*
        Component management.
    */
    template <typename T> void AddComponent(Entity e, T component);
    template <typename T, typename ... Args> void AddComponent(Entity e, Args && ... args);
    template <typename T> void RemoveComponent(Entity e);
    template <typename T> bool HasComponent(Entity e) const;
    template <typename T> T& GetComponent(Entity e) const;
    const ComponentMask& GetComponentMask(Entity e) const;

    /*
        Tag management.
    */
    void TagEntity(Entity e, std::string tagName);
    bool HasTag(std::string tagName) const;
    bool HasTaggedEntity(std::string tagName, Entity e) const;
    Entity GetEntityByTag(std::string tagName);
    int GetTagCount() const;

    /*
        Group management.
    */
    void GroupEntity(Entity e, std::string groupName);
    bool HasGroup(std::string groupName) const;
    bool HasEntityInGroup(std::string groupName, Entity e) const;
    std::vector<Entity> GetEntityGroup(std::string groupName);
    int GetGroupCount() const;
    int GetEntityGroupCount(std::string groupName);

private:
    template <typename T>
    std::shared_ptr<Pool<T>> AccommodateComponent();

    // minimum amount of free indices before we reuse one
    const std::uint32_t MinimumFreeIds = MINIMUM_FREE_IDS;

    // deque of free entity indices
    std::deque<Entity::Id> freeIds;

    // vector of versions (index = entity index)
    std::vector<Entity::Version> versions;

    // vector of component pools, each pool contains all the data for a certain component type
    // vector index = component id, pool index = entity id
    std::vector<std::shared_ptr<BasePool>> componentPools;

    // vector of component masks, each mask lets us know which components are turned "on" for a specific entity
    // vector index = entity id, each bit set to 1 means that the entity has that component
    std::vector<ComponentMask> componentMasks;

    // maps a tag to an entity
    std::unordered_map<std::string, Entity> taggedEntities;
    std::unordered_map<Entity::Id, std::string> entityTags;

    // maps a tag to a group of entities
    std::unordered_map<std::string, std::set<Entity>> groupedEntities;
    std::unordered_map<Entity::Id, std::string> entityGroups;

    World &world;
};

template <typename T>
void Entity::AddComponent(T component)
{
    GetEntityManager().AddComponent<T>(*this, component);
}

template <typename T, typename ... Args>
void Entity::AddComponent(Args && ... args)
{
    GetEntityManager().AddComponent<T>(*this, std::forward<Args>(args)...);
}

template <typename T>
void Entity::RemoveComponent()
{
    GetEntityManager().RemoveComponent<T>(*this);
}

template <typename T>
bool Entity::HasComponent() const
{
    return GetEntityManager().HasComponent<T>(*this);
}

template <typename T>
T& Entity::GetComponent() const
{
    return GetEntityManager().GetComponent<T>(*this);
}

template <typename T>
void EntityManager::AddComponent(Entity e, T component)
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = e.GetIndex();
    std::shared_ptr<Pool<T>> componentPool = AccommodateComponent<T>();

    if (entityId >= componentPool->GetSize()) {
        componentPool->Resize(versions.size());
    }

    componentPool->Set(entityId, component);
    componentMasks[entityId].set(componentId);
}

template <typename T, typename ... Args>
void EntityManager::AddComponent(Entity e, Args && ... args)
{
    T component(std::forward<Args>(args) ...);
    AddComponent<T>(e, component);
}

template <typename T>
void EntityManager::RemoveComponent(Entity e)
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = e.GetIndex();
    assert(entityId < componentMasks.size());
    componentMasks[entityId].set(componentId, false);
}

template <typename T>
bool EntityManager::HasComponent(Entity e) const
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = e.GetIndex();
    assert(entityId < componentMasks.size());
    return componentMasks[entityId].test(componentId);
}

template <typename T>
T& EntityManager::GetComponent(Entity e) const
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = e.GetIndex();

    assert(HasComponent<T>(e));
    assert(componentId < componentPools.size());
    auto componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

    assert(componentPool);
    assert(entityId < componentPool->GetSize());
    return componentPool->Get(entityId);
}

template <typename T>
std::shared_ptr<Pool<T>> EntityManager::AccommodateComponent()
{
    const auto componentId = Component<T>::GetId();

    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<T>> pool(new Pool<T>());
        componentPools[componentId] = pool;
    }

    return std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
}

}