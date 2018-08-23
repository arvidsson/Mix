#include "Mix/World.hpp"
#include <iostream>
using namespace Mix;

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
        requireComponent<PositionComponent>();
        requireComponent<VelocityComponent>();
    }

    void update()
    {
        for (auto e : getEntities()) {
            auto &position = e.getComponent<PositionComponent>();
            const auto velocity = e.getComponent<VelocityComponent>();
            position.x += velocity.x;
            position.y += velocity.y;
        }
    }
};

int main()
{
    World world;
    auto e = world.createEntity();
    e.addComponent<PositionComponent>(100, 100);
    e.addComponent<VelocityComponent>(10, 10);
    world.getSystemManager().addSystem<MoveSystem>();

    for (int i = 0; i < 10; i++) {
        world.update();
        world.getSystemManager().getSystem<MoveSystem>().update();
        auto &position = e.getComponent<PositionComponent>();
        std::cout << "x: " << position.x << ", y: " << position.y << std::endl;
    }

    return 0;
}
