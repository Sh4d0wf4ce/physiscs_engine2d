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
public:
    Renderer(sf::RenderWindow& window): window(window) {}
    void render(const PhysicsEngine& engine, std::string debugInfo = "");
    Vector2d screenToReal(const Vector2d& pos);
    Vector2d RealToScreen(const Vector2d& pos);
    void drawSelection(const Body& body);
};

#endif