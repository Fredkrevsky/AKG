#pragma once
#include "Matrix.hpp"
#include <vector>



class Camera final {
public:
    enum class MoveDirection {
        FORWARD,
        BACK,
        LEFT,
        RIGHT
    };
    
    Camera() noexcept = default;
    
    void move(MoveDirection direction);
    void rotate(double angle_x, double angle_y);
    void scale(bool is_getting_closer);
    
    Vector4 get_eye() const;
    Vector4 get_target() const;
    Vector4 get_up() const;
    double get_scale() const;
    
private:
    constexpr static double rotation_sensitivity{0.1};
    constexpr static double scale_sensitivity{1.25};
    constexpr static double MIN_SCALE_FACTOR{0.2};
    constexpr static double MAX_SCALE_FACTOR{5.0};
    constexpr static double camera_speed{0.05};
    
    Vector4 eye{5.0, 5.0, 5.0, 1};
    Vector4 target{5.0, 5.0, 0, 1};
    Vector4 up{0, 1, 0, 1};
    double scale_factor{1.0};
};