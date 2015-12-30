#include "World.hpp"
#include <cassert>

namespace entity
{

World::World()
{
    entityManager = std::make_unique<EntityManager>(*this);
    systemManager = std::make_unique<SystemManager>(*this);
    eventManager = std::make_unique<EventManager>(*this);
}

EntityManager& World::GetEntityManager() const
{
    assert(entityManager != nullptr);
    return *entityManager;
}

SystemManager& World::GetSystemManager() const
{
    assert(systemManager != nullptr);
    return *systemManager;
}

EventManager& World::GetEventManager() const
{
    assert(eventManager != nullptr);
    return *eventManager;
}

void World::Update()
{
    for (auto e : createdEntities) {
        GetSystemManager().AddToSystems(e);
    }
    createdEntities.clear();

    for (auto e : killedEntities) {
        GetSystemManager().RemoveFromSystems(e);
        GetEntityManager().DestroyEntity(e);
    }
    killedEntities.clear();

    GetEventManager().DestroyEvents();
}

Entity World::CreateEntity()
{
    auto e = GetEntityManager().CreateEntity();
    createdEntities.push_back(e);
    return e;
}

void World::KillEntity(Entity e)
{
    killedEntities.push_back(e);
}

Entity World::GetEntity(std::string tagName) const
{
    return GetEntityManager().GetEntityByTag(tagName);
}

std::vector<Entity> World::GetEntityGroup(std::string groupName) const
{
    return GetEntityManager().GetEntityGroup(groupName);
}

}