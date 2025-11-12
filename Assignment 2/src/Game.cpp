#include "Game.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <random>
#include <chrono>

Game::Game(const std::string & config)
    : m_text(m_font, "Default", 24)
{
    init(config);
}

void Game::init(const std::string& path)
{
    // TODO: read in config file here
    //       use the premade PlayerConfig, EnemyConfig, BulletConfig variables

    std::fstream file("./assets/config.txt");
    if (!file) {
        std::cerr << "Couldn't open config, exiting..." << std::endl;
        std::exit(-1);
    }

    std::string line;

    while(std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;

        iss >> key;

        if (key == "Window") {
            iss >> m_windowConfig.W
                >> m_windowConfig.H
                >> m_windowConfig.FL
                >> m_windowConfig.FS;
        }
        else if (key == "Font") {
            iss >> m_fontConfig.P 
                >> m_fontConfig.S 
                >> m_fontConfig.R 
                >> m_fontConfig.G 
                >> m_fontConfig.B;
            if (!m_font.openFromFile(m_fontConfig.P)) {
                std::cerr << "Couldn't load font from path in config, exiting..." << std:: endl;
                std::exit(-1);
            }
            m_text.setFont(m_font);
            m_text.setString("Score: ");
            m_text.setCharacterSize(m_fontConfig.S);
            m_text.setFillColor(sf::Color({(uint8_t)m_fontConfig.R, (uint8_t)m_fontConfig.G, (uint8_t)m_fontConfig.B}));
        }
        else if (key == "Player") {
            iss >> m_playerConfig.SR
                >> m_playerConfig.CR
                >> m_playerConfig.S
                >> m_playerConfig.FR
                >> m_playerConfig.FG
                >> m_playerConfig.FB
                >> m_playerConfig.OR
                >> m_playerConfig.OG
                >> m_playerConfig.OB
                >> m_playerConfig.OT
                >> m_playerConfig.V;
        }
        else if (key == "Enemy") {
            iss >> m_enemyConfig.SR
                >> m_enemyConfig.CR
                >> m_enemyConfig.SMIN
                >> m_enemyConfig.SMAX
                >> m_enemyConfig.OR
                >> m_enemyConfig.OG
                >> m_enemyConfig.OB
                >> m_enemyConfig.OT
                >> m_enemyConfig.VMIN
                >> m_enemyConfig.VMAX
                >> m_enemyConfig.L
                >> m_enemyConfig.SI;
        }
        else if (key == "Bullet") {
            iss >> m_bulletConfig.SR
                >> m_bulletConfig.CR
                >>m_bulletConfig.S
                >> m_bulletConfig.FR
                >> m_bulletConfig.FG
                >> m_bulletConfig.FB
                >> m_bulletConfig.OR
                >> m_bulletConfig.OG
                >> m_bulletConfig.OB
                >> m_bulletConfig.OT
                >> m_bulletConfig.V 
                >>m_bulletConfig.L; 
        }
    }

    //set up default window parameters
    sf::State state = m_windowConfig.FS ? sf::State::Fullscreen : sf::State::Windowed;
    m_window.create(sf::VideoMode({ m_windowConfig.W, m_windowConfig.H }), "Assignment 2", sf::Style::Default, state);
    m_window.setKeyRepeatEnabled(false);
    m_window.setFramerateLimit(m_windowConfig.FL);
 
    if (!ImGui::SFML::Init(m_window)) {
        std::cerr << "Could not initialize window; exiting..." << std::endl;
        std::exit(-1);
    }
    
    // scale the imgui ui and text size by 2
    // ImGui::GetStyle().ScaleAllSizes(2.0f);
    // ImGui::GetIO().FontGlobalScale = 2.0f;
    ImGui::GetStyle().ScaleAllSizes(1.0f);
    ImGui::GetIO().FontGlobalScale = 1.0f;

    spawnPlayer();
}

std::shared_ptr<Entity> Game::player()
{
    return m_entities.getEntities("player").back();
}

