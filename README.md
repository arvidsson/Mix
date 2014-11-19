entity
======

A minimal entity-component system

brief synopsis
--------------
Everything begins with a `World`. The `World` contains an `EntityManager` and a `SystemManager`.

    World world;

You use the world to create an `Entity`.

    Entity e = world.create_entity();

The `Entity` is just a helper class that encapsulates an `Entity::Id`. You can use this class
to add components to the entity.

    e.add_component<PositionComponent>(100, 100);

You define new components by deriving from `Component`.

    struct PositionComponent : Component<PositionComponent> {
        PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
        int x, y;
    };

When you're done adding components, you need to refresh the `Entity`.

    e.refresh();

This will make sure that every `System` gets refreshed so that it knows which entities to handle.
You define new systems by deriving from `System`.

    class MoveSystem : System<MoveSystem> {
        MoveSystem() {
            require_component<PositionComponent>();
            require_component<VelocityComponent>()
        }

        virtual void update(float delta) {
            for (auto e : get_entities()) {
                PositionComponent &pos = e.get_component<PositionComponent>();
                const VelocityComponent &vel = e.get_component<VelocityComponent>();
                pos.x += vel.x * delta;
                pos.y += vel.y * delta;
            }
        }
    };

By using `require_component<T>()` in the system's constructor, we define what entities the system is interested in.
All entities that have (at least) these two component types will be included in the vector of entities that the system
want to do something with (the entity is put into this vector of interest when using `refresh()` above). You also need
to define an `update()` method. It's in this method where the logic of the system takes place.

You need to add the system to the world's `SystemManager`.

SystemManager &system_manager = world.get_system_manager();
system_manager.add_system<MoveSystem>();

You need to control all the systems manually in your gameloop (as we don't know in which order you want to use them).

    world.begin_frame();
    MoveSystem &move_system = world.get_system_manager().get_system<MoveSystem>();
    move_system.update();

You need to call `begin_frame()` before you update any of the systems. The reason behind this is that when you `refresh()` or `kill()`
an entity, the world delays this action from happening until all the systems have had time to act upon the entity. So, it's only when
a new frame (or game loop tick) begins that the created or killed entity system actually exists or is removed.

TODO: what else do we need to explain?
