#include "System.hpp"
#include "World.hpp"
#include <algorithm>

namespace entity
{

void System::AddEntity(Entity e)
{
    entities.push_back(e);
}

void System::RemoveEntity(Entity e)
{
    entities.erase(std::remove_if(entities.begin(), entities.end(),
        [&e](Entity other) { return e == other; }
    ), entities.end());
}

World& System::GetWorld() const
{
    assert(world != nullptr);
    return *world;
}

void SystemManager::AddToSystems(Entity e)
{
    const auto &entityComponentMask = world.GetEntityManager().GetComponentMask(e);

    for (auto &it : systems) {
        auto &system = it.second;
        const auto &systemComponentMask = system->GetComponentMask();
        auto interest = (entityComponentMask & systemComponentMask) == systemComponentMask;

        if (interest) {
            system->AddEntity(e);
        }
    }
}

void SystemManager::RemoveFromSystems(Entity e)
{
    for (auto &it : systems) {
        auto &system = it.second;
        system->RemoveEntity(e);
    }
}

}