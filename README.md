entity
======

A minimal entity-component system.

example
--------------
```c++
#include "entity/world.hpp"
#include <iostream>
using namespace entity;

// 1. define your components
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

// 2. define your systems
class MoveSystem : public System
{
public:
    MoveSystem()
    {
        // 2a. specify what components an entity must have for the system to be interested in the entity
        RequireComponent<PositionComponent>();
        RequireComponent<VelocityComponent>();
    }

    void Update()
    {
        // 2b. do stuff with all the entities of interest
        for (auto e : GetEntities()) {
            auto &position = e.GetComponent<PositionComponent>();
            const auto velocity = e.GetComponent<VelocityComponent>();
            position.x += velocity.x;
            position.y += velocity.y;
        }
    }
};

int main()
{
    // 3. create the world
    World world;
    
    // 4. add systems to the world
    world.GetSystemManager().AddSystem<MoveSystem>();
    
    // 5. create entities
    auto e = world.CreateEntity();
    e.AddComponent<PositionComponent>(50, 50);
    e.AddComponent<VelocityComponent>(10, 0);
    
    // 6. update the world and use the systems in the game loop
    for (int i = 0; i < 10; i++) {
        world.Update();
        world.GetSystemManager().GetSystem<MoveSystem>().Update();
        auto &position = e.GetComponent<PositionComponent>();
        std::cout << "x: " << position.x << ", y: " << position.y << std::endl;
    }

    return 0;
}
```
