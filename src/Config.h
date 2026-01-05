#ifndef CONFIG_H
#define CONFIG_H

#include <SFML/Graphics.hpp>

struct Config
{
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;
    
    static inline float SCALE = 1.0f;
    static inline float G = 1000.0f;
    static inline float K = 10000.0f;

    static inline bool useWindowCollision = true;
    static inline bool useBodiesCollision = true;

    static inline bool useGravity = false;
    static inline bool useNBodyGravity = true;
    static inline bool useElectrostatics = true;

    static inline bool renderTrails = true;
    static inline bool renderWorldBounds = true;

    static inline const sf::Color COLOR_BACKGROUND = sf::Color::Black;
    static inline const sf::Color COLOR_DEFAULT = sf::Color::White;
    static inline const sf::Color COLOR_POSITIVE_CHARGE = sf::Color::Red;
    static inline const sf::Color COLOR_NEGATIVE_CHARGE = sf::Color::Blue;
    static inline const sf::Color COLOR_BOX = sf::Color::Green;
    static inline const sf::Color COLOR_SELECTION = sf::Color::Yellow;
};

#endif