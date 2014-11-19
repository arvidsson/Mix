#ifndef TYPE_ID_INCLUDED
#define TYPE_ID_INCLUDED

#include <cstdint>

namespace entity {

    typedef std::uint32_t TypeId;

    // Helper class to help assign unique type ids to classes deriving from TBase.
    template <typename TBase>
    class TypeIdHelper {
    public:
        template <typename TDerived>
        static TypeId get_type_id() {
            static const TypeId type_id = next_type_id++;
            return type_id;
        }

    private:
        static TypeId next_type_id;
    };

    template <typename TBase>
    TypeId TypeIdHelper<TBase>::next_type_id = 0;

}

#endif
