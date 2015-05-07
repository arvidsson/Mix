#pragma once

#include <bitset>
#include <cstdint>
#include <cassert>

namespace entity
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
        static const Id MAX_COMPONENTS = 64;
    protected:
        static Id id_counter;
    };

    // Used to assign a unique id to a component type, we don't really have to make our components derive from this though.
    template <typename T>
    struct Component : BaseComponent
    {
        // Returns the unique id of Component<T>
        static Id get_id()
        {
            static auto id = id_counter++;
            assert(id < MAX_COMPONENTS);
            return id;
        }
    };

    // Used to keep track of which components an entity has and also which entities a system is interested in.
    using ComponentMask = std::bitset<BaseComponent::MAX_COMPONENTS>;

}
