#include <SFML/Graphics.hpp>
#include "Game.h"
#include <iostream>
#include "Vec2.hpp"
// #include <cassert>
// #include <filesystem>
#include <chrono>
#include <random>


int main()
{
    // std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    Game g("./assets/config.txt");
    g.run();
}