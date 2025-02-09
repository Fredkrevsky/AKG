#pragma once
#include <SFML/Graphics.hpp>
#include <Matrix.hpp>

class MainForm final {
public:
    MainForm();

    ~MainForm() = default;

    void run_main_loop();

private:
    void on_key_press(sf::Keyboard::Key code);

    std::vector<Point3D> rotate_vertices(const std::vector<Point3D>& vertices);

    void draw_line(sf::VertexArray& vertices, int x1, int y1, int x2, int y2);

    void draw_vertices(const std::vector<Point3D>& vertices, const std::vector<Face>& faces);

private:
    constexpr static int width{1600};
    constexpr static int height{900};
    constexpr static double sensitivity{0.01};
    constexpr static double keyboard_sensitivity{0.1};
    constexpr static double scale{200.0};

    sf::RenderWindow m_window;
    //Parser parser
    double m_angleX{0.0}, m_old_angleX{0.0};
    double m_angleY{0.0}, m_old_angleY{0.0};
};