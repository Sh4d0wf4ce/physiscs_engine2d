#include "PhysicsEngine.h"

void PhysicsEngine::update(float dt){
    //add forces
    for (Body* body : bodies) {
        if (body->getInvMass() == 0) continue;
        if(Config::useGravity) body->applyForce(Vector2d({0, -Config::gravity}) * body->getMass());
    }

    applyNBodyForces();

    //update bodies
    for(Body* body: bodies){
        if (body->getInvMass() == 0) continue;
        body->update(dt);
    }

    //check for collisions
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
            // Gravity
            if(Config::useNBodyGravity){
                float fGrav = (Config::G * b1->getMass() * b2->getMass()) / distSqr;
                force += r.normalize() * fGrav;
            }

            // Electrostatics
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
    // Positional correction
    // Move bodies apart so they no longer overlap
    // Weighted by mass (heavier bodies move less)
    Vector2d corr = m.normal * (m.depth/(b1->getInvMass() + b2->getInvMass()));
    b1->pos -= corr * b1->getInvMass();
    b2->pos += corr * b2->getInvMass();

    // Velocity resolution
    // Calculate relative velocity along the collision normal
    Vector2d relativeVel = b2->vel - b1->vel;
    float dirV = relativeVel.dot(m.normal);

    // If bodies are moving apart, do nothing
    if (dirV > 0) return;
    
    // Calculate restitution - take the smaller of the two
    float e = std::min(b1->restitution, b2->restitution);

    // Calculate impulse scalar, formula derived from convervation of momentum
    float j = -(1+e) * dirV / (b1->getInvMass() + b2->getInvMass());

    Vector2d impulse = m.normal * j;

    // Apply impulse to the velocities
    b1->vel -= impulse * b1->getInvMass();
    b2->vel += impulse * b2->getInvMass();
}

