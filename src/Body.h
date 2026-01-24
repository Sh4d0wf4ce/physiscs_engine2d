#ifndef BODY_H
#define BODY_H

#include <deque>
#include "Vector2d.h"
#include "Collider.h"

/// @brief  Represents a physical object in the simulation.
/// Holds physical properties (position, velocity, mass, etc.), collision shape, and methods to update its state.
class Body {
    /// @brief Inverse of the mass (1/mass). Stored for performance optimization. Zero indicates a static body.
    float invMass;

    /// @brief Mass of the body.
    float mass;
public:
    /// @brief Current position in world coordinates.
    Vector2d pos;

    /// @brief Current velocity vector.
    Vector2d vel;

    /// @brief Accumulated force acting on the body in the current frame.
    /// @note This force is reset after each update.
    Vector2d force;

    /// @brief Coefficient of restitution (bounciness).
    /// Range: 0.0 (no bounce, inelastic) to 1.0 (perfectly elastic).
    float restitution;

    /// @brief Electric charge of the body for electrostatic interactions.
    float charge;

    /// @brief Collision shape geometry of the body.
    Collider* collider;

    /// @brief Stores the history of positions for trail rendering.
    std::deque<Vector2d> trail = std::deque<Vector2d>();

    /// @brief Maximum length of the trail.
    int maxTrailLength = 100;

    /// @brief Counter to control trail update frequency.
    int trailcounter = 0;


    /// @brief Constructor for the Body class.
    /// @param pos  Initial position.
    /// @param vel  Initial velocity.
    /// @param mass Mass of the body.
    /// @param restitution Coefficient of restitution (bounciness).
    /// @param charge Electric charge.
    /// @param collider Collision shape geometry of the body.
    Body(const Vector2d& pos = Vector2d(0, 0), const Vector2d& vel = Vector2d(0, 0), float mass = 1, float restitution = 1, float charge = 0, Collider* collider = nullptr)
        : invMass(1.0f/mass), mass(mass), pos(pos), vel(vel), force(0, 0), restitution(restitution), charge(charge), collider(collider) {}

    /// @brief Destructor for the Body class.
    ~Body() {delete collider;}

    /// @brief Creates a deep copy of the Body object.
    /// @return A pointer to the cloned Body object.
    Body* clone() const;

    /// @brief Applies a force to the body.
    /// @param f Force vector to apply.
    void applyForce(const Vector2d& f) {force += f;}

    /// @brief Clears all accumulated forces acting on the body.
    void clearForces() {force = Vector2d(0, 0);}

    /// @brief Updates the body's position and velocity based on the accumulated forces and the time step.
    /// Uses Semi-implicit Euler integration.
    /// @param dt Delta time (time step) in seconds.
    void update(float dt);

    /// @brief Sets the mass and recalculates the inverse mass. 
    // If m <= 0 the mass is not changed.
    /// @param m  New mass value.
    void setMass(float m);

    /// @brief Returns the mass of the body.
    float getMass() const {return mass;}

    /// @brief Returns the inverse mass of the body.
    float getInvMass() const {return invMass;}

    /// @brief Checks if the body is static (immovable).
    /// @return True if infinite mass (invMass == 0), false otherwise.
    bool isStatic() const {return invMass == 0;}

    /// @brief Toogles the static state of the body.
    /// @param isStatic If true sets the body to static, otherwise makes it dynamic.
    void setStatic(bool isStatic);
};

#endif