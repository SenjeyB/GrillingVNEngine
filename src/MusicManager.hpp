#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <string>
#include <map>
#include <filesystem>

class MusicManager {
public:
    void loadTrack(const std::string& name, const std::string& path, bool loop = false);
    void playTrack(const std::string& name, float volume = 100.f, float fadeInTime = 0.f, float fadeOutTime = 0.f);
    void stopMusic(float fadeOutTime = 0.f);
    void update(float deltaTime);

private:
    struct MusicState {
        std::unique_ptr<sf::Music> music;
        float targetVolume{0.f};
        float currentVolume{0.f};
        float fadeTime{0.f};
        float fadeTimer{0.f};
        bool fading{false};
    };

    struct TrackInfo {
        std::unique_ptr<sf::Music> music;
        std::string path;
        bool loop{false};
    };

    std::map<std::string, TrackInfo> tracks;
    MusicState currentTrack;
    std::string currentTrackName;
}; 