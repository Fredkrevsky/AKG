#pragma once
#include <SFML/Graphics.hpp>
#include <Matrix.hpp>
#include <Parser.hpp>
#include <FPSCounter.hpp>
#include <unordered_set>


class MainForm final {
    public:
    MainForm() noexcept;
    ~MainForm() noexcept = default;
    
    void run_main_loop();
    
    private:
    void on_key_press(sf::Keyboard::Key code);
    void load_from_file();
    void update_camera();
    void handle_mouse_rotation();
    void handle_keyboard_movement();
    std::vector<Point> transform_vertices(const std::vector<Point>& vertices);
    
    private:
    constexpr static double sensitivity{0.01};
    constexpr static double keyboard_sensitivity{0.1};
    constexpr static double scale_sensitivity{1.25};
    constexpr static double camera_speed{0.1};
    constexpr static int width{1600};
    constexpr static int height{900};

    sf::RenderWindow m_window;
    Vertices m_vertices;
    Faces m_faces;
    FPSCounter m_counter;
    Bitmap m_bitmap;

    Point eye{0, 0, 5, 1};
    Point target{0, 0, 0, 1};
    Point up{0, 1, 0, 0};

    double scalar{1.0};

    sf::Vector2i mouse_press_position{};
    bool is_mouse_pressed{false};

    TransformMatrix m_view_matrix{};
    TransformMatrix m_projection_matrix{};
    TransformMatrix m_viewport_matrix{};
    TransformMatrix m_scale{};
};
