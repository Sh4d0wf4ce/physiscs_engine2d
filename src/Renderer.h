#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "Body.h"
#include "PhysicsEngine.h"
#include "Config.h"

class Renderer{
    sf::RenderWindow& window;
    Vector2d cameraPos;

    void drawCircle(const Body& body);
    void drawBox(const Body& body);
    void drawTrail(const Body& body);
    void drawVector(const Vector2d& start, const Vector2d& vec);
public:
    Renderer(sf::RenderWindow& window): window(window), cameraPos({0,0}) {}
    void render(const PhysicsEngine& engine, std::string debugInfo = "");
    void drawSelection(const Body& body);
    Vector2d screenToReal(const Vector2d& pos);
    Vector2d realToScreen(const Vector2d& pos);
    void setCameraPos(const Vector2d& pos) {cameraPos = pos;}
    void moveCamera(const Vector2d& offset) {cameraPos += offset;}
    Vector2d getCameraPos() const {return cameraPos;}
};

#endif