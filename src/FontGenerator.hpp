#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

class FontGenerator {
public:
    struct GlyphInfo {
        sf::Vector2f texCoords;
        sf::Vector2f size;
        float advance;
        sf::Vector2f offset;
    };

    static FontGenerator& getInstance();
    
    bool generateBitmapFont(const std::string& ttfPath, unsigned int fontSize);
    const sf::Texture& getTexture() const { return fontTexture; }
    const GlyphInfo* getGlyphInfo(uint32_t charcode) const;
    float getLineHeight() const { return lineHeight; }
    void refreshTexture() {
        fontTexture = renderTex.getTexture();
    }

    void renderDebugInfo(sf::RenderWindow& window, sf::Vector2f position = {10, 10}) const;

    std::vector<uint32_t> getAvailableCharacters() const {
        std::vector<uint32_t> chars;
        for (const auto& pair : glyphMap) {
            chars.push_back(pair.first);
        }
        return chars;
    }

private:
    FontGenerator() {
        renderTex = sf::RenderTexture(sf::Vector2u{1, 1});
        fontTexture = renderTex.getTexture();
    }
    
    sf::RenderTexture renderTex;
    sf::Texture fontTexture;
    std::unordered_map<uint32_t, GlyphInfo> glyphMap;
    float lineHeight{0.0f};
};
