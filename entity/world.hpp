#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED

#include "entity.hpp"
#include <vector>

namespace entity {

    class World {
    public:
        World();

        EntityManager& get_entity_manager() { return entity_manager; }

        Entity create_entity();
        void destroy_entity(Entity entity);
        void begin_frame();

    private:
        EntityManager entity_manager;

        std::vector<Entity> destroyed;
    };

}

#endif
