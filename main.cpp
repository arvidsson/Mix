#include "entity/world.hpp"
#include <iostream>
using namespace entity;
using namespace std;

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
    }

    void Update()
    {
        cout << "I moved!" << endl;
    }
};

int main()
{
    World world;
    auto e = world.CreateEntity();
    e.AddComponent<PositionComponent>(50, 50);
    world.GetSystemManager().AddSystem<MoveSystem>();

    world.Update();
    auto& moveSystem = world.GetSystemManager().GetSystem<MoveSystem>();
    moveSystem.Update();

    auto &position = e.GetComponent<PositionComponent>();

    cout << "x: " << position.x << ", y: " << position.y << endl;

    return 0;
}
