#ifndef CONFIG_H
#define CONFIG_H

#include <SFML/Graphics.hpp>

/// @brief Global configuration settings for the physics engine and renderer.
/// Contains default values for physics parameters, rendering toggles, and color schemes.
struct Config
{
    // ---PHYSICS CONSTANTS---
    /// @name Physics Constants
    /// @{

    /// @brief Pixels per meter ratio. Used to convert physics units to screen coordinates.
    static inline float scale = 1.0f;

    /// @brief Global gravity acceleration (downward force).
    static inline float gravity = 100.0f;

    /// @brief Gravitational constant for N-body gravitational interactions.
    /// Arbitrary value scaled for better visualization.
    static inline float G = 1000.0f;

    /// @brief Coulomb's constant for electrostatic interactions.
    /// Arbitrary value scaled for better visualization.
    static inline float K = 10000.0f;
    /// @}


    // ---COLLISION TOGGLES---
    /// @name Collision Toggles
    /// @{
    static inline bool useWindowCollision = true; ///< Bounce off simulation boundaries.
    static inline bool useBodiesCollision = true; ///< Bounce off other bodies.
    /// @}

    // ---FORCE TOGGLES---
    /// @name Force Toggles
    /// @{
    static inline bool useGravity = false; ///< Apply global downward gravity.
    static inline bool useNBodyGravity = true; ///< Apply N-body gravitational attraction.
    static inline bool useElectrostatics = true; ///< Apply electrostatic forces between charged bodies.
    /// @}

    // ---RENDERING TOGGLES---
    /// @name Rendering Toggles
    /// @{
    static inline bool renderTrails = true; ///< Render trails behind moving bodies.
    static inline bool renderVelocityVectors = true; ///< Render velocity vectors for bodies.
    static inline bool renderWorldBounds = true; ///< Render the simulation boundary box.
    /// @}

    // ---COLORS---
    /// @name Color Scheme
    /// @{
    static inline const sf::Color COLOR_BACKGROUND = sf::Color::Black;
    static inline const sf::Color COLOR_DEFAULT = sf::Color::White;
    static inline const sf::Color COLOR_POSITIVE_CHARGE = sf::Color::Red;
    static inline const sf::Color COLOR_NEGATIVE_CHARGE = sf::Color::Blue;
    static inline const sf::Color COLOR_BOX = sf::Color::Green;
    static inline const sf::Color COLOR_SELECTION = sf::Color::Yellow;
    static inline const sf::Color COLOR_VECTOR = sf::Color::Red;
    /// @}
};

#endif