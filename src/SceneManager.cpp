#include "SceneManager.hpp"
#include "ScriptedScene.hpp"
#include <filesystem>
#include <algorithm>
#include <utility>

SceneManager::SceneManager(std::string  scriptDir)
    : currentScene(nullptr), scriptsDirectory(std::move(scriptDir)), shouldQuit(false)
{
    try {
        if (const std::filesystem::path path(scriptsDirectory); !path.is_absolute()) {
            scriptsDirectory = std::filesystem::absolute(path).string();
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Path error: " << e.what() << std::endl;
    }
}     

bool SceneManager::initialize() {
    if (!std::filesystem::exists(scriptsDirectory)) {
        return false;
    }

    currentScriptPath = findFirstScript();
    if (currentScriptPath.empty()) {
        return false;
    }
    
    auto scene = std::make_unique<ScriptedScene>(currentScriptPath, game);
    addScene(currentScriptPath, std::move(scene));
    switchScene(currentScriptPath);
    return true;
}

std::string SceneManager::findFirstScript() const {
    try {
        std::vector<std::string> scriptFiles;
        
        for (const auto& entry : std::filesystem::directory_iterator(scriptsDirectory)) {
            if (entry.path().extension() == ".yaml") {
                scriptFiles.push_back(entry.path().string());
            }
        }
        
        if (scriptFiles.empty()) {
            return "";
        }
        
        std::sort(scriptFiles.begin(), scriptFiles.end());
        return scriptFiles[0];
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return "";
    }
}

bool SceneManager::loadNextScene() {
    if (!currentScene) return false;
    
    auto* scriptedScene = dynamic_cast<ScriptedScene*>(currentScene);
    if (!scriptedScene) return false;

    const std::string& nextScenePath = scriptedScene->getScriptData().nextScenePath;
    
    if (nextScenePath == "exit") {
        shouldQuit = true;
        return false;
    }
    
    scriptedScene->stopMusic();
    
    std::string fullPath = scriptsDirectory;
    fullPath += "/" + nextScenePath;
    
    auto scene = std::make_unique<ScriptedScene>(fullPath, game);
    currentScriptPath = fullPath;
    addScene(fullPath, std::move(scene));
    switchScene(fullPath);
    return true;
}

void SceneManager::addScene(const std::string& name, std::unique_ptr<Scene> scene) {
    scenes[name] = std::move(scene);
    if (!currentScene) {
        currentScene = scenes[name].get();
    }
}

void SceneManager::switchScene(const std::string& name) {
    if (const auto it = scenes.find(name); it != scenes.end()) {
        currentScene = it->second.get();
        currentScene->load();
    }
}

void SceneManager::update(const float deltaTime) const {
    if (currentScene) {
        currentScene->update(deltaTime);
    }
}

void SceneManager::render(sf::RenderWindow& window) const {
    if (currentScene) {
        currentScene->render(window);
    }
}
