#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <array>
#include <cstdint>
#include "Body.h"
#include "PhysicsEngine.h"
#include "Config.h"


/// @brief Responsible for rendering the physics simulation onto the SFML window.
/// Handles coordinate transformations, drawing bodies, trails, velocity vectors, and selection highlights.
/// Also manages the camera position within the simulation world.
class Renderer{
    /// @brief Reference to the SFML render window.
    sf::RenderWindow& window;

    /// @brief Current position of the camera in world coordinates.
    Vector2d cameraPos;


    /// @brief Draws a specific body as a circle.
    void drawCircle(const Body& body);

    /// @brief Draws a specific body as a box.
    void drawBox(const Body& body);

    /// @brief Draws the movement history trail of a body.
    void drawTrail(const Body& body);

    /// @brief Draws a vector arrow (e.g., velocity) starting from a given position.
    void drawVector(const Vector2d& start, const Vector2d& vec);
public:
    /// @brief Constructor for the Renderer class.
    /// @param window Reference to the SFML render window.
    Renderer(sf::RenderWindow& window): window(window), cameraPos({0,0}) {}

    /// @brief Main rendering function.
    /// Draws all bodies, trails, vectors, and world bounds based on the current physics engine state.
    /// @param engine Reference to the physics engine containing the simulation data.
    void render(const PhysicsEngine& engine);

    /// @brief Draws a visual highlight around a selected body.
    /// @param body The body to highlight.
    void drawSelection(const Body& body);

    /// @brief Draws a semi-transparent rectangle representing a mouse drag selection area.
    /// @param start Mouse position where the drag started. (world coordinates)
    /// @param current Mouse position where the drag ended. (world coordinates)
    void drawSelectionBox(const Vector2d& start, const Vector2d& current);


    // --- Coordinate Transformations ---

    /// @brief Converts screen coordinates (pixels) to world coordinates (meters).
    /// Takes into account the current camera position and scaling factor.
    /// @param pos Screen coordinates (pixels).
    /// @return World coordinates (meters).
    Vector2d screenToReal(const Vector2d& pos);

    /// @brief Converts world coordinates (meters) to screen coordinates (pixels).
    /// Takes into account the current camera position and scaling factor.
    /// @param pos World coordinates (meters).
    /// @return Screen coordinates (pixels).
    Vector2d realToScreen(const Vector2d& pos);


    // --- Camera Control ---

    /// @brief Sets the camera position in world coordinates.
    void setCameraPos(const Vector2d& pos) {cameraPos = pos;}

    /// @brief Moves the camera relative to its current position.
    /// @param offset Vector offset to move the camera by.
    void moveCamera(const Vector2d& offset) {cameraPos += offset;}

    /// @brief Gets the current camera position in world coordinates.
    Vector2d getCameraPos() const {return cameraPos;}
};

#endif