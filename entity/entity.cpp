#include "entity.hpp"
#include "world.hpp"
#include <cassert>

namespace entity
{

    void Entity::kill()
    {
        get_entity_manager().kill_entity(*this);
    }

    bool Entity::is_alive() const
    {
        return get_entity_manager().is_entity_alive(*this);
    }

    void Entity::tag(std::string tag_name)
    {
        get_entity_manager().tag_entity(*this, tag_name);
    }

    void Entity::group(std::string group_name)
    {
        get_entity_manager().group_entity(*this, group_name);
    }

    std::string Entity::to_string() const
    {
        std::string s = "entity id: " + std::to_string(get_index()) + ", version: " + std::to_string(get_version());
        return s;
    }

    EntityManager& Entity::get_entity_manager() const
    {
        assert(entity_manager != nullptr);
        return *entity_manager;
    }

    Entity EntityManager::create_entity()
    {
        Entity::Id index;

        if (free_ids.size() > MINIMUM_FREE_IDS) {
            index = free_ids.front();
            free_ids.pop_front();
        }
        else {
            versions.push_back(0);
            index = (unsigned int)versions.size() - 1;
            assert(index < (1 << Entity::INDEX_BITS));

            if (index >= component_masks.size()) {
                // TODO: grow by doubling?
                component_masks.resize(index + 1);
            }
        }

        assert(index < versions.size());
        Entity e(index, versions[index]);
        e.entity_manager = this;

        return e;
    }

    void EntityManager::destroy_entity(Entity e)
    {
        const auto index = e.get_index();
        assert(index < versions.size());        // sanity check
        assert(index < component_masks.size());
        ++versions[index];                      // increase the version for that id
        free_ids.push_back(index);              // make the id available for reuse
        component_masks[index].reset();         // reset the component mask for that id
    }

    void EntityManager::kill_entity(Entity e)
    {
        world.kill_entity(e);
    }

    bool EntityManager::is_entity_alive(Entity e) const
    {
        const auto index = e.get_index();
        assert(index < versions.size());
        return versions[index] == e.get_version();
    }

    const ComponentMask& EntityManager::get_component_mask(Entity e) const
    {
        const auto index = e.get_index();
        assert(index < component_masks.size());
        return component_masks[index];
    }

    void EntityManager::tag_entity(Entity e, std::string tag_name)
    {
        tagged_entities.emplace(tag_name, e);
    }

    bool EntityManager::has_tagged_entity(std::string tag_name) const
    {
        return tagged_entities.find(tag_name) != tagged_entities.end();
    }

    Entity EntityManager::get_entity_by_tag(std::string tag_name)
    {
        assert(has_tagged_entity(tag_name));
        return tagged_entities[tag_name];
    }

    void EntityManager::group_entity(Entity e, std::string group_name)
    {
        entity_groups.emplace(group_name, std::set<Entity>());
        entity_groups[group_name].emplace(e);
    }

    bool EntityManager::has_entity_group(std::string group_name) const
    {
        return entity_groups.find(group_name) != entity_groups.end();
    }

    std::vector<Entity> EntityManager::get_entity_group(std::string group_name)
    {
        assert(has_entity_group(group_name));
        auto &s = entity_groups[group_name];
        return std::vector<Entity>(s.begin(), s.end());
    }

}
