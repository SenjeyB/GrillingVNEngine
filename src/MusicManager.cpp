#include "MusicManager.hpp"
#include <iostream>

void MusicManager::loadTrack(const std::string& name, const std::string& path, bool loop) {
    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(path)) {
        std::cerr << "Failed to load music track: " << path << std::endl;
        return;
    }
    TrackInfo trackInfo;
    trackInfo.music = std::move(music);
    trackInfo.path = path;
    trackInfo.loop = loop;
    tracks[name] = std::move(trackInfo);
}

void MusicManager::playTrack(const std::string& name, const float volume, float fadeInTime, float fadeOutTime) {
    if (currentTrack.music && currentTrack.music->getStatus() == sf::SoundSource::Status::Playing) {
        if (fadeOutTime > 0.f) {
            currentTrack.targetVolume = 0.f;
            currentTrack.fadeTime = fadeOutTime;
            currentTrack.fadeTimer = 0.f;
            currentTrack.fading = true;
        } else {
            currentTrack.music->stop();
        }
    }

    if (const auto it = tracks.find(name); it != tracks.end()) {
        currentTrack.music = std::make_unique<sf::Music>();
        if (!currentTrack.music->openFromFile(it->second.path)) {
            std::cerr << "Failed to open music from file: " << it->second.path << std::endl;
            return;
        }

        currentTrack.music->setLooping(false);
        
        if (it->second.loop) {
            currentTrack.music->setLooping(true);
            currentTrack.music->setLoopPoints(sf::Music::TimeSpan{
                sf::Time::Zero,
                currentTrack.music->getDuration()
            });
        }
        
        if (fadeInTime > 0.f) {
            currentTrack.music->setVolume(0.f);
            currentTrack.targetVolume = volume;
            currentTrack.currentVolume = 0.f;
            currentTrack.fadeTime = fadeInTime;
            currentTrack.fadeTimer = 0.f;
            currentTrack.fading = true;
        } else {
            currentTrack.music->setVolume(volume);
            currentTrack.currentVolume = volume;
            currentTrack.fading = false;
        }
        currentTrack.music->play();
        currentTrackName = name;
    }
}

void MusicManager::stopMusic(float fadeOutTime) {
    if (currentTrack.music && currentTrack.music->getStatus() == sf::SoundSource::Status::Playing) {
        if (fadeOutTime > 0.f) {
            currentTrack.targetVolume = 0.f;
            currentTrack.fadeTime = fadeOutTime;
            currentTrack.fadeTimer = 0.f;
            currentTrack.fading = true;
        } else {
            currentTrack.music->stop();
        }
    }
}

void MusicManager::update(float deltaTime) {
    if (currentTrack.music && currentTrack.fading) {
        currentTrack.fadeTimer += deltaTime;

        if (const float progress = currentTrack.fadeTimer / currentTrack.fadeTime; progress >= 1.f) {
            if (currentTrack.targetVolume == 0.f) {
                currentTrack.music->stop();
            } else {
                currentTrack.music->setVolume(currentTrack.targetVolume);
            }
            currentTrack.fading = false;
        } else {
            const float newVolume = currentTrack.currentVolume +
                                    (currentTrack.targetVolume - currentTrack.currentVolume) * progress;
            currentTrack.music->setVolume(newVolume);
        }
    }
} 