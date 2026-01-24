#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <filesystem>
#include "Config.h"
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "Serializer.h"

namespace fs = std::filesystem;

// Appplication state: Editor (paused time, body manipulation) or Simulation (running time)
enum AppMode {EDITOR, SIMULATION};

// Helper function to display tooltips in ImGui
static void HelpMarker(const char* desc){
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered() && ImGui::BeginTooltip()){
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Helper function to check if a body is in the selected bodies list
bool isBodySelected(const std::vector<Body*>& bodies, Body* b){
    for(Body* body: bodies){
        if(body == b) return true;
    }
    return false;
}

int main() {
    // --- Window Setup ---
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // Create a window that is 80% of the screen
    unsigned int w = static_cast<unsigned int>(desktop.size.x * 0.8f);
    unsigned int h = static_cast<unsigned int>(desktop.size.y * 0.8f);

    sf::RenderWindow window(sf::VideoMode({w, h}), "Physics Engine 2D", sf::Style::Default);

    window.setPosition({(desktop.size.x - w)/2, (desktop.size.y - h)/2});
    window.setFramerateLimit(60);

    // Initialize ImGui-SFML
    if(!ImGui::SFML::Init(window)) return -1;


    // --- Simulation Setup ---
    PhysicsEngine engine;
    AppMode mode = AppMode::EDITOR;

    // Selection and clipboard buffers
    std::vector<Body*> selectedBodies;
    std::vector<Body*> clipboard;

    Renderer renderer(window);

    // Ensure saves directory exists
    if(!fs::exists("saves")){
        fs::create_directory("saves");
    }

    char saveFileName[64] = "save1";
    

    // --- Default Scene Setup (Solar System Demo) ---
    // Creating initial objects programmatically for demonstration
    Body* sun = new Body({0, 0}, {0, 0}, 10000, 0.0f, 0, new CircleCollider(30));
    Body* planet1 = new Body({200, 0}, {0, 223.61f}, 1, 0.0f, 0, new CircleCollider(10));
    Body* planet2 = new Body({-100, 0}, {0, -316.23f}, 1, 0.0f, 0, new CircleCollider(10));
    Body* planet3 = new Body({0, 150}, {258.2f, 0}, 1, 0.0f, 0, new CircleCollider(10));
    engine.addBody(sun);
    engine.addBody(planet1);
    engine.addBody(planet2);
    engine.addBody(planet3);
    
    // Serialize the initial state to allow "Reset" functionality
    nlohmann::json initialState = Serializer::serialize(engine);
    sf::Clock clock;
    sf::Clock deltaClock; // For ImGui-SFML


    // --- Interaction Flags ---
    bool isCameraFollowing = false; // Camera follows selected body
    bool isDragging = false; // Body dragging with mouse
    bool isVelocityDragging = false; // Velocity vector dragging with mouse
    bool isPanning = false; // Moving the camera
    bool isSelectingBox = false; // Dragging selection box
    bool showPanel = true; // Show/hide control panel

    Vector2d boxStartPos = {0, 0};
    sf::Vector2i lastMousePos;

    // --- Main loop ---
    while(window.isOpen()){
        // 1. Event Handling
        while (const std::optional event = window.pollEvent()){
            ImGui::SFML::ProcessEvent(window, *event);

            if(event->is<sf::Event::Closed>()) window.close();

            // Handle window resizing
            if(const auto* resized = event->getIf<sf::Event::Resized>()){
                sf::FloatRect visibleArea({0, 0}, {resized->size.x, resized->size.y});
                window.setView(sf::View(visibleArea));
            }

            // Check if mouse/keyboard is captured by ImGui
            bool mouseOnUI = ImGui::GetIO().WantCaptureMouse;
            bool keyboardOnUI = ImGui::GetIO().WantCaptureKeyboard;


            if(!keyboardOnUI && !mouseOnUI){
                // --- Keyboard handling ---
                if(const auto& keyEvent = event->getIf<sf::Event::KeyPressed>()){
                    if(keyEvent->code == sf::Keyboard::Key::Space){
                        // Toggle between Editor and Simulation modes (Space key)
                        if(mode ==  AppMode::EDITOR) mode = AppMode::SIMULATION;
                        else mode = AppMode::EDITOR;
                    }else if(keyEvent->code == sf::Keyboard::Key::R){
                        // Reset simulation state to the one saved in initialState buffer (R key)
                        selectedBodies.clear();
                        renderer.setCameraPos({0,0});
                        Serializer::deserialize(engine, initialState);
                    }else if(keyEvent->code == sf::Keyboard::Key::F){
                        // Toggle camera follow on selected body (F key)
                        if(selectedBodies.size() == 1) isCameraFollowing = !isCameraFollowing;
                        else isCameraFollowing = false;
                    }else if(keyEvent->code == sf::Keyboard::Key::C && keyEvent->control){
                        // Copy selected bodies to clipboard (Ctrl + C)
                        if(!selectedBodies.empty()){
                            for(Body* b: clipboard) delete b;
                            clipboard.clear();
                            for(Body* b: selectedBodies) clipboard.push_back(b->clone());
                        }
                    }else if(keyEvent->code == sf::Keyboard::Key::V && keyEvent->control){
                        // Paste bodies from clipboard into the scene (Ctrl + V)
                        if(!clipboard.empty()){
                            Vector2d groupCenter = {0, 0};
                            for(Body* b: clipboard) groupCenter = groupCenter + b->pos;
                            groupCenter = groupCenter / float(clipboard.size());

                            sf::Vector2i mouseScreenPos = sf::Mouse::getPosition(window);
                            Vector2d mousePos = renderer.screenToReal({mouseScreenPos.x, mouseScreenPos.y});
                            Vector2d offset = mousePos - groupCenter;

                            selectedBodies.clear();

                            for(Body* b: clipboard){
                                Body* newBody = b->clone();
                                newBody->pos += offset;
                                engine.addBody(newBody);
                                selectedBodies.push_back(newBody);
                            }
                        }
                    }else if(keyEvent->code == sf::Keyboard::Key::Tab){
                        // Toggle control panel visibility (Tab key)
                        showPanel = !showPanel;
                    }
                }
                // --- Mouse handling ---
                if(const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()){
                    Vector2d mousePos = renderer.screenToReal({mouseEvent->position.x, mouseEvent->position.y});
                    Body* clickedBody = engine.findBodyAt(mousePos);
                    lastMousePos = mouseEvent->position;
                    
                    if(mouseEvent->button == sf::Mouse::Button::Left){
                        if(clickedBody){
                            // Clicked on a body
                            if(mode == AppMode::EDITOR){
                                // Multi-selection logic (Ctrl + Click)
                                bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
    
                                if(ctrlPressed){
                                    if(isBodySelected(selectedBodies, clickedBody)){
                                        selectedBodies.erase(std::remove(selectedBodies.begin(), selectedBodies.end(), clickedBody), selectedBodies.end());
                                    }else{
                                        selectedBodies.push_back(clickedBody);
                                    }
                                } else {
                                    if(!isBodySelected(selectedBodies, clickedBody)){
                                        selectedBodies.clear();
                                        selectedBodies.push_back(clickedBody);
                                    }
                                }
    
                                isDragging = true;
                            }else{
                                // In Simulation mode: single selection only
                                selectedBodies.clear();
                                selectedBodies.push_back(clickedBody);
                                isDragging = false;
                            }

                            isCameraFollowing = false;
                            isSelectingBox = false;
                        }else{
                            // Clicked on empty space
                            if(mode == AppMode::EDITOR){
                                if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)){
                                    selectedBodies.clear();
                                }
    
                                isSelectingBox = true;
                                boxStartPos = mousePos;
                            }else{
                                selectedBodies.clear();
                                isSelectingBox = false;
                            }

                            isDragging = false;
                        }
                    }else if(mouseEvent->button == sf::Mouse::Button::Right){
                        // Right mouse button: initiate velocity vector drag or camera panning
                        if (clickedBody && mode == AppMode::EDITOR) {
                            selectedBodies.clear();
                            selectedBodies.push_back(clickedBody);
                            isVelocityDragging = true;
                        } else {
                            isPanning = true;
                        }
                    }
                }

                // Mouse release logic
                if(const auto& mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()){
                    Vector2d mousePos = renderer.screenToReal({mouseEvent->position.x, mouseEvent->position.y});
                    if(mouseEvent->button == sf::Mouse::Button::Left){
                        if(isSelectingBox){
                            // Select all bodies within the selection box
                            float minX = std::min(boxStartPos.x, mousePos.x);
                            float maxX = std::max(boxStartPos.x, mousePos.x);
                            float minY = std::min(boxStartPos.y, mousePos.y);
                            float maxY = std::max(boxStartPos.y, mousePos.y);

                            for (Body* b : engine.getBodies()) {
                                if (b->pos.x >= minX && b->pos.x <= maxX && b->pos.y >= minY && b->pos.y <= maxY){
                                    if (!isBodySelected(selectedBodies, b)) {
                                        selectedBodies.push_back(b);
                                    }
                                }
                            }
                        }

                        isDragging = false;
                        isSelectingBox = false;
                    }
                    if(mouseEvent->button == sf::Mouse::Button::Right){
                        isVelocityDragging = false;
                        isPanning = false;
                    }
                }

                // Mouse wheel - Zooming in/out
                if(const auto& mouseEvent = event->getIf<sf::Event::MouseWheelScrolled>()){
                    float zoomFactor = 1.1f;
                    float multiplier = mouseEvent->delta > 0 ? zoomFactor : (1.0f / zoomFactor);
                    Config::scale = std::clamp(Config::scale * multiplier, 0.1f, 100.0f);
                }
            }
        }

        // 2. ImGui UI
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 workPos = viewport->WorkPos;
        ImVec2 workSize = viewport->WorkSize;
        
        float controlPanelWidth = 320.0f;
        float inspectorWidth = 300.0f;
        float inspectorHeight = 260.0f;
        float padding = 10.0f;

        ImGuiWindowFlags staticWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

        ImGui::SFML::Update(window, deltaClock.restart());


        // --- Control Panel ---
        if(showPanel){
            ImGui::SetNextWindowPos(workPos);
            ImGui::SetNextWindowSize({controlPanelWidth, workSize.y});

            ImGui::Begin("Control Panel", NULL, staticWindowFlags);

            float width = ImGui::GetContentRegionAvail().x;

            // Play, Pause, Reset, Save State, Add new object buttons
            if (mode == AppMode::EDITOR) {
                if (ImGui::Button("PLAY", ImVec2(width * 0.3f, 30))) mode = AppMode::SIMULATION;
            } else {
                if (ImGui::Button("PAUSE", ImVec2(width * 0.3f, 30))) mode = AppMode::EDITOR;
            }

            ImGui::SameLine();
            if (ImGui::Button("RESET", ImVec2(width * 0.3f, 30))) {
                selectedBodies.clear();
                Serializer::deserialize(engine, initialState);
                renderer.setCameraPos({0,0});
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

            // Physics settings
            if (ImGui::CollapsingHeader("Physics Rules", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Check Body Collisions", &Config::useBodiesCollision);
                ImGui::Checkbox("Check Wall Collisions", &Config::useWindowCollision);
                
                ImGui::Separator();
                
                ImGui::Checkbox("Global Gravity (Down)", &Config::useGravity);
                ImGui::SameLine(); HelpMarker("Standard gravity pulling objects down (F = m*g)");
                
                if (Config::useGravity) {
                    ImGui::DragFloat("Gravity", &Config::gravity, 0.5f, -200.0f, 200.0f, "%.2f");
                }

                ImGui::Checkbox("N-Body Gravity", &Config::useNBodyGravity);
                ImGui::SameLine(); HelpMarker("Orbital mechanics. Every object attracts every other object.");

                if (Config::useNBodyGravity) {
                    ImGui::DragFloat("G Constant", &Config::G, 10.0f, 1.0f, 10000.0f, "%.0f");
                }
                
                ImGui::Checkbox("Electrostatics", &Config::useElectrostatics);
                ImGui::SameLine(); HelpMarker("Coulomb's Law. Objects need 'charge' to interact.");

                if (Config::useElectrostatics) {
                    ImGui::DragFloat("Coulomb K", &Config::K, 100.0f, 0.0f, 100000.0f); 
                }
            }

            // World and view settings
            if (ImGui::CollapsingHeader("World & View", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Camera Zoom:");
                ImGui::SliderFloat("##zoom", &Config::scale, 0.1f, 100.0f, "%.1f px/m", ImGuiSliderFlags_Logarithmic);
                
                ImGui::Text("Simulation Bounds (Meters):");
                Vector2d bounds = engine.getSimBounds();
                float size[2] = { bounds.x, bounds.y };
                if (ImGui::DragFloat2("##bounds", size, 0.5f, 10.0f, 10000.0f)) {
                    engine.setSimBounds(size[0], size[1]);
                }

                ImGui::Separator();
                ImGui::Checkbox("Show Trails", &Config::renderTrails);
                ImGui::Checkbox("Show Boundaries", &Config::renderWorldBounds);
                ImGui::Checkbox("Show Velocity Vectors", &Config::renderVelocityVectors);
            }

            // Save/Load System
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
                        renderer.setCameraPos({0,0});
                        selectedBodies.clear();

                        initialState = Serializer::serialize(engine);
                        mode = AppMode::EDITOR;
                    }
                }
            }

            // Modal popup for Object Creation
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Create Object", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                static int newItemType = 0; // 0 = Circle, 1 = Box
                static float newItemMass = 10.0f;
                static float newItemSize[2] = { 20.0f, 20.0f };
                static float newItemPos[2] = { 0.0f, 0.0f };
                static float newItemVel[2] = { 0.0f, 0.0f };
                static float newItemCharge = 0.0f;
                static float newItemRestitution = 0.8f;
                static bool newItemStatic = false;

                ImGui::Combo("Shape", &newItemType, "Circle\0Box\0");
                
                ImGui::Separator();

                ImGui::InputFloat2("Position (m)", newItemPos);
                ImGui::InputFloat2("Velocity (m/s)", newItemVel);

                ImGui::Checkbox("Is Static", &newItemStatic);

                if (newItemStatic) ImGui::BeginDisabled();
                ImGui::InputFloat("Mass", &newItemMass);
                if (newItemStatic) ImGui::EndDisabled();

                ImGui::SliderFloat("Bounciness", &newItemRestitution, 0.0f, 1.0f);
                ImGui::InputFloat("Charge", &newItemCharge);

                if (newItemType == 0) {
                    ImGui::InputFloat("Radius", &newItemSize[0]);
                } else {
                    ImGui::InputFloat2("Width / Height", newItemSize);
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

                    if(newItemStatic) b->setStatic(true);
                    
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
        }


        // --- Inspector Panel ---
        if (!selectedBodies.empty()) {
            ImVec2 inspectorPos;
            inspectorPos.x = workPos.x + workSize.x - inspectorWidth;
            inspectorPos.y = workPos.y + workSize.y - inspectorHeight;

            ImGui::SetNextWindowPos(inspectorPos);
            ImGui::SetNextWindowSize({inspectorWidth, inspectorHeight});
            
            ImGui::Begin("Inspector", NULL, staticWindowFlags);

            // Single object inspector
            if(selectedBodies.size() == 1){
                const std::vector<Body*>& bodies = engine.getBodies();
                Body* selectedBody = selectedBodies[0];
                int currentIndex = -1;
                for(int i = 0; i < bodies.size(); i++){
                    if(bodies[i] == selectedBody){
                        currentIndex = i;
                        break;
                    }
                }

                // Navigation arrows to cycle through bodies
                if(currentIndex != -1){
                    if(ImGui::ArrowButton("##left", ImGuiDir_Left)){
                        currentIndex--;
                        if(currentIndex < 0) currentIndex = bodies.size() - 1;
                        selectedBodies[0] = bodies[currentIndex];
                        selectedBody = bodies[currentIndex];
                    }
                    
                    ImGui::SameLine();
                    ImGui::Text("Body %d / %d", currentIndex + 1, (int)bodies.size());
                    ImGui::SameLine();
                    
                    if(ImGui::ArrowButton("##right", ImGuiDir_Right)){
                        currentIndex++;
                        if(currentIndex >= bodies.size()) currentIndex = 0;
                        selectedBodies[0] = bodies[currentIndex];
                        selectedBody = bodies[currentIndex];
                    }
                }

                ImGui::SameLine();

                if (isCameraFollowing) {
                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.35f, 0.6f, 0.6f));
                    if (ImGui::Button("LOCKED (F)")) isCameraFollowing = false;
                    ImGui::PopStyleColor();
                } else {
                    if (ImGui::Button("Follow (F)")) isCameraFollowing = true;
                }

                ImGui::Separator();

                // Property editors
                float pos[2] = { selectedBody->pos.x, selectedBody->pos.y };
                float vel[2] = { selectedBody->vel.x, selectedBody->vel.y };

                if (ImGui::DragFloat2("Position", pos)) {
                    selectedBody->pos = Vector2d(pos[0], pos[1]);
                }
                if (ImGui::DragFloat2("Velocity", vel)) {
                    selectedBody->vel = Vector2d(vel[0], vel[1]);
                }

                bool isStatic = selectedBody->isStatic();
                if (ImGui::Checkbox("Static Body", &isStatic)) {
                    selectedBody->setStatic(isStatic);
                }

                if (isStatic) {
                    ImGui::BeginDisabled();
                }

                float mass = selectedBody->getMass();
                if (ImGui::DragFloat("Mass", &mass, 1.0f, 0.1f, 10000.0f)) {
                    selectedBody->setMass(mass);
                }

                if (isStatic) {
                    ImGui::EndDisabled();
                    ImGui::SameLine(); HelpMarker("Uncheck 'Static' to edit mass.");
                }

                float charge = selectedBody->charge;
                if(ImGui::DragFloat("Charge", &charge, 1.0f, 0.0f, 0.0f)){
                    selectedBody->charge = charge;
                }

                float restitution = selectedBody->restitution;
                if (ImGui::SliderFloat("Bounciness", &restitution, 0.0f, 1.0f)){
                    selectedBody->restitution = restitution;
                }
                
                ImGui::Separator();

                // Shape-specific properties
                if (selectedBody->collider->shapeType == CIRCLE){
                    CircleCollider* c = static_cast<CircleCollider*>(selectedBody->collider);
                    ImGui::DragFloat("Radius", &c->r, 0.5f, 1.0f, 500.0f);
                } 
                else if (selectedBody->collider->shapeType == BOX){
                    BoxCollider* b = static_cast<BoxCollider*>(selectedBody->collider);
                    float bodySize[2] = {b->width, b->height};
                    if(ImGui::DragFloat2("Width/Height", bodySize, 1.0f, 1.0f, 1000.0f)){
                        b->width = bodySize[0];
                        b->height = bodySize[1];
                    }
                }
                
                ImGui::Separator();
                if (ImGui::Button("Delete Body", ImVec2(-1, 0))){
                    engine.removeBody(selectedBody);
                    selectedBodies.clear();
                }
            }else{
                // Multiple objects inspector
                ImGui::Text("%d objects selected", (int)selectedBodies.size());

                bool allCircles = true;
                bool allBoxes = true;
                
                for (Body* b : selectedBodies){
                    if (b->collider->shapeType != CIRCLE) allCircles = false;
                    if (b->collider->shapeType != BOX) allBoxes = false;
                }

                ImGui::Separator();

                // Batch property editors
                bool allStatic = true;
                for(auto* b : selectedBodies) if(!b->isStatic()) allStatic = false;

                if (ImGui::Checkbox("Static (All)", &allStatic)) {
                    for(auto* b : selectedBodies) b->setStatic(allStatic);
                }

                if (allStatic) ImGui::BeginDisabled();
                
                float commonMass = selectedBodies[0]->getMass(); 
                if (ImGui::DragFloat("Mass (All)", &commonMass, 1.0f, 0.1f, 10000.0f)) {
                    for (auto* b : selectedBodies) b->setMass(commonMass);
                }

                if (allStatic) ImGui::EndDisabled();
                
                float commonRestitution = selectedBodies[0]->restitution;
                if (ImGui::SliderFloat("Bounciness (All)", &commonRestitution, 0.0f, 1.0f)){
                    for (Body* b : selectedBodies) b->restitution = commonRestitution;
                }

                float commonCharge = selectedBodies[0]->charge;
                if (ImGui::DragFloat("Charge (All)", &commonCharge, 1.0f, -1000.0f, 1000.0f)){
                    for (Body* b : selectedBodies) b->charge = commonCharge;
                }

                ImGui::Separator();

                // Batch resize
                if (allCircles) {
                    float r = static_cast<CircleCollider*>(selectedBodies[0]->collider)->r;
                    if (ImGui::DragFloat("Radius (All)", &r, 0.5f, 1.0f, 500.0f)) {
                        for (Body* b : selectedBodies) {
                            static_cast<CircleCollider*>(b->collider)->r = r;
                        }
                    }
                }else if (allBoxes){
                    BoxCollider* col = static_cast<BoxCollider*>(selectedBodies[0]->collider);
                    float w = col->width;
                    float h = col->height;
                    
                    bool changed = false;
                    changed |= ImGui::DragFloat("Width (All)", &w, 1.0f, 1.0f, 1000.0f);
                    changed |= ImGui::DragFloat("Height (All)", &h, 1.0f, 1.0f, 1000.0f);

                    if (changed) {
                        for (Body* b : selectedBodies) {
                            BoxCollider* box = static_cast<BoxCollider*>(b->collider);
                            box->width = w;
                            box->height = h;
                        }
                    }
                }
                
                ImGui::Separator();
                
                if (ImGui::Button("Delete All Selected", ImVec2(-1, 0))) {
                    for(Body* b: selectedBodies) engine.removeBody(b);
                    selectedBodies.clear();
                }
            }
            ImGui::End();
        }


        // 3. Performance Overlay
        float dt = clock.restart().asSeconds();
        if(dt > 0.1f) dt = 0.1f; // Cap dt to prevent explosion on frame drops

        ImVec2 windowPos({workPos.x + workSize.x - padding, workPos.y + padding});
        ImVec2 windowPosPivot({1.0f, 0.0f});

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        
        if (ImGui::Begin("Performance Overlay", NULL, windowFlags)) {
            float fps = 1.0f / dt;
            float ms = dt * 1000.0f;

            ImGui::Text("FPS: %.1f", fps);
            ImGui::Text("Frame Time: %.2f ms", ms);
            ImGui::Separator();
            ImGui::Text("Bodies: %d", (int)engine.getBodies().size());
            
            static float values[90] = {};
            static int values_offset = 0;
            static float refresh_time = 0.0f;
            
            if (refresh_time == 0.0f) refresh_time = ImGui::GetTime();
            while (refresh_time < ImGui::GetTime()) {
                values[values_offset] = fps;
                values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
                refresh_time += 1.0f / 60.0f;
            }
            
            ImGui::PlotLines("##fps", values, IM_ARRAYSIZE(values), values_offset, NULL, 0.0f, 100.0f, ImVec2(0, 40.0f));
        }
        ImGui::End();

        // 4. Simulation Update & Rendering
        if(mode == AppMode::SIMULATION){
            engine.update(dt);
        }

        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        Vector2d mousePos = renderer.screenToReal({mouse.x, mouse.y});

        // Mouse dragging logic
        if(mode == AppMode::EDITOR) {
            if(isDragging && !selectedBodies.empty()){
                Vector2d prevMouseWorld = renderer.screenToReal({lastMousePos.x, lastMousePos.y});
                Vector2d delta = mousePos - prevMouseWorld;

                for(Body* b: selectedBodies) {
                    b->pos += delta;
                }
            }else if(isVelocityDragging && selectedBodies.size() == 1){
                Body* selectedBody = selectedBodies[0];
                selectedBody->vel = -1*(mousePos - selectedBody->pos);
            }
        }
        
        // Camera panning logic
        if(isPanning){
            sf::Vector2i mousePosDiff = lastMousePos - mouse;
            float moveX = mousePosDiff.x / Config::scale;
            float moveY = -mousePosDiff.y / Config::scale;

            renderer.moveCamera({moveX, moveY});

            lastMousePos = mouse;
        }

        if(isCameraFollowing && selectedBodies.size() == 1){
            renderer.setCameraPos(selectedBodies[0]->pos);
        }

        lastMousePos = mouse;

        // Rendering
        window.clear(Config::COLOR_BACKGROUND);
        renderer.render(engine);

        if(isSelectingBox) {
            renderer.drawSelectionBox(boxStartPos, mousePos);
        }
        
        for(Body* b: selectedBodies){
           renderer.drawSelection(*b);
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}