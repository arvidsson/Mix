// g++ main.cpp entity/world.cpp entity/entity.cpp entity/system.cpp -o main -std=c++11
#include "entity/world.hpp"
#include <iostream>
using namespace entity;
using namespace std;

struct PositionComponent : Component<PositionComponent> {
    PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
    int x, y;
};

struct VelocityComponent : Component<VelocityComponent> {
    VelocityComponent(int dx = 0, int dy = 0) : dx(dx), dy(dy) {}
    int dx, dy;
};

int main() {
    World world;
    Entity e = world.create_entity();
    e.add_component<PositionComponent>(50, 50);

    PositionComponent &pc = e.get_component<PositionComponent>();

    cout << "x: " << pc.x << ", y: " << pc.y << endl;

    e.remove();

    cout << "pos id: " << PositionComponent::get_type_id() << endl;
    cout << "vel id: " << VelocityComponent::get_type_id() << endl;

    return 0;
}
