entity
======

A minimal entity-component system.

brief synopsis
--------------
Everything begins with a world.

```c++
#include "entity/world.hpp"

World world;
```

The world creates entities.

```c++
auto e = world.CreateEntity();
```

An entity is basically just an id. Entities can have components.

```c++
struct PositionComponent
{
    PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
    int x, y;
};

e.AddComponent<PositionComponent>(100, 100);
```

The entities interact with the world through systems.

```c++
class MoveSystem : System
{
    MoveSystem()
    {
        RequireComponent<PositionComponent>();
        RequireComponent<VelocityComponent>()
    }

    void Update()
    {
        for (auto e : GetEntities()) {
            auto &pos = e.GetComponent<PositionComponent>();
            const auto vel = e.GetComponent<VelocityComponent>();
            pos.x += vel.x * delta;
            pos.y += vel.y * delta;
        }
    }
};

world.GetSystemManager().AddSystem<MoveSystem>();
```

The system tells the world what components an entity must have (as a minimum) in order for the system to be interested in processing the entity.
Each frame, the system processes all the entities of interest. If the entity is still alive, the system does its thing. If not, then the entity
is removed from the system's list of interest.

Now, we have all we need. Entities, components and systems. The only thing missing is the game loop, or rather, what needs to happen each frame in the game loop.

```c++
world.Update();
auto &moveSystem = world.GetSystemManager().GetSystem<MoveSystem>();
moveSystem.Update();
```

Each frame, the world needs to be updated. This means that the entities that were created and destroyed in the previous frame are *actually* created and destroyed
and thus ready for processing by the systems. Then, as we don't know what exact order you want your systems to process entities, you need to get each system
from the system manager and call their update method manually.
