#ifndef COLLIDER_H
#define COLLIDER_H

/// @brief Enum representing the type of the geometric shape of the collider.
enum ShapeType{
    CIRCLE,
    BOX,
};

/// @brief Base structure for all collision shapes.
/// Stores the shape type to allow safe downcasting in the physics engine.
struct Collider{
    /// @brief The type of the shape (CIRCLE or BOX).
    ShapeType shapeType;

    /// @brief Constructor. Sets the shape type.
    Collider(ShapeType type): shapeType(type) {}

    /// @brief Virtual destructor.
    virtual ~Collider() {}
};


/// @brief Represents a circular collider.
struct CircleCollider : public Collider {
    float r; ///< Radius of the circle.

    CircleCollider(float r) : Collider(CIRCLE), r(r) {}
};

/// @brief Represents a box (rectangle) collider.
struct BoxCollider: public Collider{
    float width; ///< Width of the box.
    float height; ///< Height of the box.
    BoxCollider(float width, float height): Collider(BOX), width(width), height(height) {}
};


#endif