#include "event.hpp"
#include "world.hpp"

namespace entity
{

    BaseEvent::Id BaseEvent::id_counter = 0;

    void EventManager::destroy_events()
    {
        for (auto &it : event_pools) {
            auto pool = it.second;
            pool->clear();
        }
    }

}