CollisionManifold PhysicsEngine::checkCollision(Body* b1, Body* b2){
    ShapeType type1 = b1->collider->shapeType;
    ShapeType type2 = b2->collider->shapeType;
    
    // Determine collision types and call appropriate function
    if(type1 == CIRCLE && type2 == CIRCLE){
        return interesectCircleCircle(b1, b2);
    }else if(type1 == BOX && type2 == BOX){
        return intersectBoxBox(b1, b2);
    }else if(type1 == BOX && type2 == CIRCLE){
        // Intersect function expects (Circle, Box), so we swap arguments
        return interesectCircleBox(b2, b1);
    }else if(type1 == CIRCLE && type2 == BOX){
        CollisionManifold m = interesectCircleBox(b1, b2);
        // The intersect function returns normal pointing from box to circle.
        // We need normal b1 -> b2, so we invert it.
        m.normal *= -1.0f;
        return m;
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

    // Calculate overlap on X axis
    float dx = b2->pos.x - b1->pos.x;
    float overlapX = (coll1->width + coll2->width)/2 - std::abs(dx);
    if(overlapX <= 0) return CollisionManifold{false, Vector2d(0,0), 0};

    // Calculate overlap on Y axis
    float dy = b2->pos.y - b1->pos.y;
    float overlapY = (coll1->height + coll2->height)/2 - std::abs(dy);
    if(overlapY <= 0) return CollisionManifold{false, Vector2d(0,0), 0};

    // Determine the axis of least penetration
    Vector2d normal;
    if(overlapX < overlapY){
        normal = Vector2d(dx > 0 ? 1 : -1, 0);
    }else{
        normal = Vector2d(0, dy > 0 ? 1 : -1);
    }

    return CollisionManifold{true, normal, std::min(overlapX, overlapY)};
}

CollisionManifold PhysicsEngine::interesectCircleBox(Body* circle, Body* box){
    CollisionManifold m{false, {0,0}, 0.0f};

    BoxCollider* boxColl = static_cast<BoxCollider*>(box->collider);
    CircleCollider* circleColl = static_cast<CircleCollider*>(circle->collider);

    Vector2d circlePos = circle->pos;
    Vector2d boxPos = box->pos;
    Vector2d relativePos = circlePos - boxPos;

    float hx = boxColl->width / 2.0f;
    float hy = boxColl->height / 2.0f;

    // Find the closest point on the box to the circle center
    float clampedX = std::clamp(relativePos.x, -hx, hx);
    float clampedY = std::clamp(relativePos.y, -hy, hy);

    Vector2d closestPoint(clampedX, clampedY);
    Vector2d normal = relativePos - closestPoint; // Vector from closest point to circle center

    float dist = normal.length();
    float r = circleColl->r;

    // Check if circle center is inside the box
    bool inside = (relativePos == closestPoint);

    // Collision occurs when circle is inside the box or distance to closest point is less than radius
    if(inside){
        m.isColliding = true;

        // Find the nearest edge to push the circle out
        float dx = std::abs(relativePos.x) - hx;
        float dy = std::abs(relativePos.y) - hy;

        if(dx > dy){
            m.normal = (relativePos.x > 0) ? Vector2d(1,0) : Vector2d(-1, 0);
            m.depth = r - dx;
        }else{
            m.normal = (relativePos.y > 0) ? Vector2d(0, 1) : Vector2d(0, -1);
            m.depth = r - dy;
        }
    }else if(dist < r){
        m.isColliding = true;
        m.normal = (dist != 0) ? (normal / dist) : Vector2d(0, 1);
        m.depth = r - dist;
    }

    return m;
}


void PhysicsEngine::handleWallCollision(Body* b){
    float halfWidth = 0.0f;
    float halfHeight = 0.0f;

    if (b->collider->shapeType == CIRCLE) {
        float r = static_cast<CircleCollider*>(b->collider)->r;
        halfWidth = r;
        halfHeight = r;
    } else if (b->collider->shapeType == BOX) {
        BoxCollider* box = static_cast<BoxCollider*>(b->collider);
        halfWidth = box->width / 2.0f;
        halfHeight = box->height / 2.0f;
    }
    
    // Check collision with left and right walls
    if (b->pos.x - halfWidth < -simWidth / 2.0f) {
        b->pos.x = -simWidth / 2.0f + halfWidth;
        b->vel.x *= -1;
        b->vel *= b->restitution;
    }else if (b->pos.x + halfWidth > simWidth / 2.0f) {
        b->pos.x = simWidth / 2.0f - halfWidth;
        b->vel.x *= -1;
        b->vel *= b->restitution;
    }

    // Check collision with top and bottom walls
    if (b->pos.y + halfHeight > simHeight / 2.0f) {
        b->pos.y = simHeight / 2.0f - halfHeight;
        b->vel.y *= -1;
        b->vel *= b->restitution;
    }else if (b->pos.y - halfHeight < -simHeight / 2.0f) {
        b->pos.y = -simHeight / 2.0f + halfHeight;
        b->vel.y *= -1;
        b->vel *= b->restitution;
    }
}


Body* PhysicsEngine::findBodyAt(Vector2d pos){
    // Iterate in reverse order to select topmost body
    for (auto it = bodies.rbegin(); it != bodies.rend(); it++) {
        Body* b = *it;
        Collider* col = b->collider;
        float padding = 5.0f; // Extra padding for easier selection

        if (col->shapeType == CIRCLE) {
            float r = static_cast<CircleCollider*>(col)->r;
            float rHit = r + padding;

            Vector2d diff = pos - b->pos;
            if (diff.lengthSquared() <= (rHit * rHit)) {
                return b;
            }
        } 
        else if (col->shapeType == BOX) {
            BoxCollider* box = static_cast<BoxCollider*>(col);
            float w = box->width;
            float h = box->height;
            
            float halfW = (w / 2.0f) + padding;
            float halfH = (h / 2.0f) + padding;

            if (pos.x >= b->pos.x - halfW && pos.x <= b->pos.x + halfW &&
                pos.y >= b->pos.y - halfH && pos.y <= b->pos.y + halfH) {
                return b;
            }
        }
    }

    return nullptr;
}

void PhysicsEngine::removeBody(Body* body){
    auto it = std::find(bodies.begin(), bodies.end(), body);
    if(it != bodies.end()){
        delete *it;
        bodies.erase(it);
    }
}

void PhysicsEngine::clearBodies(){
    for(Body* b: bodies) delete b;
    bodies.clear();
}