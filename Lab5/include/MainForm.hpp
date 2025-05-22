#pragma once
#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "FPSCounter.hpp"
#include "Logger.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"

class MainForm final {
public:
    MainForm() noexcept;
    
    void run_main_loop();
    
private:
    void on_key_press(sf::Keyboard::Key code);
    void handle_mouse();
    void handle_keyboard();
    void draw();
    
private:
    sf::RenderWindow m_window;
    sf::Texture m_texture;
    Renderer m_renderer;
    Scene m_scene;
    Logger m_logger;
    
    std::shared_ptr<FPSCounter> m_counter;
    std::shared_ptr<Camera> m_camera;

    sf::Vector2i m_mouse_press_position{};
    sf::Vector2i m_center{};
    float m_delta_time{1.0};
};
