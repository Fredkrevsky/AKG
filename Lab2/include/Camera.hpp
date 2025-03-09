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
    
    void move(MoveDirection direction);
    void rotate(double angle_x, double angle_y);
    void scale(bool is_getting_closer);
    
    TransformMatrix get_projection_matrix() const;
    TransformMatrix get_view_matrix() const;
    TransformMatrix get_scale_matrix() const;
    TransformMatrix get_viewport_matrix() const;

    Point get_eye() const;
    Point get_target() const;
    Point get_up() const;
    double get_scale() const;
    
private:
    constexpr static double rotation_sensitivity{0.1};
    constexpr static double scale_sensitivity{1.25};
    constexpr static double MIN_SCALE_FACTOR{0.2};
    constexpr static double MAX_SCALE_FACTOR{5.0};
    constexpr static double camera_speed{0.05};
    constexpr static double width{1600.0};
    constexpr static double height{900.0};
    constexpr static double fov = PI / 2.0;
    constexpr static double znear = 0.01; 
    constexpr static double zfar = 100.0; 
    constexpr static double aspect = width / height; 
    
    Point eye{0, 0, -10, 1};
    Point target{0, 0, 0, 1};
    Point up{0, 1, 0, 1};

    double scale_factor{1.0};
};