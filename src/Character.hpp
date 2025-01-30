#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <map>
#include <memory>

class Character {
public:
    explicit Character(std::string  characterName);

    Character(const Character&) = delete;
    Character& operator=(const Character&) = delete;

    Character(Character&&) noexcept = default;
    Character& operator=(Character&&) noexcept = default;

    void addExpression(const std::string& expressionName, const sf::Texture& texture);
    void setExpression(const std::string& expressionName);
    void setPosition(const sf::Vector2f& pos);
    void render(sf::RenderWindow& window) const;
    const std::string& getName() const { return name; }
    const sf::Vector2f& getPosition() const { return position; }

private:
    std::string name;
    sf::Vector2f position;
    std::unique_ptr<sf::Texture> defaultTexture;
    sf::Sprite sprite;
    std::unique_ptr<sf::Sprite> currentSprite;
    std::map<std::string, sf::Texture> expressions;
};