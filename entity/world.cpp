#include "world.hpp"
#include <algorithm>

namespace entity {

    World::World() : entity_manager(*this), system_manager(*this) {}

    Entity World::create_entity() {
        return Entity(entity_manager.create_entity(), *this);
    }

    void World::refresh_entity(Entity entity) {
        refreshed_entities.push_back(entity);
    }

    void World::remove_entity(Entity entity) {
        auto it = std::find_if(removed_entities.begin(),
                               removed_entities.end(),
                               [&entity](const Entity &removed_entity) {
                                   return entity.get_id() == removed_entity.get_id();
                               });
        if (it == removed_entities.end()) {
            removed_entities.push_back(entity);
        }
    }

    void World::begin() {
        for (auto &entity : refreshed_entities)
            entity_manager.refresh_entity(entity.get_id());
        refreshed_entities.clear();

        for (auto &entity : removed_entities) {
            entity_manager.remove_entity(entity.get_id());
        }
        removed_entities.clear();
    }

}
