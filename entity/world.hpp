#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED

#include "entity.hpp"
#include "system.hpp"
#include <vector>

namespace entity {

    class World {
    public:
        World();

        EntityManager& get_entity_manager() { return entity_manager; }
        SystemManager& get_system_manager() { return system_manager; }

        Entity create_entity();
        void refresh_entity(Entity entity);
        void remove_entity(Entity entity);
        void begin_frame();

    private:
        EntityManager entity_manager;
        SystemManager system_manager;

        std::vector<Entity> refreshed_entities;
        std::vector<Entity> removed_entities;
    };

}

#endif
