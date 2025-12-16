#ifndef BODY_H
#define BODY_H

#include "Vector2d.h"

class Body {
    float invMass;
    float mass;
public:
    Vector2d pos;
    Vector2d vel;
    Vector2d force;
    float restitution;
    float charge;

    Body(const Vector2d& pos = Vector2d(0, 0), const Vector2d& vel = Vector2d(0, 0), float mass = 1, float restitution = 1, float charge = 0)
        : pos(pos), vel(vel), force(0, 0), restitution(restitution), charge(charge) { setMass(mass);}
    
    void applyForce(const Vector2d& f) {force += f;}
    void clearForces() {force = Vector2d(0, 0);}
    void update(float dt);

    void setMass(float m);
    float getMass() const { return mass; }
    float getInvMass() const { return invMass; }
};

#endif