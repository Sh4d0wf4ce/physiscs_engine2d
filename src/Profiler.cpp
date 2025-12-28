#include "Profiler.h"

void Profiler::reset(const PhysicsEngine& engine){
    initialTotalEnergy = engine.getTotalEnergy();
    energyInitialized = true;
}

void Profiler::update(float dt){
    frameCount++;
    timeSinceLastUpdate += sf::seconds(dt);

    if (timeSinceLastUpdate.asSeconds() >= 0.5f) {
        currentFPS = frameCount / timeSinceLastUpdate.asSeconds();
        frameCount = 0;
        timeSinceLastUpdate = sf::Time::Zero;
    }
}

std::string Profiler::getDebugInfo(const PhysicsEngine& engine){
    float currentEnergy = engine.getTotalEnergy();

    if(!energyInitialized && engine.getBodies().size() > 0){
        reset(engine);
    }

    float deviation = 0.0f;
    if(initialTotalEnergy != 0.0f){
        deviation = ((currentEnergy - initialTotalEnergy) / initialTotalEnergy) * 100.0f;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "FPS: " << static_cast<int>(currentFPS) << "\n";
    ss << "Objects: " << engine.getBodies().size() << "\n";
    ss << "Total Energy: " << currentEnergy << " J\n";
    ss << "Initial Energy: " << initialTotalEnergy << " J\n";
    ss << "Total momentum: " << engine.getTotalMomentum() << " kg*m/s\n";

    ss << "Deviation: ";
    if (std::abs(deviation) < 1.0f) ss << "[OK] ";
    else if (std::abs(deviation) < 5.0f) ss << "[WARN] ";
    else ss << "[FAIL] ";
    
    ss << deviation << "%\n";

    return ss.str();
}