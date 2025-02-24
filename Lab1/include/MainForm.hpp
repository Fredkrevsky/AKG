#pragma once
#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "Matrix.hpp"
#include "Parser.hpp"
#include "FPSCounter.hpp"
#include "Logger.hpp"

class MainForm final {
public:
    MainForm() noexcept;
    ~MainForm() noexcept = default;
    
    void run_main_loop();
    
private:
    void on_key_press(sf::Keyboard::Key code);
    void load_from_file();
    void handle_mouse_rotation();
    void handle_keyboard_movement();
    
private:
    constexpr static double sensitivity{0.1};
    constexpr static double keyboard_sensitivity{0.1};
    constexpr static int width{1600};
    constexpr static int height{900};

    sf::RenderWindow m_window;
    Vertices m_vertices;
    Faces m_faces;
    Bitmap m_bitmap;
    
    std::shared_ptr<FPSCounter> m_counter;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Logger> m_logger;

    sf::Vector2i mouse_press_position{};
    sf::Vector2i center{width / 2, height / 2};
};
