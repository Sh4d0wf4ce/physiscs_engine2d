#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <vector>
#include "Vector2d.h"
#include "Body.h"
#include "Collider.h"
#include "CircleCollider.h"

class PhysicsEngine {
    std::vector<Body*> bodies;
    Vector2d gravity;
    bool checkCollision(Body* b1, Body* b2);
    void handleCollision(Body* b1, Body* b2);
    void handleWallCollision(Body* b);
public:
    PhysicsEngine(const Vector2d& gravity = Vector2d(0, 9.81)) : gravity(gravity) {}
    void addBody(Body* body) {bodies.push_back(body);}
    void update(float dt);
    std::vector<Body*> getBodies() const { return bodies; }
};

#endif