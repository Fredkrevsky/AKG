#pragma once
#include <SFML/Graphics.hpp>

class FPSCounter final {    
public:
    FPSCounter() noexcept = default;
    ~FPSCounter() noexcept = default;

    void update();
    double get_fps() const;

private:
    double m_fps;
    sf::Clock m_clock;
};