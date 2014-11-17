entity
======

A minimal entity-component system

example
-------

    struct PositionComponent : Component<PositionComponent>
    {
        PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
        int x, y;
    };

    struct VelocityComponent : Component<VelocityComponent>
    {
        VelocityComponent(int x = 0, int y = 0) : x(x), y(y) {}
        int x, y;
    };

    class MovementSystem : public System<MovementSystem>
    {
    public:
        MovementSystem()
        {
            AddComponentType<PositionComponent>();
            AddComponentType<VelocityComponent>();
        }

        virtual void Update(float delta)
        {
            for (auto e : entities)
            {
                PositionComponent &pos = e.GetComponent<PositionComponent>();
                const VelocityComponent &vel = e.GetComponent<VelocityComponent>();

                pos.x += vel.x * delta;
                pos.y += vel.y * delta;
            }
        }
    };
