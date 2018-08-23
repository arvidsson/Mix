#include "Event.h"
#include "World.h"

namespace Mix
{

BaseEvent::Id BaseEvent::nextId = 0;

void EventManager::destroyEvents()
{
    for (auto &it : eventPools) {
        auto pool = it.second;
        pool->clear();
    }
}

}
