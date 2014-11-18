#ifndef COMPONENT_INCLUDED
#define COMPONENT_INCLUDED

#include <cstdint>
#include <cassert>

namespace entity {

    // The maximum number of different component classes we can have.
    const std::uint32_t max_components = 64;

    // Base class used to automatically increment the component type id.
    class BaseComponent {
    public:
        typedef std::uint32_t Type;

    protected:
        static Type type_counter;
    };

    // Derive your components from this one!
    template <typename T>
    class Component : public BaseComponent {
    public:
        static Type get_type() {
            static Type type = type_counter++;
            assert(type < max_components);
            return type;
        }
    };

}

#endif
