#pragma once

#include "Config.h"
#include <bitset>
#include <cstdint>
#include <cassert>

namespace Mix
{

/*
    Example component:

    struct PositionComponent
    {
        // important that we have a default constructor (i.e. no args required) in order for the creation of components to work
        PositionComponent(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
        float x, y;
    };
*/

// Used to be able to assign unique ids to each component type.
struct BaseComponent
{
    using Id = uint8_t;
    static const Id MaxComponents = MAX_COMPONENTS;
protected:
    static Id nextId;
};

// Used to assign a unique id to a component type, we don't really have to make our components derive from this though.
template <typename T>
struct Component : BaseComponent
{
    // Returns the unique id of Component<T>
    static Id getId()
    {
        static auto id = nextId++;
        assert(id < MaxComponents);
        return id;
    }
};

// Used to keep track of which components an entity has and also which entities a system is interested in.
using ComponentMask = std::bitset<BaseComponent::MaxComponents>;

}
