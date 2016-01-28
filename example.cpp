#include "entity/World.hpp"
#include <iostream>
using namespace entity;

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

class MoveSystem : public System
{
public:
    MoveSystem()
    {
        RequireComponent<PositionComponent>();
        RequireComponent<VelocityComponent>();
    }

    void Update()
    {
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
    World world;
    auto e = world.CreateEntity();
    e.AddComponent<PositionComponent>(100, 100);
    e.AddComponent<VelocityComponent>(10, 10);
    world.GetSystemManager().AddSystem<MoveSystem>();

    for (int i = 0; i < 10; i++) {
        world.Update();
        world.GetSystemManager().GetSystem<MoveSystem>().Update();
        auto &position = e.GetComponent<PositionComponent>();
        std::cout << "x: " << position.x << ", y: " << position.y << std::endl;
    }

    return 0;
}
