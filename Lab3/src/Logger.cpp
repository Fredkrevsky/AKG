#include "Logger.hpp"
#include <format>

bool Logger::initialize(){
    if (!m_font.loadFromFile(FONT_PATH)) {
        return false;
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(30);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(10.f, 10.f);
    return true;
}

void Logger::set_fps_counter(std::shared_ptr<FPSCounter> counter) {
    m_counter = std::move(counter);
}

void Logger::set_camera(std::shared_ptr<Camera> camera) {
    m_camera = std::move(camera);
}

void Logger::draw(sf::RenderWindow& window) {
    window.draw(m_text);
}

void Logger::update() {
    double fps = m_counter->get_fps();
    Vector4 eye = m_camera->get_eye();
    Vector4 target = m_camera->get_target();
    Vector4 up = m_camera->get_up();
    double scale = m_camera->get_scale();

    std::string text_str = std::format(
        "FPS = {:.0f}\n"
        "Eye = [{:.2f}, {:.2f}, {:.2f}]\n"
        "target = [{:.2f}, {:.2f}, {:.2f}]\n"
        "up = [{:.2f}, {:.2f}, {:.2f}]\n"
        "Scale = {:.2f}",
        fps, 
        eye.x, eye.y, eye.z,
        target.x, target.y, target.z,
        up.x, up.y, up.z,
        scale
    );

    m_text.setString(text_str);
}
