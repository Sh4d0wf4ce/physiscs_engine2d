#include <SFML/Graphics.hpp>
#include "Config.h"
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "Profiler.h"
#include <random>
#include <math.h>

enum AppState {EDITOR, SIMULATION};

int main() {
    sf::RenderWindow window(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Physics Engine 2D");
    window.setFramerateLimit(60);

    PhysicsEngine engine;
    AppState state = AppState::EDITOR;
    Body* selectedBody = nullptr;

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
    

    float simWidth = Config::WINDOW_WIDTH / Config::SCALE;
    float simHeight = Config::WINDOW_HEIGHT / Config::SCALE;
    engine.setSimBounds(simWidth, simHeight);
    engine.saveState();

    profiler.reset(engine);
    sf::Clock clock;

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close();
            if(const auto& keyEvent = event->getIf<sf::Event::KeyPressed>()){
                if(keyEvent->code == sf::Keyboard::Key::Space){
                    if(state ==  AppState::EDITOR){
                        engine.saveState();
                        state = AppState::SIMULATION;
                    }else{
                        state = AppState::EDITOR;
                    }
                }


                if(keyEvent->code == sf::Keyboard::Key::R){
                    selectedBody = nullptr;
                    engine.restoreState();
                }
            }

            if(const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()){
                if(state != AppState::EDITOR) continue;
                if(mouseEvent->button == sf::Mouse::Button::Left){
                    Vector2d mousePos = renderer.RealToScreen({mouseEvent->position.x, mouseEvent->position.y});
                    selectedBody = engine.findBodyAt(mousePos);
                }
            }
        }

        float dt = clock.restart().asSeconds();
        
        if(state == AppState::SIMULATION){
            engine.update(dt);
            profiler.update(dt);
        }else if(AppState::EDITOR){
            if(selectedBody && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                Vector2d mPos = renderer.RealToScreen(Vector2d(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
                selectedBody->pos = mPos;
            }
        }

        window.clear(Config::COLOR_BACKGROUND);
        renderer.render(engine, profiler.getDebugInfo(engine));

        if(state == AppState::EDITOR && selectedBody){
           renderer.drawSelection(*selectedBody);
        }

        window.display();
    }

    return 0;
}