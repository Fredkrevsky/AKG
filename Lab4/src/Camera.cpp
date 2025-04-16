#include "Camera.hpp"
#include <algorithm>

void Camera::move(MoveDirection direction) {
    Vector4 forward = (target - eye).normalize();
    Vector4 left = up.cross(forward).normalize();
    
    Vector4 forward_move_vector = forward * camera_speed;
    Vector4 left_move_vector = left * camera_speed;

    switch (direction)
    {
        case MoveDirection::FORWARD:
            eye += forward_move_vector;
            target += forward_move_vector;
            break;

        case MoveDirection::BACK:
            eye -= forward_move_vector;
            target -= forward_move_vector;
            break;

        case MoveDirection::LEFT:      
            eye += left_move_vector;
            target += left_move_vector;
            break;

        case MoveDirection::RIGHT:  
            eye -= left_move_vector;
            target -= left_move_vector;
            break;
    }
}

void Camera::rotate(double delta_x, double delta_y) {
    constexpr auto radians = [](double angle) {
        return PI * angle / 180.0;
    };

    constexpr double max_pitch = radians(89.0);
    constexpr static Vector4 world_up{0.0, 1.0, 0.0, 0.0};

    Vector4 forward = (target - eye).normalize();
    double yaw = std::atan2(forward.z, forward.x);
    double pitch = std::asin(forward.y);

    yaw -= delta_x * rotation_sensitivity;
    pitch += delta_y * rotation_sensitivity;
    pitch = std::clamp(pitch, -max_pitch, max_pitch);

    forward = {
        std::cos(yaw) * std::cos(pitch),
        std::sin(pitch),
        std::sin(yaw) * std::cos(pitch),
        1.0
    };
    forward.normalize();

    Vector4 right = world_up.cross(forward).normalize();
    up = forward.cross(right).normalize();
    target = eye + forward;
}

void Camera::scale(bool is_getting_closer) {
    double new_scale_factor = is_getting_closer 
        ? scale_factor * scale_sensitivity
        : scale_factor / scale_sensitivity;

    scale_factor = std::clamp(
        new_scale_factor,
        MIN_SCALE_FACTOR,
        MAX_SCALE_FACTOR
    );
}

Vector4 Camera::get_eye() const {
    return eye;
}

Vector4 Camera::get_target() const {
    return target;
}

Vector4 Camera::get_up() const {
    return up;
}

double Camera::get_scale() const {
    return scale_factor;
}
