#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <cassert>
#include <iostream>
#include <fstream>

class Animation;

class Assets
{
    std::map<std::string, sf::Texture>  m_textureMap;
    std::map<std::string, Animation>    m_animationMap;
    std::map<std::string, sf::Font>     m_fontMap;

    void addTexture(const std::string& textureName, const std::string& path, bool smooth = true);
    void addAnimation(const std::string& animationName, const std::string& textureName, size_t numberFrames, size_t speedFrames);
    void addFont(const std::string& fontName, const std::string& path);

    Assets() = default;

public:

    static Assets& Instance();

    void loadFromFile(const std::string& path);

    const sf::Texture& getTexture(const std::string& textureName) const;
    const Animation& getAnimation(const std::string& animationName) const;
    const sf::Font& getFont(const std::string& fontName) const;
    const std::map<std::string, sf::Texture>& getTextures() const;
    const std::map<std::string, Animation>& getAnimations() const;
    // const std::map<std::string, sf::Font>& getFonts() const;
};