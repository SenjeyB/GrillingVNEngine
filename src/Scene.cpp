#include "Scene.hpp"

Scene::Scene()
: background(defaultTexture)
{
    const sf::Image img({1280u, 720u}, sf::Color(50, 50, 50));
    defaultTexture.loadFromImage(img);
    
    background.setTexture(defaultTexture, true);

    constexpr float targetWidth = 1280.0f;
    constexpr float targetHeight = 720.0f;
    const float textureWidth = static_cast<float>(defaultTexture.getSize().x);
    const float textureHeight = static_cast<float>(defaultTexture.getSize().y);
    
    float scaleX = targetWidth / textureWidth;
    float scaleY = targetHeight / textureHeight;
    float scale = std::min(scaleX, scaleY);
    
    background.setScale({scale, scale});
    
    float xOffset = (targetWidth - (textureWidth * scale)) / 2.0f;
    float yOffset = (targetHeight - (textureHeight * scale)) / 2.0f;
    background.setPosition({xOffset, yOffset});
}

void Scene::load() {
    for (auto& character : characters) {
        character.setExpression("default");
    }
}

void Scene::update(const float deltaTime) {
    dialog.update(deltaTime);
}

void Scene::render(sf::RenderWindow& window) {
    window.draw(background);
    for (auto& character : characters) {
        character.render(window);
    }
    dialog.render(window);
}

void Scene::addCharacter(Character&& character) {
    characters.push_back(std::move(character));
}
