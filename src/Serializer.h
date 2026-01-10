#include <fstream>
#include "json.hpp"
#include "PhysicsEngine.h"

using json = nlohmann::json;

class Serializer {
public:
    static json serialize(const PhysicsEngine& engine);
    static void deserialize(PhysicsEngine& engine, const json& j);
    static void saveToFile(const std::string& filename, const PhysicsEngine& engine);
    static void loadFromFile(const std::string& filename,  PhysicsEngine& engine);
};