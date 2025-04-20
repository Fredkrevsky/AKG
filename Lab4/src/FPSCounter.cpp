#include <FPSCounter.hpp>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std::string_literals;

void FPSCounter::update() {
    static int frameCount{};
    static float elapsedTime{};
    
    frameCount++;
    elapsedTime += m_clock.restart().asSeconds();

    if (elapsedTime >= 1.0f) {
        m_fps = frameCount / elapsedTime;
        frameCount = 0;
        elapsedTime = 0.0f;
    }
}

float FPSCounter::get_fps() const {
    return m_fps;
}
