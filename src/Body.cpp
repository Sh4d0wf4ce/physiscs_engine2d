#include "Body.h"

void Body::setMass(float m) {
    mass = m;
    if (mass <= 0) {
        invMass = 0;
    } else {
        invMass = 1.0f / mass;
    }
}