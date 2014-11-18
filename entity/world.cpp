#include "world.hpp"

namespace entity {

    World::World() : entity_manager(*this) {}

    Entity World::create_entity() {
        return Entity(entity_manager.create_entity(), *this);
    }

}
