#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <fstream>
#include "json.hpp"
#include "PhysicsEngine.h"

using json = nlohmann::json;

/// @brief Static class responsible for saving and loading simulation states.
/// Uses the nlohmannn/json library to serialize the PhysicsEngine state
/// (bodies, parameters, global settings) into human-readable JSON format.
class Serializer {
public:
    /// @brief Converts the current state of the PhysiscsEngine into a JSON object.
    /// Captures all bodies, their properties, and global simulation settings.
    /// @param engine The physics engine instance to serialize.
    /// @return A JSON object containing full simulation state.
    static json serialize(const PhysicsEngine& engine);

    /// @brief Restores the simulation state from a JSON object.
    /// Clears existing bodies in the engine and recreates them based on the JSON data.
    /// Also updates global simulation settings from the file.
    /// @param engine The physics engine instance to update.
    /// @param j The JSON object containing the saved simulation state.
    static void deserialize(PhysicsEngine& engine, const json& j);

    /// @brief Saves the current simulation state to a file in JSON format.
    /// @param filename The path/name of the file to save to
    /// @param engine The physics engine instance to serialize and save.
    static void saveToFile(const std::string& filename, const PhysicsEngine& engine);

    /// @brief Loads a simulation state from a JSON file.
    /// @param filename The path/name of the file to load from.
    /// @param engine The physics engine instance to update with loaded data (Previous data will be cleared).
    static void loadFromFile(const std::string& filename,  PhysicsEngine& engine);
};

#endif