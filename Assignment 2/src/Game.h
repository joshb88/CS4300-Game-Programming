#pragma once

#include "Entity.hpp"
#include "EntityManager.hpp"

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_stdlib.h"

struct GameConfig {bool sPlayer = true, sMovement = true, sUserInput = true, sLifespan = true, sEnemySpawner = true, sCollision = true, sShoot = true; };
struct WindowConfig { unsigned int W, H, FL; bool FS; };
struct FontConfig   { std::string P; int S, R, G, B; };
/*
Player Specification:
Player SR CR S FR FG FB OR OG OB OT V
Shape Radius       SR          int
Collision Radius   CR          int
Speed              S           float
Fill Color         FR,FG,FB    int,int, int
Outline Color      OR,OG,OB    int, int, int
Outline Thickness  OT          int
Shape Vertices     V           int
*/ 
struct PlayerConfig { int SR, CR; float S; int FR, FG, FB, OR, OG, OB, OT, V; };
/*
Enemy Specification:
Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
  Shape Radius          SR              int
  Collision Radius      CR              int
  Min / Max Speed       SMIN, SMAX      float, float
  Outline Color         OR, OG, OB      int, int, int
  Outline Thickness     OT              int
  Min/Max Vertices      VMIN, VMAX      int, int
  Small Lifespan        L               int
  Spawn Interval        SI              int
*/
struct EnemyConfig  { int SR, CR; float SMIN, SMAX; int OR, OG, OB, OT, VMIN, VMAX, L, SI; };
/*
Bullet Specification:
Bullet SR CRS FR FG FB OR OG OB OT V L
Shape Radius            SR              int
Collision Radius        CR              int
Speed                   S               float
Fill Color              FR, FG, FB      int, int, int
Outline Color           OR, OG, OB      int, int, int
Outline Thickness       OT              int
Shape Vertices          V               int
Lifespan                L               int
*/
struct BulletConfig { int SR, CR; float S; int FR, FG, FB, OR, OG, OB, OT, V, L; };

class Game
{
    sf::RenderWindow m_window;             // the window we will draw to
    EntityManager m_entities;              // vector of entities to maintain
    sf::Font m_font;                       // the font we will use to draw
    sf::Text m_text;                       // the score text to be drawn to the screen
    GameConfig m_gameConfig;
    WindowConfig m_windowConfig;
    FontConfig m_fontConfig;
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;
    sf::Clock m_deltaClock;
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0;
    bool m_paused = false;                  // whether we update game logic
    bool m_mainMenuUp = false;

    void init(const std::string& config);   // initialize the GameState with a config file
    void setPaused(bool paused);            // paused the game
    
    void sMovement();                       // System: Entity position / movement update
    void sUserInput();                      // System: User input
    void sLifespan();                       // System: Lifespan
    void sRender();                         // System: Render / Drawing
    void sGUI();                            
    void sEnemySpawner();                   // System: Spawns Enemies
    void sCollision();                      // System: Collision
    void sShoot();                          // for shooting
    
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

    std::shared_ptr<Entity> player();

public:

    Game(const std::string& config); // constructor, takes in game config

    void run();    
};