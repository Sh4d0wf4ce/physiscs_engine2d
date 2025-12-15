#include <SFML/Graphics.hpp>
#include "Vector2d.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({640, 480}), "Test SFML 3.0");
    
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition({220.f, 140.f});
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        
        window.clear();
        window.draw(shape);
        window.display();
    }


    
    return 0;
}