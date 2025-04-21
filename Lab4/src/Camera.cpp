#include "Camera.hpp"
#include <algorithm>

void Camera::move(MoveDirection direction, float distance) {
    glm::vec3 forward = glm::normalize(target - eye);
    glm::vec3 left = glm::normalize(glm::cross(up, forward));
    
    glm::vec3 forward_move_vector = forward * camera_speed * distance;
    glm::vec3 left_move_vector = left * camera_speed * distance;

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

void Camera::rotate(float delta_x, float delta_y) {
    constexpr auto radians = [](float angle) {
        return PI * angle / 180.0;
    };

    constexpr float max_pitch = radians(89.0);
    constexpr static glm::vec3 world_up{0.0, 1.0, 0.0};

    glm::vec3 forward = glm::normalize(target - eye);
    float yaw = std::atan2(forward.z, forward.x);
    float pitch = std::asin(forward.y);

    yaw -= delta_x * rotation_sensitivity;
    pitch += delta_y * rotation_sensitivity;
    pitch = std::clamp(pitch, -max_pitch, max_pitch);

    forward = glm::normalize(glm::vec3{
        std::cos(yaw) * std::cos(pitch),
        std::sin(pitch),
        std::sin(yaw) * std::cos(pitch)
    });

    glm::vec3 right = glm::normalize(glm::cross(world_up, forward));
    up = glm::normalize(glm::cross(forward, right));
    target = eye + forward;
}

void Camera::scale(bool is_getting_closer) {
    float new_scale_factor = is_getting_closer 
        ? scale_factor * scale_sensitivity
        : scale_factor / scale_sensitivity;

    scale_factor = std::clamp(
        new_scale_factor,
        MIN_SCALE_FACTOR,
        MAX_SCALE_FACTOR
    );
}

glm::vec3 Camera::get_eye() const {
    return eye;
}

glm::vec3 Camera::get_target() const {
    return target;
}

glm::vec3 Camera::get_up() const {
    return up;
}

float Camera::get_scale() const {
    return scale_factor;
}
