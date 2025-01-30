#pragma once
#include "Scene.hpp"
#include "ScriptParser.hpp"
#include "MusicManager.hpp"

class Game;

class ScriptedScene : public Scene {
private:
    Game* game{nullptr};
    ScriptData scriptData;
    size_t currentCommand{0};
    bool commandInProgress{false};
    sf::Texture backgroundTexture;
    float commandTimer{0.0f};
    MusicManager musicManager;
    bool sceneInitialized{false};

public:
    explicit ScriptedScene(const std::string& scriptPath, Game* gameInstance);
    void load() override;
    ~ScriptedScene() override;
    void update(float deltaTime) override;
    const ScriptData& getScriptData() const { return scriptData; }
    bool isComplete() const { return currentCommand > scriptData.commands.size(); }
    bool isCommandInProgress() const { return commandInProgress; }
    void stopMusic() { musicManager.stopMusic(0.0f); }
    bool isSceneInitialized() const { return sceneInitialized; }

private:
    void executeNextCommand();
    void completeCurrentCommand();
    void completeCurrentAnimations();
    bool processCommand(const ScriptCommand& cmd, float deltaTime);
    void initializeCharacters();
};
