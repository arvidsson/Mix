#ifndef ENTITY_INCLUDED
#define ENTITY_INCLUDED

#include "type_id.hpp"
#include "pool.hpp"
#include <bitset>
#include <memory>
#include <cassert>

namespace entity {

    class World;
    class EntityManager;

    const int max_components = 64;

    struct BaseComponent {
        virtual ~BaseComponent() {}
    };

    // Derive your components from this one!
    template <typename TComponent>
    struct Component : BaseComponent {
        static TypeId get_type_id() {
            TypeId type_id = TypeIdHelper<BaseComponent>::get_type_id<TComponent>();
            assert(type_id < max_components);
            return type_id;
        }
    };

    class Entity {
    public:
        typedef std::uint32_t Id;

        Entity(Id id, World &world);

        Id get_id() const { return id; }

        template <typename TComponent, typename ... Args>
        void add_component(Args && ... args);

        template <typename TComponent>
        void remove_component();

        template <typename TComponent>
        TComponent& get_component();

        template <typename TComponent>
        bool has_component() const;

        void refresh();
        void remove();

    private:
        EntityManager& get_entity_manager();
        const EntityManager& get_entity_manager() const;

        Id id;
        World &world;
    };

    class EntityManager {
    public:
        EntityManager(World &world);

        Entity::Id create_entity();
        void refresh_entity(Entity::Id id);
        void remove_entity(Entity::Id id);

        template <typename TComponent, typename ... Args>
        void add_component(Entity::Id id, Args && ... args);

        template <typename TComponent>
        void remove_component(Entity::Id id);

        template <typename TComponent>
        TComponent& get_component(Entity::Id id);

        template <typename TComponent>
        bool has_component(Entity::Id id) const;

        std::bitset<max_components> get_components_for_entity(Entity::Id id);

    private:
        static Entity::Id next_id;

        // vector index = component type, pool index = entity id
        std::vector<std::shared_ptr<BasePool>> component_pools;

        // vector index = entity id, each bit set to 1 means that the entity has that component
        std::vector<std::bitset<max_components>> components_per_entity;

        // available entity ids
        std::vector<Entity::Id> reusable_ids;

        World &world;
    };

    template <typename TComponent, typename ... Args>
    void Entity::add_component(Args && ... args) {
        get_entity_manager().add_component<TComponent>(id, std::forward<Args>(args)...);
    }

    template <typename TComponent>
    void Entity::remove_component() {
        get_entity_manager().remove_component<TComponent>(id);
    }

    template <typename TComponent>
    TComponent& Entity::get_component() {
        return get_entity_manager().get_component<TComponent>(id);
    }

    template <typename TComponent>
    bool Entity::has_component() const {
        return get_entity_manager().has_component<TComponent>(id);
    }

    template <typename TComponent, typename ... Args>
    void EntityManager::add_component(Entity::Id id, Args && ... args) {
        const TypeId type_id = TComponent::get_type_id();
        std::shared_ptr<Pool<TComponent>> pool;

        if (component_pools.size() <= type_id) {
            component_pools.resize(type_id + 1);
        }

        if (!component_pools[type_id]) {
            pool = std::shared_ptr<Pool<TComponent>>(new Pool<TComponent>());
            component_pools[type_id] = pool;
        }
        else {
            pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[type_id]);
        }

        pool->reserve(next_id);
        TComponent component(std::forward<Args>(args) ...);
        pool->set(id, component);

        components_per_entity[id].set(type_id);
    }

    template <typename TComponent>
    void EntityManager::remove_component(Entity::Id id) {
        assert(id < components_per_entity.size());
        const TypeId type_id = TComponent::get_type_id();
        assert(type_id < component_pools.size());
        components_per_entity[id].reset(type_id);

        std::shared_ptr<Pool<TComponent>> pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[type_id]);
        assert(pool);
        pool->set(id, 0);
    }

    template <typename TComponent>
    TComponent& EntityManager::get_component(Entity::Id id) {
        const TypeId type_id = TComponent::get_type_id();
        assert(type_id < component_pools.size());
        std::shared_ptr<Pool<TComponent>> pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[type_id]);
        assert(pool);
        return pool->get(id);
    }

    template <typename TComponent>
    bool EntityManager::has_component(Entity::Id id) const {
        assert(id < components_per_entity.size());
        const TypeId type_id = TComponent::get_type_id();
        return components_per_entity[id].test(type_id);
    }

}

#endif
