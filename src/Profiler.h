#ifndef PROFILER_H
#define PROFILER_H

#include <SFML/System.hpp>
#include "PhysicsEngine.h"
#include <sstream>
#include <iomanip>

class Profiler {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;
    int frameCount;
    float currentFPS;

    float initialTotalEnergy;
    bool energyInitialized;

public:
    Profiler(): frameCount(0), currentFPS(0.0f), initialTotalEnergy(0.0f), energyInitialized(false)  {};
    void update(float dt);
    void reset(const PhysicsEngine& engine);
    
    std::string getDebugInfo(const PhysicsEngine& engine);
};

#endif