void Game::run()
{
    // TODO: add pause functionality in here
    //       some systems should function while paused (rendering)

    while (m_window.isOpen())
    {
        while (auto event = m_window.pollEvent()) {
            // pass the event to imgui to be parse
            ImGui::SFML::ProcessEvent(m_window, *event);

            // this event triggers when the window is closed
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }
        }

        // update the entity manager
        m_entities.update();

        // required update call to imgui
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        if (!player()->isAlive()) { spawnPlayer(); }
        if (m_gameConfig.sUserInput) { sUserInput(); }
        if (m_gameConfig.sEnemySpawner) { sEnemySpawner(); }
        if (m_gameConfig.sShoot) { sShoot(); }
        if (m_gameConfig.sMovement) { sMovement(); }
        if (m_gameConfig.sLifespan) { sLifespan(); }
        if (m_gameConfig.sCollision) { sCollision(); }
        sGUI();
        sRender();

        // increment the current frame
        // may need to be moved when pause implemented
        m_currentFrame++;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            std::cout << "Exiting..." << std::endl;
            std::exit(0);
        }
    }
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
    // TODO Finish adding all the properties of the player with the correct values from the config

    // We create every entity by calling EntityManager.addEntity(tag)
    // This returns a std::shared_ptr<Entity>, so we use auto to save typing
    auto e = m_entities.addEntity("player");

    //Give this entity a Transform so it spawns at (200, 200) with a velocity of (1, 1) and angle 0
    e->add<CTransform>(Vec2f(m_window.getSize().x / 2, m_window.getSize().y / 2), Vec2f(1.0f, 1.0f), 0.0f, m_playerConfig.S);

    e->add<CCollision>(m_playerConfig.CR);

    // The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
    e->add<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

    // Add an input component to the player so that we can use inputs
    e->add<CInput>();

    // Add a weapon component for auto firing or powerups (if we feel like it later)
    e->add<CWeapon>();

    e->add<CScore>();
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
    auto enemy = m_entities.addEntity("enemy");

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // unsigned seed = 12345;
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> rand_255(0U, 255U);
    std::uniform_int_distribution<int> rand_verts(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

    unsigned fr = rand_255(rng), fg = rand_255(rng), fb = rand_255(rng);
    unsigned vertices = rand_verts(rng);

    enemy->add<CShape>(
        m_enemyConfig.SR,
        vertices,
        sf::Color(fr, fg, fb),
        sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
        m_enemyConfig.OT);

    // nvm, being lazy
    // // dont spawn the mobs on top of the player
    // unsigned rand_x, rand_y;
    // do
    // {
    //     std::uniform_int_distribution<int> distx(m_enemyConfig.SR, m_window.getSize().x - m_enemyConfig.SR);
    //     std::uniform_int_distribution<int> disty(m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR);
    //     rand_x = distx(rng);
    //     rand_y = disty(rng);
    // } while (rand_x == player()->get<CShape>().circle.getPosition().x && rand_y == player()->get<CShape>().circle.getPosition().y);
    
    // safe bounds
    int minX = std::max(m_enemyConfig.SR, 0);
    int maxX = std::max(minX + 1, (int)m_window.getSize().x - m_enemyConfig.SR);
    int minY = std::max(m_enemyConfig.SR, 0);
    int maxY = std::max(minY + 1, (int)m_window.getSize().y - m_enemyConfig.SR);

    std::uniform_int_distribution<int> distx(minX, maxX);
    std::uniform_int_distribution<int> disty(minY, maxY);
    unsigned rand_x = distx(rng);
    unsigned rand_y = disty(rng);
    std::uniform_real_distribution<float> dist_speed(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
    float rand_speed = dist_speed(rng);
    std::uniform_real_distribution<float> dist_velox(-1, 1);
    std::uniform_real_distribution<float> dist_veloy(-1, 1);
    float rand_velox = dist_velox(rng);
    float rand_veloy = dist_veloy(rng);

    Vec2f velo{Vec2f(rand_velox, rand_veloy)};
    velo.normalize();

    enemy->add<CTransform>(Vec2f(rand_x, rand_y), velo, 0.0f, rand_speed);

    enemy->add<CScore>(vertices * 100);

    enemy->add<CCollision>(m_enemyConfig.CR);

    enemy->add<CLifespan>(m_enemyConfig.L);

    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    auto& shape = e->get<CShape>().circle;
    int verts = shape.getPointCount();

    // for each number of points,
    // - spawn a number of small enemies equal to the vertices of the original enemy
    for (int i = 0; i < verts; ++i) {
        Vec2f point_i{ shape.getTransform().transformPoint(shape.getPoint(i)) };
        Vec2f velo{ point_i - shape.getPosition() };
        velo.normalize();
        velo *= (e->get<CTransform>().speed);

        auto se = m_entities.addEntity("smallEnemy");

        // - small enemies are worth double points of the original enemy
        se->add<CScore>(e->get<CScore>().score * e->get<CScore>().multiplier);
        se->add<CTransform>(point_i, velo, 0.f);
        se->add<CCollision>(e->get<CCollision>().radius / 2);
        // - set each small enemy to the same color as the original, half the size
        se->add<CShape>(shape.getRadius() / 2, verts, shape.getFillColor(), shape.getOutlineColor(), shape.getOutlineThickness());
        se->add<CLifespan>(e->get<CLifespan>().lifespan);
    }
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target)
{
    // TODO: implement the spawning of a bullet which travels toward target
    //       - bullet speed is given as a scalar speed
    //       - you must set the velocity by using the formula in notes
    auto bullet = m_entities.addEntity("bullet");

    bullet->add<CShape>(m_bulletConfig.SR, m_bulletConfig.V, 
        sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), 
        sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), 
        m_bulletConfig.OT);
    
    bullet->add<CCollision>(m_bulletConfig.CR);
    
    auto& shape = entity->get<CShape>().circle;
    Vec2f topleft{shape.getPosition()};
    Vec2f origin{topleft + Vec2f{shape.getRadius(), shape.getRadius()}};
    Vec2f velo = target - origin;
    if (velo.length() < 1e-6f) {std::cerr << "Skipped bullet: target too close." << std::endl; return;};
    velo.normalize();
    float speed = m_bulletConfig.S;
    velo *= speed;

    bullet->add<CTransform>(origin, velo, 0.0f, speed);

    bullet->add<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // TODO: implement your own special weapon
}

