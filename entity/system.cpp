#include "system.hpp"
#include "world.hpp"
#include <cassert>

namespace entity {

    SystemManager::SystemManager(World &world) : world(world) {}

}
