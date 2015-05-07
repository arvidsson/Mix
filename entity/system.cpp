#include "system.hpp"
#include "world.hpp"
#include <algorithm>

namespace entity
{

    void System::add_entity(Entity e)
    {
        entities.push_back(e);
    }

    void System::remove_entity(Entity e)
    {
        entities.erase(std::remove_if(entities.begin(), entities.end(),
            [&e](Entity other) { return e == other; }
        ), entities.end());
    }

    World& System::get_world() const
    {
        assert(world != nullptr);
        return *world;
    }

    void SystemManager::update_systems(Entity e)
    {
        const auto &entity_component_mask = world.get_entity_manager().get_component_mask(e);

        for (auto &it : systems) {
            auto system = it.second;
            const auto &system_component_mask = system->get_component_mask();
            auto interest = (entity_component_mask & system_component_mask) == system_component_mask;

            if (interest) {
                system->add_entity(e);
            }
        }
    }

}
