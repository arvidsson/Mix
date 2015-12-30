#include "Event.hpp"
#include "World.hpp"

namespace entity
{

BaseEvent::Id BaseEvent::idCounter = 0;

void EventManager::DestroyEvents()
{
    for (auto &it : eventPools) {
        auto pool = it.second;
        pool->Clear();
    }
}

}