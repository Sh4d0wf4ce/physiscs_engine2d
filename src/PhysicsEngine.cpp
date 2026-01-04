#include "PhysicsEngine.h"

void PhysicsEngine::update(float dt){
    //add forces
    for (Body* body : bodies) {
        if (body->getInvMass() == 0) continue;
        if(Config::useGravity) body->applyForce(gravity * body->getMass());
    }

    applyNBodyForces();


    //update bodies
    for(Body* body: bodies){
        body->update(dt);
    }

    //check for collsions
    for(unsigned int i = 0; i < bodies.size(); i++){
        if(Config::useWindowCollision) handleWallCollision(bodies[i]);
        if(!Config::useBodiesCollision) continue;
        for(unsigned int j = i+1; j < bodies.size(); j++){
            Body* b1 = bodies[i];
            Body* b2 = bodies[j];

            if(b1->getInvMass() == 0 && b2->getInvMass() == 0) continue;
            CollisionManifold m = checkCollision(b1, b2);
            if(m.isColliding){
                handleCollision(b1, b2, m);
            }
        }
    }
}

void PhysicsEngine::applyNBodyForces(){
    if(!Config::useNBodyGravity && !Config::useElectrostatics) return;

    for(int i = 0; i < bodies.size(); i++){
        for(int j = i+1; j < bodies.size(); j++){
            Body* b1 = bodies[i];
            Body* b2 = bodies[j];

            if(b1->getInvMass() == 0 && b2->getInvMass() == 0) continue;
            Vector2d r = b2->pos - b1->pos;
            float distSqr = r.lengthSquared();
            if(distSqr == 0) continue;

            Vector2d force(0,0);
            if(Config::useNBodyGravity){
                float fGrav = (Config::G * b1->getMass() * b2->getMass()) / distSqr;
                force += r.normalize() * fGrav;
            }

            if(Config::useElectrostatics){
                float fElec = -(Config::K * b1->charge * b2->charge) / distSqr;
                force += r.normalize() * fElec;
            }
    
            b1->applyForce(force);
            b2->applyForce(-1 * force);
        }
    }
}

void PhysicsEngine::handleCollision(Body* b1, Body* b2, const CollisionManifold& m){
    Vector2d corr = m.normal * (m.depth/(b1->getInvMass() + b2->getInvMass()));
    b1->pos -= corr * b1->getInvMass();
    b2->pos += corr * b2->getInvMass();

    Vector2d relativeVel = b2->vel - b1->vel;
    float dirV = relativeVel.dot(m.normal);
    if (dirV > 0) return;
    
    float e = std::min(b1->restitution, b2->restitution);
    float j = -(1+e) * dirV / (b1->getInvMass() + b2->getInvMass());

    Vector2d impulse = m.normal * j;

    b1->vel -= impulse * b1->getInvMass();
    b2->vel += impulse * b2->getInvMass();
}

CollisionManifold PhysicsEngine::checkCollision(Body* b1, Body* b2){
    ShapeType type1 = b1->collider->shapeType;
    ShapeType type2 = b2->collider->shapeType;
    
    if(type1 == CIRCLE && type2 == CIRCLE){
        return interesectCircleCircle(b1, b2);
    }else if((type1 == BOX && type2 == CIRCLE) || (type1 == CIRCLE && type2 == BOX)){
        Body* box;
        Body* circle;
        
        if(b1->collider->shapeType == BOX){
            box = b1;
            circle = b2;
        }else{
            box = b2;
            circle = b1;
        }

        return interesectCircleBox(circle, box);
    }else if(type1 == BOX && type2 == BOX){
        return intersectBoxBox(b1, b2);
    }

    return CollisionManifold{false, Vector2d(0,0), 0};
}

CollisionManifold PhysicsEngine::interesectCircleCircle(Body* b1, Body* b2){
    CircleCollider* coll1 = static_cast<CircleCollider*>(b1->collider);
    CircleCollider* coll2 = static_cast<CircleCollider*>(b2->collider);

    Vector2d dist = b2->pos - b1->pos;
    float radiusSum = coll1->r + coll2->r;
    float distLen = dist.length();

    if(distLen >= radiusSum)
        return CollisionManifold{false, Vector2d(0,0), 0};

    return CollisionManifold{true, dist.normalize(), radiusSum - distLen};
}

CollisionManifold PhysicsEngine::intersectBoxBox(Body* b1, Body* b2){
    BoxCollider* coll1 = static_cast<BoxCollider*>(b1->collider);
    BoxCollider* coll2 = static_cast<BoxCollider*>(b2->collider);

    float dx = b2->pos.x - b1->pos.x;
    float overlapX = (coll1->width + coll2->width)/2 - std::abs(dx);
    if(overlapX <= 0) return CollisionManifold{false, Vector2d(0,0), 0};

    float dy = b2->pos.y - b1->pos.y;
    float overlapY = (coll1->height + coll2->height)/2 - std::abs(dy);
    if(overlapY <= 0) return CollisionManifold{false, Vector2d(0,0), 0};

    Vector2d normal;
    if(overlapX < overlapY){
        normal = Vector2d(dx > 0 ? 1 : -1, 0);
    }else{
        normal = Vector2d(0, dy > 0 ? 1 : -1);
    }

    return CollisionManifold{true, normal, std::min(overlapX, overlapY)};
}

