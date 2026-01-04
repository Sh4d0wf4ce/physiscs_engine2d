#include <SFML/Graphics.hpp>
#include "Config.h"
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "Profiler.h"
#include <random>
#include <math.h>


int main() {
    sf::RenderWindow window(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Physics Engine 2D");
    window.setFramerateLimit(60);

    PhysicsEngine engine;
    Renderer renderer(window, Config::SCALE);
    Profiler profiler;
    Body* sun = new Body({0, 0}, {0, 0}, 10000, 0.0f, 0, new CircleCollider(30));
    Body* planet1 = new Body({200, 0}, {0, 223.61f}, 1, 0.0f, 0, new CircleCollider(10));
    Body* planet2 = new Body({-100, 0}, {0, -316.23f}, 1, 0.0f, 0, new CircleCollider(10));
    Body* planet3 = new Body({0, 150}, {258.2f, 0}, 1, 0.0f, 0, new CircleCollider(10));
    engine.addBody(sun);
    engine.addBody(planet1);
    engine.addBody(planet2);
    engine.addBody(planet3);

    Body* b1 = new Body(Vector2d(-0.97000436, 0.24308753), Vector2d(0.466203685, 0.43236573), 1, 0.0f, 0, new CircleCollider(5));
    Body* b2 = new Body(Vector2d(0.0, 0.0), Vector2d(-0.93240737, -0.86473146), 1, 0.0f, 0, new CircleCollider(5));
    Body* b3 = new Body(Vector2d(0.97000436, -0.24308753), Vector2d(0.466203685, 0.43236573), 1, 0.0f, 0, new CircleCollider(5));
    engine.addBody(b1);
    engine.addBody(b2);
    engine.addBody(b3);

    //create 200 electrons in random positions
    // for(int i = 0; i < 200; i++){
    //     float x = static_cast<float>(rand() % Config::WINDOW_WIDTH);
    //     float y = static_cast<float>(rand() % Config::WINDOW_HEIGHT);
    //     engine.addBody(new Body({x, y}, {0, 0}, 1, 0.1f, -1, new CircleCollider(5)));
    // }
    //add four protons in the center
    
    // engine.addBody(new Body({Config::WINDOW_WIDTH / 2.0f, Config::WINDOW_HEIGHT / 2.0f}, {0, 0}, 100, 0.1f, 1, new CircleCollider(10)));
    // engine.addBody(new Body({Config::WINDOW_WIDTH / 2.0f + 50, Config::WINDOW_HEIGHT / 2.0f}, {0, 0}, 100, 0.1f, 1, new CircleCollider(10)));
    // engine.addBody(new Body({Config::WINDOW_WIDTH / 2.0f - 50, Config::WINDOW_HEIGHT / 2.0f}, {0, 0}, 100, 0.1f, 1, new CircleCollider(10)));
    // engine.addBody(new Body({Config::WINDOW_WIDTH / 2.0f, Config::WINDOW_HEIGHT / 2.0f + 50}, {0, 0}, 100, 0.1f, 1, new CircleCollider(15)));

    profiler.reset(engine);
    sf::Clock clock;

    while(window.isOpen()){
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())window.close();
            if(event->is<sf::Event::MouseButtonPressed>()){
                sf::Vector2 pos = sf::Mouse::getPosition(window);
                engine.addBody(new Body({pos.x, pos.y}, {0, 0}, 1, 1, 1, new CircleCollider(20)));
            }
        }
        
        engine.update(dt);
        profiler.update(dt);
        window.clear(Config::COLOR_BACKGROUND);
        renderer.render(engine, profiler.getDebugInfo(engine));
        window.display();
    }

    return 0;
}