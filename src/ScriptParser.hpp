#pragma once
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include <map>
#include <yaml-cpp/yaml.h>

struct ScriptCommand {
    enum Type {
        DIALOG,
        MOVE,
        MUSIC
    } type;
    
    std::string character;
    std::string text;
    std::string expression;
    sf::Vector2f position;
    float duration{0.0f};
    bool smooth{true};
    
    std::string musicName;
    float volume{100.0f};
    float fadeInTime{0.0f};
    float fadeOutTime{0.0f};
    bool loop{false};
};

struct ScriptData {
    std::string sceneName;
    std::string backgroundPath;
    std::string nextScenePath;
    std::string fontPath{"assets/resources/fonts/arial.ttf"};
    struct CharacterData {
        std::map<std::string, std::string> sprites;
        sf::Vector2f initial_position;
    };
    std::map<std::string, CharacterData> characterData;
    
    struct MusicData {
        std::string path;
        bool loop{false};
    };
    std::map<std::string, MusicData> musicTracks;
    
    std::vector<ScriptCommand> commands;
};

class ScriptParser {
public:
    static ScriptData parseScript(const std::string& filename);
private:
    static ScriptCommand parseCommand(const YAML::Node& node);
};
