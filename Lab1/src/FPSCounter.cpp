#include <FPSCounter.hpp>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std::string_literals;

bool FPSCounter::initialize(){
    if (!font.loadFromFile(font_name)) {
        return false;
    }
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10.f, 10.f);
    return true;
}

void FPSCounter::update() {
    static int frameCount{};
    static float elapsedTime{};
    static float fps{};
    
    frameCount++;
    elapsedTime += clock.restart().asSeconds();

    if (elapsedTime >= 1.0f) {
        fps = frameCount / elapsedTime;
        frameCount = 0;
        elapsedTime = 0.0f;

        std::string str_fps = "FPS: "s + std::to_string(static_cast<int>(fps));
        fpsText.setString(str_fps);
    }
}

void FPSCounter::draw(sf::RenderWindow& window) const {
    window.draw(fpsText);
}
