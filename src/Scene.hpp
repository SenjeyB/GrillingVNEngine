#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Character.hpp"
#include "Dialog.hpp"

class Scene {
protected:
    sf::Texture defaultTexture;
    sf::Sprite background;
    std::vector<Character> characters;
    Dialog dialog;
    std::vector<std::string> dialogLines;
    
public:
    Scene();
    virtual ~Scene() = default;
    virtual void load();
    virtual void update(float deltaTime);
    virtual void render(sf::RenderWindow& window);
    void addCharacter(Character&& character);
    
    void setBackground(const sf::Texture& texture) {
        background.setTexture(texture);

        constexpr float targetWidth = 1280.0f;
        constexpr float targetHeight = 720.0f;

        const float textureWidth = static_cast<float>(texture.getSize().x);
        const float textureHeight = static_cast<float>(texture.getSize().y);

        float scaleX = targetWidth / textureWidth;
        float scaleY = targetHeight / textureHeight;

        float scale = std::max(scaleX, scaleY);
        background.setScale({scale, scale});

        float scaledWidth = textureWidth * scale;
        float scaledHeight = textureHeight * scale;
        float xOffset = (targetWidth - scaledWidth) / 2.0f;
        float yOffset = (targetHeight - scaledHeight) / 2.0f;
        background.setPosition({xOffset, yOffset});
    }
    void setDialogLines(const std::string& line) {
        dialog.addLine(line);
    }
    sf::Sprite& getBackground() { return background; }
};
