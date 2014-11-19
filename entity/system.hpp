#ifndef SYSTEM_INCLUDED
#define SYSTEM_INCLUDED

#include "entity.hpp"
#include <unordered_map>

namespace entity {

    class World;
    class SystemManager;

    struct BaseEvent {
        virtual ~BaseEvent() {}
    };

    // Derive your events from this one!
    template <typename TDerivedEvent>
    struct Event : BaseEvent {
        static TypeId get_type_id() {
            return TypeIdHelper<BaseEvent>::get_type_id<TDerivedEvent>();
        }
    };

    class BaseSystem {
    public:
        virtual ~BaseSystem() {}
        virtual void update(float delta) = 0;
    };

    // Derive your systems from this one!
    template <typename TDerivedSystem>
    class System : public BaseSystem {
    public:
        static TypeId get_type_id() {
            return TypeIdHelper<BaseSystem>::get_type_id<TDerivedSystem>();
        }

        std::vector<Entity> get_entities() { return entities; }

        template <typename U>
        void requires_component();

    private:
        std::bitset<max_components> required_components;
        std::vector<Entity> entities; // all components that has the above required components
    };

    class SystemManager
    {
    public:
        SystemManager(World &world);

        template <typename T>
        void add_system();

    private:
        std::unordered_map<TypeId, std::shared_ptr<BaseSystem>> systems;
        World &world;
    };

    template <typename T>
    template <typename U>
    void System<T>::requires_component() {
        required_components.set(U::get_type_id());
    }

    template <typename T>
    void SystemManager::add_system() {
        std::shared_ptr<T> system(new T);
        //system->systemManager = this;
        systems.insert(std::make_pair(T::get_type_id(), system));
    }

}

#endif
