#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <vector>
#include "Vector2d.h"
#include "Body.h"
#include "Collider.h"
#include "Config.h"

class PhysicsEngine {
    std::vector<Body*> bodies;
    Vector2d gravity;
    float initialEnergy;
    bool checkCollision(Body* b1, Body* b2);
    void handleCollision(Body* b1, Body* b2);
    void handleWallCollision(Body* b);
    bool checkAABB(Body* b1, Body* b2);
public:
    PhysicsEngine(const Vector2d& gravity = Vector2d(0, 9.81)) : gravity(gravity), initialEnergy(0) {}
    void addBody(Body* body) {bodies.push_back(body); initialEnergy=getTotalEnergy();}
    void update(float dt);
    std::vector<Body*> getBodies() const { return bodies; }
    float getTotalEnergy() const;
    float getInitialEnergy() const {return initialEnergy;}
};

#endif