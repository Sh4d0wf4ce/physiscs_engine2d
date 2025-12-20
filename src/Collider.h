#ifndef COLLIDER_H
#define COLLIDER_H

enum ShapeType{
    CIRCLE,
    BOX,
};

class Collider{
public:
    ShapeType shapeType;
    Collider(ShapeType type): shapeType(type) {}
    virtual ~Collider() {}
};

#endif