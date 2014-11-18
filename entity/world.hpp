#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED

#include "entity.hpp"

namespace entity {

    class World {
    public:
        World();

        EntityManager& get_entity_manager() { return entity_manager; }

        Entity create_entity();

    private:
        EntityManager entity_manager;
    };

}

#endif
