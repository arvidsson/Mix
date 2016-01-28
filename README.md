entity
======

A minimal [entity-component system](https://en.wikipedia.org/wiki/Entity_component_system).

Features
--------

* ECS
* tags and groups
* rudimentary event handling

Install
-------

Include ```entity``` folder in your project.

The Basics
----------

##### 0. include entity

```c++
#include "entity/World.hpp"
```

##### 1. define components

```c++
// note: must have default constructor

struct PositionComponent
{
    PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
    int x, y;
};

struct VelocityComponent
{
    VelocityComponent(int dx = 0, int dy = 0) : dx(dx), dy(dy) {}
    int dx, dy;
};
```

##### 2. define systems

```c++
class MoveSystem : public entity::System
{
public:
    MoveSystem()
    {
        // which components an entity must have for the system to be interested
        RequireComponent<PositionComponent>();
        RequireComponent<VelocityComponent>();
    }

    void Update()
    {
        // do stuff with all the entities of interest
        for (auto e : GetEntities()) {
            auto &position = e.GetComponent<PositionComponent>();
            const auto &velocity = e.GetComponent<VelocityComponent>();
            position.x += velocity.x;
            position.y += velocity.y;
        }
    }
};
```

##### 3. create the world and add systems

```c++
entity::World world;
world.GetSystemManager().AddSystem<MoveSystem>();
```

##### 4. create entities

```c++
auto e = world.CreateEntity();
e.AddComponent<PositionComponent>(100, 100);
e.AddComponent<VelocityComponent>(10, 10);
// entity is created on the next call to world.Update(),
// so that no entities appear only for some systems mid-frame
```

##### 5. kill entities

```c++
// inside system's update method
entity.Kill();
if (!entity.IsAlive()) { ... };
// entity is alive until next call to world.Update(),
// so that every system gets the chance handle the entity
```

##### 6. update world and systems in game loop

```c++
while (!done) {
    world.Update(); // actually creates and kills entities since the last call to this method
    world.GetSystemManager().GetSystem<MoveSystem>().Update();
}
```

Tags & Groups
-------------

##### 1. tags

```c++
auto player = world.CreateEntity();
player.Tag("player");
if (player.HasTag("player")) { ... };
auto entity = world.GetEntity("player");
```

##### 2. groups

```c++
auto enemy = world.CreateEntity();
enemy.Group("enemies");
if (enemy.HasGroup("enemies")) { ... };
auto enemies = world.GetEntityGroup("enemies");
```

Events
------

Events are a way for inter-system communication (e.g. CollisionSystem emits collision event which DamageSystem is interested in).

##### 1. define events

```c++
// note: must have default constructor

struct CollisionEvent
{
    PositionComponent(Entity a = Entity(), Entity b = Entity()) : a(a), b(b) {}
    Entity a, b;
};
```

##### 2. emit events

```c++
// inside system's update method
Entity player, enemy;
GetWorld().GetEventManager().EmitEvent<CollisionEvent>(player, enemy);
```

##### 3. receive events

```c++
// inside other system's update method
auto collisionEvents = GetWorld().GetEventManager().GetEvents<CollisionEvent>();
for (auto event : collisionEvents) { // handle collision };

// events can be received by any system
// events exist until the next call to world.Update()
```

What else?
----------

Code is fairly well documented. Read and experiment! :)

License
-------
MIT (c) arvidsson
