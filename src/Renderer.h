#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "Body.h"
#include "PhysicsEngine.h"

class Renderer{
    sf::RenderWindow& window;
    float scale;

    void drawCircle(const Body& body);
public:
    Renderer(sf::RenderWindow& window, float scale = 1): window(window), scale(scale) {}
    void render(const PhysicsEngine& engine);
    void setScale(float scale) {this->scale = scale;}
};

#endif