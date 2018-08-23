#include "World.h"
#include <cassert>

namespace Mix
{

World::World()
{
    entityManager = std::make_unique<EntityManager>(*this);
    systemManager = std::make_unique<SystemManager>(*this);
    eventManager = std::make_unique<EventManager>(*this);
}

EntityManager& World::getEntityManager() const
{
    assert(entityManager != nullptr);
    return *entityManager;
}

SystemManager& World::getSystemManager() const
{
    assert(systemManager != nullptr);
    return *systemManager;
}

EventManager& World::getEventManager() const
{
    assert(eventManager != nullptr);
    return *eventManager;
}

void World::update()
{
    for (auto e : createdEntities) {
        getSystemManager().addToSystems(e);
    }
    createdEntities.clear();

    for (auto e : destroyedEntities) {
        getSystemManager().removeFromSystems(e);
        getEntityManager().destroyEntity(e);
    }
    destroyedEntities.clear();

    getEventManager().destroyEvents();
}

Entity World::createEntity()
{
    auto e = getEntityManager().createEntity();
    createdEntities.push_back(e);
    return e;
}

void World::destroyEntity(Entity e)
{
    destroyedEntities.push_back(e);
}

Entity World::getEntity(std::string tag) const
{
    return getEntityManager().getEntityByTag(tag);
}

std::vector<Entity> World::getGroup(std::string group) const
{
    return getEntityManager().getEntityGroup(group);
}

}
