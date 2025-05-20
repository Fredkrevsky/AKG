#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "FPSCounter.hpp"
#include "Camera.hpp"

class Logger {
public:
    Logger() noexcept = default;
    virtual ~Logger() = default;

    [[nodiscard]] bool initialize();

    void set_fps_counter(std::shared_ptr<FPSCounter> counter);
    void set_camera(std::shared_ptr<Camera> camera);
    void draw(sf::RenderWindow& window) const;
    void update();

private:
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<FPSCounter> m_counter;
    sf::Text m_text;
    sf::Font m_font;
};