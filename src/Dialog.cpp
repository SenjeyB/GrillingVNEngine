#include "Dialog.hpp"
#include "FontGenerator.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <iostream>
#include <codecvt>
#include <vector>

Dialog::Dialog()
    : currentLine(0)
    , font(std::make_unique<sf::Font>())
    , text(nullptr)
    , characterDelay(0.05f)
    , advanceCooldown(0.2f)
    , isTextBoxVisible(false)
{
    initializeFont();

    textRenderStates = sf::RenderStates::Default;
    textRenderStates.texture = &FontGenerator::getInstance().getTexture();

    textBox.setSize(sf::Vector2f(1040.f, 150.f));
    textBox.setPosition(sf::Vector2f(120.f, 530.f));
    textBox.setFillColor(sf::Color(0, 0, 0, 200));

    nameBox.setSize(sf::Vector2f(200.f, 40.f));
    nameBox.setPosition(sf::Vector2f(140.f, 490.f));
    nameBox.setFillColor(sf::Color(0, 0, 0, 200));
}

void Dialog::initializeFont() {
    if (!FontGenerator::getInstance().generateBitmapFont("assets/resources/fonts/NotoSans.ttf", 24)) {
        std::cerr << "Failed to generate bitmap font" << std::endl;
    }
}

void Dialog::addLine(const std::string& line) {
    bool wasVisible = !dialogLine.isEmpty();
    
    clearText();
    
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string utf32 = converter.from_bytes(line);
    fullDialogLine = sf::String(utf32);
    
    if (!validateString(fullDialogLine)) {
        return;
    }

    sf::String currentLine;
    sf::String currentWord;
    float lineWidth = 0.0f;
    float wordWidth = 0.0f;

    for (size_t i = 0; i < fullDialogLine.getSize(); ++i) {
        uint32_t c = fullDialogLine[i];
        const auto* glyphInfo = FontGenerator::getInstance().getGlyphInfo(c);
        if (!glyphInfo) continue;

        if (c == ' ' || c == '\n') {
            if (lineWidth + wordWidth > MAX_LINE_WIDTH) {
                wrappedLines.push_back(currentLine);
                currentLine = currentWord;
                lineWidth = wordWidth;
            } else {
                currentLine += currentWord;
                lineWidth += wordWidth;
            }
            
            if (c == ' ' && lineWidth + glyphInfo->advance <= MAX_LINE_WIDTH) {
                currentLine += sf::String({static_cast<wchar_t>(c)});
                lineWidth += glyphInfo->advance;
            }
            if (c == '\n') {
                wrappedLines.push_back(currentLine);
                currentLine.clear();
                lineWidth = 0;
            }
            currentWord.clear();
            wordWidth = 0;
        } else {
            currentWord += sf::String({static_cast<wchar_t>(c)});
            wordWidth += glyphInfo->advance;
        }
    }

    if (!currentWord.isEmpty()) {
        if (lineWidth + wordWidth > MAX_LINE_WIDTH) {
            wrappedLines.push_back(currentLine);
            wrappedLines.push_back(currentWord);
        } else {
            currentLine += currentWord;
            wrappedLines.push_back(currentLine);
        }
    } else if (!currentLine.isEmpty()) {
        wrappedLines.push_back(currentLine);
    }
    
    isAnimating = true;
    timeSinceLastChar = 0.0f;
    advanceTimer = 0.0f;
    isTextBoxVisible = wasVisible || !line.empty();
}

void Dialog::update(const float deltaTime) {
    if (!isAnimating) {
        if (advanceTimer < advanceCooldown) {
            advanceTimer += deltaTime;
        }
        return;
    }

    timeSinceLastChar += deltaTime;
    if (timeSinceLastChar >= characterDelay) {
        timeSinceLastChar = 0.0f;

        if (dialogLine.getSize() < fullDialogLine.getSize()) {
            dialogLine += fullDialogLine[dialogLine.getSize()];
        } else {
            isAnimating = false;
        }
    }
}

