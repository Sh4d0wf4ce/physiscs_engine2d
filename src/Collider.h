#ifndef COLLIDER_H
#define COLLIDER_H

enum ShapeType{
    CIRCLE,
    BOX,
};

struct Collider{
    ShapeType shapeType;
    Collider(ShapeType type): shapeType(type) {}
    virtual ~Collider() {}
};

struct CircleCollider : public Collider {
    float r;
    CircleCollider(float r) : Collider(CIRCLE), r(r) {}
};

struct BoxCollider: public Collider{
    float width;
    float height;
    BoxCollider(float width, float height): Collider(BOX), width(width), height(height) {}
};


#endif