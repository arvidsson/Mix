#ifndef ENTITY_INCLUDED
#define ENTITY_INCLUDED

#include "component.hpp"
#include "pool.hpp"
#include <vector>
#include <bitset>
#include <memory>

namespace entity {

    class EntityManager;

    class Entity {
    public:
        typedef std::uint32_t Id;

        Entity(Id id, EntityManager &entity_manager) : id(id), entity_manager(entity_manager) {}

        Id get_id() const { return id; }

        template <typename T, typename ... Args>
        void add_component(Args && ... args);

        template <typename T>
        T& get_component();

    private:
        Id id;
        EntityManager &entity_manager;
    };

    class EntityManager {
    public:
        Entity::Id create_entity();
        void remove_entity(Entity::Id id);

        template <typename T, typename ... Args>
        void add_component(Entity::Id id, Args && ... args);

        template <typename T>
        T& GetComponent(Entity::Id id);

    private:
        static Entity::Id id_counter;

        // vector index = component type, pool index = entity id
        std::vector<std::shared_ptr<BasePool>> component_pools;

        // vector index = entity id, each bit set to 1 means that the entity has that component
        std::vector<std::bitset<max_components>> components_per_entity;

        // available entity ids
        std::vector<Entity::Id> reusable_ids;
    };

    template <typename T, typename ... Args>
    void Entity::add_component(Args && ... args) {
        entity_manager.add_component<T>(id, std::forward<Args>(args)...);
    }

    template <typename T>
    T& Entity::get_component() {
        return entity_manager.get_component<T>(id);
    }

    template <typename T, typename ... Args>
    void EntityManager::add_component(Entity::Id id, Args && ... args) {
        const BaseComponent::Type type = T::get_type();
        std::shared_ptr<Pool<T>> pool;

        if (component_pools.size() <= type) {
            component_pools.resize(type + 1);
        }

        if (!component_pools[type]) {
            pool = std::shared_ptr<Pool<T>>(new Pool<T>());
            component_pools[type] = pool;
        }
        else {
            pool = std::static_pointer_cast<Pool<T>>(component_pools[type]);
        }

        pool->reserve(id_counter);
        T component(std::forward<Args>(args) ...);
        pool->set(id, component);

        components_per_entity[id].set(type);
    }

    template <typename T>
    T& EntityManager::get_component(Entity::Id id) {
        const BaseComponent::Type type = T::get_type();
        assert(type < component_pools.size());
        std::shared_ptr<Pool<T>> pool = std::static_pointer_cast<Pool<T>>(component_pools[type]);
        assert(pool);
        return pool->get(id);
    }

}

#endif
