entity
======

A minimal entity-component system

example
-------

    struct PositionComponent : Component<PositionComponent> {
        PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
        int x, y;
    };

    struct VelocityComponent : Component<VelocityComponent> {
        VelocityComponent(int x = 0, int y = 0) : x(x), y(y) {}
        int x, y;
    };

    class MovementSystem : public System<MovementSystem> {
    public:
        MovementSystem() {
            register_component<PositionComponent>();
            register_component<VelocityComponent>();
        }

        virtual void update(float delta) {
            for (auto e : entities) {
                PositionComponent &pos = e.get_component<PositionComponent>();
                const VelocityComponent &vel = e.get_component<VelocityComponent>();

                pos.x += vel.x * delta;
                pos.y += vel.y * delta;
            }
        }
    };

    int main() {
        World world;

        SystemManager &system_manager = world.get_system_manager();
        system_manager.add_system<MovementSystem>();

        Entity e = world.create_entity();
        e.add_component<PositionComponent>(100, 50);
        e.add_component<VelocityComponent>(100, 0);
        e.finalize();
    }
