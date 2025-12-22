#include "Renderer.h"

void Renderer::drawCircle(const Body& body){
    sf::CircleShape shape;
    shape.setRadius(static_cast<CircleCollider*>(body.collider)->r * scale);
    shape.setOrigin(sf::Vector2f(shape.getRadius(), shape.getRadius()));
    shape.setPosition(sf::Vector2f(body.pos.x * scale, body.pos.y * scale));

    if(body.charge > 0) shape.setFillColor(Config::COLOR_POSITIVE_CHARGE);
    else if(body.charge < 0) shape.setFillColor(Config::COLOR_NEGATIVE_CHARGE);
    else shape.setFillColor(Config::COLOR_DEFAULT);

    window.draw(shape);
}

void Renderer::drawBox(const Body& body){
    BoxCollider* box = static_cast<BoxCollider*>(body.collider);
    sf::RectangleShape shape(sf::Vector2f(box->width * scale, box->height * scale));
    
    shape.setOrigin(sf::Vector2f(box->width * scale / 2.0f, box->height * scale / 2.0f));
    shape.setPosition(sf::Vector2f(body.pos.x * scale, body.pos.y * scale));
    shape.setFillColor(Config::COLOR_BOX);

    window.draw(shape);
}

void Renderer::render(const PhysicsEngine& engine){
    const std::vector<Body*> bodies = engine.getBodies();

    for(const Body* body: bodies){
        switch (body->collider->shapeType)
        {
        case CIRCLE:
            drawCircle(*body);
            break;
        case BOX:
            drawBox(*body);
            break;
        default:
            break;
        }
        
    }

    const sf::Font font("assets/fonts/GoogleSans-Regular.ttf");
    float energyPercentage = engine.getTotalEnergy()/engine.getInitialEnergy() * 100;
    sf::Text t(font, "Total energy: "+std::to_string(energyPercentage)+"%");
    window.draw(t);
}