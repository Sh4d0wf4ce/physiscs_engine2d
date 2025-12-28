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

    void handleWallCollision(Body* b);
    void handleCollision(Body* b1, Body* b2, const CollisionManifold& m);

    CollisionManifold checkCollision(Body* b1, Body* b2);
    CollisionManifold interesectCircleCircle(Body* b1, Body* b2);
    CollisionManifold interesectCircleBox(Body* circle, Body* box);
    CollisionManifold intersectBoxBox(Body* b1, Body* b2);

    void applyNBodyForces();

public:
    PhysicsEngine(const Vector2d& gravity = Vector2d(0, 9.81)) : gravity(gravity) {}
    void addBody(Body* body) {bodies.push_back(body);}
    void update(float dt);
    std::vector<Body*> getBodies() const { return bodies; }
    float getKineticEnergy() const;
    float getPotentialEnergy() const;
    float getTotalEnergy() const {return getKineticEnergy() + getPotentialEnergy();}
    Vector2d getTotalMomentum() const;
};

#endif