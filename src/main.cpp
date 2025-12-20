#include <SFML/Graphics.hpp>
#include "PhysicsEngine.h"
#include "Renderer.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Physics Engine 2D");
    window.setFramerateLimit(60);

    PhysicsEngine engine({0, 0});
    // for(int i = 0; i<100; i++){
    //     engine.addBody(new Body({rand()%800, rand()%600}, {0,0}, 1, 0.9f, (rand()%2==0?1:-1)*50, new CircleCollider(5 + rand()%15)));
    // }
    for(int i = 0; i<200; i++){
        engine.addBody(new Body({rand()%800, rand()%600}, {rand()%20, rand()%20}, 1, 1, (rand()%2==0?1:-1)*50, new CircleCollider(5 + rand()%10)));
    }

    // engine.addBody(new Body({200, 200}, {10, 0}, 100, 1, 0, new CircleCollider(20)));
    // engine.addBody(new Body({600, 200}, {0, 0}, 1, 1, 0, new CircleCollider(20)));


    Renderer renderer(window, 1);

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