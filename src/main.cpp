#include <SFML/Graphics.hpp>
#include "Config.h"
#include "PhysicsEngine.h"
#include "Renderer.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Physics Engine 2D");
    window.setFramerateLimit(60);

    PhysicsEngine engine;
    // engine.addBody(new Body({400, 100}, {0, 0}, 1, 1, 1, new CircleCollider(20)));
    // engine.addBody(new Body({100, 900}, {0, 0}, -1, 1, 1, new BoxCollider(400,20)));
    //add 1000 small circle bodies
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 10; j++){
            engine.addBody(new Body({50 + i * 30, 50 + j * 30}, {0, 0}, 1, 0.8f, 1, new CircleCollider(10)));
        }
    }

    Renderer renderer(window, Config::SCALE);

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())window.close();
            if(event->is<sf::Event::MouseButtonPressed>()){
                sf::Vector2 pos = sf::Mouse::getPosition(window);
                engine.addBody(new Body({pos.x, pos.y}, {0, 0}, 1, 1, 1, new CircleCollider(20)));
            }
        }
        
        engine.update(10.0f / 60.0f);
        window.clear(Config::COLOR_BACKGROUND);
        renderer.render(engine);
        window.display();
    }

    return 0;
}