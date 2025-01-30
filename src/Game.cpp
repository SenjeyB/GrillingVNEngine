#include "Game.hpp"
#include "ScriptedScene.hpp"
#include <SFML/Window/Event.hpp>

#include "FontGenerator.hpp"

Game::Game()
    : window(sf::VideoMode({1280u, 720u}), "GRILLING Visual Novel Engine")
    , sceneManager(this, "assets/scripts")
    , isRunning(true) {
    window.setFramerateLimit(60);
    if (!sceneManager.initialize()) {
        isRunning = false;
    }
}

void Game::run() {
    sf::Clock clock;
    while (isRunning && window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(deltaTime.asSeconds());
        render();
    }
}

void Game::processEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            isRunning = false;
        }
        
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
                isRunning = false;
            }
        }
    }
}

void Game::update(const float deltaTime) {
    sceneManager.update(deltaTime);
    
    if (auto* scriptedScene = dynamic_cast<ScriptedScene*>(sceneManager.getCurrentScene())) {
        if (scriptedScene->isSceneInitialized() && 
            scriptedScene->isComplete() && 
            !scriptedScene->isCommandInProgress()) {
            if (!sceneManager.loadNextScene()) {
                isRunning = false;
            }
        }
    }
}

void Game::render() {
    window.clear();
    sceneManager.render(window);
    window.display();
}
