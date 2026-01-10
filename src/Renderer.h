#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "Body.h"
#include "PhysicsEngine.h"
#include "Config.h"

class Renderer{
    sf::RenderWindow& window;

    void drawCircle(const Body& body);
    void drawBox(const Body& body);
    void drawTrail(const Body& body);
    void drawVector(const Vector2d& start, const Vector2d& vec);
public:
    Renderer(sf::RenderWindow& window): window(window) {}
    void render(const PhysicsEngine& engine, std::string debugInfo = "");
    void drawSelection(const Body& body);
    static Vector2d screenToReal(const Vector2d& pos);
    static Vector2d realToScreen(const Vector2d& pos);
};

#endif