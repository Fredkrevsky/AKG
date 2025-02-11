#pragma once
#include <SFML/Graphics.hpp>

class FPSCounter final {    
public:
    FPSCounter() noexcept = default;
    ~FPSCounter() noexcept = default;

    [[nodiscard]] bool initialize();
    void update();
    void draw(sf::RenderWindow& window) const;

private:
    constexpr static auto font_name = "arial.ttf";

    sf::Font font;
    sf::Text fpsText;
    sf::Clock clock;
};