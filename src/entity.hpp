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

    private:
        Id id;
        EntityManager &entity_manager;
    };

    class EntityManager {
    public:
        Entity::Id create_entity();
        void remove_entity(Entity::Id id);

    private:
        static Entity::Id id_counter;

        // vector index = component type, pool index = entity id
        std::vector<std::shared_ptr<BasePool>> component_pools;

        // vector index = entity id, each bit set to 1 means that the entity has that component
        std::vector<std::bitset<max_components>> components_per_entity;

        // available entity ids
        std::vector<Entity::Id> reusable_ids;
    };

}

#endif
