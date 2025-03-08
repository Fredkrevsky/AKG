#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "FPSCounter.hpp"
#include "Camera.hpp"

class Logger {
public:
    Logger() noexcept = default;
    ~Logger() noexcept = default;

    [[nodiscard]] bool initialize();

    void set_fps_counter(std::shared_ptr<FPSCounter> counter);
    void set_camera(std::shared_ptr<Camera> camera);
    void draw(sf::RenderWindow& window);
    void update();

private:
    constexpr static auto FONT_PATH = "../arial.ttf";

    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<FPSCounter> m_counter;
    sf::Text m_text;
    sf::Font m_font;
};