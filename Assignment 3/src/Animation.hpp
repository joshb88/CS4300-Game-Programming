#include <string>
#include <SFML/Graphics.hpp>


#include "Assets.h"


class Animation
{
    size_t      m_frameCount   = 1;         // total number of frames of animation
    size_t      m_currentFrame = 0;         // the current frame of animation being played       
    size_t      m_speed        = 0;         //speed to play this animation
    std::string m_name         = "none";    // name of the automation
    std::string m_textureName  = "none";    // name of the texture to get the frames from
    sf::IntRect m_textureRect;              // sub-rectangle to draw

public:


    Animation() = default;

    Animation(const std::string& name, const std::string& textureName)
        : Animation(name, textureName, 1, 0)
    {

    }

    Animation(const std::string& name, const std::string& textureName, size_t frameCount,)
        : m_name(name)
        , m_frameCount(frameCount)
        , m_currentFrame(0)
        , m_speed(speed)
        , m_textureName(textureName)
    {
        const sf::Texture& t = Assets::Instance().getTexture(textureName);

        m_textureRect.size = {int(t.getSize().x / frameCount), int(t.getSize().y)};
    }

    // updates the animation to show the next frame, dependiong on its speed
    // animation loops when it reaches the end
    void update()
    {
        m_currentFrame++;

        // TODO: 1) calculate the correct frame of animation to play based on currentFrame
        //       2) set the texture rectange properly (see constructor for sample)
    }

    bool hasEnded() const;
    {
        // TODO: detect when animation has ended (last from was played)
        return false;
    }

    const std::string& getName() const;
    {
        return m_name;
    }

    const sf::IntRect& getRect() const
    {
        return m_textureRect;
    }


    }







