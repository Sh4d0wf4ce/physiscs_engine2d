#ifndef CIRCLECOLLIDER_H
#define CIRCLECOLLIDER_H

#include "Collider.h"

class CircleCollider : public Collider {
    float radius;
public:
    CircleCollider(float r) : Collider(CIRCLE), radius(r) {}
    float getRadius() const { return radius; }
};

#endif