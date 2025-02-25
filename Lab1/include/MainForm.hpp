#pragma once
#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "FPSCounter.hpp"
#include "Logger.hpp"
#include "Scene.hpp"

class MainForm final {
public:
    MainForm() noexcept;
    ~MainForm() noexcept = default;
    
    void run_main_loop();
    
private:
    void on_key_press(sf::Keyboard::Key code);
    void handle_mouse_rotation();
    void handle_keyboard_movement();
    void draw();
    
private:
    constexpr static double sensitivity{0.1};
    constexpr static double keyboard_sensitivity{0.1};
    constexpr static int width{1600};
    constexpr static int height{900};

    sf::RenderWindow m_window;
    sf::Texture m_texture;
    Bitmap m_bitmap;
    Scene m_scene;
    Logger m_logger;
    
    std::shared_ptr<FPSCounter> m_counter;
    std::shared_ptr<Camera> m_camera;

    sf::Vector2i mouse_press_position{};
    sf::Vector2i center{width / 2, height / 2};
};
