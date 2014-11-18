#include "entity/world.hpp"
#include <iostream>
using namespace entity;
using namespace std;

struct PositionComponent : Component<PositionComponent> {
    PositionComponent(int x = 0, int y = 0) : x(x), y(y) {}
    int x, y;
};

int main() {
    World world;
    Entity e = world.create_entity();
    e.add_component<PositionComponent>(50, 50);

    PositionComponent &pc = e.get_component<PositionComponent>();

    cout << "x: " << pc.x << ", y: " << pc.y << endl;

    e.kill();

    return 0;
}
