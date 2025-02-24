#pragma once
#include "Matrix.hpp"
#include <vector>

enum class MoveDirection {
    FORWARD,
    BACK,
    LEFT,
    RIGHT
};

class Camera final {
public:
    Camera() noexcept = default;
    ~Camera() noexcept = default;

    void move(MoveDirection direction);
    void rotate(double angle_x, double angle_y);
    void scale(bool is_getting_closer);

    std::vector<Point> transform_vertices(const std::vector<Point>& vertices);
    TransformMatrix get_transform_matrix();

    Point get_eye() const;
    Point get_target() const;
    Point get_up() const;
    double get_scale() const;

private:
    constexpr static double rotation_sensitivity{0.5};
    constexpr static double scale_sensitivity{1.25};
    constexpr static double camera_speed{0.1};
    constexpr static double width{1600.0};
    constexpr static double height{900.0};
    constexpr static double fov = 2.0 * PI / 3.0;

    Point eye{0, 0, -10, 1};
    Point target{0, 0, 0, 1};
    Point up{0, 1, 0, 0};

    double scale_factor{1.0};
};