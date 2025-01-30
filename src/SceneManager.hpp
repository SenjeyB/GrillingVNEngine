#pragma once
#include <memory>
#include <map>
#include <string>
#include <filesystem>
#include <utility>
#include "Scene.hpp"

class Game;

class SceneManager {
private:
    Game* game{nullptr};
    std::map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* currentScene{nullptr};
    std::string scriptsDirectory;
    std::string currentScriptPath;
    bool shouldQuit{false};

public:
    explicit SceneManager(Game* gameInstance, std::string  scriptDir = "assets/scripts")
        : game(gameInstance), scriptsDirectory(std::move(scriptDir)) {}

    explicit SceneManager(std::string scriptDir);

    bool initialize();
    bool loadNextScene();
    [[nodiscard]] bool shouldExit() const { return shouldQuit; }
    void addScene(const std::string& name, std::unique_ptr<Scene> scene);
    void switchScene(const std::string& name);
    void update(float deltaTime) const;
    void render(sf::RenderWindow& window) const;
    Scene* getCurrentScene() { return currentScene; }
    [[nodiscard]] const Scene* getCurrentScene() const { return currentScene; }

private:
    [[nodiscard]] std::string findFirstScript() const;
};
