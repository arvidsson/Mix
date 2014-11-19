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

When you're done adding components, you need to finalize the `Entity`.

    e.finalize();

This will make sure that every `System` gets updated so that it knows which entities to handle.

TODO: document how to define new systems and how to use them...
