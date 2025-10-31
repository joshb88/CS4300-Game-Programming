// #include <print>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

int main(int argc, char* argv[]) {

    unsigned int height, width;

    std::ifstream config_file("./config.txt");

    if (!config_file) {
        std::cerr << "Couldn't open the config file." << std::endl;
        return -1;
    }

    struct shapeData
    {
        std::string name;
        float speed[2]{0.0, 0.0};
        bool visible{true};
        bool text_visible{true};
        float scale{1.0f};
        unsigned int vertices{32};
        bool is_square{false};

        std::unique_ptr<sf::Shape> shape;
        std::unique_ptr<sf::Text> text;
    };
    
    bool windowSizeFound = false;
    bool fontFound = false;
    sf::Font myFont;
    unsigned int font_size;
    sf::Color font_color;

    std::vector<shapeData> shape_list;
    
    std::string line;
    
    // check for the window/font things first.
    while(std::getline(config_file, line)) {
        std::istringstream iss(line);
        std::string key;

        iss >> key;

        if (key == "Window") {
            // Window config
            iss >> width >> height;
            windowSizeFound = true;
        }

        if (key == "Font") {
            // File Config
            std::string font_path_str;
            
            iss >> font_path_str >> font_size >> font_color.r >> font_color.g >> font_color.b;
            std::filesystem::path font_path = std::filesystem::path(font_path_str);
            if (!myFont.openFromFile(font_path)) {
                std::cerr << "Couldn't open the font: " << font_path << std::endl;
                return -1;
            }
            fontFound = true;
        }
    }
    
    if (!windowSizeFound) {
        std::cout << "No config settings for Window Size; using default (800x600)" << std::endl;
        unsigned int width = 800;
        unsigned int height = 600;
    }
    const unsigned int wWidth = width, wHeight = height;
    
    if (!fontFound) {
        std::cerr << "No font file found, exiting..." << std::endl;
        return -1;
    }

    // clear and look for shapes
    config_file.clear();
    config_file.seekg(0);

    while(std::getline(config_file, line)) {
        std::istringstream iss(line);
        std::string key;

        iss >> key;

        if (key == "Rectangle") {
            shapeData rect;
            float x, y;
            int r, g, b;
            float size[2];
            iss >> rect.name >> x >> y >> rect.speed[0] >> rect.speed[1] >> r >> g >> b >> size[0] >> size[1];
            rect.shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(size[0], size[1]));
            rect.shape->setPosition({x,y});
            rect.shape->setFillColor({(uint8_t)r, (uint8_t)g, (uint8_t)b});
            rect.text = std::make_unique<sf::Text>(myFont, rect.name, font_size);
            sf::Vector2f textSize{rect.text->getLocalBounds().size};
            rect.text->setPosition({
                rect.shape->getPosition().x + ((rect.shape->getPosition().x - textSize.x) / 2),
                rect.shape->getPosition().y + ((rect.shape->getPosition().y - textSize.y) / 2)
            });
            shape_list.push_back(std::move(rect));
        }

        else if (key == "Circle") {
            shapeData circle;
            float x, y;
            int r, g, b;
            float rad;
            iss >> circle.name >> x >> y >> circle.speed[0] >> circle.speed[1] >> r >> g >> b >> rad;
            circle.shape = std::make_unique<sf::CircleShape>(rad, circle.vertices);
            circle.shape->setPosition({x,y});
            circle.shape->setFillColor({(uint8_t)r, (uint8_t)g, (uint8_t)b});
            circle.text = std::make_unique<sf::Text>(myFont, circle.name, font_size);
            sf::Vector2f textSize{circle.text->getLocalBounds().size};
            circle.text->setPosition({
                circle.shape->getPosition().x + ((circle.shape->getPosition().x - textSize.x)/2),
                circle.shape->getPosition().y + ((circle.shape->getPosition().y - textSize.y)/2)
            });
            shape_list.push_back(std::move(circle));
        }
    }
    
    sf::RenderWindow window(sf::VideoMode({ wWidth, wHeight }), "SFML works!");
    window.setFramerateLimit(60); // limit frame rate to 60 fps

    // initialize imgui and create a clock used for its internal timing
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Could not initialize window" << std::endl;
        return -1;
    }

    sf::Clock deltaClock;

    // scale the imgui ui and textd size by 2
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;
    
    // attempt to load the font from a file
    if (!myFont.openFromFile("./fonts/PressStart2P.ttf")) {
        // if we can't load the font, print and error and exit
        std::cerr << "Could not load the font" << std::endl;
        return -1;
    }

    // main loop - continues for each from while windows is open
    while (window.isOpen()) {
        // event handling
        while (auto event = window.pollEvent()) {
            // pass the event to imgui to be parse
            ImGui::SFML::ProcessEvent(window, *event);

            // this event triggers when the window is closed
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // update imgui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());

        // clear draw window (or have solitaire end game)
        window.clear();

        // draw the UI
        ImGui::Begin("Shapes");
        ImGui::Text("Shape Settings");

        std::vector<const char*> shapes_available;
        for (const auto& shape : shape_list) {
            shapes_available.push_back(shape.name.c_str());
        }

        static int item_selected_idx = 0; // Here we store our selection data as an index.
        const char* combo_preview_value = (item_selected_idx >= 0 && item_selected_idx < shapes_available.size()) ? shapes_available[item_selected_idx] : "Select a shape";
        if (ImGui::BeginCombo("Shape Selection", combo_preview_value, ImGuiComboFlags_WidthFitPreview))
        {
            static ImGuiTextFilter filter;
            if (ImGui::IsWindowAppearing())
            {
                ImGui::SetKeyboardFocusHere();
                filter.Clear();
            }
            ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
            filter.Draw("##Filter", -FLT_MIN);

            for (int n = 0; n < shapes_available.size(); n++)
            {
                const bool is_selected = (item_selected_idx == n);
                if (filter.PassFilter(shapes_available[n]))
                    if (ImGui::Selectable(shapes_available[n], is_selected))
                        item_selected_idx = n;
            }
            ImGui::EndCombo();
        }

        shapeData& shape_selected = shape_list[item_selected_idx];
        // if its a circle, do circle stuff
        if (auto* circle = dynamic_cast<sf::CircleShape*>(shape_selected.shape.get())) {
            float radius = circle->getRadius();
            ImGui::SliderFloat("Radius", &radius, 0.0f, 300.0f);
            circle->setRadius(radius);

            ImGui::SliderInt("Sides", (int*)&shape_selected.vertices, 3, 64);
            circle->setPointCount(shape_selected.vertices);
        }

        // if its a rectangle, do rectangle stuff
        if (auto* rect = dynamic_cast<sf::RectangleShape*>(shape_selected.shape.get())) {
            sf::Vector2f rect_size = rect->getSize();
            if (!shape_selected.is_square) {
                ImGui::SliderFloat("Width", &rect_size.x, 0.0f, 300.0f);
                rect->setSize({ rect_size.x, rect->getSize().y });
                ImGui::SliderFloat("Height", &rect_size.y, 0.0f, 300.0f);
                rect->setSize({ rect->getSize().x, rect_size.y });
            } else {
                ImGui::SliderFloat("Side Length", &rect_size.x, 0.0f, 300.0f);
                rect->setSize({ rect_size.x, rect_size.x });
            }
            ImGui::Checkbox("Square", &shape_selected.is_square);

            
        }

        sf::Color shape_color{(shape_selected.shape->getFillColor())};
        float imgui_shape_color[3] = { 
            (float)shape_color.r / 255.0f, 
            (float)shape_color.g / 255.0f, 
            (float)shape_color.b / 255.0f 
        };
        
        ImGui::ColorEdit3("Color Shape", imgui_shape_color);
        shape_selected.shape->setFillColor(sf::Color(uint8_t(imgui_shape_color[0]*255), uint8_t(imgui_shape_color[1]*255), uint8_t(imgui_shape_color[2]*255)));
        ImGui::Checkbox("Draw Shape", &shape_selected.visible);
        ImGui::SameLine(); ImGui::Checkbox("Show Text", &shape_selected.text_visible);
        if (ImGui::Button("Reset Shape")) {
            shape_selected.shape->setPosition({ 0, 0 });
        }
        ImGui::End();

        for (shapeData& current_shape : shape_list) {
    
            // basic animation - move each frame if it's still in frame
            current_shape.shape->setPosition({ current_shape.shape->getPosition().x + current_shape.speed[0], current_shape.shape->getPosition().y + current_shape.speed[1] });
    
            // keep shapes in bounds
            sf::Vector2f selected_size = current_shape.shape->getLocalBounds().size;
            sf::Vector2f selected_pos = current_shape.shape->getPosition();
            if (selected_pos.x < 0 || (selected_pos.x + selected_size.x) > wWidth) {
                current_shape.speed[0] *= -1;
            }
            if (selected_pos.y < 0 || (selected_pos.y + selected_size.y) > wHeight) {
                current_shape.speed[1] *= -1;
            }
    
            sf::Vector2f textSize{current_shape.text->getLocalBounds().size};
            if (auto* circle = dynamic_cast<sf::CircleShape*>(current_shape.shape.get())) {
                current_shape.text->setPosition({
                    (float)circle->getPosition().x + (circle->getRadius() - textSize.x / 2.0f),
                    (float)circle->getPosition().y + (circle->getRadius() - textSize.y / 2.0f)
                });
            } else if (auto* rect = dynamic_cast<sf::RectangleShape*>(current_shape.shape.get())) {
                current_shape.text->setPosition({
                    (float)rect->getPosition().x + ((rect->getSize().x - textSize.x) / 2.0f),
                    (float)rect->getPosition().y + ((rect->getSize().y - textSize.y) / 2.0f)
                });
            }
    
                             // clear draw window (or have solitaire end game)
            // basic render function calls
            if (current_shape.visible) {       // draw the circle if the boolean is true
                window.draw(*current_shape.shape);
            }
            if (current_shape.text_visible) {         // draw the text if boolean is true
                window.draw(*current_shape.text);
            }
        }
    
        ImGui::SFML::Render(window);    //draw the ui last so its on top
        window.display();               // call the window display function
    }

    return 0;
}