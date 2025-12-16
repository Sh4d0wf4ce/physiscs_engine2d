#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <vector>
#include "Vector2d.h"
#include "Body.h"

class PhysicsEngine {
public:
    std::vector<Body*> bodies;
    Vector2d gravity;

    PhysicsEngine(const Vector2d& gravity = Vector2d(0, 9.81)) : gravity(gravity) {}
    void addBody(Body* body) {bodies.push_back(body);}
    void step(float dt);
};

#endif