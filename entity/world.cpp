#include "world.hpp"
#include <algorithm>

namespace entity {

    World::World() : entity_manager(*this) {}

    Entity World::create_entity() {
        return Entity(entity_manager.create_entity(), *this);
    }

    void World::destroy_entity(Entity entity) {
        auto it = std::find_if(destroyed.begin(),
                               destroyed.end(),
                               [&entity](const Entity &deleted_entity) {
                                   return entity.get_id() == deleted_entity.get_id();
                               });
        if (it == destroyed.end()) {
            destroyed.push_back(entity);
        }
    }

    void World::begin_frame() {
        for (auto &entity : destroyed) {
            entity_manager.remove_entity(entity.get_id());
        }
        destroyed.clear();
    }

}
