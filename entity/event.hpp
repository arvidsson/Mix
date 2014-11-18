#ifndef EVENT_INCLUDED
#define EVENT_INCLUDED

#include <cstdint>
#include <cassert>

namespace entity {

    class BaseEvent {
    public:
        typedef std::uint32_t Type;

    protected:
        static Type type_counter;
    };

    template <typename T>
    class Event : public BaseEvent {
    public:
        static Type get_type() {
            static Type type = type_counter++;
            return type;
        }
    };

}

#endif
