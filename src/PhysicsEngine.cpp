#include "PhysicsEngine.h"

void PhysicsEngine::update(float dt){
    for (Body* body : bodies) {
        if (body->getInvMass() == 0) continue;

        if(Config::useGravity) body->applyForce(gravity * body->getMass());
        // Vector2d d = Vector2d(400, 300) - body->pos;
        // body->applyForce(50 * d.normalize());
        // if(body != bodies[0]){
        //     Vector2d d = bodies[0]->pos - body->pos;
        //     body->applyForce(50 * d.normalize());
        // }
    }

    for(Body* body: bodies){
        body->update(dt);
    }

    for(unsigned int i = 0; i < bodies.size(); i++){
        if(Config::useWindowCollision) handleWallCollision(bodies[i]);
        for(unsigned int j = i+1; j < bodies.size(); j++){
            if(checkCollision(bodies[i], bodies[j])){
                handleCollision(bodies[i], bodies[j]);
            }
        }
    }
}

void PhysicsEngine::handleCollision(Body* b1, Body* b2){
    CircleCollider* coll1 = static_cast<CircleCollider*>(b1->collider);
    CircleCollider* coll2 = static_cast<CircleCollider*>(b2->collider);

    Vector2d dir = b2->pos - b1->pos;
    float penetration = coll1->r + coll2->r - dir.length();
    dir.normalize();
    float totalInvmass = b1->getInvMass() + b2->getInvMass();   
     
    Vector2d corr = dir * (penetration/totalInvmass);
    b1->pos -= corr * b1->getInvMass();
    b2->pos += corr * b2->getInvMass();

    Vector2d relativeVel = b2->vel - b1->vel;
    float dirV = relativeVel.dot(dir);
    if (dirV > 0) return;
    
    float e = std::min(b1->restitution, b2->restitution);
    float j = -(1+e) * dirV / totalInvmass;

    Vector2d impulse = dir * j;

    b1->vel -= impulse * b1->getInvMass();
    b2->vel += impulse * b2->getInvMass();
}

void PhysicsEngine::handleWallCollision(Body* b){
    CircleCollider* coll = static_cast<CircleCollider*>(b->collider);
    if(b->pos.x < coll->r){
        b->pos.x = coll->r;
        b->vel.x *= -1;
        b->vel *= b->restitution;
    }
    if(b->pos.x > Config::WINDOW_WIDTH - coll->r){
        b->pos.x = Config::WINDOW_WIDTH - coll->r;
        b->vel.x *= -1;
        b->vel *= b->restitution;
    }
    if(b->pos.y > Config::WINDOW_HEIGHT - coll->r){
        b->pos.y = Config::WINDOW_HEIGHT - coll->r;
        b->vel.y *= -1;
        b->vel *= b->restitution;
    }
    if(b->pos.y < coll->r){
        b->pos.y = coll->r;
        b->vel.y *= -1;
        b->vel *= b->restitution;
    }
}

bool PhysicsEngine::checkCollision(Body* b1, Body* b2){
    if(b1->collider->shapeType == CIRCLE && b2->collider->shapeType == CIRCLE){
        CircleCollider* coll1 = static_cast<CircleCollider*>(b1->collider);
        CircleCollider* coll2 = static_cast<CircleCollider*>(b2->collider);

        Vector2d dist = b2->pos - b1->pos;
        float radiusSum = coll1->r + coll2->r;
        if(dist.lengthSquared() < radiusSum * radiusSum){
            return true;
        }
    }

    return false;
}

bool PhysicsEngine::checkAABB(Body* b1, Body* b2){
    BoxCollider* box1 = static_cast<BoxCollider*>(b1->collider);
    BoxCollider* box2 = static_cast<BoxCollider*>(b2->collider);

    float dx = std::abs(b1->pos.x - b2->pos.x);
    float minDistX = (box1->width + box2->width)/2.0f;
    if(dx >= minDistX) return false;

    
    float dy = std::abs(b1->pos.y - b2->pos.y);
    float minDistY = (box1->height + box2->height)/2.0f;
    if(dy >= minDistY) return false;

    return true;
}

float PhysicsEngine::getTotalEnergy() const{
    float energy = 0;
    for(Body* b: bodies){
        energy += b->getMass()/2 * b->vel.dot(b->vel);
    }
    return energy;
}