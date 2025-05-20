#include "Logger.hpp"
#include <iostream>
#include <format>

namespace {

constexpr auto FONT_PATH = "../arial.ttf";
constexpr int CHARACTER_SIZE = 30;
constexpr float TEXT_POSITION_X = 10.f;
constexpr float TEXT_POSITION_Y = 10.f;

}

bool Logger::initialize(){
    if (!m_font.loadFromFile(FONT_PATH)) {
        std::cerr << "Failed to load font from " << FONT_PATH << '\n';
        return false;
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(CHARACTER_SIZE);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(TEXT_POSITION_X, TEXT_POSITION_Y);
    return true;
}

void Logger::set_fps_counter(std::shared_ptr<FPSCounter> counter) {
    m_counter = counter;
}

void Logger::set_camera(std::shared_ptr<Camera> camera) {
    m_camera = camera;
}

void Logger::draw(sf::RenderWindow& window) const {
    window.draw(m_text);
}

void Logger::update() {
    float fps = m_counter->get_fps();
    glm::vec3 eye = m_camera->get_eye();
    glm::vec3 target = m_camera->get_target();
    glm::vec3 up = m_camera->get_up();
    float scale = m_camera->get_scale();

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
