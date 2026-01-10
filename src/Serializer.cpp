#include "Serializer.h"
#include "Config.h"
#include <fstream>
#include <iostream>

json Serializer::serialize(const PhysicsEngine& engine){
    json j;

    j["settings"] = {
        {"G", Config::G},
        {"scale", Config::SCALE},
        {"useGravity", Config::useGravity},
        {"useNBodyGravity", Config::useNBodyGravity},
        {"useElectrostatics", Config::useElectrostatics},
        {"checkBodyCollisions", Config::useBodiesCollision},
        {"checkWindowCollisions", Config::useWindowCollision},
        {"simWidth", engine.getSimBounds().x},
        {"simHeight", engine.getSimBounds().y}
    };

    j["bodies"] = json::array();
    for(const Body* body: engine.getBodies()){
        json b;
        b["pos"] = { body->pos.x, body->pos.y };
        b["vel"] = { body->vel.x, body->vel.y };
        b["mass"] = body->getMass();
        b["restitution"] = body->restitution;
        b["charge"] = body->charge;

        if (body->collider->shapeType == CIRCLE) {
            b["type"] = "CIRCLE";
            b["radius"] = static_cast<CircleCollider*>(body->collider)->r;
        } 
        else if (body->collider->shapeType == BOX) {
            b["type"] = "BOX";
            BoxCollider* box = static_cast<BoxCollider*>(body->collider);
            b["width"] = box->width;
            b["height"] = box->height;
        }
        j["bodies"].push_back(b);
    }

    return j;
}

void Serializer::deserialize(PhysicsEngine& engine, const json& j){
    if(j.contains("settings")){
        json s = j["settings"];
        Config::G = s.value("G", 1000.0f);
        Config::SCALE = s.value("scale", 50.0f);
        Config::useGravity = s.value("useGravity", true);
        Config::useNBodyGravity = s.value("useNBodyGravity", false);
        Config::useElectrostatics = s.value("useElectrostatics", false);
        Config::useBodiesCollision = s.value("checkBodyCollisions", true);
        Config::useWindowCollision = s.value("checkWindowCollisions", true);
        
        float sw = s.value("simWidth", 20.0f);
        float sh = s.value("simHeight", 15.0f);
        engine.setSimBounds(sw, sh);
    }

    engine.clearBodies();

    if (j.contains("bodies")) {
        for(const auto& b : j["bodies"]){
            Vector2d pos(b["pos"][0], b["pos"][1]);
            Vector2d vel(b["vel"][0], b["vel"][1]);
            float mass = b.value("mass", 1.0f);
            float restitution = b.value("restitution", 1.0f);
            float charge = b.value("charge", 0.0f);

            Collider* collider = nullptr;
            std::string type = b.value("type", "CIRCLE");
            if(type == "CIRCLE"){
                float r = b.value("radius", 10.0f);
                collider = new CircleCollider(r);
            }else if(type == "BOX"){
                float width = b.value("width", 10.0f);
                float height = b.value("height", 10.0f);
                collider = new BoxCollider(width, height);
            }

            Body* body = new Body(pos, vel, mass, restitution, charge, collider);
            engine.addBody(body);
        }
    }
}

void Serializer::saveToFile(const std::string& filename, const PhysicsEngine& engine){
    std::ofstream file(filename);
    if(file.is_open()) {
        json j = serialize(engine);
        file << j.dump(4);
        file.close();
        std::cout << "Saved simulation state to " << filename << std::endl;
    }
}

void Serializer::loadFromFile(const std::string& filename, PhysicsEngine& engine){
    std::ifstream file(filename);
    if(file.is_open()){
        json j;
        file >> j;
        deserialize(engine, j);
        file.close();
        std::cout << "Loaded simulation state from " << filename << std::endl;
    }else{
        std::cerr << "Failed to open file " << filename << std::endl;
    }
}