CollisionManifold PhysicsEngine::interesectCircleBox(Body* circle, Body* box){
    BoxCollider* boxColl = static_cast<BoxCollider*>(box->collider);
    CircleCollider* circleColl = static_cast<CircleCollider*>(circle->collider);

    Vector2d circlePos = circle->pos;
    Vector2d boxPos = box->pos;

    float minX = boxPos.x - boxColl->width / 2;
    float maxX = boxPos.x + boxColl->width / 2;
    float minY = boxPos.y - boxColl->height / 2;
    float maxY = boxPos.y + boxColl->height / 2;

    Vector2d closestPoint;
    closestPoint.x = std::clamp(circlePos.x, minX, maxX);
    closestPoint.y = std::clamp(circlePos.y, minY, maxY);

    Vector2d dist = circlePos - closestPoint;
    if(dist.length() > circleColl->r)
        return CollisionManifold{false, Vector2d(0,0), 0};
    
    float distLen = dist.length();
    CollisionManifold m;
    m.isColliding = true;

    if(distLen == 0.0f){
        float dLeft = circlePos.x - minX;
        float dRight = maxX - circlePos.x;
        float dTop = circlePos.y - minY;
        float dBottom = maxY - circlePos.y;

        float minDistX = std::min(dLeft, dRight);
        float minDistY = std::min(dTop, dBottom);

        if(minDistX < minDistY){
            m.depth = circleColl->r + minDistX;
            m.normal = (dLeft < dRight) ? Vector2d(1, 0) : Vector2d(-1, 0);
        }else{
            m.depth = circleColl->r + minDistY;
            m.normal = (dTop < dBottom) ? Vector2d(0, 1) : Vector2d(0, -1);
        }
    }else{
        m.normal = dist.normalize();
        m.depth = circleColl->r - distLen;
    }

    return m;
}


void PhysicsEngine::handleWallCollision(Body* b){
    CircleCollider* coll = static_cast<CircleCollider*>(b->collider);
    if(b->pos.x - coll->r < -simWidth/2.0f){
        b->pos.x = -simWidth/2.0f + coll->r;
        b->vel.x *= -1;
        b->vel *= b->restitution;
    }
    if(b->pos.x + coll->r > simWidth/2.0f){
        b->pos.x = simWidth/2.0f - coll->r;
        b->vel.x *= -1;
        b->vel *= b->restitution;
    }
    if(b->pos.y + coll->r > simHeight/2.0f){
        b->pos.y = simHeight/2.0f - coll->r;
        b->vel.y *= -1;
        b->vel *= b->restitution;
    }
    if(b->pos.y - coll->r < -simHeight/2.0f){
        b->pos.y = -simHeight/2.0f + coll->r;
        b->vel.y *= -1;
        b->vel *= b->restitution;
    }
}

float PhysicsEngine::getKineticEnergy() const{
    float energy = 0.0f;
    for(const Body* body: bodies){
        if(body->getInvMass() == 0) continue;
        energy += 0.5f * body->getMass() * body->vel.lengthSquared();
    }
    return energy;
}

float PhysicsEngine::getPotentialEnergy() const{
    float energy = 0.0f;
    
    if(Config::useGravity){
        for(const Body* body: bodies){
            if(body->getInvMass() == 0) continue;
            energy -= body->getMass() * gravity.y * body->pos.y;
            energy -= body->getMass() *  gravity.x * body->pos.x;
        }
    }

    if(Config::useNBodyGravity || Config::useElectrostatics){
        for (int i = 0; i < bodies.size(); i++)
        {
            for (int j = i + 1; j < bodies.size(); j++)
            {
                Body* b1 = bodies[i];
                Body* b2 = bodies[j];

                if(b1->getInvMass() == 0 && b2->getInvMass() == 0) continue;
                Vector2d r = b2->pos - b1->pos;
                float dist = r.length();
                dist = std::max(dist, 0.001f);

                if (Config::useNBodyGravity) {
                    energy -= (Config::G * b1->getMass() * b2->getMass()) / dist;
                }
                
                if(Config::useElectrostatics){
                    energy += (Config::K * b1->charge * b2->charge) / dist;
                }
            }
            
        }
        
    }  

    return energy;
}

Vector2d PhysicsEngine::getTotalMomentum() const{
    Vector2d totalMomentum(0,0);
    for(const Body* body: bodies){
        if(body->getInvMass() == 0) continue;
        totalMomentum += body->vel * body->getMass();
    }
    return totalMomentum;
}