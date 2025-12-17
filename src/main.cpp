#include <SFML/Graphics.hpp>
#include "PhysicsEngine.h"
#include "Renderer.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Physics Engine 2D");
    window.setFramerateLimit(60);

    PhysicsEngine engine;
    engine.addBody(new Body(Vector2d(100, 100), Vector2d(100, 0), 1, 1, 10));
    engine.addBody(new Body(Vector2d(300, 100), Vector2d(50, 0), 1, 1, -10));

    Renderer renderer(window, 0.5f);

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent())
            if (event->is<sf::Event::Closed>())window.close();
        
        engine.update(10.0f / 60.0f);
        window.clear(sf::Color::Black);
        renderer.render(engine);
        window.display();
    }

    return 0;
}