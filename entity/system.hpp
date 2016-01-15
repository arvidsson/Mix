#pragma once

#include "Event.hpp"
#include "Entity.hpp"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace entity
{

class SystemManager;
class World;

// The system processes entities that it's interested in each frame. Derive from this one!
class System
{
public:
    virtual ~System() {}

    // what component types the system requires of entities (we can use this method in the constructor for example)
    template <typename T>
    void RequireComponent();

    // returns a list of entities that the system should process each frame
    std::vector<Entity> GetEntities() { return entities; }

    // adds an entity of interest
    void AddEntity(Entity e);

    // if the entity is not alive anymore (during processing), the entity should be removed
    void RemoveEntity(Entity e);

    const ComponentMask& GetComponentMask() const { return componentMask; }

protected:
    World& GetWorld() const;

private:
    // which components an entity must have in order for the system to process the entity
    ComponentMask componentMask;

    // vector of all entities that the system is interested in
    std::vector<Entity> entities;

    World *world = nullptr;
    friend class SystemManager;
};

class SystemManager
{
public:
    SystemManager(World &world) : world(world) {}

    template <typename T> void AddSystem();
    template <typename T, typename ... Args> void AddSystem(Args && ... args);
    template <typename T> void RemoveSystem();
    template <typename T> T& GetSystem();
    template <typename T> bool HasSystem() const;

    // adds an entity to each system that is interested of the entity
    void AddToSystems(Entity e);

    // removes an entity from interested systems' entity lists
    void RemoveFromSystems(Entity e);

private:
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    World &world;
};

template <typename T>
void System::RequireComponent()
{
    const auto componentId = Component<T>::GetId();
    componentMask.set(componentId);
}

template <typename T>
void SystemManager::AddSystem()
{
    if (HasSystem<T>()) {
        return;
    }

    std::shared_ptr<T> system(new T);
    system->world = &world;
    systems.insert(std::make_pair(std::type_index(typeid(T)), system));
}

template <typename T, typename ... Args>
void SystemManager::AddSystem(Args && ... args)
{
    if (HasSystem<T>()) {
        return;
    }

    std::shared_ptr<T> system(new T(std::forward<Args>(args) ...));
    system->world = &world;
    systems.insert(std::make_pair(std::type_index(typeid(T)), system));
}

template <typename T>
void SystemManager::RemoveSystem()
{
    if (!HasSystem<T>()) {
        return;
    }

    auto it = systems.find(std::type_index(typeid(T)));
    systems.erase(it);
}

template <typename T>
T& SystemManager::GetSystem()
{
    if (!HasSystem<T>()) {
        throw std::runtime_error(std::string("Failed to get system: ") + typeid(T).name());
    }

    auto it = systems.find(std::type_index(typeid(T)));
    return *(std::static_pointer_cast<T>(it->second));
}

template <typename T>
bool SystemManager::HasSystem() const
{
    return systems.find(std::type_index(typeid(T))) != systems.end();
}

}