void Game::sMovement()
{
    // TODO: implement all entity movement in this function
    //       you should read the m_player->cInput component to determine if the player is moving

    // Sample movement speed update for the player
    // auto& transform = player()->get<CTransform>();
    // transform.pos.x += transform.velocity.x;
    // transform.pos.y += transform.velocity.y;

    for (auto& entity : m_entities.getEntities()) {
        auto& transform = entity->get<CTransform>();
        if (entity->tag() == "player") {
            auto& input = entity->get<CInput>();
            Vec2f dir{(float)input.right - (float)input.left,
                      (float)input.down - (float)input.up};
            
            if (dir.x != 0.f || dir.y != 0.f) {
                dir.normalize();
            }
            transform.velocity = dir * entity->get<CTransform>().speed; // this should be stored in transform most likely
        }
        
        transform.pos += transform.velocity;
    }
}

void Game::sLifespan()
{
    // TODO: implement all lifespan functionality

    // for all entities
    for (auto& entity : m_entities.getEntities()) {
        // if entity has no lifespan component, skip it
        if (!entity->has<CLifespan>()) {continue;}
        entity->get<CLifespan>().remaining -= 1;
        // if entity has > 0 remaining lifespan, subtract 1
        if (entity->get<CLifespan>().remaining > 0) {
            // if it has lifespan and is alive
            if (entity->isAlive() && entity->has<CShape>()) {
                // scale its alpha channel properly
                int tot{entity->get<CLifespan>().lifespan};
                int rem{entity->get<CLifespan>().remaining};
                float alpha{(float)rem/(float)tot * 255};
                auto& shape{entity->get<CShape>().circle};
                shape.setFillColor(sf::Color(
                    shape.getFillColor().r,
                    shape.getFillColor().g,
                    shape.getFillColor().b,
                    alpha
                ));
            }
        } 
        // if it has lifespan and its time is up
        else {
            // destroy the entity
            entity->destroy();
        }

    }
}

void Game::sCollision()
{
    // TODO: Implement all proper collisions between entities
    //       be sure to use the collision radius, NOT the shape radius

    float win_width = (float)m_window.getSize().x;
    float win_height = (float)m_window.getSize().y;
    for (auto& e : m_entities.getEntities("enemy")) {
        auto& transform = e->get<CTransform>();
        auto& collision = e->get<CCollision>();

        // left
        if (transform.pos.x - collision.radius < 0.f) {
            transform.pos.x = collision.radius;
            transform.velocity.x *= -1.f;
        // right
        } else if (transform.pos.x + collision.radius > win_width) {
            transform.pos.x = win_width - collision.radius;
            transform.velocity.x *= -1.f;
        }
        // top
        if (transform.pos.y - collision.radius < 0.f) {
            transform.pos.y = collision.radius;
            transform.velocity.y *= -1.f;
        // bottom
        } else if (transform.pos.y + collision.radius > win_height) {
            transform.pos.y = win_height - collision.radius;
            transform.velocity.y *= -1.f;
        }
    }

    for (auto b : m_entities.getEntities("bullet"))
    {
        for (auto e : m_entities.getEntities("enemy"))
        {
            float radii_sum{b->get<CCollision>().radius + e->get<CCollision>().radius};
            Vec2f b_pos{b->get<CShape>().circle.getPosition()};
            Vec2f e_pos{e->get<CShape>().circle.getPosition()};
            float dist = b_pos.dist(e_pos);
            // if the distance between the two shapes 
            // is less than the sum of their collision radii
            if (radii_sum > dist) {
                player()->get<CScore>().score += e->get<CScore>().score;
                b->destroy();
                spawnSmallEnemies(e);
                e->destroy();
            }
        }

        for (auto e : m_entities.getEntities("smallEnemy"))
        {
            float radii_sum{b->get<CCollision>().radius + e->get<CCollision>().radius};
            Vec2f b_pos{b->get<CShape>().circle.getPosition()};
            Vec2f e_pos{e->get<CShape>().circle.getPosition()};
            float dist = b_pos.dist(e_pos);
            if (radii_sum > dist) {
                player()->get<CScore>().score += e->get<CScore>().score;
                b->destroy();
                e->destroy();
            }
        }
    }
}

