#include "world.hpp"
#include <cassert>

namespace entity
{

    World::World()
    {
        entity_manager = std::make_unique<EntityManager>(*this);
        system_manager = std::make_unique<SystemManager>(*this);
        event_manager = std::make_unique<EventManager>(*this);
    }

    EntityManager& World::get_entity_manager() const
    {
        assert(entity_manager != nullptr);
        return *entity_manager;
    }

    SystemManager& World::get_system_manager() const
    {
        assert(system_manager != nullptr);
        return *system_manager;
    }

    EventManager& World::get_event_manager() const
    {
        assert(event_manager != nullptr);
        return *event_manager;
    }

    void World::update()
    {
        for (auto e : created_entities) {
            get_system_manager().update_systems(e);
        }
        created_entities.clear();

        for (auto e : killed_entities) {
            get_entity_manager().destroy_entity(e);
        }
        killed_entities.clear();

        get_event_manager().destroy_events();
    }

    Entity World::create_entity()
    {
        auto e = get_entity_manager().create_entity();
        created_entities.push_back(e);
        return e;
    }

    void World::kill_entity(Entity e)
    {
        killed_entities.push_back(e);
    }

    Entity World::get_entity(std::string tag_name) const
    {
        return get_entity_manager().get_entity_by_tag(tag_name);
    }

    std::vector<Entity> World::get_entity_group(std::string group_name) const
    {
        return get_entity_manager().get_entity_group(group_name);
    }

}
