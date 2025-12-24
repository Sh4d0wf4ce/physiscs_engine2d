#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <vector>
#include <algorithm>
#include "Vector2d.h"
#include "Body.h"
#include "Collider.h"
#include "Config.h"

struct CollisionManifold{
        bool isColliding;
        Vector2d normal;
        float depth;
    };


class PhysicsEngine {
    std::vector<Body*> bodies;
    Vector2d gravity;
    float initialEnergy;
    void handleWallCollision(Body* b);

    CollisionManifold checkCollision(Body* b1, Body* b2);

    CollisionManifold interesectCircleCircle(Body* b1, Body* b2);
    CollisionManifold interesectCircleBox(Body* circle, Body* box);
    CollisionManifold intersectBoxBox(Body* b1, Body* b2);

    void handleCollision(Body* b1, Body* b2, const CollisionManifold& m);
public:
    PhysicsEngine(const Vector2d& gravity = Vector2d(0, 9.81)) : gravity(gravity), initialEnergy(0) {}
    void addBody(Body* body) {bodies.push_back(body); initialEnergy=getTotalEnergy();}
    void update(float dt);
    std::vector<Body*> getBodies() const { return bodies; }
    float getTotalEnergy() const;
    float getInitialEnergy() const {return initialEnergy;}
};

#endif