void Game::sEnemySpawner()
{
    // TODO: code which emplements enemy spawning should go here
    bool shouldSpawnEnemy = ((m_currentFrame - m_lastEnemySpawnTime) >= m_enemyConfig.SI);
    if (!shouldSpawnEnemy) {return;}
    spawnEnemy();

}

void Game::sGUI()
{
    ImGui::Begin("Geometry Wars");
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Systems"))
        {
            ImGui::Text("There will be Systems that can turn on and off here");
            ImGui::Separator();
            ImGui::Checkbox("Player System",       &m_gameConfig.sPlayer);
            ImGui::Checkbox("Movement System",     &m_gameConfig.sMovement);
            ImGui::Checkbox("User Input System",   &m_gameConfig.sUserInput);
            ImGui::Checkbox("Lifespan System",     &m_gameConfig.sLifespan);
            ImGui::Checkbox("Enemy Spawner",       &m_gameConfig.sEnemySpawner);
            ImGui::Checkbox("Collision System",    &m_gameConfig.sCollision);
            ImGui::Checkbox("Shoot System",        &m_gameConfig.sShoot);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Broccoli"))
        {
            ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Cucumber"))
        {
            ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    // ImGui::Separator();
        

    ImGui::End();
}

void Game::sRender()
{
    if (!m_window.isOpen()) {
        std::cerr << "The window didn't open, exiting..." << std::endl;
        std::exit(-1);
    }

    m_window.clear();

    for (const auto& entity : m_entities.getEntities()) {
        if (!entity->has<CShape>() || !entity->has<CTransform>()) {continue;}
        entity->get<CShape>().circle.setPosition(entity->get<CTransform>().pos);
        entity->get<CTransform>().angle += 1.0f;
        entity->get<CShape>().circle.setRotation(sf::degrees(entity->get<CTransform>().angle));
        m_window.draw(entity->get<CShape>().circle);
    }



    // TODO: change the code below to draw ALL of the entities
    //       sample drawing of the player Entity we have created
    // set the position of the shape based on the entity's transform->pos
    // player()->get<CShape>().circle.setPosition(player()->get<CTransform>().pos);


    // // set the rotation of the shape based on the entity's transform->angle
    // player()->get<CTransform>().angle += 1.0f;
    // player()->get<CShape>().circle.setRotation(sf::degrees(player()->get<CTransform>().angle));

    // // draw the entity's sf::CircleShape
    // m_window.draw(player()->get<CShape>().circle);

    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput()
{
    auto& input = player()->get<CInput>();

    input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
    input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

    input.shoot = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    if (input.shoot) {
        std::cout << "Left button pressed at (" << sf::Mouse::getPosition(m_window).x << ", " << sf::Mouse::getPosition(m_window).y << ")." << std::endl;
    }

    // input.special = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
}

void Game::sShoot()
{
    for (auto& entity : m_entities.getEntities()) {
        if (!entity->has<CWeapon>() || !entity->has<CInput>()) {continue;}

        auto& weap = entity->get<CWeapon>();

        // if the user is clicking shoot and bullet's ready
        if (entity->has<CInput>() && entity->get<CInput>().shoot && weap.time_since_shot >= weap.fire_rate) {
            auto mpos = sf::Mouse::getPosition(m_window);
            // sf::Vector2f worldPos = m_window.mapPixelToCoords(mpos);
            // Vec2f target(worldPos);
            Vec2f target{m_window.mapPixelToCoords(mpos)};
            spawnBullet(entity, target);
            weap.time_since_shot = 0.0f;
        }

        weap.time_since_shot += (1.0f / m_windowConfig.FL);
    }
}