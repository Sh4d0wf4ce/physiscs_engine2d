#include "PhysicsEngine.h"

void PhysicsEngine::step(float dt) {
    for (Body* body : bodies) {
        if (body->getInvMass() == 0) continue;

        body->applyForce(gravity * body->getMass());
        Vector2d acceleration = body->force * body->getInvMass();
        body->vel += acceleration * dt;
        body->pos += body->vel * dt;
        body->clearForces();
    }
}