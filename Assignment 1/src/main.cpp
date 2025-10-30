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

    std::ifstream config_file("./bin/config.txt");

    if (!config_file) {
        std::cerr << "Couldn't open the config file." << std::endl;
        return -1;
    }

    struct shapeData
    {
        std::string name;
        float speed[2]{0.0, 0.0};
        bool visible = true;
        float scale = 1.0f;
        unsigned int vertices = 32;

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
            std::filesystem::path font_path = std::filesystem::path("./bin/") / font_path_str;
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

        if (key == "Rectange") {
            shapeData rect;
            float x, y;
            uint8_t r, g, b;
            float size[2];
            iss >> rect.name >> x >> y >> rect.speed[0] >> rect.speed[1] >> r >> g >> b >> size[0] >> size[1];
            rect.shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(size[0], size[1]));
            rect.shape->setPosition({x,y});
            rect.shape->setFillColor({r, g, b});
            rect.text = std::make_unique<sf::Text>(myFont, rect.name, font_size);
            shape_list.push_back(std::move(rect));
        }

        if (key == "Circle") {
            shapeData circle;
            float x, y;
            uint8_t r, g, b;
            float rad;
            iss >> circle.name >> x >> y >> circle.speed[0] >> circle.speed[1] >> r >> g >> b >> rad;
            circle.shape = std::make_unique<sf::CircleShape>(rad, circle.vertices);
            circle.shape->setPosition({x,y});
            circle.shape->setFillColor({r, g, b});
            circle.text = std::make_unique<sf::Text>(myFont, circle.name, font_size);
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

    // trhe iumgui color {r, g, b} wheel requires floats from 0-1
    // sfml will require instead uint8_t from 0-255
    // this is the only really annoying conversion between sfml and imgui
    float c[3] = { 0.0f, 1.0f, 1.0f };

    // let's make a shape that we will draw to the screen
    float circleRadius = 50;    // radius to draw the circle
    int circleSegments = 32;    // number of segments to draw the circle with
    float circleSpeedX = 1.0f;  // we will use this to move the circle  later
    float circleSpeedY = 0.5f;  // you will read these values from the file
    bool drawCircle = true;     // whether or not to draw the circle
    bool drawText = true;       // whether or not to draw the text

    // create the sfml circle shape based on our parameters
    sf::CircleShape circle(circleRadius, circleSegments);       // create a circle shape 
    circle.setPosition({ 10.0f, 10.0f });                       // set the top-left position

    // let's load a font so we can display text
    // sf::Font myFont;

    // attempt to load the font from a file
    if (!myFont.openFromFile("./bin/fonts/PressStart2P.ttf")) {
        // if we can't load the font, print and error and exit
        std::cerr << "Could not load the font" << std::endl;
        return -1;
    }

    // set up the text object that will be drawn to the screen
    sf::Text text(myFont, "Sample Text", 24);

    // position to top-left corner of the text so that the text aligns on the bottom
    // textd character size is in pixels, so move the text up from the bottom by its height
    text.setPosition({ 0, wHeight - (float)text.getCharacterSize() });

    // set up a character array to set the text
    char displayString[255] = "Sample Text";

    // main loop - continues for each from while windows is open
    while (window.isOpen()) {
        // event handling
        while (auto event = window.pollEvent()) {
            // pass the event to imgui to be parse
            ImGui::SFML::ProcessEvent(window, *event);

            // this event driggers when the window is closed
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // this event is triggered when a key is pressed
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // print the key that was pressed into the console
                std::cout << "Key was pressed with code " << int(keyPressed->scancode) << std::endl;

                if (keyPressed->scancode == sf::Keyboard::Scancode::X) {
                    circleSpeedX *= -1.0f;
                }
            }
        }

        // update imgui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());

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
        ImGui::Checkbox("Draw Circle", &shape_selected.visible);
        ImGui::ColorEdit3("Color Circle", c);
        ImGui::InputText("Text", displayString, 255);
        if (ImGui::Button("Set Texts")) {
            text.setString(displayString);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Circle")) {
            circle.setPosition({ 0, 0 });
        }
        ImGui::End();

        // set the circle properties, because they may have been updated with the ui
        circle.setPointCount(circleSegments);
        circle.setRadius(circleRadius);

        // imgui uses 0-1 float for colors, sfml uses 0-255 for colors
        // we must convert from the ui floats to sfml Uint8
        circle.setFillColor(sf::Color(uint8_t(c[0]*255), uint8_t(c[1]*255), uint8_t(c[2]*255)));

        // basic animation - move each frame if it's still in frame
        circle.setPosition({ circle.getPosition().x + circleSpeedX, circle.getPosition().y + circleSpeedY });

        // basic render function calls
        window.clear();         // clear draw window (or have solitaire end game)
        if (drawCircle) {       // draw the circle if the boolean is true
            window.draw(circle);
        }
        if (drawText) {         // draw the text if boolean is true
            window.draw(text);
        }
        ImGui::SFML::Render(window);    //draw the ui last so its on top
        window.display();               // call the window display function
    }

    return 0;
}

