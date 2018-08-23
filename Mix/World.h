// Mix - A minimal entity-component-system.
// Copyright 2014-2018 Par Arvidsson. All rights reserved.
// Licensed under the MIT license (https://github.com/arvidsson/Mix/blob/master/LICENSE).

#pragma once

#include "Entity.h"
#include "System.h"
#include "Event.h"
#include <vector>
#include <string>
#include <memory>

namespace Mix
{

// The World manages the creation and destruction of entities so that entities.
class World
{
public:
    World();

    EntityManager& getEntityManager() const;
    SystemManager& getSystemManager() const;
    EventManager& getEventManager() const;

    /*
        Updates the systems so that created/deleted entities are removed from the systems' vectors of entities.
        Updates the entity manager so that the version of a destructed entity's index is incremented.
        Destroys all the events that were created during the last frame.
    */
    void update();

    Entity createEntity();
    void destroyEntity(Entity e);

    Entity getEntity(std::string tag) const;
    std::vector<Entity> getGroup(std::string group) const;

private:
    // vector of entities that are awaiting creation
    std::vector<Entity> createdEntities;

    // vector of entities that are awaiting destruction
    std::vector<Entity> destroyedEntities;

    std::unique_ptr<EntityManager> entityManager = nullptr;
    std::unique_ptr<SystemManager> systemManager = nullptr;
    std::unique_ptr<EventManager> eventManager = nullptr;
};

}
