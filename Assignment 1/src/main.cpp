// #include <print>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

int main(int argc, char* argv[]) {

    std::ifstream config_file("./Assignment 1/bin/config.txt");

    if (!config_file) {
        std::cerr << "Coudn't open the config file." << std::endl;
        return -1;
    }

    std::string line;
    unsigned int width, height;
    bool windowSizeFound = false;
    
    while(std::getline(config_file, line)) {
        std::istringstream iss(line);
        std::string key;

        iss >> key;

        if (key == "Window") {
            iss >> width >> height;
            windowSizeFound = true;
        }
    }
    
    if (!windowSizeFound) {
        std::cout << "No config settings for Window Size; using default (800x600)" << std::endl;
        width = 800;
        height = 600;
    }
    const unsigned int wWidth = width, wHeight = height;

    // create a new window of size w*h pixels
    // top left of the window is (0,0) and bottom right is (w, h)
    // you will have to read these from the config file
    // const int wWidth = 1280;
    // const int wHeight = 720;
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
    sf::Font myFont;

    // attempt to load the font from a file
    if (!myFont.openFromFile("./Assignment 1/bin/fonts/PressStart2P.ttf")) {
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
        ImGui::Begin("Window title");
        ImGui::Text("WIndow text!");
        ImGui::Checkbox("Draw Circle", &drawCircle);
        ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f);
        ImGui::SliderInt("Sides", &circleSegments, 3, 64);
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
        if (drawText) {         // draw the texdt if boolean is true
            window.draw(text);
        }
        ImGui::SFML::Render(window);    //draw the ui last so its on top
        window.display();               // call the window display function
    }

    return 0;
}

