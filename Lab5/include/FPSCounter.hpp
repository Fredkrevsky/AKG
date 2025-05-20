#pragma once
#include <SFML/Graphics.hpp>

class FPSCounter {    
public:
    FPSCounter() noexcept = default;
    virtual ~FPSCounter() = default;

    void update();
    float get_fps() const;

private:
    sf::Clock m_clock;
    int m_frameCount{};
    float m_elapsedTime{};
    float m_fps{};
};