void Dialog::updateTextVertices() {
    textVertices.clear();
    if (dialogLine.isEmpty()) {
        return;
    }

    size_t totalChars = 0;
    size_t currentLineIndex = 0;
    
    for (const auto& line : wrappedLines) {
        constexpr float startY = 545.f;
        constexpr float startX = 140.f;
        float x = startX;
        float y = startY + (LINE_HEIGHT * currentLineIndex);

        for (size_t i = 0; i < line.getSize() && totalChars < dialogLine.getSize(); ++i, ++totalChars) {
            constexpr float scale = 1.0f;
            uint32_t c = line[i];
            const auto* glyphInfo = FontGenerator::getInstance().getGlyphInfo(c);
            if (!glyphInfo) continue;
            
            TextVertex vertex;
            float xOffset = glyphInfo->offset.x * scale;
            float yOffset = glyphInfo->offset.y * scale;
            float left = x + xOffset;
            float top = y + yOffset;
            float right = left + glyphInfo->size.x * scale;
            float bottom = top + glyphInfo->size.y * scale;
            
            const sf::Texture& texture = FontGenerator::getInstance().getTexture();
            float texLeft = glyphInfo->texCoords.x / static_cast<float>(texture.getSize().x);
            float texRight = texLeft + (glyphInfo->size.x / static_cast<float>(texture.getSize().x));
            float texTop = glyphInfo->texCoords.y / static_cast<float>(texture.getSize().y);
            float texBottom = texTop + (glyphInfo->size.y / static_cast<float>(texture.getSize().y));
            
            if (!validateTextureCoords(texLeft, texTop) || 
                !validateTextureCoords(texRight, texBottom)) {
                continue;
            }
            
            for (auto & vertice : vertex.vertices) {
                vertice.color = sf::Color::White;
            }
            
            vertex.vertices[0].position = sf::Vector2f(left, top);
            vertex.vertices[0].texCoords = sf::Vector2f(texLeft, texTop);
            
            vertex.vertices[1].position = sf::Vector2f(right, top);
            vertex.vertices[1].texCoords = sf::Vector2f(texRight, texTop);
            
            vertex.vertices[2].position = sf::Vector2f(left, bottom);
            vertex.vertices[2].texCoords = sf::Vector2f(texLeft, texBottom);
            
            vertex.vertices[3].position = sf::Vector2f(right, top);
            vertex.vertices[3].texCoords = sf::Vector2f(texRight, texTop);
            
            vertex.vertices[4].position = sf::Vector2f(right, bottom);
            vertex.vertices[4].texCoords = sf::Vector2f(texRight, texBottom);
            
            vertex.vertices[5].position = sf::Vector2f(left, bottom);
            vertex.vertices[5].texCoords = sf::Vector2f(texLeft, texBottom);
            
            textVertices.push_back(vertex);
            x += glyphInfo->advance * scale;
        }
        
        currentLineIndex++;
    }
}

bool Dialog::isAnimationComplete() const {
    return !isAnimating;
}

bool Dialog::canAdvance() const {
    return !isAnimating && advanceTimer >= advanceCooldown;
}

