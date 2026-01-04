#include "Renderer.h"

void Renderer::render(const PhysicsEngine& engine, std::string debugInfo){
    const std::vector<Body*> bodies = engine.getBodies();

    for(const Body* body: bodies){
        drawTrail(*body);
    }

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

    sf::Font font("assets/fonts/GoogleSans-Regular.ttf");
    sf::Text txt(font);
    txt.setCharacterSize(20);
    txt.setFillColor(sf::Color::White);
    txt.setPosition({10.0f, 10.0f});
    txt.setString(debugInfo);

    window.draw(txt);
}

Vector2d Renderer::screenToReal(const Vector2d& pos){
    return Vector2d((pos.x * scale) + Config::WINDOW_WIDTH / 2.0f, -(pos.y * scale) + Config::WINDOW_HEIGHT / 2.0f);
}

void Renderer::drawCircle(const Body& body){
    sf::CircleShape shape;
    shape.setRadius(static_cast<CircleCollider*>(body.collider)->r * scale);
    shape.setOrigin(sf::Vector2f(shape.getRadius(), shape.getRadius()));
    Vector2d screenPos = screenToReal(body.pos);
    shape.setPosition(sf::Vector2f(screenPos.x, screenPos.y));

    if(body.charge > 0) shape.setFillColor(Config::COLOR_POSITIVE_CHARGE);
    else if(body.charge < 0) shape.setFillColor(Config::COLOR_NEGATIVE_CHARGE);
    else shape.setFillColor(Config::COLOR_DEFAULT);

    window.draw(shape);
}

void Renderer::drawBox(const Body& body){
    BoxCollider* box = static_cast<BoxCollider*>(body.collider);
    sf::RectangleShape shape(sf::Vector2f(box->width * scale, box->height * scale));
    
    shape.setOrigin(sf::Vector2f((box->width * scale) / 2.0f, (box->height * scale) / 2.0f));
    Vector2d screenPos = screenToReal(body.pos);
    shape.setPosition(sf::Vector2f(screenPos.x, screenPos.y));
    shape.setFillColor(Config::COLOR_BOX);

    window.draw(shape);
}

void Renderer::drawTrail(const Body& body){
    if(body.trail.size() < 2) return;
    sf::VertexArray trail(sf::PrimitiveType::LineStrip, body.trail.size());

    for(int i = 0; i < body.trail.size(); i++){
        Vector2d screenPos = screenToReal(body.trail[i]);
        trail[i].position = sf::Vector2f(screenPos.x, screenPos.y);
        
        sf::Color color = Config::COLOR_DEFAULT;
        float alpha = static_cast<float>(i) / body.trail.size();
        color.a = static_cast<std::uint8_t>(alpha * 255);

        trail[i].color = color;
    }

    window.draw(trail);
}