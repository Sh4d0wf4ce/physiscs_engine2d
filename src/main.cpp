#include <SFML/Graphics.hpp>
#include "Config.h"
#include "PhysicsEngine.h"
#include "Renderer.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Physics Engine 2D");
    window.setFramerateLimit(60);

    PhysicsEngine engine;

    Renderer renderer(window, Config::SCALE);

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())window.close();
            if(event->is<sf::Event::MouseButtonPressed>()){
                sf::Vector2 pos = sf::Mouse::getPosition(window);
                engine.addBody(new Body({pos.x, pos.y}, {0, 0}, -1, 0.9f, 1, new BoxCollider(40,40)));
            }
        }
        
        engine.update(10.0f / 60.0f);
        window.clear(Config::COLOR_BACKGROUND);
        renderer.render(engine);
        window.display();
    }

    return 0;
}