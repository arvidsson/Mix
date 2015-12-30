#include "Entity.hpp"
#include "World.hpp"
#include <cassert>

namespace entity
{

void Entity::Kill()
{
    GetEntityManager().KillEntity(*this);
}

bool Entity::IsAlive() const
{
    return GetEntityManager().IsEntityAlive(*this);
}

void Entity::Tag(std::string tagName)
{
    GetEntityManager().TagEntity(*this, tagName);
}

bool Entity::HasTag(std::string tagName) const
{
    return GetEntityManager().HasTaggedEntity(tagName, *this);
}

void Entity::Group(std::string groupName)
{
    GetEntityManager().GroupEntity(*this, groupName);
}

bool Entity::HasGroup(std::string groupName) const
{
    return GetEntityManager().HasEntityInGroup(groupName, *this);
}

std::string Entity::ToString() const
{
    std::string s = "entity id: " + std::to_string(GetIndex()) + ", version: " + std::to_string(GetVersion());
    return s;
}

EntityManager& Entity::GetEntityManager() const
{
    assert(entityManager != nullptr);
    return *entityManager;
}

Entity EntityManager::CreateEntity()
{
    Entity::Id index;

    if (freeIds.size() > MinimumFreeIds) {
        index = freeIds.front();
        freeIds.pop_front();
    }
    else {
        versions.push_back(0);
        index = (unsigned int)versions.size() - 1;
        assert(index < (1 << Entity::IndexBits));

        if (index >= componentMasks.size()) {
            // TODO: grow by doubling?
            componentMasks.resize(index + 1);
        }
    }

    assert(index < versions.size());
    Entity e(index, versions[index]);
    e.entityManager = this;

    return e;
}

void EntityManager::DestroyEntity(Entity e)
{
    const auto index = e.GetIndex();
    assert(index < versions.size());        // sanity check
    assert(index < componentMasks.size());
    ++versions[index];                      // increase the version for that id
    freeIds.push_back(index);               // make the id available for reuse
    componentMasks[index].reset();          // reset the component mask for that id

    // if tagged, remove entity from tag management
    auto taggedEntity = entityTags.find(e.id);
    if (taggedEntity != entityTags.end()) {
        auto tag = taggedEntity->second;
        taggedEntities.erase(tag);
        entityTags.erase(taggedEntity);
    }

    // if in group, remove entity from group management
    auto groupedEntity = entityGroups.find(e.id);
    if (groupedEntity != entityGroups.end()) {
        auto groupName = groupedEntity->second;
        auto group = groupedEntities.find(groupName);
        if (group != groupedEntities.end()) {
            auto entityInGroup = group->second.find(e);
            if (entityInGroup != group->second.end()) {
                group->second.erase(entityInGroup);
            }
        }
        entityGroups.erase(groupedEntity);
    }
}

void EntityManager::KillEntity(Entity e)
{
    world.KillEntity(e);
}

bool EntityManager::IsEntityAlive(Entity e) const
{
    const auto index = e.GetIndex();
    assert(index < versions.size());
    return versions[index] == e.GetVersion();
}

Entity EntityManager::GetEntity(Entity::Id index)
{
    assert(index < versions.size());
    Entity e(index, versions[index]);
    e.entityManager = this;
    return e;
}

const ComponentMask& EntityManager::GetComponentMask(Entity e) const
{
    const auto index = e.GetIndex();
    assert(index < componentMasks.size());
    return componentMasks[index];
}

void EntityManager::TagEntity(Entity e, std::string tagName)
{
    taggedEntities.emplace(tagName, e);
    entityTags.emplace(e.id, tagName);
}

bool EntityManager::HasTag(std::string tagName) const
{
    return taggedEntities.find(tagName) != taggedEntities.end();
}

bool EntityManager::HasTaggedEntity(std::string tagName, Entity e) const
{
    auto it = taggedEntities.find(tagName);
    if (it != taggedEntities.end()) {
        if (it->second == e) {
            return true;
        }
    }
    return false;
}

Entity EntityManager::GetEntityByTag(std::string tagName)
{
    assert(HasTag(tagName));
    return taggedEntities[tagName];
}

int EntityManager::GetTagCount() const
{
    return taggedEntities.size();
}

void EntityManager::GroupEntity(Entity e, std::string groupName)
{
    groupedEntities.emplace(groupName, std::set<Entity>());
    groupedEntities[groupName].emplace(e);
    entityGroups.emplace(e.id, groupName);
}

bool EntityManager::HasGroup(std::string groupName) const
{
    return groupedEntities.find(groupName) != groupedEntities.end();
}

bool EntityManager::HasEntityInGroup(std::string groupName, Entity e) const
{
    auto it = groupedEntities.find(groupName);
    if (it != groupedEntities.end()) {
        if (it->second.find(e.id) != it->second.end()) {
            return true;
        }
    }
    return false;
}

std::vector<Entity> EntityManager::GetEntityGroup(std::string groupName)
{
    assert(HasGroup(groupName));
    auto &s = groupedEntities[groupName];
    return std::vector<Entity>(s.begin(), s.end());
}

int EntityManager::GetGroupCount() const
{
    return groupedEntities.size();
}

int EntityManager::GetEntityGroupCount(std::string groupName)
{
    if (!HasGroup(groupName)) {
        return 0;
    }

    return groupedEntities[groupName].size();
}

}