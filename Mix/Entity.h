#pragma once

#include "Config.h"
#include "Component.h"
#include "Pool.h"
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <memory>
#include <string>
#include <cstdint>

namespace Mix
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
    bool operator==(const Entity &e) const { return getIndex() == e.getIndex(); }
    bool operator!=(const Entity &e) const { return getIndex() != e.getIndex(); }
    bool operator<(const Entity &e) const { return getIndex() < e.getIndex(); }

    /*
        Returns the index part of the id.
    */
    Id getIndex() const { return id & IndexMask; }

    /*
        Returns the version part of the id.
    */
    Version getVersion() const { return (id >> IndexBits) & VersionMask; }

    /*
        Kills the entity (destroyed when the world updates).
    */
    void kill();

    /*
        Checks whether the entity is still alive.
    */
    bool isAlive() const;

    /*
        Component management.
    */
    template <typename T> void addComponent(T component);
    template <typename T, typename ... Args> void addComponent(Args && ... args);
    template <typename T> void removeComponent();
    template <typename T> bool hasComponent() const;
    template <typename T> T& getComponent() const;

    /*
        Tags the entity.
    */
    void tag(std::string tag);
    bool hasTag(std::string tag) const;

    /*
        Adds the entity to a certain group.
    */
    void group(std::string group);
    bool hasGroup(std::string group) const;

    /*
        Returns a string of the entity (id + version).
    */
    std::string toString() const;

private:
    EntityManager& getEntityManager() const;

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
    Entity createEntity();
    void destroyEntity(Entity e);
    void killEntity(Entity e);
    bool isEntityAlive(Entity e) const;
    Entity getEntity(Entity::Id index);

    /*
        Component management.
    */
    template <typename T> void addComponent(Entity e, T component);
    template <typename T, typename ... Args> void addComponent(Entity e, Args && ... args);
    template <typename T> void removeComponent(Entity e);
    template <typename T> bool hasComponent(Entity e) const;
    template <typename T> T& getComponent(Entity e) const;
    const ComponentMask& getComponentMask(Entity e) const;

    /*
        Tag management.
    */
    void tagEntity(Entity e, std::string tag);
    bool hasTag(std::string tag) const;
    bool hasTaggedEntity(std::string tag, Entity e) const;
    Entity getEntityByTag(std::string tag);
    int getTagCount() const;

    /*
        Group management.
    */
    void groupEntity(Entity e, std::string group);
    bool hasGroup(std::string group) const;
    bool hasEntityInGroup(std::string group, Entity e) const;
    std::vector<Entity> getEntityGroup(std::string group);
    int getGroupCount() const;
    int getEntityGroupCount(std::string group);

private:
    template <typename T>
    std::shared_ptr<Pool<T>> accommodateComponent();

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
void Entity::addComponent(T component)
{
    getEntityManager().addComponent<T>(*this, component);
}

template <typename T, typename ... Args>
void Entity::addComponent(Args && ... args)
{
    getEntityManager().addComponent<T>(*this, std::forward<Args>(args)...);
}

template <typename T>
void Entity::removeComponent()
{
    getEntityManager().removeComponent<T>(*this);
}

template <typename T>
bool Entity::hasComponent() const
{
    return getEntityManager().hasComponent<T>(*this);
}

template <typename T>
T& Entity::getComponent() const
{
    return getEntityManager().getComponent<T>(*this);
}

template <typename T>
void EntityManager::addComponent(Entity e, T component)
{
    const auto componentId = Component<T>::getId();
    const auto entityId = e.getIndex();
    std::shared_ptr<Pool<T>> componentPool = accommodateComponent<T>();

    if (entityId >= componentPool->getSize()) {
        componentPool->resize(versions.size());
    }

    componentPool->set(entityId, component);
    componentMasks[entityId].set(componentId);
}

template <typename T, typename ... Args>
void EntityManager::addComponent(Entity e, Args && ... args)
{
    T component(std::forward<Args>(args) ...);
    addComponent<T>(e, component);
}

template <typename T>
void EntityManager::removeComponent(Entity e)
{
    const auto componentId = Component<T>::getId();
    const auto entityId = e.getIndex();
    assert(entityId < componentMasks.size());
    componentMasks[entityId].set(componentId, false);
}

template <typename T>
bool EntityManager::hasComponent(Entity e) const
{
    const auto componentId = Component<T>::getId();
    const auto entityId = e.getIndex();
    assert(entityId < componentMasks.size());
    return componentMasks[entityId].test(componentId);
}

template <typename T>
T& EntityManager::getComponent(Entity e) const
{
    const auto componentId = Component<T>::getId();
    const auto entityId = e.getIndex();

    assert(hasComponent<T>(e));
    assert(componentId < componentPools.size());
    auto componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

    assert(componentPool);
    assert(entityId < componentPool->getSize());
    return componentPool->get(entityId);
}

template <typename T>
std::shared_ptr<Pool<T>> EntityManager::accommodateComponent()
{
    const auto componentId = Component<T>::getId();

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
