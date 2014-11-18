#include "system.hpp"
#include "world.hpp"
#include <cassert>

namespace entity {

    BaseEvent::Type BaseEvent::type_counter = 0;

    BaseSystem::Type BaseSystem::type_counter = 0;

    SystemManager::SystemManager(World &world) : world(world) {}
}
