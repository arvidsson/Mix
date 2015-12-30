#pragma once

#include "Entity.hpp"
#include "System.hpp"
#include "Event.hpp"
#include <vector>
#include <string>
#include <memory>

namespace entity
{

/*
    The World manages the creation and destruction of entities so that entities don't show up/disappear mid-frame.
    It also has all the managers (entity, system, event), which can be accessed by the systems using the getters.
*/
class World
{
public:
    /*
        Creates all the managers.
    */
    World();

    /*
        Getters to the managers.
    */
    EntityManager& GetEntityManager() const;
    SystemManager& GetSystemManager() const;
    EventManager& GetEventManager() const;

    /*
        Updates the systems so that created/deleted entities are removed from the systems' vectors of entities.
        Updates the entity manager so that the version of a destructed entity's index is incremented.
        Destroys all the events that were created during the last frame.
    */
    void Update();

    /*
        Creates an entity.
    */
    Entity CreateEntity();

    /*
        Kills an entity.
    */
    void KillEntity(Entity e);

    /*
        Returns an entity by its tag.
    */
    Entity GetEntity(std::string tagName) const;

    /*
        Returns a vector of entities belonging to a certain group.
    */
    std::vector<Entity> GetEntityGroup(std::string groupName) const;

private:
    // vector of entities that are awaiting creation
    std::vector<Entity> createdEntities;

    // vector of entities that are awaiting destruction
    std::vector<Entity> killedEntities;

    // the managers
    std::unique_ptr<EntityManager> entityManager = nullptr;
    std::unique_ptr<SystemManager> systemManager = nullptr;
    std::unique_ptr<EventManager> eventManager = nullptr;
};

}