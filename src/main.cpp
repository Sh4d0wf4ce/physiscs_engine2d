#include <SFML/Graphics.hpp>
#include "Vector2d.h"
#include "Body.h"
#include "PhysicsEngine.h"

int main() {
    // sf::RenderWindow window(sf::VideoMode({640, 480}), "Test SFML 3.0");
    
    // sf::CircleShape shape(100.f);
    // shape.setFillColor(sf::Color::Green);
    // shape.setPosition({220.f, 140.f});
    
    // while (window.isOpen()) {
    //     while (auto event = window.pollEvent()) {
    //         if (event->is<sf::Event::Closed>()) {
    //             window.close();
    //         }
    //     }
        
    //     window.clear();
    //     window.draw(shape);
    //     window.display();
    // }

    PhysicsEngine engine;
    Body body(Vector2d(0, 0), Vector2d(10, 0), 1.0f);
    engine.addBody(&body);
    
    std::cout<<"Initial body position: "<<body.pos<<"\n";
    engine.step(0.016f);
    std::cout<<"Body position after step: "<<body.pos<<"\n";


    
    return 0;
}