void Dialog::render(sf::RenderWindow& window) {
    try {
        if (isTextBoxVisible) {
            if (!characterName.isEmpty()) {
                window.draw(nameBox);
                
                const sf::Texture& texture = FontGenerator::getInstance().getTexture();
                if (texture.getSize().x == 0 || texture.getSize().y == 0) return;
                
                sf::RenderStates states;
                states.texture = &texture;
                states.blendMode = sf::BlendAlpha;

                auto x = 160.f;
                auto y = 520.f;
                
                for (size_t i = 0; i < characterName.getSize(); ++i) {
                    uint32_t c = characterName[i];
                    const auto* glyphInfo = FontGenerator::getInstance().getGlyphInfo(c);
                    if (!glyphInfo) continue;

                    sf::Sprite sprite(texture);
                    
                    sf::IntRect textureRect(
                        sf::Vector2i(
                            static_cast<int>(glyphInfo->texCoords.x * texture.getSize().x),
                            static_cast<int>(glyphInfo->texCoords.y * texture.getSize().y)
                        ),
                        sf::Vector2i(
                            static_cast<int>(glyphInfo->size.x),
                            static_cast<int>(glyphInfo->size.y)
                        )
                    );
                    sprite.setTextureRect(textureRect);
                    sprite.setPosition(sf::Vector2f(
                        x + glyphInfo->offset.x,
                        y + glyphInfo->offset.y
                    ));
                    
                    window.draw(sprite, states);
                    x += glyphInfo->advance;
                }
            }

            window.draw(textBox);
            updateTextVertices();
            
            if (textVertices.empty()) return;
            
            const sf::Texture& texture = FontGenerator::getInstance().getTexture();
            if (texture.getSize().x == 0 || texture.getSize().y == 0) return;
            
            textRenderStates.texture = &texture;
            textRenderStates.blendMode = sf::BlendAlpha;
            
            for (const auto& vertex : textVertices) {
                window.draw(vertex.vertices, 6, sf::PrimitiveType::Triangles, textRenderStates);
            }
            
            sf::RenderStates states;
            states.texture = &texture;
            states.blendMode = sf::BlendAlpha;

            float startX = 140.f;
            float startY = 560.f;
            size_t totalChars = 0;
            size_t currentLineIndex = 0;
            
            for (const auto& line : wrappedLines) {
                float x = startX;
                float y = startY + (currentLineIndex * LINE_HEIGHT);
                
                for (size_t i = 0; i < line.getSize() && totalChars < dialogLine.getSize(); ++i, ++totalChars) {
                    uint32_t c = line[i];
                    const auto* glyphInfo = FontGenerator::getInstance().getGlyphInfo(c);
                    if (!glyphInfo) continue;

                    sf::Sprite sprite(texture);
                    
                    sf::IntRect textureRect(
                        sf::Vector2i(
                            static_cast<int>(glyphInfo->texCoords.x * texture.getSize().x),
                            static_cast<int>(glyphInfo->texCoords.y * texture.getSize().y)
                        ),
                        sf::Vector2i(
                            static_cast<int>(glyphInfo->size.x),
                            static_cast<int>(glyphInfo->size.y)
                        )
                    );
                    sprite.setTextureRect(textureRect);
                    
                    sprite.setPosition(sf::Vector2f(
                        x + glyphInfo->offset.x,
                        y + glyphInfo->offset.y
                    ));
                    
                    window.draw(sprite);
                    x += glyphInfo->advance;
                }
                
                currentLineIndex++;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in render: " << e.what() << "\n";
    }
}

void Dialog::completeAnimation() {
    if (isAnimating) {
        dialogLine = fullDialogLine;
        isAnimating = false;
        advanceTimer = 0.0f;
        isTextBoxVisible = !dialogLine.isEmpty();
    }
}

void Dialog::setCharacterName(const std::string& name) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string utf32 = converter.from_bytes(name);
    characterName = sf::String(utf32);

    float nameWidth = 0.f;
    for (size_t i = 0; i < characterName.getSize(); ++i) {
        uint32_t c = characterName[i];
        const auto* glyphInfo = FontGenerator::getInstance().getGlyphInfo(c);
        if (glyphInfo) {
            nameWidth += glyphInfo->advance;
        }
    }

    const float PADDING = 20.f;
    nameWidth += PADDING * 2;

    nameBox.setSize(sf::Vector2f(nameWidth, 40.f));
    
    updateNameVertices();
}

void Dialog::updateNameVertices() {
    nameVertices.clear();
    if (characterName.isEmpty()) return;

    const float PADDING = 20.f;
    float startX = nameBox.getPosition().x + PADDING;
    float startY = nameBox.getPosition().y + (nameBox.getSize().y - 24.f) / 2;
    const float scale = 1.0f;
    float x = startX;

    for (size_t i = 0; i < characterName.getSize(); ++i) {
        const unsigned c = characterName[i];
        const auto* glyphInfo = FontGenerator::getInstance().getGlyphInfo(c);
        if (!glyphInfo) continue;

        TextVertex vertex;
        float xOffset = glyphInfo->offset.x * scale;
        float yOffset = glyphInfo->offset.y * scale;
        float left = x + xOffset;
        float top = startY + yOffset;
        float right = left + glyphInfo->size.x * scale;
        float bottom = top + glyphInfo->size.y * scale;

        const sf::Texture& texture = FontGenerator::getInstance().getTexture();
        float texLeft = glyphInfo->texCoords.x / texture.getSize().x;
        float texRight = texLeft + (glyphInfo->size.x / texture.getSize().x);
        float texTop = glyphInfo->texCoords.y / texture.getSize().y;
        float texBottom = texTop + (glyphInfo->size.y / texture.getSize().y);

        for (int v = 0; v < 6; ++v) {
            vertex.vertices[v].color = sf::Color::White;
        }
        
        vertex.vertices[0].position = sf::Vector2f(left, top);
        vertex.vertices[0].texCoords = sf::Vector2f(texLeft, texTop);
        
        vertex.vertices[1].position = sf::Vector2f(right, top);
        vertex.vertices[1].texCoords = sf::Vector2f(texRight, texTop);
        
        vertex.vertices[2].position = sf::Vector2f(left, bottom);
        vertex.vertices[2].texCoords = sf::Vector2f(texLeft, texBottom);
        
        vertex.vertices[3].position = sf::Vector2f(right, top);
        vertex.vertices[3].texCoords = sf::Vector2f(texRight, texTop);
        
        vertex.vertices[4].position = sf::Vector2f(right, bottom);
        vertex.vertices[4].texCoords = sf::Vector2f(texRight, texBottom);
        
        vertex.vertices[5].position = sf::Vector2f(left, bottom);
        vertex.vertices[5].texCoords = sf::Vector2f(texLeft, texBottom);

        nameVertices.push_back(vertex);
        x += glyphInfo->advance * scale;
    }
}

