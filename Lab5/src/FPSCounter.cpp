#include <FPSCounter.hpp>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std::string_literals;

void FPSCounter::update() { 
    m_frameCount++;
    m_elapsedTime += m_clock.restart().asSeconds();

    if (m_elapsedTime >= 1.0f) {
        m_fps = m_frameCount / m_elapsedTime;
        m_frameCount = 0;
        m_elapsedTime = 0.0f;
    }
}

float FPSCounter::get_fps() const {
    return m_fps;
}
