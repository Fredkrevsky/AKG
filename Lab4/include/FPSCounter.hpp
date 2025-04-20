#pragma once
#include <SFML/Graphics.hpp>

class FPSCounter final {    
public:
    FPSCounter() noexcept = default;

    void update();
    float get_fps() const;

private:
    float m_fps;
    sf::Clock m_clock;
};