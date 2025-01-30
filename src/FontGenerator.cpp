#include "FontGenerator.hpp"
#include <vector>
#include <iostream>

FontGenerator& FontGenerator::getInstance() {
    static FontGenerator instance;
    return instance;
}

bool FontGenerator::generateBitmapFont(const std::string& ttfPath, unsigned int fontSize) {
    sf::Font ttfFont;
    if (!ttfFont.openFromFile(ttfPath)) {
        std::cerr << "Failed to load TTF font: " << ttfPath << std::endl;
        return false;
    }

    std::vector<std::pair<uint32_t, uint32_t>> ranges = {
        {32, 126},
        {1024, 1279}
    };

    constexpr unsigned int padding = 2;
    unsigned int maxGlyphHeight = 0;
    unsigned int totalWidth = padding;
    std::vector<sf::Glyph> glyphs;

    for (const auto& range : ranges) {
        for (uint32_t c = range.first; c <= range.second; ++c) {
            try {
                sf::Glyph glyph = ttfFont.getGlyph(c, fontSize, false);
                float glyphWidth = glyph.bounds.size.x;
                float glyphHeight = glyph.bounds.size.y;
                maxGlyphHeight = std::max(maxGlyphHeight, static_cast<unsigned>(glyphHeight + padding * 2));
                totalWidth += static_cast<unsigned>(glyphWidth + padding * 2);
                glyphs.push_back(glyph);
            } catch (const std::exception& e) {
                std::cerr << "Failed to get glyph " << c << ": " << e.what() << "\n";
            }
        }
    }

    unsigned int texWidth = 1;
    while (texWidth < totalWidth) texWidth *= 2;
    unsigned int texHeight = 1;
    while (texHeight < maxGlyphHeight) texHeight *= 2;
    
    renderTex = sf::RenderTexture(sf::Vector2u{texWidth, texHeight});
    if (!renderTex.setActive()) {
        std::cerr << "Failed to create render texture" << std::endl;
        return false;
    }

    if (renderTex.getSize().x == 0 || renderTex.getSize().y == 0) {
        std::cerr << "RenderTexture has invalid size!" << std::endl;
        return false;
    }

    renderTex.clear(sf::Color::Transparent);
    
    float currentX = padding;
    float currentY = padding;
    size_t glyphIndex = 0;
    
    for (const auto& range : ranges) {
        for (uint32_t c = range.first; c <= range.second; ++c) {
            if (glyphIndex >= glyphs.size()) {
                std::cerr << "Glyph index out of bounds!\n";
                continue;
            }
            
            const sf::Glyph& glyph = glyphs[glyphIndex++];
            
            sf::Sprite glyphSprite(ttfFont.getTexture(fontSize));
            glyphSprite.setTextureRect(glyph.textureRect);
            glyphSprite.setPosition(sf::Vector2f{currentX, currentY});
            glyphSprite.setColor(sf::Color::White);
            
            renderTex.draw(glyphSprite);

            GlyphInfo info;
            info.texCoords = sf::Vector2f(
                static_cast<float>(currentX) / texWidth,
                static_cast<float>(currentY) / texHeight
            );
            float glyphWidth = glyph.bounds.size.x;
            float glyphHeight = glyph.bounds.size.y;
            info.size = sf::Vector2f(glyphWidth, glyphHeight);
            info.advance = glyph.advance;
            info.offset = sf::Vector2f(glyph.bounds.position.x, glyph.bounds.position.y);
            
            glyphMap[c] = info;

            currentX += glyphWidth + padding;
            if (currentX + glyphWidth + padding > texWidth) {
                currentX = padding;
                currentY += maxGlyphHeight + padding;
                if (currentY + maxGlyphHeight + padding > texHeight) {
                    std::cerr << "Not enough texture height to place all glyphs.\n";
                    return false;
                }
            }
        }
    }

    renderTex.display();
    
    if (!renderTex.setActive() || renderTex.getSize().x == 0 || renderTex.getSize().y == 0) {
        std::cerr << "RenderTexture is invalid after creation!" << std::endl;
        return false;
    }
    
    fontTexture = renderTex.getTexture();
    if (fontTexture.getSize().x == 0 || fontTexture.getSize().y == 0) {
        std::cerr << "Failed to create valid font texture!\n";
        return false;
    }
    
    refreshTexture();
    lineHeight = ttfFont.getLineSpacing(fontSize);
    
    return true;
}

const FontGenerator::GlyphInfo* FontGenerator::getGlyphInfo(uint32_t charcode) const {
    const auto it = glyphMap.find(charcode);
    return it != glyphMap.end() ? &it->second : nullptr;
}
