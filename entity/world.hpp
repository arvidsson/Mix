#pragma once

#include "entity.hpp"
#include "system.hpp"
#include "event.hpp"
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
        EntityManager& get_entity_manager() const;
        SystemManager& get_system_manager() const;
        EventManager& get_event_manager() const;

        /*
        Updates the systems so that created/deleted entities are removed from the systems' vectors of entities.
        Updates the entity manager so that the version of a destructed entity's index is incremented.
        Destroys all the events that were created during the last frame.
        */
        void update();

        /*
        Creates an entity.
        */
        Entity create_entity();

        /*
        Kills an entity.
        */
        void kill_entity(Entity e);

        /*
        Returns an entity by its tag.
        */
        Entity get_entity(std::string tag_name) const;

        /*
        Returns a vector of entities belonging to a certain group.
        */
        std::vector<Entity> get_entity_group(std::string group_name) const;

    private:
        // vector of entities that are awaiting creation
        std::vector<Entity> created_entities;

        // vector of entities that are awaiting destruction
        std::vector<Entity> killed_entities;

        // the managers
        std::unique_ptr<EntityManager> entity_manager = nullptr;
        std::unique_ptr<SystemManager> system_manager = nullptr;
        std::unique_ptr<EventManager> event_manager = nullptr;
    };

}
