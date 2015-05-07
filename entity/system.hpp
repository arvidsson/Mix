#pragma once

#include "event.hpp"
#include "entity.hpp"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace entity
{
    class SystemManager;
    class World;

    // The system processes entities that it's interested in each frame. Derive from this one!
    class System
    {
    public:
        virtual ~System() {}

        // override this!
        virtual void update(float dt = 0.0f) = 0;

        // what component types the system requires of entities (we can use this method in the constructor for example)
        template <typename T>
        void require_component();

        // returns a list of entities that the system should process each frame
        std::vector<Entity> get_entities() { return entities; }

        // adds an entity of interest
        void add_entity(Entity e);

        // if the entity is not alive anymore (during processing), the entity should be removed
        void remove_entity(Entity e);

        const ComponentMask& get_component_mask() const { return component_mask; }

    private:
        World& get_world() const;

        // which components an entity must have in order for the system to process the entity
        ComponentMask component_mask;

        // vector of all entities that the system is interested in
        std::vector<Entity> entities;

        World *world = nullptr;
        friend class SystemManager;
    };

    class SystemManager
    {
    public:
        SystemManager(World &world) : world(world) {}

        /* System */
        template <typename T> void add_system();
        template <typename T> void remove_system();
        template <typename T> T& get_system();
        template <typename T> bool has_system() const;

        // adds an entity to each system that is interested of the entity
        void update_systems(Entity e);

    private:
        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

        World &world;
    };

    template <typename T>
    void System::require_component()
    {
        const auto component_id = Component<T>::get_id();
        component_mask.set(component_id);
    }

    template <typename T>
    void SystemManager::add_system()
    {
        if (has_system<T>()) {
            return;
        }

        std::shared_ptr<T> system(new T);
        system->world = &world;
        systems.insert(std::make_pair(std::type_index(typeid(T)), system));
    }

    template <typename T>
    void SystemManager::remove_system()
    {
        if (!has_system<T>()) {
            return;
        }

        auto it = systems.find(std::type_index(typeid(T)));
        systems.erase(it);
    }

    template <typename T>
    T& SystemManager::get_system()
    {
        if (!has_system<T>()) {
            throw std::runtime_error(std::string("Failed to get system: ") + typeid(T).name());
        }

        auto it = systems.find(std::type_index(typeid(T)));
        return *(std::static_pointer_cast<T>(it->second));
    }

    template <typename T>
    bool SystemManager::has_system() const
    {
        return systems.find(std::type_index(typeid(T))) != systems.end();
    }

}
