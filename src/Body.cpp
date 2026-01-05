#include "Body.h"

void Body::setMass(float m){
    mass = m;
    if (mass <= 0) {
        invMass = 0;
    } else {
        invMass = 1.0f / mass;
    }
}

void Body::update(float dt){
    if(invMass == 0) return;

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