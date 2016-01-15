#pragma once

#include "Pool.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <cstdint>

namespace entity
{

class World;

struct BaseEvent
{
    using Id = uint8_t;
protected:
    static Id idCounter;
};

template <typename T>
struct Event : BaseEvent
{
    // Returns the unique id of Event<T>
    static Id GetId()
    {
        static auto id = idCounter++;
        return id;
    }
};

class EventManager
{
public:
    EventManager(World &world) : world(world) {}

    template <typename T> void EmitEvent(T event);
    template <typename T, typename ... Args> void EmitEvent(Args && ... args);

    template <typename T> std::vector<T> GetEvents();

    void DestroyEvents();

private:
    template <typename T>
    std::shared_ptr<Pool<T>> AccommodateEvent();

    std::unordered_map<std::type_index, std::shared_ptr<BasePool>> eventPools;

    World &world;
};

template <typename T>
void EventManager::EmitEvent(T event)
{
    std::shared_ptr<Pool<T>> eventPool = AccommodateEvent<T>();
    eventPool->Add(event);
}

template <typename T, typename ... Args>
void EventManager::EmitEvent(Args && ... args)
{
    T event(std::forward<Args>(args) ...);
    EmitEvent<T>(event);
}

template <typename T>
std::shared_ptr<Pool<T>> EventManager::AccommodateEvent()
{
    if (eventPools.find(std::type_index(typeid(T))) == eventPools.end()) {
        std::shared_ptr<Pool<T>> pool(new Pool<T>());
        eventPools.insert(std::make_pair(std::type_index(typeid(T)), pool));
    }

    return std::static_pointer_cast<Pool<T>>(eventPools[std::type_index(typeid(T))]);
}

template <typename T>
std::vector<T> EventManager::GetEvents()
{
    return std::static_pointer_cast<Pool<T>>(eventPools[std::type_index(typeid(T))]).GetData();
}

}