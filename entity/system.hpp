#ifndef SYSTEM_INCLUDED
#define SYSTEM_INCLUDED

#include "entity.hpp"
#include "event.hpp"
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace entity {

    class World;
    class SystemManager;

    class BaseSystem {
    public:
        typedef std::uint32_t Type;

        virtual void update(float delta) = 0;

    protected:
        static Type type_counter;
    };

    template <typename T>
    class System : public BaseSystem {
    public:
        static Type get_type() {
            static Type type = type_counter++;
            return type;
        }

        std::vector<Entity> get_entities() { return entities; }

        template <typename U>
        void requires_component();

    private:
        std::bitset<max_components> required_components;
        std::vector<Entity> entities; // all components that has the above required components
    };

    class SystemManager
    {
    public:
        SystemManager(World &world);

        template <typename T>
        void add_system();

    private:
        std::unordered_map<BaseSystem::Type, std::shared_ptr<BaseSystem>> systems;
        World &world;
    };

    template <typename T>
    template <typename U>
    void System<T>::requires_component() {
        required_components.set(U::get_type());
    }

    template <typename T>
    void SystemManager::add_system() {
        std::shared_ptr<T> system(new T);
        //system->systemManager = this;
        systems.insert(std::make_pair(T::get_type(), system));
    }

}

#endif
