#include "Body.h"

void Body::setMass(float m){
    if(m <= 0.0f) return;
   
    mass = m;
    if(invMass != 0.0f){
        invMass = 1.0f / mass;
    }
}

void Body::setStatic(bool isStatic){
    if(isStatic){
        invMass = 0;
        vel = {0.0f, 0.0f};
    }else{
        invMass = 1.0f/mass;
    }
}

void Body::update(float dt){
    if(invMass == 0.0f) return;

    Vector2d a = force*invMass;
    vel += a*dt;
    pos += vel*dt;
    clearForces();

    trail.push_back(pos);
    if(trail.size() > maxTrailLength){
        trail.pop_front();
    }
}

Body* Body::clone() const {
    Collider* newCol = nullptr;
    if(collider->shapeType == CIRCLE){
        newCol = new CircleCollider(*static_cast<CircleCollider*>(collider));
    }else if(collider->shapeType == BOX){
        newCol = new BoxCollider(*static_cast<BoxCollider*>(collider));
    }

    Body* newBody = new Body(pos, vel, mass, restitution, charge, newCol);
    return newBody;
}