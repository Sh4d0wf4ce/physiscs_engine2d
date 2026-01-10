#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <filesystem>
#include "Config.h"
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "Profiler.h"
#include "Serializer.h"

namespace fs = std::filesystem;

enum AppState {EDITOR, SIMULATION};

static void HelpMarker(const char* desc){
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered() && ImGui::BeginTooltip()){
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Physics Engine 2D");
    window.setFramerateLimit(60);

    if(!ImGui::SFML::Init(window)) return -1;

    PhysicsEngine engine;
    AppState state = AppState::EDITOR;
    Body* selectedBody = nullptr;

    Renderer renderer(window);
    Profiler profiler;

    if(!fs::exists("saves")){
        fs::create_directory("saves");
    }

    char saveFileName[64] = "save1";
    


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
    profiler.reset(engine);
    
    nlohmann::json initialState = Serializer::serialize(engine);
    sf::Clock clock;
    sf::Clock deltaClock;

    bool isDragging = false;
    bool isVelocityDragging = false;
    Vector2d dragOffset = {0, 0};

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            ImGui::SFML::ProcessEvent(window, *event);

            if(event->is<sf::Event::Closed>()) window.close();

            bool mouseOnUI = ImGui::GetIO().WantCaptureMouse;
            bool keyboardOnUI = ImGui::GetIO().WantCaptureKeyboard;

            if(!keyboardOnUI && !mouseOnUI){
                if(const auto& keyEvent = event->getIf<sf::Event::KeyPressed>()){
                    if(keyEvent->code == sf::Keyboard::Key::Space){
                        if(state ==  AppState::EDITOR){
                            state = AppState::SIMULATION;
                        }else{
                            state = AppState::EDITOR;
                        }
                    }


                    if(keyEvent->code == sf::Keyboard::Key::R){
                        selectedBody = nullptr;
                        Serializer::deserialize(engine, initialState);
                    }
                }

                if(const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()){
                    Vector2d mousePos = Renderer::realToScreen({mouseEvent->position.x, mouseEvent->position.y});
                    selectedBody = engine.findBodyAt(mousePos);

                    if(selectedBody){
                        if(mouseEvent->button == sf::Mouse::Button::Left && state == AppState::EDITOR){
                            isDragging = true;
                            dragOffset = selectedBody->pos - mousePos;
                        }else if(mouseEvent->button == sf::Mouse::Button::Right && state == AppState::EDITOR){
                            isVelocityDragging = true;
                        }
                    }else{
                        if(mouseEvent->button == sf::Mouse::Button::Left) selectedBody = nullptr;
                        isDragging = false;
                        isVelocityDragging = false;
                    }  
                }

                if(const auto& mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()){
                    if(mouseEvent->button == sf::Mouse::Button::Left){
                        isDragging = false;
                    }
                    if(mouseEvent->button == sf::Mouse::Button::Right){
                        isVelocityDragging = false;
                    }
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

        ImGui::Begin("Control Panel");

        float width = ImGui::GetContentRegionAvail().x;

        if (state == AppState::EDITOR) {
            if (ImGui::Button("PLAY", ImVec2(width * 0.3f, 30))) {
                state = AppState::SIMULATION;
            }
        } else {
            if (ImGui::Button("PAUSE", ImVec2(width * 0.3f, 30))) {
                state = AppState::EDITOR;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("RESET", ImVec2(width * 0.3f, 30))) {
            selectedBody = nullptr;
            Serializer::deserialize(engine, initialState);
        }

        ImGui::SameLine();
        if (ImGui::Button("SAVE STATE", ImVec2(width * 0.3f, 30))) {
            initialState = Serializer::serialize(engine);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Add New Object...", ImVec2(-1, 40))) {
            ImGui::OpenPopup("Create Object");
        }


        if (ImGui::CollapsingHeader("Physics Rules", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Check Body Collisions", &Config::useBodiesCollision);
            ImGui::Checkbox("Check Wall Collisions", &Config::useWindowCollision);
            
            ImGui::Separator();
            
            ImGui::Checkbox("Global Gravity (Down)", &Config::useGravity);
            ImGui::SameLine(); HelpMarker("Standard gravity pulling objects down (F = m*g)");
            
            if (Config::useGravity) {
                ImGui::SliderFloat("G Constant", &Config::G, 0.0f, 2000.0f);
            }

            ImGui::Checkbox("N-Body Gravity", &Config::useNBodyGravity);
            ImGui::SameLine(); HelpMarker("Orbital mechanics. Every object attracts every other object.");
            
            ImGui::Checkbox("Electrostatics", &Config::useElectrostatics);
            ImGui::SameLine(); HelpMarker("Coulomb's Law. Objects need 'charge' to interact.");
        }

        if (ImGui::CollapsingHeader("World & View", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Camera Zoom:");
            ImGui::SliderFloat("##zoom", &Config::SCALE, 0.1f, 100.0f, "%.1f px/m", ImGuiSliderFlags_Logarithmic);
            
            ImGui::Text("Simulation Bounds (Meters):");
            Vector2d bounds = engine.getSimBounds();
            float size[2] = { bounds.x, bounds.y };
            if (ImGui::DragFloat2("##bounds", size, 0.5f, 10.0f, 1000.0f)) {
                engine.setSimBounds(size[0], size[1]);
            }

            ImGui::Separator();
            ImGui::Checkbox("Show Trails", &Config::renderTrails);
            ImGui::Checkbox("Show Boundaries", &Config::renderWorldBounds);
            ImGui::Checkbox("Show Velocity Vectors", &Config::renderVelocityVectors);
        }

        if(ImGui::CollapsingHeader("Storage / Saves")){
            ImGui::Text("Save to file: ");
            ImGui::InputText(".json", saveFileName, IM_ARRAYSIZE(saveFileName));

            if(ImGui::Button("Save", ImVec2(-1, 0))){
                std::string path = "saves/" + std::string(saveFileName) + ".json";
                Serializer::saveToFile(path, engine);
            }

            ImGui::Separator();
            ImGui::Text("Load from file: ");

            static int selectedFileIndex = -1;
            std::vector<std::string> files;

            if(fs::exists("saves")){
                for(const auto& file: fs::directory_iterator("saves")){
                    if(file.path().extension() == ".json"){
                        files.push_back(file.path().filename().string());
                    }
                }
            }

            if(ImGui::BeginListBox("##files", ImVec2(-1, 100))){
                for(int i = 0; i < files.size(); i++){
                    const bool isSelected = (selectedFileIndex == i);
                    if(ImGui::Selectable(files[i].c_str(), isSelected)){
                        selectedFileIndex = i;
                    }

                    if(isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            if(ImGui::Button("Load", ImVec2(-1, 0))){
                if(selectedFileIndex >= 0 && selectedFileIndex < files.size()){
                    std::string path = "saves/" + files[selectedFileIndex];
                    Serializer::loadFromFile(path, engine);

                    selectedBody = nullptr;

                    initialState = Serializer::serialize(engine);
                    state = AppState::EDITOR;
                }
            }
        }

        if (ImGui::BeginPopupModal("Create Object", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static int newItemType = 0; // 0 = Circle, 1 = Box
            static float newItemMass = 10.0f;
            static float newItemSize[2] = { 20.0f, 20.0f };
            static float newItemPos[2] = { 0.0f, 0.0f };
            static float newItemVel[2] = { 0.0f, 0.0f };
            static float newItemCharge = 0.0f;
            static float newItemRestitution = 0.8f;

            ImGui::Combo("Shape", &newItemType, "Circle\0Box\0");
            
            ImGui::Separator();

            if (newItemType == 0) {
                ImGui::InputFloat("Radius", &newItemSize[0]);
            } else {
                ImGui::InputFloat2("Width / Height", newItemSize);
            }

            ImGui::InputFloat("Mass", &newItemMass);
            ImGui::InputFloat2("Position (m)", newItemPos);
            ImGui::InputFloat2("Velocity (m/s)", newItemVel);
            ImGui::SliderFloat("Bounciness", &newItemRestitution, 0.0f, 1.0f);
            
            if (Config::useElectrostatics) {
                ImGui::InputFloat("Charge", &newItemCharge);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Create", ImVec2(120, 0))) {
                Collider* col = nullptr;
                if (newItemType == 0) col = new CircleCollider(newItemSize[0]);
                else col = new BoxCollider(newItemSize[0], newItemSize[1]);

                Body* b = new Body(Vector2d(newItemPos[0], newItemPos[1]), 
                                Vector2d(newItemVel[0], newItemVel[1]), 
                                newItemMass, newItemRestitution, newItemCharge, col);
                
                engine.addBody(b);
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::End();


        if (selectedBody != nullptr) {
            ImGui::Begin("Inspector");

            const std::vector<Body*>& bodies = engine.getBodies();
            int currentIndex = -1;
            for(int i = 0; i < bodies.size(); i++){
                if(bodies[i] == selectedBody){
                    currentIndex = i;
                    break;
                }
            }

            if(currentIndex != -1){
                if(ImGui::ArrowButton("##left", ImGuiDir_Left)){
                    currentIndex--;
                    if(currentIndex < 0) currentIndex = bodies.size() - 1;
                    selectedBody = bodies[currentIndex];
                }
                
                ImGui::SameLine();
                ImGui::Text("Body %d / %d", currentIndex + 1, (int)bodies.size());
                ImGui::SameLine();
                
                if(ImGui::ArrowButton("##right", ImGuiDir_Right)){
                    currentIndex++;
                    if(currentIndex >= bodies.size()) currentIndex = 0;
                    selectedBody = bodies[currentIndex];
                }
            }
                
            ImGui::Separator();

            float pos[2] = { selectedBody->pos.x, selectedBody->pos.y };
            float vel[2] = { selectedBody->vel.x, selectedBody->vel.y };

            if (ImGui::DragFloat2("Position", pos)) {
                selectedBody->pos = Vector2d(pos[0], pos[1]);
            }
            if (ImGui::DragFloat2("Velocity", vel)) {
                selectedBody->vel = Vector2d(vel[0], vel[1]);
            }

            float mass = selectedBody->getMass();
            if (ImGui::DragFloat("Mass", &mass, 1.0f, 0.1f, 10000.0f)) {
                selectedBody->setMass(mass);
            }

            float restitution = selectedBody->restitution;
            if (ImGui::SliderFloat("Bounciness", &restitution, 0.0f, 1.0f)) {
                selectedBody->restitution = restitution;
            }
            
            ImGui::Separator();
            if (selectedBody->collider->shapeType == CIRCLE) {
                CircleCollider* c = static_cast<CircleCollider*>(selectedBody->collider);
                ImGui::DragFloat("Radius", &c->r, 0.5f, 1.0f, 500.0f);
            } 
            else if (selectedBody->collider->shapeType == BOX) {
                BoxCollider* b = static_cast<BoxCollider*>(selectedBody->collider);
                ImGui::DragFloat("Width", &b->width, 1.0f, 1.0f, 1000.0f);
                ImGui::DragFloat("Height", &b->height, 1.0f, 1.0f, 1000.0f);
            }
            
            ImGui::Separator();
            if (ImGui::Button("Delete Body", ImVec2(-1, 0))) {
                engine.removeBody(selectedBody);
                selectedBody = nullptr;
            }

            ImGui::End();
        }

        float dt = clock.restart().asSeconds();
        
        if(state == AppState::SIMULATION){
            engine.update(dt);
            profiler.update(dt);
        }

        if(state == AppState::EDITOR && selectedBody) {
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            Vector2d mousePos = Renderer::realToScreen({mouse.x, mouse.y});

            if(isDragging){
                selectedBody->pos = mousePos + dragOffset;
            }else if(isVelocityDragging){
                selectedBody->vel = -1*(mousePos - selectedBody->pos);
            }
        }

        window.clear(Config::COLOR_BACKGROUND);
        renderer.render(engine, profiler.getDebugInfo(engine));

        if(selectedBody){
           renderer.drawSelection(*selectedBody);
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}