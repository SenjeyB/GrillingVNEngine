#pragma once
#include <SFML/Graphics.hpp>
#include "SceneManager.hpp"

class Game {
private:
    sf::RenderWindow window;
    SceneManager sceneManager;
    bool isRunning;

public:
    Game();
    void run();
    void processEvents();
    void update(float deltaTime);
    void render();
    bool isWindowActive() const { return window.hasFocus(); }
};
