#include "PhysicsEngine.h"

void PhysicsEngine::update(float dt){
    for (Body* body : bodies) {
        if (body->getInvMass() == 0) continue;

        body->applyForce(gravity * body->getMass());
        Vector2d d = Vector2d(800, 600) - body->pos;
        // body->applyForce(50 * d.normalize());
    }

    for(Body* body: bodies){
        body->update(dt);
    }

    for(int i = 0; i < bodies.size(); i++){
        handleWallCollision(bodies[i]);
        for(int j = i+1; j < bodies.size(); j++){
            if(checkCollision(bodies[i], bodies[j])){
                // std::cout<<"Collision\n";
                handleCollision(bodies[i], bodies[j]);
            }
        }
    }
}

void PhysicsEngine::handleCollision(Body* b1, Body* b2){
    CircleCollider* coll1 = static_cast<CircleCollider*>(b1->collider);
    CircleCollider* coll2 = static_cast<CircleCollider*>(b2->collider);

    Vector2d dir = b2->pos - b1->pos;
    float penetration = coll1->getRadius() + coll2->getRadius() - dir.length();
    dir.normalize();
    float totalInvmass = b1->getInvMass() + b2->getInvMass();   
     
    Vector2d corr = dir * (penetration/totalInvmass);
    b1->pos -= corr * b1->getInvMass();
    b2->pos += corr * b2->getInvMass();

    Vector2d relativeVel = b2->vel - b1->vel;
    float dirV = relativeVel.dot(dir);
    if (dirV > 0) return;
    
    float e = std::min(b1->restitution, b2->restitution);
    float j = -(1+e) * dirV;
    j /= totalInvmass;

    Vector2d impulse = dir * j;

    b1->vel -= impulse * b1->getInvMass();
    b2->vel += impulse * b2->getInvMass();
}

void PhysicsEngine::handleWallCollision(Body* b){
    CircleCollider* coll = static_cast<CircleCollider*>(b->collider);
    if(b->pos.x < coll->getRadius()){
        b->pos.x = coll->getRadius();
        b->vel.x *= -1;
    }
    if(b->pos.x > 800 - coll->getRadius()){
        b->pos.x = 800 - coll->getRadius();
        b->vel.x *= -1;
    }
    if(b->pos.y > 600 - coll->getRadius()){
        b->pos.y = 600 - coll->getRadius();
        b->vel.y *= -1;
    }
    if(b->pos.y < coll->getRadius()){
        b->pos.y = coll->getRadius();
        b->vel.y *= -1;
    }
}

bool PhysicsEngine::checkCollision(Body* b1, Body* b2){
    if(b1->collider->shapeType == CIRCLE && b2->collider->shapeType == CIRCLE){
        CircleCollider* coll1 = static_cast<CircleCollider*>(b1->collider);
        CircleCollider* coll2 = static_cast<CircleCollider*>(b2->collider);

        Vector2d dist = b2->pos - b1->pos;
        float radiusSum = coll1->getRadius() + coll2->getRadius();
        if(dist.lengthSquared() < radiusSum * radiusSum){
            return true;
        }
    }

    return false;
}