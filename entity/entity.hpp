#pragma once

#include "component.hpp"
#include "pool.hpp"
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <memory>
#include <string>
#include <cstdint>

namespace entity
{
    class World;
    class EntityManager;

    // Basically just an id.
    class Entity
    {
    public:
        using Id = uint32_t;
        using Version = uint8_t;

        /*
        Id = index + version (kinda).
        */
        Entity(Id index = 0, Version version = 0) { id = (version << INDEX_BITS) | index; }

        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;

        /*
        Comparison operators.
        */
        bool operator==(const Entity &e) const { return get_index() == e.get_index(); }
        bool operator!=(const Entity &e) const { return get_index() != e.get_index(); }
        bool operator<(const Entity &e) const { return get_index() < e.get_index(); }

        /*
        Returns the index part of the id.
        */
        Id get_index() const { return id & INDEX_MASK; }

        /*
        Returns the version part of the id.
        */
        Version get_version() const { return (id >> INDEX_BITS) & VERSION_MASK; }

        /*
        Kills the entity (destroyed when the world updates).
        */
        void kill();

        /*
        Checks whether the entity is still alive.
        */
        bool is_alive() const;

        /*
        Component management.
        */
        template <typename T> void add_component(T component);
        template <typename T, typename ... Args> void add_component(Args && ... args);
        template <typename T> void remove_component();
        template <typename T> bool has_component() const;
        template <typename T> T& get_component() const;

        /*
        Tags the entity.
        */
        void tag(std::string tag_name);

        /*
        Adds the entity to a certain group.
        */
        void group(std::string group_name);

        /*
        Returns a string of the entity (id + version).
        */
        std::string to_string() const;

    private:
        EntityManager& get_entity_manager() const;

        // Note: we only use 30 bits, as we probably needs two bits for lua lightuserdata in the future?
        static const uint32_t INDEX_BITS = 22;
        static const uint32_t INDEX_MASK = (1 << INDEX_BITS) - 1;
        static const uint32_t VERSION_BITS = 8;
        static const uint32_t VERSION_MASK = (1 << VERSION_BITS) - 1;

        // Id = index + version (kinda).
        Id id;

        EntityManager *entity_manager = nullptr;
        friend class EntityManager;
    };

    class EntityManager
    {
    public:
        EntityManager(World &world) : world(world) {}

        /*
        Entity management.
        */
        Entity create_entity();
        void destroy_entity(Entity e);
        void kill_entity(Entity e);
        bool is_entity_alive(Entity e) const;

        /*
        Component management.
        */
        template <typename T> void add_component(Entity e, T component);
        template <typename T, typename ... Args> void add_component(Entity e, Args && ... args);
        template <typename T> void remove_component(Entity e);
        template <typename T> bool has_component(Entity e) const;
        template <typename T> T& get_component(Entity e) const;
        const ComponentMask& get_component_mask(Entity e) const;

        /*
        Tag management.
        */
        void tag_entity(Entity e, std::string tag_name);
        bool has_tagged_entity(std::string tag_name) const;
        Entity get_entity_by_tag(std::string tag_name);

        /*
        Group management.
        */
        void group_entity(Entity e, std::string group_name);
        bool has_entity_group(std::string group_name) const;
        std::vector<Entity> get_entity_group(std::string group_name);

    private:
        template <typename T>
        std::shared_ptr<Pool<T>> accommodate_component();

        // minimum amount of free indices before we reuse one
        const std::uint32_t MINIMUM_FREE_IDS = 1024;

        // deque of free entity indices
        std::deque<Entity::Id> free_ids;

        // vector of versions (index = entity index)
        std::vector<Entity::Version> versions;

        // vector of component pools, each pool contains all the data for a certain component type
        // vector index = component id, pool index = entity id
        std::vector<std::shared_ptr<BasePool>> component_pools;

        // vector of component masks, each mask lets us know which components are turned "on" for a specific entity
        // vector index = entity id, each bit set to 1 means that the entity has that component
        std::vector<ComponentMask> component_masks;

        // maps a tag to an entity
        std::unordered_map<std::string, Entity> tagged_entities;

        // maps a tag to a group of entities
        std::unordered_map<std::string, std::set<Entity>> entity_groups;

        World &world;
    };

    template <typename T>
    void Entity::add_component(T component)
    {
        get_entity_manager().add_component<T>(*this, component);
    }

    template <typename T, typename ... Args>
    void Entity::add_component(Args && ... args)
    {
        get_entity_manager().add_component<T>(*this, std::forward<Args>(args)...);
    }

    template <typename T>
    void Entity::remove_component()
    {
        get_entity_manager().remove_component<T>(*this);
    }

    template <typename T>
    bool Entity::has_component() const
    {
        return get_entity_manager().has_component<T>(*this);
    }

    template <typename T>
    T& Entity::get_component() const
    {
        return get_entity_manager().get_component<T>(*this);
    }

    template <typename T>
    void EntityManager::add_component(Entity e, T component)
    {
        const auto component_id = Component<T>::get_id();
        const auto entity_id = e.get_index();
        std::shared_ptr<Pool<T>> component_pool = accommodate_component<T>();

        if (entity_id >= component_pool->get_size()) {
            component_pool->resize(versions.size());
        }

        component_pool->set(entity_id, component);
        component_masks[entity_id].set(component_id);
    }

    template <typename T, typename ... Args>
    void EntityManager::add_component(Entity e, Args && ... args)
    {
        T component(std::forward<Args>(args) ...);
        add_component<T>(e, component);
    }

    template <typename T>
    void EntityManager::remove_component(Entity e)
    {
        const auto component_id = Component<T>::get_id();
        const auto entity_id = e.get_index();
        assert(entity_id < component_masks.size());
        component_masks[entity_id].set(component_id, false);
    }

    template <typename T>
    bool EntityManager::has_component(Entity e) const
    {
        const auto component_id = Component<T>::get_id();
        const auto entity_id = e.get_index();
        assert(entity_id < component_masks.size());
        return component_masks[entity_id].test(component_id);
    }

    template <typename T>
    T& EntityManager::get_component(Entity e) const
    {
        const auto component_id = Component<T>::get_id();
        const auto entity_id = e.get_index();

        assert(has_component<T>(e));
        assert(component_id < component_pools.size());
        auto component_pool = std::static_pointer_cast<Pool<T>>(component_pools[component_id]);

        assert(component_pool);
        assert(entity_id < component_pool->get_size());
        return component_pool->get(entity_id);
    }

    template <typename T>
    std::shared_ptr<Pool<T>> EntityManager::accommodate_component()
    {
        const auto component_id = Component<T>::get_id();

        if (component_id >= component_pools.size()) {
            component_pools.resize(component_id + 1, nullptr);
        }

        if (!component_pools[component_id]) {
            std::shared_ptr<Pool<T>> pool(new Pool<T>());
            component_pools[component_id] = pool;
        }

        return std::static_pointer_cast<Pool<T>>(component_pools[component_id]);
    }

}
