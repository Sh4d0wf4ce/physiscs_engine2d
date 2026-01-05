#include "Renderer.h"

void Renderer::render(const PhysicsEngine& engine, std::string debugInfo){
    const std::vector<Body*> bodies = engine.getBodies();

    if(Config::renderWorldBounds){
        Vector2d simBounds = engine.getSimBounds();
        sf::RectangleShape boundsRect(sf::Vector2f(simBounds.x * Config::SCALE, simBounds.y * Config::SCALE));

        boundsRect.setOrigin(sf::Vector2f((simBounds.x * Config::SCALE) / 2.0f, (simBounds.y * Config::SCALE) / 2.0f));
        Vector2d screenPos = screenToReal(Vector2d(0,0));
        boundsRect.setPosition(sf::Vector2f(screenPos.x, screenPos.y));
        boundsRect.setFillColor(sf::Color::Transparent);
        boundsRect.setOutlineColor(sf::Color::White);
        boundsRect.setOutlineThickness(2.0f);

        window.draw(boundsRect);
    }

    if(Config::renderTrails){
        for(const Body* body: bodies){
            drawTrail(*body);
        }
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
    return Vector2d((pos.x * Config::SCALE) + Config::WINDOW_WIDTH / 2.0f, -(pos.y * Config::SCALE) + Config::WINDOW_HEIGHT / 2.0f);
}

Vector2d Renderer::RealToScreen(const Vector2d& pos){
    return Vector2d((pos.x - Config::WINDOW_WIDTH / 2.0f)/Config::SCALE, -(pos.y -Config::WINDOW_HEIGHT / 2.0f)/Config::SCALE);
}

void Renderer::drawCircle(const Body& body){
    sf::CircleShape shape;
    shape.setRadius(static_cast<CircleCollider*>(body.collider)->r * Config::SCALE);
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
    sf::RectangleShape shape(sf::Vector2f(box->width * Config::SCALE, box->height * Config::SCALE));
    
    shape.setOrigin(sf::Vector2f((box->width * Config::SCALE) / 2.0f, (box->height * Config::SCALE) / 2.0f));
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

void Renderer::drawSelection(const Body& body){
    float padding = 5.0f;
    sf::Shape* shape = nullptr;
    Collider* col = body.collider;
    if(col->shapeType == CIRCLE){
        CircleCollider* circle = static_cast<CircleCollider*>(col);
        sf::CircleShape circleShape;
        circleShape.setRadius((circle->r + padding) * Config::SCALE);
        circleShape.setOrigin(sf::Vector2f(circleShape.getRadius(), circleShape.getRadius()));
        Vector2d screenPos = screenToReal(body.pos);
        circleShape.setPosition(sf::Vector2f(screenPos.x, screenPos.y));
        circleShape.setFillColor(sf::Color::Transparent);
        circleShape.setOutlineColor(Config::COLOR_SELECTION);
        circleShape.setOutlineThickness(2.0f);
        shape = new sf::CircleShape(circleShape);
    }else if(col->shapeType == BOX){
        BoxCollider* box = static_cast<BoxCollider*>(col);
        sf::RectangleShape rectShape(sf::Vector2f((box->width + 2*padding) * Config::SCALE, (box->height + 2*padding) * Config::SCALE));
        rectShape.setOrigin(sf::Vector2f(rectShape.getSize().x / 2.0f, rectShape.getSize().y / 2.0f));
        Vector2d screenPos = screenToReal(body.pos);
        rectShape.setPosition(sf::Vector2f(screenPos.x, screenPos.y));
        rectShape.setFillColor(sf::Color::Transparent);
        rectShape.setOutlineColor(Config::COLOR_SELECTION);
        rectShape.setOutlineThickness(2.0f);
        shape = new sf::RectangleShape(rectShape);
    }

    window.draw(*shape);
}