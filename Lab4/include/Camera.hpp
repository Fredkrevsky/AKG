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
    
    void move(MoveDirection direction, float distance);
    void rotate(float angle_x, float angle_y);
    void scale(bool is_getting_closer);
    
    Vector4 get_eye() const;
    Vector4 get_target() const;
    Vector4 get_up() const;
    float get_scale() const;
    
private:
    constexpr static float rotation_sensitivity{0.1};
    constexpr static float scale_sensitivity{1.25};
    constexpr static float MIN_SCALE_FACTOR{0.2};
    constexpr static float MAX_SCALE_FACTOR{5.0};
    constexpr static float camera_speed{1.0};
    
    Vector4 eye{5.0, 5.0, 5.0, 1};
    Vector4 target{0, 0, 0, 1};
    Vector4 up{0, 1, 0, 1};
    float scale_factor{1.0};
};