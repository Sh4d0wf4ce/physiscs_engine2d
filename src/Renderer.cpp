#include "Renderer.h"

void Renderer::drawCircle(const Body& body){
    sf::CircleShape shape;
    shape.setRadius(static_cast<CircleCollider*>(body.collider)->getRadius() * scale);
    shape.setOrigin(sf::Vector2f(shape.getRadius(), shape.getRadius()));
    shape.setPosition(sf::Vector2f(body.pos.x * scale, body.pos.y * scale));

    if(body.charge > 0) shape.setFillColor(sf::Color::Red);
    else if(body.charge < 0) shape.setFillColor(sf::Color::Blue);
    else shape.setFillColor(sf::Color::White);

    window.draw(shape);
}

void Renderer::render(const PhysicsEngine& engine){
    const std::vector<Body*> bodies = engine.getBodies();

    for(const Body* body: bodies){
        drawCircle(*body);
    }
}