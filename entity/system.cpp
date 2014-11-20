#include "system.hpp"
#include "world.hpp"
#include <cassert>

namespace entity {

    SystemManager::SystemManager(World &world) : world(world) {}

    void SystemManager::refresh_systems(Entity entity) {
        for (auto &it : systems) {
            auto system = it.second;
            auto components = world.get_entity_manager().get_components_for_entity(entity.get_id());
            bool interest = (components & system->required_components) == components;
            if (interest) {
                system->entities.push_back(entity);
            }
        }
    }

}
