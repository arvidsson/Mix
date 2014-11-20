// g++ main.cpp entity/world.cpp entity/entity.cpp entity/system.cpp -o main -std=c++11 -Wall -pedantic
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

class MoveSystem : public System<MoveSystem> {
public:
    MoveSystem() {
        require_component<PositionComponent>();
    }

    virtual void update(float delta) {
        cout << "i moved!" << endl;
    }
};

int main() {
    World world;
    Entity e = world.create_entity();
    e.add_component<PositionComponent>(50, 50);
    world.get_system_manager().add_system<MoveSystem>();

    MoveSystem& ms = world.get_system_manager().get_system<MoveSystem>();
    ms.update(1.0f);

    PositionComponent &pc = e.get_component<PositionComponent>();

    cout << "x: " << pc.x << ", y: " << pc.y << endl;

    e.remove();

    cout << "pos id: " << PositionComponent::get_type_id() << endl;
    cout << "vel id: " << VelocityComponent::get_type_id() << endl;

    return 0;
}
