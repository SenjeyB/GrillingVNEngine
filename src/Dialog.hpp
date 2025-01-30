#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <codecvt>
#include <iostream>
#include <locale>

namespace sf {
    class Font;
    class Text;
}

class Dialog {
public:
    Dialog();

    void addLine(const std::string& line);
    void render(sf::RenderWindow& window);
    void update(float deltaTime);
    bool isAnimationComplete() const;
    void completeAnimation();
    bool canAdvance() const;

    static constexpr float NORMAL_CHAR_DELAY = 0.05f;
    static constexpr float FAST_CHAR_DELAY = 0.01f;
    
    void setFastMode(bool fast) {
        characterDelay = fast ? FAST_CHAR_DELAY : NORMAL_CHAR_DELAY;
    }

    void setCharacterName(const std::string& name);

    void clearText() {
        dialogLine.clear();
        fullDialogLine.clear();
        wrappedLines.clear();
        isAnimating = false;
        timeSinceLastChar = 0.0f;
    }

private:
    struct TextVertex {
        sf::Vertex vertices[6];
    };

    std::vector<TextVertex> textVertices;
    sf::RenderStates textRenderStates;
    void updateTextVertices();

    size_t currentLine;
    std::vector<std::string> dialogLines;
    sf::String dialogLine;
    std::unique_ptr<sf::Font> font;
    std::unique_ptr<sf::Text> text;
    sf::RectangleShape textBox;
    sf::String fullDialogLine;
    float characterDelay{0.03f};
    float timeSinceLastChar{0.0f};
    bool isAnimating{false};
    float advanceCooldown{0.3f};
    float advanceTimer{0.0f};

    static void initializeFont();

    const float MAX_LINE_WIDTH = 980.0f;
    const float LINE_HEIGHT = 30.0f;
    std::vector<sf::String> wrappedLines;

    sf::RectangleShape nameBox;
    sf::String characterName;
    std::vector<TextVertex> nameVertices;
    void updateNameVertices();

    bool isTextBoxVisible{false};

    static bool validateString(const sf::String& str) {
        if (str.isEmpty()) {
            return false;
        }

        for (std::size_t i = 0; i < str.getSize(); ++i) {
            uint32_t c = str[i];
            if (c < 32 && c != '\n') {
                return false;
            }
        }
        
        return str.getSize() < 1000;
    }

    static bool validateTextureCoords(const float x, const float y) {
        return x >= 0.0f && x <= 1.0f && 
               y >= 0.0f && y <= 1.0f;
    }

    static void debugTextureCoords(const TextVertex& vertex) {
        std::cout << "Texture coordinates:\n";
        for (int i = 0; i < 6; ++i) {
            std::cout << "Vertex " << i << ": ("
                     << vertex.vertices[i].texCoords.x << ", "
                     << vertex.vertices[i].texCoords.y << ")\n";
        }
    }
};