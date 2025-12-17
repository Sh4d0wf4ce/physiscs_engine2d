#include "PhysicsEngine.h"

void PhysicsEngine::update(float dt){
    for (Body* body : bodies) {
        if (body->getInvMass() == 0) continue;

        body->applyForce(gravity * body->getMass());
        Vector2d d = Vector2d(800, 600) - body->pos;
        body->applyForce(50 * d.normalize());
    }

    for(Body* body: bodies){
        body->update(dt);
    }

    //#TODO
    //Collision detection
}