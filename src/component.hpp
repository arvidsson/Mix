#ifndef INCLUDED_COMPONENT_HPP
#define INCLUDED_COMPONENT_HPP

#include <bitset>
#include <cassert>

namespace entity {

    // The maximum number of different component classes we can have.
    const std::uint32_t MAX_COMPONENTS = 64;

    // A bitfield where flags corresponding to our different component classes can be set or reset.
    typedef std::bitset<MAX_COMPONENTS> ComponentFlags;

    // Base class used to automatically increment the component type id.
    class BaseComponent {
    public:
        typedef std::uint32_t TypeId;

    protected:
        static TypeId next_type_id;
    };

    // Derive your components from this one!
    template <typename Derived>
    class Component : public BaseComponent {
    public:
        static TypeId get_type_id() {
            static TypeId type_id = next_type_id++;
            assert(type_id < MAX_COMPONENTS);
            return type_id;
        }
    };

}

#endif
