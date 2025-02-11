#pragma once
#include <SFML/Graphics.hpp>
#include <Matrix.hpp>
#include <Parser.hpp>
#include <FPSCounter.hpp>
#include <unordered_set>

constexpr static int width{1600};
constexpr static int height{900};

using Bitmap = std::array<uint32_t, width * height>;


class MainForm final {
public:
    MainForm() noexcept;
    ~MainForm() noexcept = default;

    void run_main_loop();

private:
    void on_key_press(sf::Keyboard::Key code);
    void draw_line(int x1, int y1, int x2, int y2);
    void draw_vertices(const std::vector<Point3D>& vertices);
    void load_from_file();    

private:
    constexpr static double sensitivity{0.01};
    constexpr static double keyboard_sensitivity{0.1};
    constexpr static double scale_sensitivity{1.25};
    
    sf::RenderWindow m_window;
    Vertices m_vertices;
    Faces m_faces;
    FPSCounter m_counter;
    Bitmap m_bitmap;

    double m_angleX{0.0}, m_old_angleX{0.0};
    double m_angleY{0.0}, m_old_angleY{0.0};
    double m_posX{0.0}, m_posY{0.0};
    double scale{200.0};
};