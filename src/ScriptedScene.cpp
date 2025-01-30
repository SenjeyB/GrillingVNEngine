#include "ScriptedScene.hpp"
#include <filesystem>
#include "FontGenerator.hpp"
#include "Game.hpp"

ScriptedScene::ScriptedScene(const std::string& scriptPath, Game* gameInstance) 
    : game(gameInstance) {
    ScriptParser parser;
    scriptData = parser.parseScript(scriptPath);

    if (!FontGenerator::getInstance().generateBitmapFont(scriptData.fontPath, 24)) {
        std::cerr << "Failed to load font " << scriptData.fontPath << ", falling back to default\n";
        if (!FontGenerator::getInstance().generateBitmapFont("assets/resources/fonts/arial.ttf", 24)) {
            std::cerr << "Failed to load default font!\n";
        }
    }
    
    std::filesystem::path bgPath(scriptData.backgroundPath);
    if (!bgPath.is_absolute()) {
        bgPath = std::filesystem::current_path() / bgPath;
    }

    if (!backgroundTexture.loadFromFile(bgPath.string())) {
        sf::Image fallbackImg({1920u, 1080u}, sf::Color(50, 50, 50));
        backgroundTexture.loadFromImage(fallbackImg);
    }
    
    setBackground(backgroundTexture);
    initializeCharacters();

    for (const auto& [trackName, trackData] : scriptData.musicTracks) {
        musicManager.loadTrack(trackName, trackData.path, trackData.loop);
    }
}

void ScriptedScene::initializeCharacters() {
    for (const auto& [charName, charData] : scriptData.characterData) {
        Character character(charName);
        for (const auto& [exprName, texturePath] : charData.sprites) {
            if (sf::Texture texture; texture.loadFromFile(texturePath)) {
                character.addExpression(exprName, texture);
            }
        }
        character.setPosition(charData.initial_position);
        addCharacter(std::move(character));
    }
}

void ScriptedScene::load() {
    currentCommand = 0;
    commandInProgress = false;
    commandTimer = 0.0f;
    sceneInitialized = false;
    Scene::load();
}

void ScriptedScene::update(const float deltaTime) {
    Scene::update(deltaTime);
    musicManager.update(deltaTime);

    if (!sceneInitialized) {
        sceneInitialized = true;
        executeNextCommand();
        return;
    }

    if (commandInProgress && currentCommand <= scriptData.commands.size()) {
        const auto& cmd = scriptData.commands[currentCommand - 1];
        if (processCommand(cmd, deltaTime)) {
            commandInProgress = false;
        }
    }

    if (!game || !game->isWindowActive()) {
        return;
    }

    static bool spaceWasPressed = false;
    static bool mouseWasPressed = false;

    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
    bool rightMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    if (ctrlPressed || rightMousePressed) {
        if (dialog.isAnimationComplete()) {
            completeCurrentAnimations();
            executeNextCommand();
        } else {
            completeCurrentAnimations();
        }
    }
    else if (spacePressed || mousePressed) {
        if (!spaceWasPressed && !mouseWasPressed) {
            if (dialog.isAnimationComplete()) {
                if (dialog.canAdvance()) {
                    completeCurrentAnimations();
                    executeNextCommand();
                }
            } else {
                completeCurrentAnimations();
            }
            spaceWasPressed = spacePressed;
            mouseWasPressed = mousePressed;
        }
    } else {
        spaceWasPressed = false;
        mouseWasPressed = false;
    }
}

void ScriptedScene::completeCurrentCommand() {
    if (currentCommand <= scriptData.commands.size()) {
        const auto& cmd = scriptData.commands[currentCommand - 1];
        if (cmd.type == ScriptCommand::MOVE) {
            for (auto& character : characters) {
                if (character.getName() == cmd.character) {
                    character.setPosition(cmd.position);
                    break;
                }
            }
        }
    }
}

void ScriptedScene::executeNextCommand() {
    if (currentCommand > scriptData.commands.size()) return;
    commandInProgress = false;
    commandTimer = 0.0f;
    if (currentCommand < scriptData.commands.size()) {
        const auto& cmd = scriptData.commands[currentCommand];
        bool finishedNow = processCommand(cmd, 0.0f); 
        commandInProgress = !finishedNow;
        if (finishedNow && cmd.type == ScriptCommand::MUSIC) {
            currentCommand++;
            executeNextCommand();
            return;
        }
    }
    currentCommand++;
}

bool ScriptedScene::processCommand(const ScriptCommand& cmd, const float deltaTime) {
    switch (cmd.type) {
        case ScriptCommand::DIALOG: {
            const std::string lines = cmd.text;
            setDialogLines(lines);

            if (!cmd.character.empty()) {
                dialog.setCharacterName(cmd.character);
            } else {
                dialog.setCharacterName("");
            }

            commandInProgress = false;
            if (!cmd.expression.empty()) {
                for (auto& character : characters) {
                    if (character.getName() == cmd.character) {
                        character.setExpression(cmd.expression);
                        break;
                    }
                }
            }
            return true;
        }
        
        case ScriptCommand::MOVE: {
            for (auto& character : characters) {
                if (character.getName() == cmd.character) {
                    bool skipAnimation = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) ||
                                       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                                       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

                    if (cmd.duration > 0 && cmd.smooth && !skipAnimation) {
                        commandTimer += deltaTime;
                        float progress = std::min(commandTimer / cmd.duration, 1.0f);
                        const sf::Vector2f currentPos = character.getPosition();
                        const sf::Vector2f targetPos = cmd.position;
                        const sf::Vector2f newPos = currentPos + (targetPos - currentPos) * progress;
                        character.setPosition(newPos);
                        return progress >= 1.0f;
                    }
                    character.setPosition(cmd.position);
                    return true;
                }
            }
            return true;
        }
        
        
        case ScriptCommand::MUSIC: {
            if (cmd.musicName.empty()) {
                musicManager.stopMusic(cmd.fadeOutTime);
            } else {
                auto it = scriptData.musicTracks.find(cmd.musicName);
                if (it != scriptData.musicTracks.end()) {
                    bool shouldLoop = cmd.loop;
                    musicManager.loadTrack(cmd.musicName, it->second.path, shouldLoop);
                }
                musicManager.playTrack(cmd.musicName, cmd.volume, cmd.fadeInTime, cmd.fadeOutTime);
            }
            return true;
        }
        
        default:
            return true;
    }
}

void ScriptedScene::completeCurrentAnimations() {
    dialog.completeAnimation();
    completeCurrentCommand();
}

ScriptedScene::~ScriptedScene() {
    musicManager.stopMusic(0.0f);
}