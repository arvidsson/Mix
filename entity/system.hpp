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

        std::vector<Entity> get_entities() { return entities; }

        template <typename TComponent>
        void require_component();

    private:
        std::bitset<max_components> required_components;
        std::vector<Entity> entities; // all components that has the above required components

        friend class SystemManager;
    };

    // Derive your systems from this one!
    template <typename TSystem>
    class System : public BaseSystem {
    public:
        static TypeId get_type_id() {
            return TypeIdHelper<BaseSystem>::get_type_id<TSystem>();
        }
    };

    class SystemManager
    {
    public:
        SystemManager(World &world);

        template <typename TSystem>
        void add_system();

        template <typename TSystem>
        TSystem& get_system();

        void refresh_systems(Entity entity);

    private:
        std::unordered_map<TypeId, std::shared_ptr<BaseSystem>> systems;
        World &world;
    };

    template <typename TComponent>
    void BaseSystem::require_component() {
        required_components.set(TComponent::get_type_id());
    }

    template <typename TSystem>
    void SystemManager::add_system() {
        std::shared_ptr<TSystem> system(new TSystem);
        //system->systemManager = this;
        systems.insert(std::make_pair(TSystem::get_type_id(), system));
    }

    template <typename TSystem>
    TSystem& SystemManager::get_system() {
        auto it = systems.find(TSystem::get_type_id());
        assert(it != systems.end());
        return *(std::static_pointer_cast<TSystem>(it->second));
    }

}

#endif
