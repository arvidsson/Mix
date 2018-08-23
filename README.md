Mix
======

A minimal [entity-component-system](https://en.wikipedia.org/wiki/Entity_component_system).

Features
--------

* entity–component–system implementation
* tags and groups
* rudimentary event handling

Install
-------

Include ```Mix``` folder in your project.

The Basics
----------

##### 0. include Mix

```c++
#include "Mix/World.h"
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
class MoveSystem : public Mix::System
{
public:
    MoveSystem()
    {
        // which components an entity must have for the system to be interested
        requireComponent<PositionComponent>();
        requireComponent<VelocityComponent>();
    }

    void update()
    {
        // do stuff with all the entities of interest
        for (auto e : getEntities()) {
            auto &position = e.getComponent<PositionComponent>();
            const auto &velocity = e.getComponent<VelocityComponent>();
            position.x += velocity.x;
            position.y += velocity.y;
        }
    }
};
```

##### 3. create the world and add systems

```c++
Mix::World world;
world.getSystemManager().addSystem<MoveSystem>();
```

##### 4. create entities

```c++
auto e = world.createEntity();
e.addComponent<PositionComponent>(100, 100);
e.addComponent<VelocityComponent>(10, 10);
// entity is created on the next call to world.update(),
// so that no entities appear only for some systems mid-frame
```

##### 5. kill entities

```c++
// inside system's update method
entity.kill();
if (!entity.isAlive()) { ... };
// entity is alive until next call to world.update(),
// so that every system gets the chance handle the entity
```

##### 6. update world and systems in game loop

```c++
while (!done) {
    world.update(); // actually creates and kills entities since the last call to this method
    world.getSystemManager().getSystem<MoveSystem>().update();
}
```

Tags & Groups
-------------

##### 1. tags

```c++
auto player = world.createEntity();
player.tag("player");
if (player.hasTag("player")) { ... };
auto entity = world.getEntity("player");
```

##### 2. groups

```c++
auto enemy = world.createEntity();
enemy.group("enemies");
if (enemy.hasGroup("enemies")) { ... };
auto enemies = world.getEntityGroup("enemies");
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
getWorld().getEventManager().emitEvent<CollisionEvent>(player, enemy);
```

##### 3. receive events

```c++
// inside other system's update method
auto collisionEvents = getWorld().getEventManager().getEvents<CollisionEvent>();
for (auto event : collisionEvents) { // handle collision };

// events can be received by any system
// events exist until the next call to world.update()
```

What else?
----------

Code is fairly well documented. Read and experiment! :)

License
-------
MIT (c) Pär Arvidsson 2014-2018
