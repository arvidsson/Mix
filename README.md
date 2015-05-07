entity
======

A minimal entity-component system

brief synopsis
--------------
Everything begins with a ```World```.

```c++
World world;
```

You use the world to create entities.

```c++
auto e = world.create_entity();
```

An entity is basically just an id. You can add components to entities.

```c++
e.add_component<PositionComponent>(100, 100);
```

Before you add components, you must define them.

```c++
struct PositionComponent
{
    PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
    int x, y;
};
```

You can create entities and add components to them anytime you want. However, the entities are not really created until
the world is updated. Same goes when destroying entities. This a design decision regarding this ECS implementation.
The entities are not created/destroyed until the world is updated so that the systems all have the opportunity
to act on the entities that existed in that frame. You update the world like this:

```c++
world.update();
```

It's good to do this at the start of each game tick. The update makes sure that all systems are refreshed so they know
which entities to handle.

You define new systems by deriving from ```System```.

```c++
class MoveSystem : System
{
    
    MoveSystem()
    {
        require_component<PositionComponent>();
        require_component<VelocityComponent>()
    }

    virtual void update(float dt)
    {
        for (auto e : get_entities()) {
            if (e.is_alive()) {
                auto &pos = e.get_component<PositionComponent>();
                const auto vel = e.get_component<VelocityComponent>();
                pos.x += vel.x * delta;
                pos.y += vel.y * delta;
            }
            else {
                remove_entity(e);
            }
        }
    }
};
```

By using ```require_component<T>()``` in the system's constructor, we define what entities the system is interested in.
All entities that have (at least) these two component types will be included in the vector of entities that the system
want to do something with. You also need to define an ```update()``` method.
It's in this method where the logic of the system takes place. Use ```get_entities()``` to loop through all entities.
Check if the entity is alive, if not use ```remove_entity()``` so that the system does not process the entity in the next frame
(again, a design decision - so that the processing of entities are fast, but the creation and destruction are slow-ish).

You need to add the system to the world's ```SystemManager```.

```c++
auto &system_manager = world.get_system_manager();
system_manager.add_system<MoveSystem>();
```

You need to control all the systems manually in your game loop (as we don't know in which order you want to use them).

```c++
world.update();
auto &move_system = world.get_system_manager().get_system<MoveSystem>();
move_system.update();
```

That's it for now. Questions?
