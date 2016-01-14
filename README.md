entity
======

A minimal [entity-component system](https://en.wikipedia.org/wiki/Entity_component_system).

the basics
----------

##### 0. include entity

```c++
#include "entity/world.hpp"
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
```

##### 5. update world and systems in game loop

```c++
while (!done) {
    world.Update();
    world.GetSystemManager().GetSystem<MoveSystem>().Update();
}
```

events
----------

todo: write this
