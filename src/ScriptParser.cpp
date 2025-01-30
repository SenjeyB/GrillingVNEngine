#include "ScriptParser.hpp"

ScriptData ScriptParser::parseScript(const std::string& filename) {
    ScriptData data;
    YAML::Node script = YAML::LoadFile(filename);
    
    data.sceneName = script["scene_name"].as<std::string>();
    if (script["background"]) {
        data.backgroundPath = script["background"].as<std::string>();
    } else {
        data.backgroundPath = "assets/default.png";
    }

    if (script["font"]) {
        data.fontPath = "assets/resources/fonts/" + script["font"].as<std::string>();
    }

    for (const auto& character : script["characters"]) {
        auto name = character["name"].as<std::string>();
        ScriptData::CharacterData charData;

        for (const auto& sprite : character["sprites"]) {
            charData.sprites[sprite.first.as<std::string>()] = sprite.second.as<std::string>();
        }

        if (character["initial_position"]) {
            auto pos = character["initial_position"].as<std::vector<float>>();
            charData.initial_position = sf::Vector2f(pos[0], pos[1]);
        } else {
            charData.initial_position = sf::Vector2f(960.f, 540.f);
        }
        
        data.characterData[name] = charData;
    }

    for (const auto& cmd : script["script"]) {
        data.commands.push_back(parseCommand(cmd));
    }

    if (script["next_scene"]) {
        data.nextScenePath = script["next_scene"].as<std::string>();
    } else {
        data.nextScenePath = "exit";
    }

    if (script["music"]) {
        for (const auto& track : script["music"]) {
            auto name = track["name"].as<std::string>();
            ScriptData::MusicData musicData;
            musicData.path = track["path"].as<std::string>();
            if (track["loop"]) {
                musicData.loop = track["loop"].as<bool>();
            }
            data.musicTracks[name] = musicData;
        }
    }

    return data;
}

ScriptCommand ScriptParser::parseCommand(const YAML::Node& node) {
    ScriptCommand cmd;

    if (const auto type = node["type"].as<std::string>(); type == "dialog") {
        cmd.type = ScriptCommand::DIALOG;
        if (node["character"]) {
            cmd.character = node["character"].as<std::string>();
        }
        cmd.text = node["text"].as<std::string>();
        if (node["expression"]) {
            cmd.expression = node["expression"].as<std::string>();
        }
    }
    else if (type == "move") {
        cmd.type = ScriptCommand::MOVE;
        cmd.character = node["character"].as<std::string>();
        const auto pos = node["position"].as<std::vector<float>>();
        cmd.position = sf::Vector2f(pos[0], pos[1]);
        if (node["duration"]) {
            cmd.duration = node["duration"].as<float>();
        }
        if (node["smooth"]) {
            cmd.smooth = node["smooth"].as<bool>();
        }
    }
    else if (type == "music") {
        cmd.type = ScriptCommand::MUSIC;
        cmd.musicName = node["track"].as<std::string>();
        
        if (node["volume"]) {
            cmd.volume = node["volume"].as<float>();
        }
        if (node["fade_in"]) {
            cmd.fadeInTime = node["fade_in"].as<float>();
        }
        if (node["fade_out"]) {
            cmd.fadeOutTime = node["fade_out"].as<float>();
        }
        if (node["loop"]) {
            cmd.loop = node["loop"].as<bool>();
        }
    }

    return cmd;
}
