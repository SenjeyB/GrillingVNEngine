#include "Character.hpp"

#include <utility>

Character::Character(std::string characterName)
    : name(std::move(characterName))
    , position(0.f, 0.f)
    , defaultTexture(std::make_unique<sf::Texture>())
    , sprite(*defaultTexture)
{
    defaultTexture->resize({1, 1});
}

void Character::addExpression(const std::string& expressionName, const sf::Texture& texture) {
    expressions[expressionName] = texture;
    if (expressions.size() == 1) {
        setExpression(expressionName);
    }
}

void Character::setExpression(const std::string& expressionName) {
    if (const auto it = expressions.find(expressionName); it != expressions.end()) {
        currentSprite = std::make_unique<sf::Sprite>(it->second);
        currentSprite->setPosition(position);
    }
}

void Character::setPosition(const sf::Vector2f& pos) {
    position = pos;
    if (currentSprite) {
        currentSprite->setPosition(position);
    } else {
        sprite.setPosition(position);
    }
}

void Character::render(sf::RenderWindow& window) const {
    if (currentSprite) {
        window.draw(*currentSprite);
    } else {
        window.draw(sprite);
    }
}
