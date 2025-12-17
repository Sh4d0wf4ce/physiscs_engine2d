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
}