#pragma once

#include "pool.hpp"
#include <unordered_map>
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
        static Id id_counter;
    };

    template <typename T>
    struct Event : BaseEvent
    {
        // Returns the unique id of Event<T>
        static Id get_id()
        {
            static auto id = id_counter++;
            return id;
        }
    };

    class EventManager
    {
    public:
        EventManager(World &world) : world(world) {}

        template <typename T> void emit_event(T event);
        template <typename T, typename ... Args> void emit_event(Args && ... args);

        template <typename T> std::shared_ptr<Pool<T>> get_events();

        void destroy_events();

    private:
        template <typename T>
        std::shared_ptr<Pool<T>> accommodate_event();

        std::unordered_map<std::type_index, std::shared_ptr<BasePool>> event_pools;

        World &world;
    };

    template <typename T>
    void EventManager::emit_event(T event)
    {
        std::shared_ptr<Pool<T>> event_pool = accommodate_event<T>();
        event_pool->add(event);
    }

    template <typename T, typename ... Args>
    void EventManager::emit_event(Args && ... args)
    {
        T event(std::forward<Args>(args) ...);
        emit_event<T>(event);
    }

    template <typename T>
    std::shared_ptr<Pool<T>> EventManager::accommodate_event()
    {
        if (event_pools.find(std::type_index(typeid(T))) == event_pools.end()) {
            std::shared_ptr<Pool<T>> pool(new Pool<T>());
            event_pools.insert(std::make_pair(std::type_index(typeid(T)), pool));
        }

        return std::static_pointer_cast<Pool<T>>(event_pools[std::type_index(typeid(T))]);
    }

    template <typename T>
    std::shared_ptr<Pool<T>> EventManager::get_events()
    {
        return std::static_pointer_cast<Pool<T>>(event_pools[std::type_index(typeid(T))]);
    }

}
